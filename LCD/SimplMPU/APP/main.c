#include "SWM221.h"

#include "ugui.h"
#include "NT35510.h"

UG_GUI gui;

void MPULCDInit(void);

int main(void)
{	
	SystemInit();
		
	MPULCDInit();
	
	NT35510_Init();
	
#if 0
	NT35510_Clear(C_RED);
#else
	NT35510_DMAClear(C_RED);
	while(!NT35510_Done()) __NOP();
#endif
	
	UG_Init(&gui,(void(*)(UG_S16,UG_S16,UG_COLOR))NT35510_DrawPoint, 480, 800);
	
	UG_DrawLine(0, 68, 480, 68, C_GREEN);
	UG_DrawLine(0, 136, 480, 136, C_GREEN);
	UG_DrawLine(0, 204, 480, 204, C_GREEN);
	
 	UG_FillFrame(120, 69, 360, 135, C_BLACK);
 	
  	UG_FontSelect(&FONT_12X20);
	UG_PutString(120, 80, "Hi from Synwit");
   	
	while(1==1)
	{
	}
}


void MPULCDInit(void)
{
	uint32_t i;
	MPU_InitStructure MPU_initStruct;
	
	GPIO_Init(GPIOB, PIN2, 1, 0, 0, 0);		//ÆÁÄ»±³¹â
	GPIO_SetBit(GPIOB, PIN2);
	GPIO_Init(GPIOB, PIN3, 1, 0, 0, 0);		//ÆÁÄ»¸´Î»
	GPIO_ClrBit(GPIOB, PIN3);
	for(i = 0; i < 1000000; i++) __NOP();
	GPIO_SetBit(GPIOB, PIN3);
	for(i = 0; i < 1000000; i++) __NOP();
	
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_MPU_D0,  1);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_MPU_D1,  1);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_MPU_D2, 1);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_MPU_D3, 1);
	PORT_Init(PORTA, PIN12, PORTA_PIN12_MPU_D4, 1);
	PORT_Init(PORTA, PIN13, PORTA_PIN13_MPU_D5, 1);
	PORT_Init(PORTA, PIN14, PORTA_PIN14_MPU_D6, 1);
	PORT_Init(PORTA, PIN15, PORTA_PIN15_MPU_D7, 1);
	PORT_Init(PORTA, PIN0,  PORTA_PIN0_MPU_D8,  1);
	PORT_Init(PORTA, PIN1,  PORTA_PIN1_MPU_D9,  1);
	PORT_Init(PORTA, PIN2,  PORTA_PIN2_MPU_D10, 1);
	PORT_Init(PORTA, PIN3,  PORTA_PIN3_MPU_D11, 1);
	PORT_Init(PORTA, PIN4,  PORTA_PIN4_MPU_D12, 1);
	PORT_Init(PORTA, PIN5,  PORTA_PIN5_MPU_D13, 1);
	PORT_Init(PORTA, PIN6,  PORTA_PIN6_MPU_D14, 1);
	PORT_Init(PORTA, PIN7,  PORTA_PIN7_MPU_D15, 1);
	
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_MPU_CS,  0);
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_MPU_RS,  0);
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_MPU_WR,  0);
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_MPU_RD,  0);
	
	MPU_initStruct.RDHoldTime = 2;
	MPU_initStruct.WRHoldTime = 2;
	MPU_initStruct.CSFall_WRFall = 1;
	MPU_initStruct.WRRise_CSRise = 1;
	MPU_initStruct.RDCSRise_Fall = 2;
	MPU_initStruct.WRCSRise_Fall = 2;
	MPU_Init(MPU, &MPU_initStruct);
}
