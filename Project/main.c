
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "global.h"
#include "../Libraries/MFRC522/mfrc522.h"
#include "stdlib.h"
#include "ds1307.h"

/* Private function prototypes -----------------------------------------------*/

extern uint8_t CT[];
extern uint8_t SN[];
extern uint8_t dataWrite[];
extern uint8_t dataRead[];
extern uint8_t KEY[];

extern uint8_t rx_buffer[];

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

uint8_t finish=1;	
uint8_t msgReceiveComplete=0;
uint8_t time_update=0;
uint8_t temp_hour=0;

DS1307Date date;
uint8_t i2c_buff_write[8];
uint8_t i2c_buff_read[8];
	
int main(void)
{
	uint8_t tagStatus=0, processStatus=0;
	uint16_t loopCount=0;
	uint8_t processCompleted=0;
	RCC_Configuration();
	delay_ms(1);
	GPIO_Configuration();
	delay_ms(1);
	USART_Configuration();
	delay_ms(1);
// 	printf("USART IS OK...\n");
 	SPI_Configuration();
	delay_ms(1);
	I2C_Configuration();
	delay_ms(1);
	EXTI_Configuration();
	delay_ms(1);
	TIM_Configuration();
	delay_ms(1);
	NVIC_Configuration();
	delay_ms(1);

	InitMFRC522();
	delay_ms(5);
	
	Led(Bit_RESET); // Led OFF
	
 	if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == RESET)  // set current time to RTC by pull PA.0 pin to 0, just do this one time when init system
	{
		i2c_buff_write[DS1307_SECONDS] = dec_2_bcd(0);  // 0 second in decimal format
		i2c_buff_write[DS1307_MINUTES] = dec_2_bcd(55);  // 2 minute in decimal format
		i2c_buff_write[DS1307_HOURS] = dec_2_bcd(23);   // 16 hour in decimal (24 hour mode)
		i2c_buff_write[DS1307_DAY] = dec_2_bcd(3);			// today is Wednesday (1: Monday, 2: Tuesday, 7: Sunday)
		i2c_buff_write[DS1307_DATE] = dec_2_bcd(21);    // today is 18th
		i2c_buff_write[DS1307_MONTH] = dec_2_bcd(5);    // august
		i2c_buff_write[DS1307_YEAR] = dec_2_bcd(14);    // this year is 2013
		i2c_buff_write[DS1307_CONTROL] = 0x10;  //enable SQWE 1Hz
		
		ds1307_writeBlockData(DS1307_ADDRESS, DS1307_SECONDS, i2c_buff_write, 8);
// 		printf("RTC setting finished...\n");		
 	}
	
	Led(Bit_SET); // Led OFF
	
	while (1)
	{
		loopCount++;
		if(msgReceiveComplete == 1)
		{
			ax12ReceivedMsgProcess();			
			insValue = (uint8_t)atoi((const char*)insValueStr);		
			msgReceiveComplete = 0;	
		}
		
		
// 			if(insValue == OUTPUT_PULSE)
// 			{
// 				pulseCount=0;
// 				TIM_SetCounter(TIM2, 0);
// 				pppValue = atoi((const char*)pppValueStr);
// 				TIM_Cmd(TIM2, ENABLE);
// 			}
		
		
		if(time_update == SET)
		{
			time_update = RESET;
			ds1307_readBlockData(DS1307_ADDRESS, DS1307_SECONDS, i2c_buff_read, 8);	
			date.second = bcd_2_dec(i2c_buff_read[DS1307_SECONDS]);
			date.minute = bcd_2_dec(i2c_buff_read[DS1307_MINUTES]);
			
			temp_hour = i2c_buff_read[DS1307_HOURS];
			date.day = bcd_2_dec(i2c_buff_read[DS1307_DAY]);
			date.date = bcd_2_dec(i2c_buff_read[DS1307_DATE]);
			date.month = bcd_2_dec(i2c_buff_read[DS1307_MONTH]);
			date.year = bcd_2_dec(i2c_buff_read[DS1307_YEAR]);
			
			date.control = i2c_buff_read[DS1307_CONTROL];
			
			//	process the hour
			if(temp_hour & 0x40)  // 0x01000000
			{
				//	if bit 6 of hour is high we need to find the AM/PM
				date.meridian = (temp_hour & 0x20)?MeridianPM:MeridianAM;  // 0x00100000
				//	mask the upper 3 bits
				date.hour = bcd_2_dec(temp_hour & 0x1F); // 0b00011111
			}
			else
			{
				date.meridian = MeridianMilitary;
				//	mask the upper 2 bits, b/c we need bit 5 to represent the full range of 
				//	the upper bcd digit (we can't represent 2n in bcd without bit 5)
				date.hour = bcd_2_dec(temp_hour & 0x3F);  // 0b00111111
			}	
			
// 			printf("\n");		
// 			
// 			switch(date.day)
// 			{
// 				case 1:
// 					printf("Monday, ");
// 				break;
// 				case 2:
// 					printf("Tuesday, ");
// 				break;
// 				case 3:
// 					printf("Wednesday, ");
// 				break;
// 				case 4:
// 					printf("Thursday, ");
// 				break;
// 				case 5:
// 					printf("Friday, ");
// 				break;
// 				case 6:
// 					printf("Saturday, ");
// 				break;
// 				case 7:
// 					printf("Sunday, ");
// 				break;
// 				default:
// 				break;
// 																																
// 			}		
// 			printf("%d-%d-%d\n", date.date, date.month, 2000 + date.year);
// 			printf("%d:%d:%d\n", date.hour, date.minute, date.second);
		}
		
		
		if(loopCount>=65000)
		{
			loopCount=0;
			if((insValue==WRITE_DATA) && (processCompleted==0))
			{
				processCompleted=1;
				processStatus = WriteRFIDProcess();
				if(processStatus == MI_OK) insValue = READ_DATA;
				processCompleted=0;
			}
			else if((insValue==READ_DATA) && (processCompleted==0))
			{
				processCompleted=1;
				processStatus = ReadRFIDProcess();
				if(processStatus == MI_OK) insValue = READ_DATA;
				processCompleted=0;
			}	
		}			
	}
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
