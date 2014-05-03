#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stdio.h>
#include "stm32f10x.h" 

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif /* __GNUC__ */

#define BUZZER_PIN      GPIO_Pin_0
#define BUZZER_PORT     GPIOA

#define LED_PIN      GPIO_Pin_1
#define LED_PORT     GPIOA

#define MFRC522_RST_PIN      GPIO_Pin_3
#define MFRC522_RST_PORT     GPIOA

#define SPI1_NSS_PIN  	GPIO_Pin_4
#define SPI1_NSS_PORT  	GPIOA

#define SPI1_SCK_PIN  	GPIO_Pin_5
#define SPI1_SCK_PORT  	GPIOA

#define SPI1_MISO_PIN  	GPIO_Pin_6
#define SPI1_MISO_PORT  GPIOA

#define SPI1_MOSI_PIN  	GPIO_Pin_7
#define SPI1_MOSI_PORT  GPIOA

#define SPI1_PORT  GPIOA

extern uint8_t USART_RX_BUF[64];     
extern uint8_t USART_RX_STA;         
void delay_us(uint32_t Nus);
void delay_ms(uint32_t Nus);
void NVIC_Configuration(void);
void RCC_Configuration(void);
void GPIO_Configuration(void);
void beep_Buzzer(uint8_t ton, uint8_t toff, uint8_t times);
void USART_Configuration(void);
void SPI_Configuration(void);
void EXTI_Configuration(void);
void Led(BitAction cmd);

#endif
