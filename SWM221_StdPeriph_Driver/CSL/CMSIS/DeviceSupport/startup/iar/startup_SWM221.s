;******************************************************************************************************************************************
; 文件名称:    startup_SWM221.s
; 功能说明:    SWM221单片机的启动文件
; 技术支持:    http://www.synwit.com.cn/e/tool/gbook/?bid=1
; 注意事项:
; 版本日期: V1.0.0        2016年1月30日
; 升级记录:
;
;
;******************************************************************************************************************************************
; @attention
;
; THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
; REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
; FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
; OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
; -ECTION WITH THEIR PRODUCTS.
;
; COPYRIGHT 2012 Synwit Technology
;******************************************************************************************************************************************

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler              ; Reset Handler
        DCD     NMI_Handler                ; NMI Handler
        DCD     HardFault_Handler          ; Hard Fault Handler
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     SVC_Handler                ; SVCall Handler
        DCD     0                          ; Reserved
        DCD     0                          ; Reserved
        DCD     PendSV_Handler             ; PendSV Handler
        DCD     SysTick_Handler            ; SysTick Handler

        ; External Interrupts
        DCD    UART0_Handler
        DCD    TIMR0_Handler
        DCD    CAN0_Handler
        DCD    UART1_Handler
        DCD    PWM1_Handler
        DCD    TIMR1_Handler
        DCD    HALL_Handler
        DCD    PWM0_Handler
        DCD    QSPI0_Handler
        DCD    PWMBRK_Handler
        DCD    USART0_Handler
        DCD    WDT_Handler
        DCD    I2C0_Handler
        DCD    XTALSTOP_Handler
        DCD    ADC_Handler
        DCD    ACMP_Handler
        DCD    BTIMR0_Handler
        DCD    BTIMR1_Handler
        DCD    BTIMR2_Handler
        DCD    BTIMR3_Handler
        DCD    GPIOA_Handler
        DCD    GPIOB_Handler
        DCD    GPIOC_Handler
        DCD    GPIOA0_GPIOC0_Handler
        DCD    GPIOA1_GPIOC1_Handler
        DCD    GPIOA2_GPIOC2_MPU_Handler
        DCD    GPIOA3_GPIOC3_PVD_Handler
        DCD    GPIOB0_GPIOA8_TIMR2_Handler
        DCD    GPIOB1_GPIOA9_DMA_Handler
        DCD    GPIOB2_GPIOA10_DIV_Handler
        DCD    GPIOB3_GPIOA11_SPI0_Handler
        DCD    GPIOB4_GPIOB10_QEI_Handler
        

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =__iar_program_start
        BX      R0
        
        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        PUBWEAK UART0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART0_Handler
        B UART0_Handler

        PUBWEAK TIMR0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMR0_Handler
        B TIMR0_Handler

        PUBWEAK CAN0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN0_Handler
        B CAN0_Handler

        PUBWEAK UART1_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART1_Handler
        B UART1_Handler

        PUBWEAK PWM1_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PWM1_Handler
        B PWM1_Handler

        PUBWEAK TIMR1_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMR1_Handler
        B TIMR1_Handler

        PUBWEAK HALL_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HALL_Handler
        B HALL_Handler

        PUBWEAK PWM0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PWM0_Handler
        B PWM0_Handler

        PUBWEAK QSPI0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
QSPI0_Handler
        B QSPI0_Handler

        PUBWEAK PWMBRK_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PWMBRK_Handler
        B PWMBRK_Handler

        PUBWEAK USART0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART0_Handler
        B USART0_Handler

        PUBWEAK WDT_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
WDT_Handler
        B WDT_Handler

        PUBWEAK I2C0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C0_Handler
        B I2C0_Handler

        PUBWEAK XTALSTOP_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
XTALSTOP_Handler
        B XTALSTOP_Handler

        PUBWEAK ADC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_Handler
        B ADC_Handler

        PUBWEAK ACMP_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
ACMP_Handler
        B ACMP_Handler

        PUBWEAK BTIMR0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BTIMR0_Handler
        B BTIMR0_Handler

        PUBWEAK BTIMR1_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BTIMR1_Handler
        B BTIMR1_Handler

        PUBWEAK BTIMR2_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BTIMR2_Handler
        B BTIMR2_Handler

        PUBWEAK BTIMR3_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BTIMR3_Handler
        B BTIMR3_Handler

        PUBWEAK GPIOA_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA_Handler
        B GPIOA_Handler

        PUBWEAK GPIOB_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB_Handler
        B GPIOB_Handler

        PUBWEAK GPIOC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOC_Handler
        B GPIOC_Handler

        PUBWEAK GPIOA0_GPIOC0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA0_GPIOC0_Handler
        B GPIOA0_GPIOC0_Handler

        PUBWEAK GPIOA1_GPIOC1_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA1_GPIOC1_Handler
        B GPIOA1_GPIOC1_Handler

        PUBWEAK GPIOA2_GPIOC2_MPU_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA2_GPIOC2_MPU_Handler
        B GPIOA2_GPIOC2_MPU_Handler

        PUBWEAK GPIOA3_GPIOC3_PVD_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOA3_GPIOC3_PVD_Handler
        B GPIOA3_GPIOC3_PVD_Handler

        PUBWEAK GPIOB0_GPIOA8_TIMR2_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB0_GPIOA8_TIMR2_Handler
        B GPIOB0_GPIOA8_TIMR2_Handler

        PUBWEAK GPIOB1_GPIOA9_DMA_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB1_GPIOA9_DMA_Handler
        B GPIOB1_GPIOA9_DMA_Handler

        PUBWEAK GPIOB2_GPIOA10_DIV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB2_GPIOA10_DIV_Handler
        B GPIOB2_GPIOA10_DIV_Handler

        PUBWEAK GPIOB3_GPIOA11_SPI0_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB3_GPIOA11_SPI0_Handler
        B GPIOB3_GPIOA11_SPI0_Handler

        PUBWEAK GPIOB4_GPIOB10_QEI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIOB4_GPIOB10_QEI_Handler
        B GPIOB4_GPIOB10_QEI_Handler

        END
