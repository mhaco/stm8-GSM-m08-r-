// In the name of GOD \\
// MHA \\ 
// 1400-08-23 \\ Receive SMS and Save number
//******************************************************************************/
// MHA \\ 
// 1400-08-16 \\ Send SMS
//******************************************************************************/
#include "stm8l15x.h"//STM8L051/151
#include "string.h"
#include "stdio.h"  
#include "stdlib.h"

/* UART1 TX (PC5) */
/* UART1 RX (PC6) */

/* Global Define */
#define	  TEST		        "AT"
#define	  DELETE_ALL_SMS	"AT+CMGD=1,4"
#define   TEXT_MODE	        "AT+CMGF=1"
#define   Read_command          "AT+CNMI=2,1,0,1,0"
#define   READ_TEXT_SMS	        "AT+CMGR=1"

#define UART_BUFFER_SIZE		500
#define SMS_BUFFER_SIZE			160
//#define CALL       ATD+989361521577;
//#define TE_Character AT+CSCS="GSM"
int Num;

/* Global Variables */

char Message[5]={0};
char Number[13]={0};
char str_num[50];
char str_msg[50];
char str_tmp[50];


uint8_t uart_receive_buffer[UART_BUFFER_SIZE];
uint8_t sms_buffer[SMS_BUFFER_SIZE];
uint8_t value[100];

uint8_t index=0,count_cr_lf=0,sms_start_index=0,sms_end_index=0,sms_text_index=0,prefix_sms_check=0;
volatile uint8_t new_cmd_from_user=0,state=0;
uint8_t temp=32;

char E[]={10},C[]={13},G[]={34},S[]={26};
//#define Enter     USART_SendData8(USART1,(unsigned char)*E);  // USART_SendData8(USART1,)
#define Enter     USART_SendData8(USART1,(char)*E);   //UART1_SendData8(0x0D); 
#define CR        USART_SendData8(USART1,(char)*C);
#define GIM       USART_SendData8(USART1,(char)*G); 
#define SUB       USART_SendData8(USART1,(char)*S);
//
#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
#define KEY_PORT  GPIOB
#define KEY_PINS  GPIO_Pin_2
#define LEDstate  GPIO_ReadOutputData(LED_PORT)
/* Declaring Function Prototype */
void UART_Init(void);
void uart_isr(void);
void empty_buffer(uint8_t *buffer,uint16_t size);
void send_cmd(uint8_t *cmd,uint8_t length);

void Delay(uint16_t nCount);

void CMGF_1(void);
char get_char(void);
void wait_to_get(char ch);
void del_All_SMS(void);
void cmti_1(void) ;
void cmgr_1(void);
void cscs(void);

void Send_SMS(void);
void SendSMS(char str_num,char str_msg);

 void GPIO_setup(void)
{   
    
    GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Slow);
/* Configure GPIO for Relay Control */
    GPIO_Init(GPIOB,GPIO_Pin_1,GPIO_Mode_In_PU_No_IT);
    
    GPIO_Init(GPIOB,GPIO_Pin_2,GPIO_Mode_In_PU_No_IT);
    /* Relay2 and Relay1 */
    GPIO_Init(GPIOC,GPIO_Pin_1,GPIO_Mode_Out_PP_Low_Slow);
    /* Relay 3 */
    GPIO_Init(GPIOC,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOC,GPIO_Pin_4,GPIO_Mode_Out_PP_Low_Slow);
    GPIO_Init(GPIOB,GPIO_Pin_6,GPIO_Mode_Out_PP_Low_Slow);
} 

 void USART1_SendStr(char *Str) 
{
  while(*Str!=0)//????
  {
    USART_SendData8(USART1,*Str);     // 
    //while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//
    Str++;//
  }
}
 
void cmti_1(void) {
  //wait_to_get(','); 
  USART1_SendStr("AT+CMGR=1");
  Enter
  CR
}

void cmgr_1(void){
  
//        int Num;
        unsigned char x;
       wait_to_get(',');
        //get_char();
        for(x=0;x<13;x++) 
          Number[x]=get_char(); 
        
        wait_to_get(0x0D);     
        
        for(x=0;x<4;x++)
          Message[x]=get_char();  
        
        //Num=atoi(Number);
}
 void cscs(void){

  strcpy(str_tmp,"AT+CSCS");
  Delay(500);
  USART1_SendStr(str_tmp);
  GIM
  str_tmp[0]='\0';
  Delay(500);       
  strcpy(str_tmp,"GSM");
  Delay(500);       
  USART1_SendStr(str_tmp);
  GIM    
  Enter
  CR
  Delay(500);       
  str_tmp[0]='\0';
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
  
   /* Initialize uart1 mode 8bit data, 1bit stop, no parity, buad rate=115200*/ 
    CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//??USART??
    USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);
    USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);/* Enable uart1 receive complete interrupt */
    /* Enable uart1 module */
    USART_Cmd (USART1,ENABLE);
    enableInterrupts();
}
//
void Delay(__IO uint16_t nCount)
{
 for(uint8_t i=0;i<100;i++)
    {
     while (nCount != 0)
          {
           nCount--;
          }
    }
}
/*******************************************************************************

********************************************************************************/
void CMGF_1(void)
{
  str_tmp[0]='\0';
  Delay(100);   
  //str_tmp = "AT+CMGF=1";
  strcpy(str_tmp,"AT+CMGF=1");
  Delay(500);       
  USART1_SendStr(str_tmp);
  //while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
  
  USART_SendData8(USART1,0x0D); 
  //Enter
//  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  CR
    
      
  strcpy(str_tmp,"AT+CNMI=1,2,0,2,0");
  Delay(500);
  USART1_SendStr(str_tmp);
  
}
///*******************************************************************************
char get_char(void)
{

  char cha[1]="\0";
  cha[0] = USART_ReceiveData8 (USART1);//(&huart1,(unsigned char *)cha,1,200000);
  return cha[0];
}
//********************************************************************************/
void wait_to_get(char ch) {
  char Rch[1];//="\0";
  while(ch != Rch[0]){
    Rch[0] = USART_ReceiveData8 (USART1);//(&huart1,(unsigned char *)Rch,1,200000);
  }
}
void del_All_SMS(void)
{
  strcpy(str_tmp,"AT+QMGDA=");
  Delay(500);
  //USART1_SendStr(str_tmp);
  USART_SendData8(USART1,*str_tmp);
  GIM
  str_tmp[0]='\0';
  Delay(50);       
  strcpy(str_tmp,"DEL ALL");
  Delay(50);
  USART_SendData8(USART1,*str_tmp);
//  USART1_SendStr(str_tmp);
  GIM    //AT+CMGDA="del all"
   
  USART_SendData8(USART1,0x0D); 
  //Enter
  CR 
  Delay(500);       
  str_tmp[0]='\0';
  
}

    
void SendSMS(char num,char msg)
  {
    CMGF_1(); 
    Delay(500);       
    str_tmp[1]='\0';
    Delay(0xFFFF);//69904       
    strcpy(str_tmp,"AT+CMGS=");
    Delay(500);
    USART1_SendStr(str_tmp);
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
    GIM
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    str_tmp[0]='\0';
    Delay(500);

    strcpy(str_tmp,str_num);
    Delay(500);       
    USART1_SendStr(str_tmp);

    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    GIM
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    USART_SendData8(USART1,0x0D);
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    Delay(500);   

    USART1_SendStr(str_msg);
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));  
    USART_SendData8(USART1,0x0D);
    str_tmp[0]='\0';
    Delay(500); 
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    SUB
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    USART_SendData8(USART1,0x0D);
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
    Delay(500);
      
    //del_All_SMS();
  
}

////////////////  MAIN CODE \\\\\\\\\\\\\\\\\\\\\\\\\

void main(void)
{

    GPIO_setup();
    UART_Init();
    del_All_SMS();

//    GPIO_SetBits(GPIOC, GPIO_Pin_4);
      

  while (1)
  {	


    GPIO_ToggleBits(GPIOC, GPIO_Pin_0);
    // KEY = 1 //
    if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_2)==0)//  Save & Receive Phone Number
    {
        GPIO_ToggleBits(GPIOC, GPIO_Pin_0);
        Delay(0xffff);

        GPIO_ToggleBits(GPIOC, GPIO_Pin_0);
        Delay(0xffff);
        CMGF_1();
        cmti_1();
        cmgr_1();
        //cscs();
        //            char USERA[13]={0};
        if(Message == "NEW1")
        {
          GPIO_SetBits(GPIOC, GPIO_Pin_1);
          Delay(0xffff);
          strcpy(str_num,Number);
          strcpy(str_msg,"USER10 is saved...");

        //      Delay(0xffff);
        //      strcpy(str_num,"+989361521577");
        //      strcpy(str_msg,"USER20 is saved...");
          GPIO_ToggleBits(GPIOC, GPIO_Pin_0);
          Delay(0xffff);

          GPIO_ToggleBits(GPIOC, GPIO_Pin_0);
          Delay(0xffff); 
          SendSMS(*str_num, *str_msg);
     }            
    }
  }
}
//********************************************************************************/
 
#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
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


