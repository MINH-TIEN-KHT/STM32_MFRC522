#include "stm32f10x.h"
#include "ds1307.h"

//
// Converts raw data from BCD to decimal
//
uint8_t bcd_2_dec(uint8_t bcd)
{
    uint8_t dec = (bcd / 16)*10 + (bcd %16);
    return dec;
}

//
//  Converts raw data from decimal to BCD
//
uint8_t dec_2_bcd(uint8_t dec)
{
	uint8_t bcd = (dec / 10)*16 + (dec %10);
  return bcd;
}

/* set address register to be process */
uint8_t ds1307_writeSingleData(uint8_t deviceID, uint8_t RegAddr, uint8_t data)
{
	/*re-enable ACK bit incase it was disabled last call*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	/* Enable the I2C peripheral */
/*======================================================*/
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
  /* Send device address for write */  
	I2C_Send7bitAddress(I2C1, deviceID, I2C_Direction_Transmitter);
	/* Test on master Flag */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/* Send the device's internal address to write to */
	I2C_SendData(I2C1,RegAddr);
	/*wait for byte send to complete*/
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/* Send I2C1 data */
	I2C_SendData(I2C1, data);
	/*wait for byte send to complete*/ 
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
	return 1;
}

uint8_t ds1307_writeBlockData(uint8_t deviceID, uint8_t RegAddr, uint8_t *data, uint8_t len)
{
	uint8_t i=0;
	/*re-enable ACK bit incase it was disabled last call*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	
	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	
	/* Enable the I2C peripheral */
/*======================================================*/
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
  /* Send device address for write */  
	I2C_Send7bitAddress(I2C1, deviceID, I2C_Direction_Transmitter);
	/* Test on master Flag */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/* Send the device's internal address to write to */
	I2C_SendData(I2C1,RegAddr);
	/*wait for byte send to complete*/
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	/* send data buffer */
	for(i=0; i<len; i++)
	{
		/* Send I2C1 data */
		I2C_SendData(I2C1, data[i]);
		/*wait for byte send to complete*/ 
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	}
	
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
	return 1;
}

uint8_t ds1307_readSingleData(uint8_t deviceID, uint8_t RegAddr)
{
	uint8_t data=0;
	
	/*re-enable ACK bit incase it was disabled last call*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	/* Enable the I2C peripheral */
/*======================================================*/
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
  /* Send device address for write */  
	I2C_Send7bitAddress(I2C1, deviceID, I2C_Direction_Transmitter);
	/* Test on master Flag */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/* Send the device's internal address to write to */
	I2C_SendData(I2C1,RegAddr);
	/* Test on TXE FLag (data sent) */
	while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
/*=====================================================*/
	/* Send START condition a second time (Re-Start) */
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
	/* Send address for read */
	I2C_Send7bitAddress(I2C1, deviceID, I2C_Direction_Receiver);
	/* Test Receive mode Flag */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	/* load in all 6 registers */

	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	data = I2C_ReceiveData(I2C1);

	/*enable NACK bit */
	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	
	/* Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)); // stop bit flag
	
	return data;
}

uint8_t ds1307_readBlockData(uint8_t deviceID, uint8_t RegAddr, uint8_t *data, uint8_t len)
{
	uint8_t i=0;
	
	/*re-enable ACK bit incase it was disabled last call*/
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	/* Test on BUSY Flag */
	while (I2C_GetFlagStatus(I2C1,I2C_FLAG_BUSY));
	/* Enable the I2C peripheral */
/*======================================================*/
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
  /* Send device address for write */  
	I2C_Send7bitAddress(I2C1, deviceID, I2C_Direction_Transmitter);
	/* Test on master Flag */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	/* Send the device's internal address to write to */
	I2C_SendData(I2C1,RegAddr);
	/* Test on TXE FLag (data sent) */
	while (!I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE));
/*=====================================================*/
	/* Send START condition a second time (Re-Start) */
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on start flag */
	while (!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB));
	/* Send address for read */
	I2C_Send7bitAddress(I2C1, deviceID, I2C_Direction_Receiver);
	/* Test Receive mode Flag */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	/* load in all 6 registers */
	for(i=0; i<len; i++)
	{
		while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
		data[i] = I2C_ReceiveData(I2C1);
	}

	/*enable NACK bit */
	I2C_NACKPositionConfig(I2C1, I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(I2C1, DISABLE);
	
	/* Send STOP Condition */
	I2C_GenerateSTOP(I2C1, ENABLE);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF)); // stop bit flag
	
	return 1;
}

