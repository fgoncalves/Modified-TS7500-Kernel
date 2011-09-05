#ifndef STR8100_I2C_H
#define STR8100_I2C_H

typedef struct _i2c_transfer_s_
{
  unsigned int transfer_cmd;    
  unsigned int write_data_len;    
  unsigned int read_data_len;    
  unsigned int write_data;    
  unsigned int read_data;    
  unsigned int slave_addr;    
  unsigned int action_done;    
  unsigned int error_status;
}i2c_transfer_t;


/*
 * define 32 bit IO access macros
 */ 
#define IO_OUT_WORD(reg, data)     ((*((volatile u_int32 *)(reg))) = (u_int32)(data))
#define IO_IN_WORD(reg)            (*((volatile u_int32 *)(reg)))

#endif
