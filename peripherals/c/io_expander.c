
#include "io_expander.h"

//******************************************************************************
// EXTERN GLOBALS
//******************************************************************************

bool io_expander_trigger;

//******************************************************************************
// Handler for the interrupt
//******************************************************************************
//base address if GPIOF
void GPIOF_Handler(void)
{
	// alert the main thread of the program 
	io_expander_trigger = true;
	
	io_expander_read_reg(MCP23017_INTCAPB_R);
	io_expander_read_reg(MCP23017_GPIOB_R);
	// clears the interrupt
	GPIOF->ICR |= GPIO_ICR_GPIO_M;
}


// Will write one byte of data to a register
static i2c_status_t io_expander_byte_write(uint32_t i2c_base, uint8_t addr, uint8_t data)
{
  i2c_status_t status;
  
  // I2C device is idle
  while (I2CMasterBusy(i2c_base)) {};

	// Set the Slave device to be port expander
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID , I2C_WRITE);
  if (status != I2C_OK) return status;

	// Set the register to write 
  status = i2cSendByte( i2c_base, addr, I2C_MCS_START | I2C_MCS_RUN);
	if ( status != I2C_OK ) return status;
	
	// to write data to register 
	status = i2cSendByte( i2c_base, data, I2C_MCS_RUN | I2C_MCS_STOP);
  return status;
}

// read one byte of data from register
static i2c_status_t io_expander_byte_read(uint32_t i2c_base, uint8_t addr, uint8_t *data)
{
  i2c_status_t status;
  
  //I2C device is idle
	while ( I2CMasterBusy(i2c_base)) {};
	
	// Sets Slave address of port expander
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
  if (status != I2C_OK) return status;

	// Sets the register to read from 
  status = i2cSendByte( i2c_base, addr, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
	if (status != I2C_OK) return status;
	
	// Sets Slave address to read mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID , I2C_READ);
  if (status != I2C_OK) return status;

	// Reads one byte of data
  status = i2cGetByte(i2c_base, data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
  return status;
}

// To Configure the port expander
static void io_expander_config(void)
{
	// set port a (leds) to be outputs
	io_expander_write_reg(MCP23017_IODIRA_R, 0x00);
	// set port b (switch, buttons) to be inputs
	io_expander_write_reg(MCP23017_IODIRB_R, 0xFF);
	// enable pull-up resistors for port b
	io_expander_write_reg(MCP23017_GPPUB_R, 0x0F);
	// enable interrupts for port b[3:0]
	io_expander_write_reg(MCP23017_GPINTENB_R, 0x0F);
	// interrupt when pin differs from previous value
	io_expander_write_reg(MCP23017_INTCONB_R, 0x00);
	// reset config
	io_expander_write_reg(MCP23017_IOCONB_R, 0x00);
	// clear outstanding interrupts
	io_expander_read_reg(MCP23017_INTCAPB_R);
}

// To Configure port expander interrupts
static void io_expander_gpio_irq_config(void){

	// configure gpio pin connected to io expander to generate
	// falling edge interrupts
	gpio_enable_port(IO_EXPANDER_IRQ_GPIO_BASE);
	gpio_config_digital_enable(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	gpio_config_enable_input(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	gpio_config_enable_pullup(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	gpio_config_falling_edge_irq(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	NVIC_SetPriority(gpio_get_irq_num(IO_EXPANDER_IRQ_GPIO_BASE), 1);
	NVIC_EnableIRQ(gpio_get_irq_num(IO_EXPANDER_IRQ_GPIO_BASE));
}


// write data to the register
void io_expander_write_reg(uint8_t reg, uint8_t data)
{
	io_expander_byte_write(IO_EXPANDER_I2C_BASE, reg, data);
}

// read data from the register
uint8_t io_expander_read_reg(uint8_t reg)
{
	uint8_t data;
	io_expander_byte_read(IO_EXPANDER_I2C_BASE, reg, &data);
	return data;
}

// to initialize io expander config
bool io_expander_init(void){
	
  // Configure I2C GPIO Pins
  if(gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false)
  {
    return false;
  }
  
  // to configure SCL 
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
  
  // to configure SDA 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  } 
  if(gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  } 
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN )== false)
  {
    return false;
  } 
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE,IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
  
  //  initialize the I2C peripheral
  if (initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
	
  // to configure the io expander and corresponding interrupts
	io_expander_config();
	io_expander_gpio_irq_config();

  return true;
}
