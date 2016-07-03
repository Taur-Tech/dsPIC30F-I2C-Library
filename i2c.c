#include "i2c.h"

void I2C_Init(void)
{

    I2CCONbits.I2CEN = 0;	
	I2CCONbits.I2CSIDL 	= 0;
	I2CCONbits.IPMIEN 	= 0;
	I2CCONbits.A10M		= 0;
	I2CCONbits.DISSLW 	= 1;
	I2CCONbits.SMEN 	= 0;
    I2CBRG              = setBaudRate();    
    __delay_ms(1);
    I2CCONbits.I2CEN 	= 1;
}

int setBaudRate(){
    return (FCY/I2C_BAUDRATE - FCY/1111111) - 1;
}

int I2C_WriteReg(char dev_addr, char reg_addr, char value)
{
    char wr_dev_addr = dev_addr << 1;
    // Send I2C start condition
	I2CCONbits.SEN = 1;			
	while(I2CCONbits.SEN == 1);
	// Send I2C device address on the bus for write operation
	I2CTRN = wr_dev_addr;			
	while(I2CSTATbits.TRSTAT);			
	if (I2CSTATbits.ACKSTAT)				
	{								
		I2CCONbits.PEN = 1;
		while(I2CCONbits.PEN);			
		return I2C_ERROR;					
	}
    // Send register address on the bus
	I2CTRN = reg_addr;
	while(I2CSTATbits.TRSTAT);
	if (I2CSTATbits.ACKSTAT)
	{
		I2CCONbits.PEN = 1;
		while(I2CCONbits.PEN);
		return I2C_ERROR;
	}
	// Send register value on the bus    
	I2CTRN = value;
	while(I2CSTATbits.TRSTAT);
	if (I2CSTATbits.ACKSTAT)
	{
		I2CCONbits.PEN = 1;
		while(I2CCONbits.PEN);
		return I2C_ERROR;
	}
	/// Send I2C stop condition
	I2CCONbits.PEN = 1;
	while(I2CCONbits.PEN);
	return I2C_OK;
}

int I2C_ReadReg(char dev_addr, char reg_addr, char *value)
{
    char wr_dev_addr = dev_addr << 1;
    char rd_dev_addr = (dev_addr << 1) | 0x01;
    // Send I2C start condition
	I2CCONbits.SEN = 1;	
	while(I2CCONbits.SEN == 1);
	// Send I2C device address on the bus for write operation
	I2CTRN = wr_dev_addr;
	while(I2CSTATbits.TRSTAT);
	if (I2CSTATbits.ACKSTAT)
	{
		I2CCONbits.PEN = 1;
		while(I2CCONbits.PEN);
		return I2C_ERROR;
	}
    // Send I2C register address on the bus 
	I2CTRN = reg_addr;
	while(I2CSTATbits.TRSTAT);
	if (I2CSTATbits.ACKSTAT)
	{	
		I2CCONbits.PEN = 1;
		while(I2CCONbits.PEN);
		return I2C_ERROR;
	}
    // Send I2C restart condition
    I2CCONbits.RSEN = 1;
    while(I2CCONbits.RSEN == 1);	
    // Send I2C device address on the bus for read operation
    I2CTRN = rd_dev_addr;
	while(I2CSTATbits.TRSTAT);
	if (I2CSTATbits.ACKSTAT)
	{
		I2CCONbits.PEN = 1;
		while(I2CCONbits.PEN);
		return I2C_ERROR;	
	}
    // Enable I2C clock for read operation
	I2CCONbits.RCEN = 1;
    while(!I2CSTATbits.RBF);
    // Retrieve value from I2C register
	*value = I2CRCV;	
	// Send I2C stop condition
	I2CCONbits.PEN = 1;
	while(I2CCONbits.PEN);
	return I2C_OK;
}