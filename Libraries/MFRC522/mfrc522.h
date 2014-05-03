#ifndef __MFRC522_H
#define __MFRC522_H

#include <stdio.h>
#include "stm32f10x.h" 

//MF522 command bits
#define PCD_IDLE              0x00               //NO action; cancel current commands
#define PCD_AUTHENT           0x0E               //verify password key
#define PCD_RECEIVE           0x08               //receive data
#define PCD_TRANSMIT          0x04               //send data
#define PCD_TRANSCEIVE        0x0C               //send and receive data
#define PCD_RESETPHASE        0x0F               //reset
#define PCD_CALCCRC           0x03               //CRC check and caculation

//Mifare_One card command bits
#define PICC_REQIDL           0x26               //Search the cards that not into sleep mode in the antenna area 
#define PICC_REQALL           0x52               //Search all the cards in the antenna area
#define PICC_ANTICOLL1         0x93               //prevent conflict
#define PICC_ANTICOLL2        0x95               
#define PICC_SElECTTAG        0x93               //select card
#define PICC_AUTHENT1A        0x60               //verify A password key
#define PICC_AUTHENT1B        0x61               //verify B password key
#define PICC_READ             0x30               //read 
#define PICC_WRITE            0xA0               //write
#define PICC_DECREMENT        0xC0               //deduct value
#define PICC_INCREMENT        0xC1               //charge up value
#define PICC_RESTORE          0xC2               //Restore data into buffer
#define PICC_TRANSFER         0xB0               //Save data into buffer
#define PICC_HALT             0x50               //sleep mode

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18

/////////////////////////////////////////////////////////////////////
//------------------MFRC522 register ---------------
/////////////////////////////////////////////////////////////////////
//Page 0:Command and Status
#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
//Page 1:Command     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
//Page 2:CFG    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
//Page 3:Test Register      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		  0x3F

/////////////////////////////////////////////////////////////////////
//The mistake code that return when communicate with MF522
/////////////////////////////////////////////////////////////////////
#define 	MI_OK                 0
#define 	MI_NOTAGERR           1
#define 	MI_ERR                2

#define	SHAQU1	0X01
#define	KUAI4	0X04
#define	KUAI7	0X07
#define	REGCARD	0xa1
#define	CONSUME	0xa2
#define READCARD	0xa3
#define ADDMONEY	0xa4

#define SPIReadByte()	SPI_WriteByte(0)
uint8_t SPI_WriteByte(uint8_t byte);

void chipSellect(void);
void chipDeSellect(void);

void hardReset(void);

void InitMFRC522(void);
void ClearBitMask(uint8_t   reg,uint8_t   mask);
void SetBitMask(uint8_t   reg,uint8_t   mask);
void WriteRawRC(uint8_t   Address, uint8_t   value);
char PcdComMFRC522(uint8_t   Command, 
                 uint8_t *pIn , 
                 uint8_t   InLenByte,
                 uint8_t *pOut , 
                 uint16_t  *pOutLenBit);
void CalulateCRC(uint8_t *pIn ,uint8_t   len,uint8_t *pOut );
uint8_t ReadRawRC(uint8_t   Address);
void PcdAntennaOn(void);

char PcdReset(void);
char PcdRequest(uint8_t req_code,uint8_t *pTagType);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
char M500PcdConfigISOType(uint8_t type);
char PcdAnticoll(uint8_t *pSnr);
char PcdSelect(uint8_t *pSnr);
char PcdAuthState(uint8_t auth_mode,uint8_t addr,uint8_t *pKey,uint8_t *pSnr);
char PcdWrite(uint8_t addr,uint8_t *pData);
char PcdRead(uint8_t addr,uint8_t *pData);
char PcdHalt(void);



void ShowID(uint16_t x,uint16_t y, uint8_t *p, uint16_t charColor, uint16_t bkColor);	 
void PutNum(uint16_t x,uint16_t y, uint32_t n1,uint8_t n0, uint16_t charColor, uint16_t bkColor);	
void Store(uint8_t *p,uint8_t store,uint8_t cash);
u8 ReadData(uint8_t   addr,uint8_t *pKey,uint8_t *pSnr,uint8_t *dataout);
u8 WriteData(uint8_t   addr,uint8_t *pKey,uint8_t *pSnr,uint8_t *datain);

#endif
