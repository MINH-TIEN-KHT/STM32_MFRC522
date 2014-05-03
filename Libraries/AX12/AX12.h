#ifndef AX12_H
#define AX12_H
#include "stm32f10x.h"

#define RX_BUFFER_SIZE 32
#define TX_BUFFER_SIZE 8

#define DEVICE_ID   0x01
#define RFID_DATA_LENGTH 0x04
#define LENGTH      RFID_DATA_LENGTH + 2

#define PING        0x01
#define READ_DATA   0x02
#define WRITE_DATA  0x03

enum ERR_CODE
{
		OK,
		RxD_Timeout,
		Wrong_Header,
		Wrong_ID,
		Wrong_Length,
		Wrong_CheckSum,
};

void clearTxBuffer(void);
void clearRxBuffer(void);
unsigned int make16(unsigned char highByte,unsigned char lowByte);  
unsigned char highByte(unsigned int num);
unsigned char lowByte(unsigned int num);

void ax12ReceivedMsgProcess(uint8_t *msg);

#endif // AX12_H
