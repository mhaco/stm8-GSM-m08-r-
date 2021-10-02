// In the name of GOD \\
// MHA \\ 
// 1400-06-31 \\
//******************************************************************************/
#include "stm8l15x.h"//STM8L051/151
#include "string.h"
#include "stdio.h"  
#include "stdlib.h"

/* UART1 TX (PC5) */
/* UART1 RX (PC6) */

/* Global Define */
#define	  MS_TEST		"AT"
#define	  MS_DELETE_ALL_SMS	"AT+CMGD=1,4"
#define   MS_MODE_TEXT_SMS	"AT+CMGF=1"
#define UART_BUFFER_SIZE		500
#define SMS_BUFFER_SIZE			160


/* Global Variables */
//TextLcd_4Bit_HandleTypeDef mylcd;
uint8_t uart_receive_buffer[UART_BUFFER_SIZE];
uint8_t sms_buffer[SMS_BUFFER_SIZE];
uint8_t index=0,count_cr_lf=0,sms_start_index=0,sms_end_index=0,sms_text_index=0,prefix_sms_check=0;
volatile uint8_t new_cmd_from_user=0,state=0;
uint8_t temp=32;

/* Declaring Function Prototype */
void UART_Init(void);
void uart_isr(void);
void empty_buffer(uint8_t *buffer,uint16_t size);
void ms_send_cmd(uint8_t * cmd,uint8_t length);
void process_cmd_from_user(void);


void main(void)
{
	/* Configure Clock For STM8 using HSE */
	//CLK_CONFIG_16MHZ_HSE();

	/* Configure GPIO for Relay Control */

	/* Relay2 and Relay1 */
	GPIO_Init(GPIOC,GPIO_Pin_4,GPIO_Mode_Out_PP_High_Fast);
        GPIO_Init(GPIOC,GPIO_Pin_1,GPIO_Mode_Out_PP_High_Fast);
	/* Relay 3 */
	GPIO_Init(GPIOC,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Fast);



	/* Initialize Uart */
	UART_Init();

	/* Wait Module SIM Start */
	//Delay_ms(3000);

	/* Send Command To Module Sim */
	ms_send_cmd(MS_TEST,strlen((const char *)MS_TEST));
	Delay_ms(200);
	ms_send_cmd(MS_DELETE_ALL_SMS,strlen((const char*)MS_DELETE_ALL_SMS));
	Delay_ms(200);
	ms_send_cmd(MS_MODE_TEXT_SMS,strlen((const char *)MS_MODE_TEXT_SMS));
	Delay_ms(200);
	empty_buffer(uart_receive_buffer,UART_BUFFER_SIZE);

	while (1)
	{
		process_cmd_from_user();
	}
}


void uart_isr(void)
{
	
        if(USART_GetFlagStatus (USART1,USART_FLAG_RXNE))
	{
		uart_receive_buffer[index]=USART_ReceiveData8 (USART1);
		if(index>2)
		{
			/* Check Key Word Of SMS */
			if((uart_receive_buffer[index-3]=='+')&&(uart_receive_buffer[index-2]=='C')&&(uart_receive_buffer[index-1]=='M')&&(uart_receive_buffer[index]=='T'))
			{
				prefix_sms_check=1;
			}
			if(prefix_sms_check)
			{
				/* Check the end of respond from module sim */
				if((uart_receive_buffer[index]=='\n')&&(uart_receive_buffer[index-1]=='\r'))
				{
					count_cr_lf++;
					if(count_cr_lf==1)
					{
						sms_start_index=index+1;
					}
					if(count_cr_lf==2)
					{
						count_cr_lf=0;
						sms_end_index=index-1;

						/* Copy new message to sms_buffer */
						for(uint8_t i=sms_start_index;i<sms_end_index;i++)
						{
							sms_buffer[sms_text_index]=uart_receive_buffer[i];
							sms_text_index++;
						}

						/* Check User Password To Control Device */
						if((sms_buffer[0]=='s')&&(sms_buffer[1]=='u')&&(sms_buffer[2]=='b')&&(sms_buffer[3]=='z')&&(sms_buffer[4]=='e')&&(sms_buffer[5]=='r')&&(sms_buffer[6]=='o')&&(sms_buffer[7]=='s'))
						{
							new_cmd_from_user=1;
						}
						sms_start_index=0;
						sms_end_index=0;
						sms_text_index=0;
						prefix_sms_check=0;
						empty_buffer(uart_receive_buffer,UART_BUFFER_SIZE);
						return;
					}
				}
			}
			else if(index>300)
			{
				empty_buffer(uart_receive_buffer,UART_BUFFER_SIZE);
			}
		}
		index++;
	}
}

void ms_send_cmd(uint8_t * cmd,uint8_t length)
{
	for(uint8_t i=0;i<length;i++)
	{
            while(IS_USART_FLAG (USART_FLAG_TC));
            USART_SendData8(USART1,*(cmd+i));
        //USART1_SendData8(*(cmd+i));
	}

	/* Send CR (Carriage Return) */
	while(IS_USART_FLAG(USART_FLAG_TC));
	USART1_SendData8(0x0D);
}

void empty_buffer(uint8_t *buffer,uint16_t size)
{
	for(uint16_t i=0;i<size;i++)
	{
		*(buffer+i)=0;
	}

	index=0;
}



void UART_Init(void)
{ 
  
   /* Initialize uart1 mode 8bit data,
   * 1bit stop, no parity, buad rate=115200
   */ 
    CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//??USART??
    USART_Init(USART1,115200,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//??USART??9600,8N1,??/??
    USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);/* Enable uart1 receive complete interrupt */
    /* Enable uart1 module */
    USART_Cmd (USART1,ENABLE);
    enableInterrupts();

}