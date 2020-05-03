#include "io_expander.h"


//*****************************************************************************
// Sets the address to read/write from
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    reg_address:    8-bit address of which register is going to be accessed.
//                    This is NOT the same as the I2C device address.
//
// Returns
// I2C_OK if the byte was written to the FT6X06.
//*****************************************************************************
static i2c_status_t io_expander_set_addr
( 
  uint32_t  i2c_base,
  uint8_t  reg_address
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // ADD CODE
  // Set the I2C device address 
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
    
    
  if ( status != I2C_OK )
  {
    return status;
  }
  
  //==============================================================
  // ADD CODE
  // Send the register address
  //==============================================================
	status = i2cSendByte(i2c_base, reg_address, I2C_MCS_START | I2C_MCS_RUN |I2C_MCS_STOP);
	if ( status != I2C_OK )
  {
    return status;
  }
  return status;
}

//*****************************************************************************
// Reads one byte of data from the current address of the FT6x06  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    data:       data to read.
//
// Returns
// I2C_OK if the byte was written to the FT6X06.
//*****************************************************************************
static i2c_status_t ft6x06_read_data
( 
  uint32_t  i2c_base,
  uint8_t *data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // ADD CODE
  // Set the I2C address 
  //==============================================================
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_READ);
    
  
  if ( status != I2C_OK )
  {
    return status;
  }

  //==============================================================
  // ADD CODE
  // get the data
  //==============================================================
  status = i2cGetByte( i2c_base, data , I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);


  return status;
}

static i2c_status_t wait_for_write( int32_t  i2c_base)
{
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

  // Set the I2C address to be the EEPROM and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);

  // Poll while the device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // The data we send does not matter.  This has been set to 0x00, but could
    // be set to anything
    status = i2cSendByte( i2c_base, 0x00, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for the address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  return  status;
}

void io_expander_write_reg(uint8_t reg, uint8_t data){
	i2c_status_t status;
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(IO_EXPANDER_I2C_BASE)) {};

  //==============================================================
  // Set the I2C address
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE,MCP23017_DEV_ID,I2C_WRITE);
	if(status!= I2C_OK)
		return;
  // If the IOExpander is still writing the last byte written, wait
  wait_for_write(IO_EXPANDER_I2C_BASE);
  

  //==============================================================
  // Send the address
  //==============================================================
	status = i2cSendByte(IO_EXPANDER_I2C_BASE,reg,I2C_MCS_START|I2C_MCS_RUN);
	if(status!= I2C_OK)
		return;

  //==============================================================
  // Send the Byte of data to write
	// ADD CODE
  //==============================================================
	status = i2cSendByte(IO_EXPANDER_I2C_BASE,data,I2C_MCS_RUN|I2C_MCS_STOP);
  return;
}




//*****************************************************************************
// 
//*****************************************************************************
bool io_expander_init(void)
{ 
  //==============================
  // Configure I2C GPIO Pins
  //==============================  
  if(gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false)
  {
    return false;
  }
  
  // Configure SCL 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
    

  
  // Configure SDA 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
  
  
  //  Initialize the I2C peripheral
  if( initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
	
	
	i2cSetSlaveAddr(IO_EXPANDER_I2C_BASE,MCP23017_DEV_ID,I2C_WRITE);
	//IODIRA
	i2cSendByte(IO_EXPANDER_I2C_BASE,0x00,I2C_MCS_START | I2C_MCS_RUN);
  // Set PortA to be outputs
  i2cSendByte( IO_EXPANDER_I2C_BASE, 0x00, I2C_MCS_RUN | I2C_MCS_STOP);
	
	
  
  return true;
  
} 

