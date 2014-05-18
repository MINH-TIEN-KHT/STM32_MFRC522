
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "global.h"
#include "../Libraries/MFRC522/mfrc522.h"
#include "stdlib.h"

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
	
int main(void)
{
	uint8_t status=0;

	RCC_Configuration();
	GPIO_Configuration();
	USART_Configuration();
	delay_ms(1);
// 	printf("USART IS OK...\n");
 	SPI_Configuration();
	EXTI_Configuration();
	TIM_Configuration();
	NVIC_Configuration();
	delay_ms(5);

	InitMFRC522();
	delay_ms(5);
	
	Led(Bit_RESET); // Led OFF

	while (1)
	{
		if(msgReceiveComplete == 1)
		{
			ax12ReceivedMsgProcess();	
			
			insValue = (uint8_t)atoi((const char*)insValueStr);		
			msgReceiveComplete = 0;				
			if(insValue == OUTPUT_PULSE)
			{
				pppValue = atoi((const char*)pppValueStr);
				TIM_Cmd(TIM2, ENABLE);
			}
			else
				finish = 0;
		}
			
		while(!finish)
		{			
			status = PcdRequest(PICC_REQALL,SN);
			if(status != MI_OK) // error
			{
				continue;
			}				
			Led(Bit_SET); // Led ON			
			//  ANTICOLLISION
			status = PcdAnticoll(SN);			
			if(status != MI_OK)
			{   				
				continue;
			}
			Led(Bit_RESET); // Led OFF			
			
			//  SELECT CARD
			status = PcdSelect(SN);
			if(status != MI_OK)
			{							
				continue;
			}			
			//  AUTHENTICATE
			status = PcdAuthState(PICC_AUTHENT1A, 1, KEY, SN);
			if(status != MI_OK)
			{
				beep_Buzzer(5, 5, 10);
				continue;
			}								
			if(insValue == WRITE_DATA)
			{
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
					continue;
				}
				beep_Buzzer(5, 5, 1);
// 				printf("WRITE:%d %d %d %d %d\n",dataWrite[0],dataWrite[1],dataWrite[2],dataWrite[3],dataWrite[4]);
			}
			else if(insValue == READ_DATA)
			{
				//  READ DATA FROM RFID TAG
				status = PcdRead(1, dataRead);
				if(status != MI_OK)
				{
					continue;
				}
				printf("a%db%dc%dn%d#",dataRead[0],dataRead[1],dataRead[2],dataRead[4]*256 + dataRead[3]);	
				beep_Buzzer(5, 5, 2);
			}
			insValue = 0;
			finish=1;
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
