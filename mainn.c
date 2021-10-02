// In the name of GOD \\
// MHA\\ 
// 1400-06-31 \\
//******************************************************************************/
#include "stm8l15x.h"//STM8L051/151
#include "string.h"
#include "stdio.h"  
#include "stdlib.h"
#include "textlcd.h"


/* UART1 TX (PC5) */
/* UART1 RX (PC6) */

/* Global Define */
#define	  MS_TEST		"AT"
#define	  MS_DELETE_ALL_SMS	"AT+CMGD=1,4"
#define   MS_MODE_TEXT_SMS	"AT+CMGF=1"

#define UART_BUFFER_SIZE		500
#define SMS_BUFFER_SIZE			160


/* Global Variables */
TextLcd_4Bit_HandleTypeDef mylcd;
uint8_t uart_receive_buffer[UART_BUFFER_SIZE];
uint8_t sms_buffer[SMS_BUFFER_SIZE];
uint8_t index=0,count_cr_lf=0,sms_start_index=0,sms_end_index=0,sms_text_index=0,prefix_sms_check=0;
volatile uint8_t new_cmd_from_user=0,state=0;
uint8_t temp=32;

/* Declaring Function Prototype */
void uart_init(void);
void uart_isr(void);
void empty_buffer(uint8_t *buffer,uint16_t size);
void ms_send_cmd(uint8_t * cmd,uint8_t length);
void process_cmd_from_user(void);



char str_tmp[50];
char buffer='+'; 

char E[]={10},C[]={13},G[]={34},S[]={26};
//#define Enter     USART_SendData8(USART1,(unsigned char)*E);  // USART_SendData8(USART1,)
#define Enter     USART_SendData8(USART1,(char)*E);   //UART1_SendData8(0x0D); 
#define CR        USART_SendData8(USART1,(char)*C);
#define GIM       USART_SendData8(USART1,(char)*G); 
#define SUB       USART_SendData8(USART1,(char)*S); 



#define user_number   09361521577



#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
#define KEY_PORT  GPIOB
#define KEY_PINS  GPIO_Pin_1

#define LEDstate  GPIO_ReadOutputData(LED_PORT)




//void uart1_puts(char *str);
 
void CMGF_1(void);
char get_char(void);
void wait_to_get(char ch);
void del_All_SMS(void);
void cmti(void) ;
void cmgr(void);
void Send_SMS(void);
void Delay(uint16_t nCount);
void USART1_SendStr(char *Str) ;
    
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

void USART1_SendStr(char *Str) 
{
  while(*Str!=0)//????
  {
    USART_SendData8(USART1,*Str);     //???? 
    while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));//??????
    Str++;//?????
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
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
  
  USART_SendData8(USART1,0x0D); 
  //Enter
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  //CR	
      
}


/*******************************************************************************
********************************************************************************/


char get_char(void)
{
//  char cha[1] ;
//  *cha = USART_ReceiveData8 (USART1);//(&huart1,(unsigned char *)cha,1,200000);
//  return cha[0];
  
  char cha ;
  cha = USART_ReceiveData8 (USART1);//(&huart1,(unsigned char *)cha,1,200000);
  return cha;
  
  //char cha[1];
  //HAL_UART_Receive(&huart1,(unsigned char *)cha,1,200000);
  //return cha[0];
  
  
}


/*******************************************************************************

********************************************************************************/

void wait_to_get(char ch) {
  char Rch[1]="\0";
  while(ch != Rch[0]){
  Rch[0] = USART_ReceiveData8 (USART1);//(&huart1,(unsigned char *)Rch,1,200000);
  }
}

/*******************************************************************************

********************************************************************************/

void del_All_SMS(void)
{
  strcpy(str_tmp,"AT+CMGDA=");
  Delay(500);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
  str_tmp[0]='\0';
  Delay(50);       
  strcpy(str_tmp,"DEL ALL");
  Delay(50);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
  GIM    //AT+CMGDA="del all"
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
   
  USART_SendData8(USART1,0x0D); 
  
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  Delay(500);       
  str_tmp[0]='\0';
}

/*******************************************************************************

********************************************************************************/

void cmti(void) {
  wait_to_get(','); 
  USART1_SendStr("AT+CMGR=1");
  
  ////////////   inja mitooni mahalle zakhireh shodane SMS ra bekhooni Va Zakhireh Koni ***************
  
  USART_SendData8(USART1,0x0D); 
 
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
}

/*******************************************************************************

********************************************************************************/

void Send_SMS(void)
{
  CMGF_1(); 
  Delay(500);       
  str_tmp[0]='\0';
       
  Delay(0xFFFF);
  
  strcpy(str_tmp,"AT+CMGS=");
  Delay(0xFFFF);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  str_tmp[0]='\0';
  Delay(500);       
  
 //strcpy(str_tmp,"+989108349709");
  strcpy(str_tmp,"+989136885240");
 //strcpy(str_tmp,"09137162925");
 //strcpy(str_tmp,"+989920126140");
  //strcpy(str_tmp,"+989361521577");
  strcpy(str_tmp,"+989136885241");
  
  
  Delay(500);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  
  USART_SendData8(USART1,0x0D);
  
  
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
       
  Delay(500);   
 
 
  
//  if(LEDstate==1)// send State
  //{
   // USART1_SendStr("LED is on");
     USART1_SendStr("SALAM LED STATE");
  //}  
 // else
  //{
  //  USART1_SendStr("LED is off");
 // }  
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
  
   
  del_All_SMS();
}

/*******************************************************************************

********************************************************************************/


/*******************************************************************************

********************************************************************************/


void Send_SMS_Str(unsigned char *SmsStr)
{
  CMGF_1(); 
  Delay(500);       
  str_tmp[0]='\0';
  Delay(100);       
  
  strcpy(str_tmp,"AT+CMGS=");
  Delay(500);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  str_tmp[0]='\0';
  Delay(100);       
  
  //strcpy(str_tmp,"+989108349709");
  strcpy(str_tmp,"+989136885240");
  //strcpy(str_tmp,"09137162925");
  //strcpy(str_tmp,"+989920126140");
  //strcpy(str_tmp,"+989361521577");
 // strcpy(str_tmp,"+989136885241");
  
  
  Delay(50);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  
  USART_SendData8(USART1,0x0D);
  
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));        
  Delay(500);   
  
  
   while(*SmsStr!=0){
     
     USART_SendData8(USART1,*SmsStr);  
     while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
     SmsStr++; 
  }
  
  USART_SendData8(USART1,0x0D);
  
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  str_tmp[0]='\0';
  Delay(500);       
  SUB
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  Delay(5000);
   
  del_All_SMS();
}

/*******************************************************************************

********************************************************************************/
void Send_SMS_char(char SmsStr)
{
  CMGF_1(); 
  Delay(500);       
  str_tmp[0]='\0';
  Delay(100);       
  
  strcpy(str_tmp,"AT+CMGS=");
  Delay(500);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  str_tmp[0]='\0';
  Delay(100);       
  
  //strcpy(str_tmp,"+989108349709");
  strcpy(str_tmp,"+989136885240");
  //strcpy(str_tmp,"09137162925");
  //strcpy(str_tmp,"+989920126140");
  //strcpy(str_tmp,"+989361521577");
 // strcpy(str_tmp,"+989136885241");
  
  
  Delay(50);       
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  GIM
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  
  USART_SendData8(USART1,0x0D);
  
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));        
  Delay(500);   
  
  
    
     
   USART_SendData8(USART1,SmsStr);  
   while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE)); 
    
     
  
  USART_SendData8(USART1,0x0D);
  
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));
  str_tmp[0]='\0';
  Delay(500);       
  SUB
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  Delay(5000);
   
  del_All_SMS();
}

/*******************************************************************************

********************************************************************************/



void cmgr(void) {
  
  unsigned char x;
  int code;
  char number[4]={0};
  char number1[13]={0};
  char message[2]={0};
  //char test[4]={0};
  
  wait_to_get(',');
  get_char();
  for(x=0;x<13;x++)
    number1[x]=get_char(); 
  
 // wait_to_get(E[0]);
  wait_to_get(0x0D);
  
  
  for(x=0;x<2;x++)
    message[x]=get_char(); 
  
  
    number[0]=number1[9];
    number[1]=number1[10];
    number[2]=number1[11];
    number[3]=number1[12];
  
  
    code=atoi(number);
  
  
  //if(code==user_number) { 
    
    if(strcmp(message,"on")==0)
    {   
      GPIO_SetBits(LED_PORT,LED_PINS);
      del_All_SMS();
    }
    else if(strcmp(message,"of")==0)
    {
      GPIO_ResetBits(LED_PORT,LED_PINS);
      del_All_SMS();
    }
    else if(strcmp(message,"st")==0)
    {
      Send_SMS();
    }	
  //}
}

/*******************************************************************************

********************************************************************************/



/*******************************************************************************

********************************************************************************/


int main(void)
{
  uint8_t i; 
  GPIO_Init(LED_PORT,LED_PINS,GPIO_Mode_Out_PP_Low_Slow); 
  GPIO_Init(KEY_PORT,KEY_PINS,GPIO_Mode_In_PU_No_IT); 
  
  //SYSCFG_REMAPPinConfig(REMAP_Pin_USART1TxRxPortA,ENABLE);//?????,??????USART1?PA2-TX?PA3-RX;????USART1?TX-PC5?RX-PC6;????USART??????PC5?PC6
  
  CLK_PeripheralClockConfig (CLK_Peripheral_USART1,ENABLE);//??USART??
  USART_Init(USART1,9600,USART_WordLength_8b,USART_StopBits_1,USART_Parity_No,USART_Mode_Tx|USART_Mode_Rx);//??USART??9600,8N1,??/??
  USART_ITConfig (USART1,USART_IT_RXNE,ENABLE);//??????
  USART_Cmd (USART1,ENABLE);//??USART
  enableInterrupts();
 
  
  CMGF_1();
  USART_ClearFlag(USART1, USART_FLAG_RXNE);
  //del_All_SMS();
  //  //Send_SMS();
  //Send_SMS_Str("SALAM LED ON-OFF prog");
  
  /*
  str_tmp[0]='\0';
  Delay(100);       
  
  strcpy(str_tmp,"AT+CNMI=2,1,0,0,0");
  
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  */
  
  str_tmp[0]='\0';
  Delay(100);   
  strcpy(str_tmp,"AT+CNMI=2,2");
  //strcpy(str_tmp,"AT+CNMI=2,2,0,0,0");
  
  USART1_SendStr(str_tmp);
  while(!USART_GetFlagStatus (USART1,USART_FLAG_TXE));   
  
  for(i=0;i<5;i++)
    {
     GPIO_SetBits(LED_PORT, LED_PINS); 
     Delay(0xCFFF);
     GPIO_ResetBits(LED_PORT, LED_PINS); 
     Delay(0xCFFF);
     }
  
   GPIO_SetBits(LED_PORT, LED_PINS); 
   
  
  while (1)
  {
/*   if(USART_GetFlagStatus (USART1,USART_FLAG_RXNE))
    { 
      for(i=0;i<3;i++)
     {
      GPIO_SetBits(LED_PORT, LED_PINS); 
      Delay(0x9FFF);
      GPIO_ResetBits(LED_PORT, LED_PINS); 
      Delay(0x9FFF);
      }
    }
     while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
   {
   for(i=0;i<8;i++)
     {
      GPIO_SetBits(LED_PORT, LED_PINS); 
      Delay(0x9FFF);
      GPIO_ResetBits(LED_PORT, LED_PINS); 
      Delay(0x9FFF);
     } 
   }
     
 */

  // buffer=get_char();
    
    
 //  while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
 while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
   buffer=USART_ReceiveData8 (USART1);
   Send_SMS_char(buffer);
   USART_ClearFlag(USART1, USART_FLAG_RXNE);
   if(buffer!='+') { 
     
     for(i=0;i<8;i++)
     {
      GPIO_SetBits(LED_PORT, LED_PINS); 
      Delay(0x9FFF);
      GPIO_ResetBits(LED_PORT, LED_PINS); 
      Delay(0x9FFF);
      } 
     
      //buffer=get_char(); 
      buffer=USART_ReceiveData8 (USART1);
    //  while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
      if(buffer=='C') { 
	 //buffer=get_char();
         buffer=USART_ReceiveData8 (USART1);
     //    while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
	 if(buffer=='M') { 
	    //buffer=get_char();
            buffer=USART_ReceiveData8 (USART1);
         //   while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
	    if(buffer=='T') {			
	       //buffer=get_char();
               buffer=USART_ReceiveData8 (USART1);
            //   while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
	       if(buffer=='I') {	
		  //buffer=get_char();
                  buffer=USART_ReceiveData8 (USART1);
              //    while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
		  if(buffer==':')
                  { 
                  for(i=0;i<3;i++)
                  {
                  GPIO_SetBits(LED_PORT, LED_PINS); 
                  Delay(0xCFFF);
                  GPIO_ResetBits(LED_PORT, LED_PINS); 
                  Delay(0xCFFF);
                  } 
                  
		  cmti();
                  }
		}
	     } 
	   else if(buffer=='G') { 
		//buffer=get_char();
                buffer=USART_ReceiveData8 (USART1);
                while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
		if(buffer=='R') {
		   //buffer=get_char();
                   buffer=USART_ReceiveData8 (USART1);
                   while(!USART_GetFlagStatus (USART1,USART_FLAG_RXNE));
		   if(buffer==':')
                   {  
                   for(i=0;i<6;i++)
                    {
                    GPIO_SetBits(LED_PORT, LED_PINS); 
                    Delay(0xCFFF);
                    GPIO_ResetBits(LED_PORT, LED_PINS); 
                    Delay(0xCFFF);
                    }  
  
		   cmgr();
                   }
		   }
           }
	 }
       }
    }
  //}
 
 }
  
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
