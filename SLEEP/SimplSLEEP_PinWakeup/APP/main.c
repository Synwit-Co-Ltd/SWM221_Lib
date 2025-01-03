#include "SWM221.h"

/* 注意：
 *	芯片的 ISP、SWD 引脚默认开启了上拉电阻，会增加休眠功耗，若想获得最低休眠功耗，休眠前请关闭所有引脚的上拉和下拉电阻
 */
 
extern void EnterSleepMode(void) ;
void gpio_outhigh(void); //GPIO无外设连接情况下,GPIO输出高降低功耗,应用中根据实际情况配置IO状态


int main(void)
{
	uint32_t i;
	for(i = 0; i < SystemCoreClock/2; i++)  __NOP();//防止调试过程中卡死
	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);						//接 LED，指示程序执行状态
	GPIO_SetBit(GPIOA, PIN5);								      //点亮LED

	GPIO_Init(GPIOA, PIN4, 0, 1, 0, 0);						//接按键，上拉使能
	SYS->PAWKEN |= (1 << PIN4);								    //开启PA4引脚低电平唤醒
	
	SYS->RCCR |= SYS_RCCR_LON_Msk;							  //休眠模式使用低频时钟 
	
	while(1==1)
	{
		GPIO_SetBit(GPIOA, PIN5);							//点亮LED
		for(int i = 0; i < SystemCoreClock/8; i++) __NOP();
		GPIO_ClrBit(GPIOA, PIN5);							//熄灭LED

		Flash_Param_at_xMHz(72);
		switchToHRC();//休眠前切换到内部RC时钟，进入低功耗模式之后自动切换到内部低频时钟
		
		SYS->XTALCR&= ~((1 << SYS_XTALCR_ON_Pos) | (1 << SYS_XTALCR_DET_Pos)  );//关闭外振,降低功耗
		
		SYS->PLLCR &= ~(1<< SYS_PLLCR_OUTEN_Pos) ;
		SYS->PLLCR |=  (1<< SYS_PLLCR_PWRDN_Pos) ;//关闭PLL,降低功耗
		
		SYS->CLKEN0 &=~ (1 << SYS_CLKEN0_ANAC_Pos); //关闭模拟模块时钟节省功耗--按需关闭
		gpio_outhigh(); //IO没有外设的情况下，将GPIO输出高功耗最低

		EnterSleepMode();//在SRAM中进入sleep,等待唤醒

		SYS->CLKEN0 |=(1 << SYS_CLKEN0_ANAC_Pos); 
		switchOnPLL(3,2,15);//唤醒后切换到PLL时钟
		switchToDIV(SYS_CLK_PLL, SYS_CLK_DIV_1);
		SystemCoreClockUpdate();
		Flash_Param_at_xMHz(CyclesPerUs);
	}
}


	
  void gpio_outhigh(void)
 {
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOA_Pos);
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOB_Pos);
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOC_Pos);

	 GPIO_Init(GPIOA ,0 ,1,0, 0, 0);//set gpio out modle
	 GPIO_Init(GPIOA ,1 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,2 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,3 ,1,0, 0, 0);
//	 GPIO_Init(GPIOA ,4 ,1,0, 0, 0);
//	 GPIO_Init(GPIOA ,5 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,6 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,7 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,8 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,9 ,1,0, 0, 0);
	 GPIO_Init(GPIOA ,10,1,0, 0, 0);
	 GPIO_Init(GPIOA ,11,1,0, 0, 0);
	 GPIO_Init(GPIOA ,12,1,0, 0, 0);
	 GPIO_Init(GPIOA ,13,1,0, 0, 0);
	 GPIO_Init(GPIOA ,14,1,0, 0, 0);
	 GPIO_Init(GPIOA ,15,1,0, 0, 0);
	 
	 GPIO_Init(GPIOB ,0 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,1 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,2 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,3 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,4 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,5 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,6 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,7 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,8 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,9 ,1,0, 0, 0);
	 GPIO_Init(GPIOB ,10,1,0, 0, 0);
	 GPIO_Init(GPIOB ,11,1,0, 0, 0);
	 GPIO_Init(GPIOB ,12,1,0, 0, 0);
	 GPIO_Init(GPIOB ,13,1,0, 0, 0);
	 GPIO_Init(GPIOB ,14,1,0, 0, 0);
	 GPIO_Init(GPIOB ,15,1,0, 0, 0);
	 
	 GPIO_Init(GPIOC ,0 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,1 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,2 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,3 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,4 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,5 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,6 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,7 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,8 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,9 ,1,0, 0, 0);
	 GPIO_Init(GPIOC ,10,1,0, 0, 0);
	 GPIO_Init(GPIOC ,11,1,0, 0, 0);
	 GPIO_Init(GPIOC ,12,1,0, 0, 0);
	 GPIO_Init(GPIOC ,13,1,0, 0, 0);
	 GPIO_Init(GPIOC ,14,1,0, 0, 0);
	 GPIO_Init(GPIOC ,15,1,0, 0, 0);
	 
	 GPIO_SetBits( GPIOA,0, 16); //set gpio 0-gpio15 out high
	 GPIO_SetBits( GPIOB,0, 16);
	 GPIO_SetBits( GPIOC,0, 16);
 }
	 
void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);	//GPIOA.0配置为UART0 RXD
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);	//GPIOA.1配置为UART0 TXD
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}


