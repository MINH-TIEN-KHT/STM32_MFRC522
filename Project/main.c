
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "global.h"
#include "../Libraries/MFRC522/mfrc522.h"
#include "../Libraries/AX12/AX12.h"

/* Private function prototypes -----------------------------------------------*/

extern uint8_t CT[2];
extern uint8_t SN[4];
extern  uint8_t dataWrite[5];
extern  uint8_t dataRead[5];
extern uint8_t KEY[6];
extern uint8_t instruction;
extern uint8_t msgReceiveComplete;

extern uint8_t rx_buffer[];
extern uint8_t finish;	
	
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
	NVIC_Configuration();
	delay_ms(5);

	InitMFRC522();
	delay_ms(5);
	
	Led(Bit_RESET); // Led OFF

	while (1)
	{
		if(msgReceiveComplete == 1)
		{
			ax12ReceivedMsgProcess(rx_buffer);
			msgReceiveComplete = 0;	
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
// 			printf("Card Detected \n");
			
			//  ANTICOLLISION
			status = PcdAnticoll(SN);			
			if(status != MI_OK)
			{   				
				continue;
			}
// 			printf("SN:%02x %02x %02x %02x\n",SN[0],SN[1],SN[2],SN[3]);	
			Led(Bit_RESET); // Led OFF			
			
			//  SELECT CARD
			status = PcdSelect(SN);
			if(status != MI_OK)
			{			
// 				printf("Card Sellect error. ERROR %d\n", status);
				
				continue;
			}
// 			printf("Card Sellect Ok. \n");
			
			//  AUTHENTICATE
			status = PcdAuthState(PICC_AUTHENT1A, 1, KEY, SN);
			if(status != MI_OK)
			{
				beep_Buzzer(10, 10, 10);
				continue;
			}
// 			printf("Authenticate Ok.\n");
								
			if(instruction == WRITE_DATA)
			{
				//  WRITE DATA TO RFID TAG
				status = PcdWrite(1, dataWrite);
				if(status != MI_OK)
				{
					continue;
				}
				beep_Buzzer(10, 10, 2);
// 				printf("WRITE:%02x %02x %02x %02x %02x\n",dataWrite[0],dataWrite[1],dataWrite[2],dataWrite[3],dataWrite[4]);
			}
			else if(instruction == READ_DATA)
			{
				//  READ DATA FROM RFID TAG
				status = PcdRead(1, dataRead);
				if(status != MI_OK)
				{
					continue;
				}
				printf("READ:%02x %02x %02x %02x %02x\n",dataRead[0],dataRead[1],dataRead[2],dataRead[3],dataRead[4]);	
				beep_Buzzer(10, 10, 3);
			}
			instruction = 0;
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
