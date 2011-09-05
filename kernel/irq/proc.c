/*
 * linux/kernel/irq/proc.c
 *
 * Copyright (C) 1992, 1998-2004 Linus Torvalds, Ingo Molnar
 *
 * This file contains the /proc/irq/ handling code.
 */

#include <linux/irq.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/poll.h>

#include "internals.h"

static struct proc_dir_entry *root_irq_dir;

#ifdef CONFIG_SMP

static int irq_affinity_read_proc(char *page, char **start, off_t off,
				  int count, int *eof, void *data)
{
	struct irq_desc *desc = irq_desc + (long)data;
	cpumask_t *mask = &desc->affinity;
	int len;

#ifdef CONFIG_GENERIC_PENDING_IRQ
	if (desc->status & IRQ_MOVE_PENDING)
		mask = &desc->pending_mask;
#endif
	len = cpumask_scnprintf(page, count, *mask);

	if (count - len < 2)
		return -EINVAL;
	len += sprintf(page + len, "\n");
	return len;
}

#ifndef is_affinity_mask_valid
#define is_affinity_mask_valid(val) 1
#endif

int no_irq_affinity;
static int irq_affinity_write_proc(struct file *file, const char __user *buffer,
				   unsigned long count, void *data)
{
	unsigned int irq = (int)(long)data, full_count = count, err;
	cpumask_t new_value, tmp;

	if (!irq_desc[irq].chip->set_affinity || no_irq_affinity ||
	    irq_balancing_disabled(irq))
		return -EIO;

	err = cpumask_parse_user(buffer, count, new_value);
	if (err)
		return err;

	if (!is_affinity_mask_valid(new_value))
		return -EINVAL;

	/*
	 * Do not allow disabling IRQs completely - it's a too easy
	 * way to make the system unusable accidentally :-) At least
	 * one online CPU still has to be targeted.
	 */
	cpus_and(tmp, new_value, cpu_online_map);
	if (cpus_empty(tmp))
		/* Special case for empty set - allow the architecture
		   code to set default SMP affinity. */
		return select_smp_affinity(irq) ? -EINVAL : full_count;

	irq_set_affinity(irq, new_value);

	return full_count;
}

#endif

#define MAX_NAMELEN 128

static int name_unique(unsigned int irq, struct irqaction *new_action)
{
	struct irq_desc *desc = irq_desc + irq;
	struct irqaction *action;
	unsigned long flags;
	int ret = 1;

	spin_lock_irqsave(&desc->lock, flags);
	for (action = desc->action ; action; action = action->next) {
		if ((action != new_action) && action->name &&
				!strcmp(new_action->name, action->name)) {
			ret = 0;
			break;
		}
	}
	spin_unlock_irqrestore(&desc->lock, flags);
	return ret;
}

void register_handler_proc(unsigned int irq, struct irqaction *action)
{
	char name [MAX_NAMELEN];

	if (!irq_desc[irq].dir || action->dir || !action->name ||
					!name_unique(irq, action))
		return;

	memset(name, 0, MAX_NAMELEN);
	snprintf(name, MAX_NAMELEN, "%s", action->name);

	/* create /proc/irq/1234/handler/ */
	action->dir = proc_mkdir(name, irq_desc[irq].dir);
}

#undef MAX_NAMELEN

#define MAX_NAMELEN 10

struct irq_proc {
 	unsigned long irq;
 	wait_queue_head_t q;
 	atomic_t count;
 	char devname[TASK_COMM_LEN];
};

static irqreturn_t irq_proc_irq_handler(int irq, void *vidp)
{
 	struct irq_proc *idp = (struct irq_proc *)vidp;
	unsigned long stamp;

 	BUG_ON(idp->irq != irq);
	
 	disable_irq_nosync(irq);
 	atomic_inc(&idp->count);
	
 	wake_up(&idp->q);
 	return IRQ_HANDLED;
}


/*
 * Signal to userspace an interrupt has occured.
 */
static ssize_t irq_proc_read(struct file *filp, char  __user *bufp, size_t len, loff_t *ppos)
{
 	struct irq_proc *ip = (struct irq_proc *)filp->private_data;
 	irq_desc_t *idp = irq_desc + ip->irq;
 	int pending;

 	DEFINE_WAIT(wait);

 	if (len < sizeof(int))
 		return -EINVAL;

	pending = atomic_read(&ip->count);
 	if (pending == 0) {
 		if (idp->status & IRQ_DISABLED)
 			enable_irq(ip->irq);
 		if (filp->f_flags & O_NONBLOCK)
 			return -EWOULDBLOCK;
 	}

 	while (pending == 0) {
 		prepare_to_wait(&ip->q, &wait, TASK_INTERRUPTIBLE);
		pending = atomic_read(&ip->count);
		if (pending == 0)
 			schedule();
 		finish_wait(&ip->q, &wait);
 		if (signal_pending(current))
 			return -ERESTARTSYS;
 	}

 	if (copy_to_user(bufp, &pending, sizeof pending))
 		return -EFAULT;

 	*ppos += sizeof pending;

 	atomic_sub(pending, &ip->count);
 	return sizeof pending;
}


static int irq_proc_open(struct inode *inop, struct file *filp)
{
 	struct irq_proc *ip;
 	struct proc_dir_entry *ent = PDE(inop);
 	int error;

 	ip = kmalloc(sizeof *ip, GFP_KERNEL);
 	if (ip == NULL)
 		return -ENOMEM;

 	memset(ip, 0, sizeof(*ip));
 	strcpy(ip->devname, current->comm);
 	init_waitqueue_head(&ip->q);
 	atomic_set(&ip->count, 0);
 	ip->irq = (unsigned long)ent->data;

 	error = request_irq(ip->irq,
			    irq_proc_irq_handler,
			    0,
			    ip->devname,
			    ip);
	if (error < 0) {
		kfree(ip);
		return error;
	}
 	filp->private_data = (void *)ip;

 	return 0;
}

static int irq_proc_release(struct inode *inop, struct file *filp)
{
 	struct irq_proc *ip = (struct irq_proc *)filp->private_data;

 	free_irq(ip->irq, ip);
 	filp->private_data = NULL;
 	kfree(ip);
 	return 0;
}

static unsigned int irq_proc_poll(struct file *filp, struct poll_table_struct *wait)
{
 	struct irq_proc *ip = (struct irq_proc *)filp->private_data;
 	irq_desc_t *idp = irq_desc + ip->irq;

 	if (atomic_read(&ip->count) > 0)
 		return POLLIN | POLLRDNORM; /* readable */

 	/* if interrupts disabled and we don't have one to process... */
 	if (idp->status & IRQ_DISABLED)
 		enable_irq(ip->irq);

 	poll_wait(filp, &ip->q, wait);

 	if (atomic_read(&ip->count) > 0)
 		return POLLIN | POLLRDNORM; /* readable */

 	return 0;
}

static struct file_operations irq_proc_file_operations = {
 	.read = irq_proc_read,
 	.open = irq_proc_open,
 	.release = irq_proc_release,
 	.poll = irq_proc_poll,
};


void register_irq_proc(unsigned int irq)
{
	struct proc_dir_entry *entry;
	char name [MAX_NAMELEN];

	if (!root_irq_dir)
		return;

	memset(name, 0, MAX_NAMELEN);
	sprintf(name, "%d", irq);

	if (!irq_desc[irq].dir) {
		/* create /proc/irq/1234 */
		irq_desc[irq].dir = proc_mkdir(name, root_irq_dir);

		/*
		 * Create handles for user-mode interrupt handlers
		 * if the kernel hasn't already grabbed the IRQ
		 */
 		entry = create_proc_entry("irq", 0600, irq_desc[irq].dir);
 		if (entry) {
 			entry->data = (void *)(unsigned long)irq;
 			entry->read_proc = NULL;
 			entry->write_proc = NULL;
 			entry->proc_fops = &irq_proc_file_operations;
 		}
	}
#ifdef CONFIG_SMP
	{
		struct proc_dir_entry *entry;

		/* create /proc/irq/<irq>/smp_affinity */
		entry = create_proc_entry("smp_affinity", 0600, irq_desc[irq].dir);

		if (entry) {
			entry->data = (void *)(long)irq;
			entry->read_proc = irq_affinity_read_proc;
			entry->write_proc = irq_affinity_write_proc;
		}
	}
#endif
}

#undef MAX_NAMELEN

void unregister_handler_proc(unsigned int irq, struct irqaction *action)
{
	if (action->dir)
		remove_proc_entry(action->dir->name, irq_desc[irq].dir);
}

void init_irq_proc(void)
{
	int i;

	/* create /proc/irq */
	root_irq_dir = proc_mkdir("irq", NULL);
	if (!root_irq_dir)
		return;

	/*
	 * Create entries for all existing IRQs.
	 */
	for (i = 0; i < NR_IRQS; i++)
		register_irq_proc(i);
}

