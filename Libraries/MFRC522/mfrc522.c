#include <string.h>
#include "global.h"
#include "mfrc522.h"

uint8_t CT[2];
uint8_t SN[4];
uint8_t dataWrite[5];
uint8_t dataRead[5];
uint8_t KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};

extern uint8_t insValue;
extern uint8_t aaValue;
extern uint8_t bbValue;
extern uint8_t ccValue;
extern uint8_t nnnValueHigh;
extern uint8_t nnnValueLow;
extern uint16_t pppValue;
extern uint16_t pulseCount;

extern uint8_t insValueStr[];
extern uint8_t aaValueStr[];
extern uint8_t bbValueStr[];
extern uint8_t ccValueStr[];
extern uint8_t nnnValueStr[];
extern uint8_t pppValueStr[];

void delay_ns(u32 ns)
{
  u32 i;
  for(i=0;i<ns;i++)
  {
    __nop();
    __nop();
    __nop();
  }
}

void chipSellect(void)
{
	delay_us(10);
	GPIO_ResetBits(SPI1_NSS_PORT, SPI1_NSS_PIN);
	delay_us(10);
}

void chipDeSellect(void)
{
	delay_us(10);
	GPIO_SetBits(SPI1_NSS_PORT, SPI1_NSS_PIN);
	delay_us(10);
}

void hardReset(void)
{
      GPIO_SetBits(MFRC522_RST_PORT, MFRC522_RST_PIN);  // NRSTPD = high
      delay_ms(2);
      GPIO_ResetBits(MFRC522_RST_PORT, MFRC522_RST_PIN);  // NRSTPD = low
      delay_ms(2);
      GPIO_SetBits(MFRC522_RST_PORT, MFRC522_RST_PIN);  // NRSTPD = high
      delay_ms(2);
}

uint8_t SPI_WriteByte(uint8_t byte)
{
    // Wait until it's 1, so we can write in
    while ((SPI1->SR & SPI_SR_TXE) == 0){}
    SPI1->DR = byte;
    // wait until it's 1, so we can read out
    while ((SPI1->SR & SPI_SR_RXNE) == 0){}
    return SPI1->DR;         	    		
}

void InitMFRC522(void)
{
  PcdReset();
  PcdAntennaOff();  
  PcdAntennaOn();
  M500PcdConfigISOType('A');
}
                         
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
char PcdRequest(uint8_t   req_code,uint8_t *pTagType)
{
	char   status;  
	uint16_t   unLen;
	uint8_t   ucComMF522Buf[MAXRLEN]; 

	ClearBitMask(Status2Reg,0x08);
	WriteRawRC(BitFramingReg,0x07);
	SetBitMask(TxControlReg,0x03);
 
	ucComMF522Buf[0] = req_code;

	status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

	if ((status == MI_OK) && (unLen == 0x10))
	{    
		*pTagType     = ucComMF522Buf[0];
		*(pTagType+1) = ucComMF522Buf[1];
	}
	else
	{   status = MI_ERR;   }
   
	return status;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////  
char PcdAnticoll(uint8_t *pSnr)
{
    char   status;
    uint8_t   i,snr_check=0;
    uint16_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);
 
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
    	 for (i=0; i<4; i++)
         {   
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])
         {   status = MI_ERR;    }
    }
    
    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
char PcdSelect(uint8_t *pSnr)
{
    char   status;
    uint8_t   i;
    uint16_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);
  
    ClearBitMask(Status2Reg,0x08);

    status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);
    
    if ((status == MI_OK) && (unLen == 0x18))
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////               
char PcdAuthState(uint8_t   auth_mode,uint8_t   addr,uint8_t *pKey,uint8_t *pSnr)
{
    char   status;
    uint16_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+2] = *(pKey+i);   }
//    for (i=0; i<6; i++)
//    {    ucComMF522Buf[i+8] = *(pSnr+i);   }
    memcpy(&ucComMF522Buf[2], pKey, 6); 
    memcpy(&ucComMF522Buf[8], pSnr, 4); 
    
    status = PcdComMFRC522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////// 
char PcdRead(uint8_t   addr,uint8_t *p )
{
    char   status;
    uint16_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
   
    status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
 //   {   memcpy(p , ucComMF522Buf, 16);   }
    {
        for (i=0; i<16; i++)
        {    *(p +i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }
    
    return status;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////                  
char PcdWrite(uint8_t   addr,uint8_t *p )
{
    char   status;
    uint16_t   unLen;
    uint8_t   i,ucComMF522Buf[MAXRLEN]; 
    
    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        //memcpy(ucComMF522Buf, p , 16);
        for (i=0; i<16; i++)
        {    
        	ucComMF522Buf[i] = *(p +i);   
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    
    return status;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    uint8_t   status;
    uint16_t   unLen;
    uint8_t   ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMFRC522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return MI_OK;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void CalulateCRC(uint8_t *pIn ,uint8_t   len,uint8_t *pOut )
{
    uint8_t   i,n;
	
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIn +i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do 
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
	hardReset();

	delay_ns(10);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	WriteRawRC(CommandReg,PCD_RESETPHASE);
	delay_ns(10);

	WriteRawRC(ModeReg,0x3D);            
	WriteRawRC(TReloadRegL,30);           
	WriteRawRC(TReloadRegH,0);
	WriteRawRC(TModeReg,0x8D);
	WriteRawRC(TPrescalerReg,0x3E);

	WriteRawRC(TxAutoReg,0x40);

	return MI_OK;
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(uint8_t   type)
{
   if (type == 'A')                     //ISO14443_A
   { 
       ClearBitMask(Status2Reg,0x08);
       WriteRawRC(ModeReg,0x3D);//3F
       WriteRawRC(RxSelReg,0x86);//84
       WriteRawRC(RFCfgReg,0x7F);   //4F
   	   WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
	   WriteRawRC(TReloadRegH,0);
       WriteRawRC(TModeReg,0x8D);
	   WriteRawRC(TPrescalerReg,0x3E);
	   delay_ns(1000);
       PcdAntennaOn();
   }
   else{ return 1; }
   
   return MI_OK;
}
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
uint8_t ReadRawRC(uint8_t   Address)
{
	uint8_t   ucAddr;
	uint8_t   ucResult=0;
	chipSellect();
	
	ucAddr = ((Address<<1)&0x7E)|0x80;

	SPI_WriteByte(ucAddr);
	ucResult=SPIReadByte();
	
	chipDeSellect();
	
	return ucResult;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void WriteRawRC(uint8_t   Address, uint8_t   value)
{  
  uint8_t   ucAddr;
	chipSellect();
	
  ucAddr = ((Address<<1)&0x7E);
	SPI_WriteByte(ucAddr);
	SPI_WriteByte(value);
	
	chipDeSellect();
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void SetBitMask(uint8_t   reg,uint8_t   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void ClearBitMask(uint8_t   reg,uint8_t   mask)  
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
} 

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
char PcdComMFRC522(uint8_t   Command, 
                 uint8_t *pIn , 
                 uint8_t   InLenByte,
                 uint8_t *pOut , 
                 uint16_t *pOutLenBit)
{
    char   status = MI_ERR;
    uint8_t   irqEn   = 0x00;
    uint8_t   waitFor = 0x00;
    uint8_t   lastBits;
    uint8_t   n;
    uint32_t   i;
    switch (Command)
    {
        case PCD_AUTHENT:
			irqEn   = 0x12;
			waitFor = 0x10;
			break;
		case PCD_TRANSCEIVE:
			irqEn   = 0x77;
			waitFor = 0x30;
			break;
		default:
			break;
    }
   
    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);	
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);	 	
    
    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pIn [i]);    }
    WriteRawRC(CommandReg, Command);	  
    
    if (Command == PCD_TRANSCEIVE)
    {    SetBitMask(BitFramingReg,0x80);  }	 
    										 
		i = 1000;
    do 
    {
			delay_us(100);
			n = ReadRawRC(ComIrqReg);
			i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {    
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {   status = MI_NOTAGERR;   }
            if (Command == PCD_TRANSCEIVE)
            {
               	n = ReadRawRC(FIFOLevelReg);
              	lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOut [i] = ReadRawRC(FIFODataReg);    }
            }
        }
        else
        {   status = MI_ERR;   }
        
    }
   

    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE); 
    return status;
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    uint8_t   i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
	ClearBitMask(TxControlReg, 0x03);
}

uint8_t WriteRFIDProcess(void)
{
	uint8_t status=0;
	uint8_t finish=0;	
// 	while(!finish)
// 	{			
		status = PcdRequest(PICC_REQALL,SN);
		if(status != MI_OK) // error
		{
// 			beep_Buzzer(5, 5, 20);
// 			continue;
		}							
		//  ANTICOLLISION
		status = PcdAnticoll(SN);			
		if(status != MI_OK)
		{   		
// 			beep_Buzzer(5, 5, 20);			
// 			continue;
		}			
		//  SELECT CARD
		status = PcdSelect(SN);
		if(status != MI_OK)
		{	
// 			beep_Buzzer(5, 5, 20);			
// 			continue;
		}			
		//  AUTHENTICATE
		status = PcdAuthState(PICC_AUTHENT1A, 1, KEY, SN);
		if(status != MI_OK)
		{
// 			beep_Buzzer(5, 5, 20);
// 			continue;
		}								
		aaValue = (uint8_t)atoi((const char*)aaValueStr);
		bbValue = (uint8_t)atoi((const char*)bbValueStr);
		ccValue = (uint8_t)atoi((const char*)ccValueStr);
		nnnValueLow = (uint8_t)atoi((const char*)nnnValueStr);
		nnnValueHigh = (uint8_t)(atoi((const char*)nnnValueStr)>>8);
		
		dataWrite[0] = aaValue;
		dataWrite[1] = bbValue;
		dataWrite[2] = ccValue;
		dataWrite[3] = nnnValueLow;
		dataWrite[4] = nnnValueHigh;
						
		//  WRITE DATA TO RFID TAG
		status = PcdWrite(1, dataWrite);
		if(status != MI_OK)
		{
// 			beep_Buzzer(5, 5, 20);
// 			continue;
		}	
		else
		{
			beep_Buzzer(5, 5, 1);

		}
// 		finish=1;
		return status;
// 	}
}

uint8_t ReadRFIDProcess(void)
{
	uint8_t status=0;
	uint8_t finish=0;	
// 	while(!finish)
// 	{			
		status = PcdRequest(PICC_REQALL,SN);
		if(status != MI_OK) // error
		{
// 			beep_Buzzer(5, 5, 20);
// 			continue;
		}							
		//  ANTICOLLISION
		status = PcdAnticoll(SN);			
		if(status != MI_OK)
		{  
// 			beep_Buzzer(5, 5, 20);			
// 			continue;
		}			
		//  SELECT CARD
		status = PcdSelect(SN);
		if(status != MI_OK)
		{		
// 			beep_Buzzer(5, 5, 20);			
// 			continue;
		}			
		//  AUTHENTICATE
		status = PcdAuthState(PICC_AUTHENT1A, 1, KEY, SN);
		if(status != MI_OK)
		{
// 			beep_Buzzer(5, 5, 20);
// 			continue;
		}								
		//  READ DATA FROM RFID TAG
		status = PcdRead(1, dataRead);
		if(status != MI_OK)
		{
// 			beep_Buzzer(5, 5, 20);
// 			continue;
		}
		else
		{
			printf("a%db%dc%dn%d#",dataRead[0],dataRead[1],dataRead[2],dataRead[4]*256 + dataRead[3]);	
			beep_Buzzer(5, 5, 2);
		}
// 		finish=1;		
		return status;
// 	}
}

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////                 
/*char PcdValue(uint8_t dd_mode,uint8_t addr,uint8_t *pValue)
{
    char status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 
    //uint8_t i;
	
    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
        
    if (status == MI_OK)
    {
        memcpy(ucComMF522Buf, pValue, 4);
        //for (i=0; i<16; i++)
        //{    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]); 
   
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}*/

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
/*char PcdBakValue(uint8_t sourceaddr, uint8_t goaladdr)
{
    char status;
    uint8_t  unLen;
    uint8_t ucComMF522Buf[MAXRLEN]; 

    ucComMF522Buf[0] = PICC_RESTORE;
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }
    
    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
 
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
		if (status != MI_ERR)
        {    status = MI_OK;    }
    }
    
    if (status != MI_OK)
    {    return MI_ERR;   }
    
    ucComMF522Buf[0] = PICC_TRANSFER;
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);
 
    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}*/
