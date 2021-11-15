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



void Delay(uint16_t nCount);


#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
#define KEY_PORT  GPIOB
#define KEY_PINS  GPIO_Pin_1

#define LEDstate  GPIO_ReadOutputData(LED_PORT)

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
 void GPIO_setup(void)
{ 
#define LED_PORT  GPIOD
#define LED_PINS  GPIO_Pin_0
GPIO_Init(LED_PORT,LED_PINS,GPIO_Mode_Out_PP_Low_Slow);
GPIO_Init(GPIOC,GPIO_Pin_4,GPIO_Mode_Out_PP_Low_Slow);
GPIO_Init(GPIOD,GPIO_Pin_0,GPIO_Mode_Out_PP_Low_Slow);  
GPIO_Init(KEY_PORT,KEY_PINS,GPIO_Mode_In_PU_No_IT);

GPIO_Init(LED_PORT,LED_PINS,GPIO_Mode_Out_PP_Low_Slow);  
/* Configure GPIO for Relay Control */
GPIO_Init(KEY_PORT,KEY_PINS,GPIO_Mode_In_PU_No_IT);
//   GPIO_Init(GPIOB,GPIO_Pin_1,GPIO_Mode_In_PU_No_IT);
/* Relay2 and Relay1 */

GPIO_Init(GPIOC,GPIO_Pin_1,GPIO_Mode_Out_PP_Low_Slow);
/* Relay 3 */
GPIO_Init(GPIOC,GPIO_Pin_0,GPIO_Mode_Out_PP_High_Fast);
GPIO_Init(GPIOB,GPIO_Pin_6,GPIO_Mode_Out_PP_High_Fast);
} 

////////////////  MAIN CODE \\\\\\\\\\\\\\\\\\\\\\\\\

void main(void)
{
    GPIO_setup();
//
//  /* Wait Module SIM Start */
//  Delay(3000);

  while (1)
  {	
    GPIO_ToggleBits(GPIOD, GPIO_Pin_0);
    Delay(30000);

    GPIO_ToggleBits(GPIOD, GPIO_Pin_0);
    Delay(30000);

  }
}

//********************************************************************************/
  //    GPIO_ResetBits(GPIOD, GPIO_Pin_0);
//    Delay(20000);
//    GPIO_ResetBits(GPIOD, GPIO_Pin_0);
//    Delay(200);


//#ifdef  USE_FULL_ASSERT
//
///**
//  * @brief  Reports the name of the source file and the source line number
//  *   where the assert_param error has occurred.
//  * @param  file: pointer to the source file name
//  * @param  line: assert_param error line source number
//  * @retval None
//  */
//void assert_failed(uint8_t* file, uint32_t line)
//{ 
//  /* User can add his own implementation to report the file name and line number,
//     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
//
//  /* Infinite loop */
//  while (1)
//  {
//  }
//}
//#endif

/**
  * @}
  */