#include "SWM221.h"

#include "NT35510.h"


#define NT35510_CMD_COUSOR_X	0x2A00
#define NT35510_CMD_COUSOR_Y	0x2B00
#define NT35510_CMD_WR_DATA		0x2C00

#define MPU_VPIX  800
#define MPU_HPIX  480

/****************************************************************************************************************************************** 
* 函数名称:	NT35510_Init()
* 功能说明: TFT液晶屏初始化，TFT使用NT35510驱动，分辨率480*800，使用正点原子4.3寸屏测试
*			屏幕模式：竖屏，从左到右、从上到下
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void NT35510_Init(void)
{
	uint32_t i, id;
	
	id = MPU_ReadReg(MPU, 0XDA00) << 16;
	id |= MPU_ReadReg(MPU, 0XDB00) << 8;
	id |= MPU_ReadReg(MPU, 0XDC00);
	
	if(id != 0x008000) return;
	
	MPU_WriteReg(MPU, 0xF000, 0x55);
	MPU_WriteReg(MPU, 0xF001, 0xAA);
	MPU_WriteReg(MPU, 0xF002, 0x52);
	MPU_WriteReg(MPU, 0xF003, 0x08);
	MPU_WriteReg(MPU, 0xF004, 0x01);
			
	//AVDD Set AVDD 5.2V
	MPU_WriteReg(MPU, 0xB000, 0x0D);
	MPU_WriteReg(MPU, 0xB001, 0x0D);
	MPU_WriteReg(MPU, 0xB002, 0x0D);
	//AVDD ratio
	MPU_WriteReg(MPU, 0xB600, 0x34);
	MPU_WriteReg(MPU, 0xB601, 0x34);
	MPU_WriteReg(MPU, 0xB602, 0x34);
	//AVEE -5.2V
	MPU_WriteReg(MPU, 0xB100, 0x0D);
	MPU_WriteReg(MPU, 0xB101, 0x0D);
	MPU_WriteReg(MPU, 0xB102, 0x0D);
	//AVEE ratio
	MPU_WriteReg(MPU, 0xB700, 0x34);
	MPU_WriteReg(MPU, 0xB701, 0x34);
	MPU_WriteReg(MPU, 0xB702, 0x34);
	//VCL -2.5V
	MPU_WriteReg(MPU, 0xB200, 0x00);
	MPU_WriteReg(MPU, 0xB201, 0x00);
	MPU_WriteReg(MPU, 0xB202, 0x00);
	//VCL ratio
	MPU_WriteReg(MPU, 0xB800, 0x24);
	MPU_WriteReg(MPU, 0xB801, 0x24);
	MPU_WriteReg(MPU, 0xB802, 0x24);
	//VGH 15V (Free pump)
	MPU_WriteReg(MPU, 0xBF00, 0x01);
	MPU_WriteReg(MPU, 0xB300, 0x0F);
	MPU_WriteReg(MPU, 0xB301, 0x0F);
	MPU_WriteReg(MPU, 0xB302, 0x0F);
	//VGH ratio
	MPU_WriteReg(MPU, 0xB900, 0x34);
	MPU_WriteReg(MPU, 0xB901, 0x34);
	MPU_WriteReg(MPU, 0xB902, 0x34);
	//VGL_REG -10V
	MPU_WriteReg(MPU, 0xB500, 0x08);
	MPU_WriteReg(MPU, 0xB501, 0x08);
	MPU_WriteReg(MPU, 0xB502, 0x08);
	MPU_WriteReg(MPU, 0xC200, 0x03);
	//VGLX ratio
	MPU_WriteReg(MPU, 0xBA00, 0x24);
	MPU_WriteReg(MPU, 0xBA01, 0x24);
	MPU_WriteReg(MPU, 0xBA02, 0x24);
	//VGMP/VGSP 4.5V/0V
	MPU_WriteReg(MPU, 0xBC00, 0x00);
	MPU_WriteReg(MPU, 0xBC01, 0x78);
	MPU_WriteReg(MPU, 0xBC02, 0x00);
	//VGMN/VGSN -4.5V/0V
	MPU_WriteReg(MPU, 0xBD00, 0x00);
	MPU_WriteReg(MPU, 0xBD01, 0x78);
	MPU_WriteReg(MPU, 0xBD02, 0x00);
	//VCOM
	MPU_WriteReg(MPU, 0xBE00, 0x00);
	MPU_WriteReg(MPU, 0xBE01, 0x64);
	//Gamma Setting
	MPU_WriteReg(MPU, 0xD100, 0x00);
	MPU_WriteReg(MPU, 0xD101, 0x33);
	MPU_WriteReg(MPU, 0xD102, 0x00);
	MPU_WriteReg(MPU, 0xD103, 0x34);
	MPU_WriteReg(MPU, 0xD104, 0x00);
	MPU_WriteReg(MPU, 0xD105, 0x3A);
	MPU_WriteReg(MPU, 0xD106, 0x00);
	MPU_WriteReg(MPU, 0xD107, 0x4A);
	MPU_WriteReg(MPU, 0xD108, 0x00);
	MPU_WriteReg(MPU, 0xD109, 0x5C);
	MPU_WriteReg(MPU, 0xD10A, 0x00);
	MPU_WriteReg(MPU, 0xD10B, 0x81);
	MPU_WriteReg(MPU, 0xD10C, 0x00);
	MPU_WriteReg(MPU, 0xD10D, 0xA6);
	MPU_WriteReg(MPU, 0xD10E, 0x00);
	MPU_WriteReg(MPU, 0xD10F, 0xE5);
	MPU_WriteReg(MPU, 0xD110, 0x01);
	MPU_WriteReg(MPU, 0xD111, 0x13);
	MPU_WriteReg(MPU, 0xD112, 0x01);
	MPU_WriteReg(MPU, 0xD113, 0x54);
	MPU_WriteReg(MPU, 0xD114, 0x01);
	MPU_WriteReg(MPU, 0xD115, 0x82);
	MPU_WriteReg(MPU, 0xD116, 0x01);
	MPU_WriteReg(MPU, 0xD117, 0xCA);
	MPU_WriteReg(MPU, 0xD118, 0x02);
	MPU_WriteReg(MPU, 0xD119, 0x00);
	MPU_WriteReg(MPU, 0xD11A, 0x02);
	MPU_WriteReg(MPU, 0xD11B, 0x01);
	MPU_WriteReg(MPU, 0xD11C, 0x02);
	MPU_WriteReg(MPU, 0xD11D, 0x34);
	MPU_WriteReg(MPU, 0xD11E, 0x02);
	MPU_WriteReg(MPU, 0xD11F, 0x67);
	MPU_WriteReg(MPU, 0xD120, 0x02);
	MPU_WriteReg(MPU, 0xD121, 0x84);
	MPU_WriteReg(MPU, 0xD122, 0x02);
	MPU_WriteReg(MPU, 0xD123, 0xA4);
	MPU_WriteReg(MPU, 0xD124, 0x02);
	MPU_WriteReg(MPU, 0xD125, 0xB7);
	MPU_WriteReg(MPU, 0xD126, 0x02);
	MPU_WriteReg(MPU, 0xD127, 0xCF);
	MPU_WriteReg(MPU, 0xD128, 0x02);
	MPU_WriteReg(MPU, 0xD129, 0xDE);
	MPU_WriteReg(MPU, 0xD12A, 0x02);
	MPU_WriteReg(MPU, 0xD12B, 0xF2);
	MPU_WriteReg(MPU, 0xD12C, 0x02);
	MPU_WriteReg(MPU, 0xD12D, 0xFE);
	MPU_WriteReg(MPU, 0xD12E, 0x03);
	MPU_WriteReg(MPU, 0xD12F, 0x10);
	MPU_WriteReg(MPU, 0xD130, 0x03);
	MPU_WriteReg(MPU, 0xD131, 0x33);
	MPU_WriteReg(MPU, 0xD132, 0x03);
	MPU_WriteReg(MPU, 0xD133, 0x6D);
	MPU_WriteReg(MPU, 0xD200, 0x00);
	MPU_WriteReg(MPU, 0xD201, 0x33);
	MPU_WriteReg(MPU, 0xD202, 0x00);
	MPU_WriteReg(MPU, 0xD203, 0x34);
	MPU_WriteReg(MPU, 0xD204, 0x00);
	MPU_WriteReg(MPU, 0xD205, 0x3A);
	MPU_WriteReg(MPU, 0xD206, 0x00);
	MPU_WriteReg(MPU, 0xD207, 0x4A);
	MPU_WriteReg(MPU, 0xD208, 0x00);
	MPU_WriteReg(MPU, 0xD209, 0x5C);
	MPU_WriteReg(MPU, 0xD20A, 0x00);

	MPU_WriteReg(MPU, 0xD20B, 0x81);
	MPU_WriteReg(MPU, 0xD20C, 0x00);
	MPU_WriteReg(MPU, 0xD20D, 0xA6);
	MPU_WriteReg(MPU, 0xD20E, 0x00);
	MPU_WriteReg(MPU, 0xD20F, 0xE5);
	MPU_WriteReg(MPU, 0xD210, 0x01);
	MPU_WriteReg(MPU, 0xD211, 0x13);
	MPU_WriteReg(MPU, 0xD212, 0x01);
	MPU_WriteReg(MPU, 0xD213, 0x54);
	MPU_WriteReg(MPU, 0xD214, 0x01);
	MPU_WriteReg(MPU, 0xD215, 0x82);
	MPU_WriteReg(MPU, 0xD216, 0x01);
	MPU_WriteReg(MPU, 0xD217, 0xCA);
	MPU_WriteReg(MPU, 0xD218, 0x02);
	MPU_WriteReg(MPU, 0xD219, 0x00);
	MPU_WriteReg(MPU, 0xD21A, 0x02);
	MPU_WriteReg(MPU, 0xD21B, 0x01);
	MPU_WriteReg(MPU, 0xD21C, 0x02);
	MPU_WriteReg(MPU, 0xD21D, 0x34);
	MPU_WriteReg(MPU, 0xD21E, 0x02);
	MPU_WriteReg(MPU, 0xD21F, 0x67);
	MPU_WriteReg(MPU, 0xD220, 0x02);
	MPU_WriteReg(MPU, 0xD221, 0x84);
	MPU_WriteReg(MPU, 0xD222, 0x02);
	MPU_WriteReg(MPU, 0xD223, 0xA4);
	MPU_WriteReg(MPU, 0xD224, 0x02);
	MPU_WriteReg(MPU, 0xD225, 0xB7);
	MPU_WriteReg(MPU, 0xD226, 0x02);
	MPU_WriteReg(MPU, 0xD227, 0xCF);
	MPU_WriteReg(MPU, 0xD228, 0x02);
	MPU_WriteReg(MPU, 0xD229, 0xDE);
	MPU_WriteReg(MPU, 0xD22A, 0x02);
	MPU_WriteReg(MPU, 0xD22B, 0xF2);
	MPU_WriteReg(MPU, 0xD22C, 0x02);
	MPU_WriteReg(MPU, 0xD22D, 0xFE);
	MPU_WriteReg(MPU, 0xD22E, 0x03);
	MPU_WriteReg(MPU, 0xD22F, 0x10);
	MPU_WriteReg(MPU, 0xD230, 0x03);
	MPU_WriteReg(MPU, 0xD231, 0x33);
	MPU_WriteReg(MPU, 0xD232, 0x03);
	MPU_WriteReg(MPU, 0xD233, 0x6D);
	MPU_WriteReg(MPU, 0xD300, 0x00);
	MPU_WriteReg(MPU, 0xD301, 0x33);
	MPU_WriteReg(MPU, 0xD302, 0x00);
	MPU_WriteReg(MPU, 0xD303, 0x34);
	MPU_WriteReg(MPU, 0xD304, 0x00);
	MPU_WriteReg(MPU, 0xD305, 0x3A);
	MPU_WriteReg(MPU, 0xD306, 0x00);
	MPU_WriteReg(MPU, 0xD307, 0x4A);
	MPU_WriteReg(MPU, 0xD308, 0x00);
	MPU_WriteReg(MPU, 0xD309, 0x5C);
	MPU_WriteReg(MPU, 0xD30A, 0x00);

	MPU_WriteReg(MPU, 0xD30B, 0x81);
	MPU_WriteReg(MPU, 0xD30C, 0x00);
	MPU_WriteReg(MPU, 0xD30D, 0xA6);
	MPU_WriteReg(MPU, 0xD30E, 0x00);
	MPU_WriteReg(MPU, 0xD30F, 0xE5);
	MPU_WriteReg(MPU, 0xD310, 0x01);
	MPU_WriteReg(MPU, 0xD311, 0x13);
	MPU_WriteReg(MPU, 0xD312, 0x01);
	MPU_WriteReg(MPU, 0xD313, 0x54);
	MPU_WriteReg(MPU, 0xD314, 0x01);
	MPU_WriteReg(MPU, 0xD315, 0x82);
	MPU_WriteReg(MPU, 0xD316, 0x01);
	MPU_WriteReg(MPU, 0xD317, 0xCA);
	MPU_WriteReg(MPU, 0xD318, 0x02);
	MPU_WriteReg(MPU, 0xD319, 0x00);
	MPU_WriteReg(MPU, 0xD31A, 0x02);
	MPU_WriteReg(MPU, 0xD31B, 0x01);
	MPU_WriteReg(MPU, 0xD31C, 0x02);
	MPU_WriteReg(MPU, 0xD31D, 0x34);
	MPU_WriteReg(MPU, 0xD31E, 0x02);
	MPU_WriteReg(MPU, 0xD31F, 0x67);
	MPU_WriteReg(MPU, 0xD320, 0x02);
	MPU_WriteReg(MPU, 0xD321, 0x84);
	MPU_WriteReg(MPU, 0xD322, 0x02);
	MPU_WriteReg(MPU, 0xD323, 0xA4);
	MPU_WriteReg(MPU, 0xD324, 0x02);
	MPU_WriteReg(MPU, 0xD325, 0xB7);
	MPU_WriteReg(MPU, 0xD326, 0x02);
	MPU_WriteReg(MPU, 0xD327, 0xCF);
	MPU_WriteReg(MPU, 0xD328, 0x02);
	MPU_WriteReg(MPU, 0xD329, 0xDE);
	MPU_WriteReg(MPU, 0xD32A, 0x02);
	MPU_WriteReg(MPU, 0xD32B, 0xF2);
	MPU_WriteReg(MPU, 0xD32C, 0x02);
	MPU_WriteReg(MPU, 0xD32D, 0xFE);
	MPU_WriteReg(MPU, 0xD32E, 0x03);
	MPU_WriteReg(MPU, 0xD32F, 0x10);
	MPU_WriteReg(MPU, 0xD330, 0x03);
	MPU_WriteReg(MPU, 0xD331, 0x33);
	MPU_WriteReg(MPU, 0xD332, 0x03);
	MPU_WriteReg(MPU, 0xD333, 0x6D);
	MPU_WriteReg(MPU, 0xD400, 0x00);
	MPU_WriteReg(MPU, 0xD401, 0x33);
	MPU_WriteReg(MPU, 0xD402, 0x00);
	MPU_WriteReg(MPU, 0xD403, 0x34);
	MPU_WriteReg(MPU, 0xD404, 0x00);
	MPU_WriteReg(MPU, 0xD405, 0x3A);
	MPU_WriteReg(MPU, 0xD406, 0x00);
	MPU_WriteReg(MPU, 0xD407, 0x4A);
	MPU_WriteReg(MPU, 0xD408, 0x00);
	MPU_WriteReg(MPU, 0xD409, 0x5C);
	MPU_WriteReg(MPU, 0xD40A, 0x00);
	MPU_WriteReg(MPU, 0xD40B, 0x81);

	MPU_WriteReg(MPU, 0xD40C, 0x00);
	MPU_WriteReg(MPU, 0xD40D, 0xA6);
	MPU_WriteReg(MPU, 0xD40E, 0x00);
	MPU_WriteReg(MPU, 0xD40F, 0xE5);
	MPU_WriteReg(MPU, 0xD410, 0x01);
	MPU_WriteReg(MPU, 0xD411, 0x13);
	MPU_WriteReg(MPU, 0xD412, 0x01);
	MPU_WriteReg(MPU, 0xD413, 0x54);
	MPU_WriteReg(MPU, 0xD414, 0x01);
	MPU_WriteReg(MPU, 0xD415, 0x82);
	MPU_WriteReg(MPU, 0xD416, 0x01);
	MPU_WriteReg(MPU, 0xD417, 0xCA);
	MPU_WriteReg(MPU, 0xD418, 0x02);
	MPU_WriteReg(MPU, 0xD419, 0x00);
	MPU_WriteReg(MPU, 0xD41A, 0x02);
	MPU_WriteReg(MPU, 0xD41B, 0x01);
	MPU_WriteReg(MPU, 0xD41C, 0x02);
	MPU_WriteReg(MPU, 0xD41D, 0x34);
	MPU_WriteReg(MPU, 0xD41E, 0x02);
	MPU_WriteReg(MPU, 0xD41F, 0x67);
	MPU_WriteReg(MPU, 0xD420, 0x02);
	MPU_WriteReg(MPU, 0xD421, 0x84);
	MPU_WriteReg(MPU, 0xD422, 0x02);
	MPU_WriteReg(MPU, 0xD423, 0xA4);
	MPU_WriteReg(MPU, 0xD424, 0x02);
	MPU_WriteReg(MPU, 0xD425, 0xB7);
	MPU_WriteReg(MPU, 0xD426, 0x02);
	MPU_WriteReg(MPU, 0xD427, 0xCF);
	MPU_WriteReg(MPU, 0xD428, 0x02);
	MPU_WriteReg(MPU, 0xD429, 0xDE);
	MPU_WriteReg(MPU, 0xD42A, 0x02);
	MPU_WriteReg(MPU, 0xD42B, 0xF2);
	MPU_WriteReg(MPU, 0xD42C, 0x02);
	MPU_WriteReg(MPU, 0xD42D, 0xFE);
	MPU_WriteReg(MPU, 0xD42E, 0x03);
	MPU_WriteReg(MPU, 0xD42F, 0x10);
	MPU_WriteReg(MPU, 0xD430, 0x03);
	MPU_WriteReg(MPU, 0xD431, 0x33);
	MPU_WriteReg(MPU, 0xD432, 0x03);
	MPU_WriteReg(MPU, 0xD433, 0x6D);
	MPU_WriteReg(MPU, 0xD500, 0x00);
	MPU_WriteReg(MPU, 0xD501, 0x33);
	MPU_WriteReg(MPU, 0xD502, 0x00);
	MPU_WriteReg(MPU, 0xD503, 0x34);
	MPU_WriteReg(MPU, 0xD504, 0x00);
	MPU_WriteReg(MPU, 0xD505, 0x3A);
	MPU_WriteReg(MPU, 0xD506, 0x00);
	MPU_WriteReg(MPU, 0xD507, 0x4A);
	MPU_WriteReg(MPU, 0xD508, 0x00);
	MPU_WriteReg(MPU, 0xD509, 0x5C);
	MPU_WriteReg(MPU, 0xD50A, 0x00);
	MPU_WriteReg(MPU, 0xD50B, 0x81);

	MPU_WriteReg(MPU, 0xD50C, 0x00);
	MPU_WriteReg(MPU, 0xD50D, 0xA6);
	MPU_WriteReg(MPU, 0xD50E, 0x00);
	MPU_WriteReg(MPU, 0xD50F, 0xE5);
	MPU_WriteReg(MPU, 0xD510, 0x01);
	MPU_WriteReg(MPU, 0xD511, 0x13);
	MPU_WriteReg(MPU, 0xD512, 0x01);
	MPU_WriteReg(MPU, 0xD513, 0x54);
	MPU_WriteReg(MPU, 0xD514, 0x01);
	MPU_WriteReg(MPU, 0xD515, 0x82);
	MPU_WriteReg(MPU, 0xD516, 0x01);
	MPU_WriteReg(MPU, 0xD517, 0xCA);
	MPU_WriteReg(MPU, 0xD518, 0x02);
	MPU_WriteReg(MPU, 0xD519, 0x00);
	MPU_WriteReg(MPU, 0xD51A, 0x02);
	MPU_WriteReg(MPU, 0xD51B, 0x01);
	MPU_WriteReg(MPU, 0xD51C, 0x02);
	MPU_WriteReg(MPU, 0xD51D, 0x34);
	MPU_WriteReg(MPU, 0xD51E, 0x02);
	MPU_WriteReg(MPU, 0xD51F, 0x67);
	MPU_WriteReg(MPU, 0xD520, 0x02);
	MPU_WriteReg(MPU, 0xD521, 0x84);
	MPU_WriteReg(MPU, 0xD522, 0x02);
	MPU_WriteReg(MPU, 0xD523, 0xA4);
	MPU_WriteReg(MPU, 0xD524, 0x02);
	MPU_WriteReg(MPU, 0xD525, 0xB7);
	MPU_WriteReg(MPU, 0xD526, 0x02);
	MPU_WriteReg(MPU, 0xD527, 0xCF);
	MPU_WriteReg(MPU, 0xD528, 0x02);
	MPU_WriteReg(MPU, 0xD529, 0xDE);
	MPU_WriteReg(MPU, 0xD52A, 0x02);
	MPU_WriteReg(MPU, 0xD52B, 0xF2);
	MPU_WriteReg(MPU, 0xD52C, 0x02);
	MPU_WriteReg(MPU, 0xD52D, 0xFE);
	MPU_WriteReg(MPU, 0xD52E, 0x03);
	MPU_WriteReg(MPU, 0xD52F, 0x10);
	MPU_WriteReg(MPU, 0xD530, 0x03);
	MPU_WriteReg(MPU, 0xD531, 0x33);
	MPU_WriteReg(MPU, 0xD532, 0x03);
	MPU_WriteReg(MPU, 0xD533, 0x6D);
	MPU_WriteReg(MPU, 0xD600, 0x00);
	MPU_WriteReg(MPU, 0xD601, 0x33);
	MPU_WriteReg(MPU, 0xD602, 0x00);
	MPU_WriteReg(MPU, 0xD603, 0x34);
	MPU_WriteReg(MPU, 0xD604, 0x00);
	MPU_WriteReg(MPU, 0xD605, 0x3A);
	MPU_WriteReg(MPU, 0xD606, 0x00);
	MPU_WriteReg(MPU, 0xD607, 0x4A);
	MPU_WriteReg(MPU, 0xD608, 0x00);
	MPU_WriteReg(MPU, 0xD609, 0x5C);
	MPU_WriteReg(MPU, 0xD60A, 0x00);
	MPU_WriteReg(MPU, 0xD60B, 0x81);

	MPU_WriteReg(MPU, 0xD60C, 0x00);
	MPU_WriteReg(MPU, 0xD60D, 0xA6);
	MPU_WriteReg(MPU, 0xD60E, 0x00);
	MPU_WriteReg(MPU, 0xD60F, 0xE5);
	MPU_WriteReg(MPU, 0xD610, 0x01);
	MPU_WriteReg(MPU, 0xD611, 0x13);
	MPU_WriteReg(MPU, 0xD612, 0x01);
	MPU_WriteReg(MPU, 0xD613, 0x54);
	MPU_WriteReg(MPU, 0xD614, 0x01);
	MPU_WriteReg(MPU, 0xD615, 0x82);
	MPU_WriteReg(MPU, 0xD616, 0x01);
	MPU_WriteReg(MPU, 0xD617, 0xCA);
	MPU_WriteReg(MPU, 0xD618, 0x02);
	MPU_WriteReg(MPU, 0xD619, 0x00);
	MPU_WriteReg(MPU, 0xD61A, 0x02);
	MPU_WriteReg(MPU, 0xD61B, 0x01);
	MPU_WriteReg(MPU, 0xD61C, 0x02);
	MPU_WriteReg(MPU, 0xD61D, 0x34);
	MPU_WriteReg(MPU, 0xD61E, 0x02);
	MPU_WriteReg(MPU, 0xD61F, 0x67);
	MPU_WriteReg(MPU, 0xD620, 0x02);
	MPU_WriteReg(MPU, 0xD621, 0x84);
	MPU_WriteReg(MPU, 0xD622, 0x02);
	MPU_WriteReg(MPU, 0xD623, 0xA4);
	MPU_WriteReg(MPU, 0xD624, 0x02);
	MPU_WriteReg(MPU, 0xD625, 0xB7);
	MPU_WriteReg(MPU, 0xD626, 0x02);
	MPU_WriteReg(MPU, 0xD627, 0xCF);
	MPU_WriteReg(MPU, 0xD628, 0x02);
	MPU_WriteReg(MPU, 0xD629, 0xDE);
	MPU_WriteReg(MPU, 0xD62A, 0x02);
	MPU_WriteReg(MPU, 0xD62B, 0xF2);
	MPU_WriteReg(MPU, 0xD62C, 0x02);
	MPU_WriteReg(MPU, 0xD62D, 0xFE);
	MPU_WriteReg(MPU, 0xD62E, 0x03);
	MPU_WriteReg(MPU, 0xD62F, 0x10);
	MPU_WriteReg(MPU, 0xD630, 0x03);
	MPU_WriteReg(MPU, 0xD631, 0x33);
	MPU_WriteReg(MPU, 0xD632, 0x03);
	MPU_WriteReg(MPU, 0xD633, 0x6D);
	//LV2 Page 0 enable
	MPU_WriteReg(MPU, 0xF000, 0x55);
	MPU_WriteReg(MPU, 0xF001, 0xAA);
	MPU_WriteReg(MPU, 0xF002, 0x52);
	MPU_WriteReg(MPU, 0xF003, 0x08);
	MPU_WriteReg(MPU, 0xF004, 0x00);
	//Display control
	MPU_WriteReg(MPU, 0xB100,  0xCC);
	MPU_WriteReg(MPU, 0xB101,  0x00);
	//Source hold time
	MPU_WriteReg(MPU, 0xB600, 0x05);
	//Gate EQ control
	MPU_WriteReg(MPU, 0xB700, 0x70);
	MPU_WriteReg(MPU, 0xB701, 0x70);
	//Source EQ control (Mode 2)
	MPU_WriteReg(MPU, 0xB800, 0x01);
	MPU_WriteReg(MPU, 0xB801, 0x03);
	MPU_WriteReg(MPU, 0xB802, 0x03);
	MPU_WriteReg(MPU, 0xB803, 0x03);
	//Inversion mode (2-dot)
	MPU_WriteReg(MPU, 0xBC00, 0x02);
	MPU_WriteReg(MPU, 0xBC01, 0x00);
	MPU_WriteReg(MPU, 0xBC02, 0x00);
	//Timing control 4H w/ 4-delay
	MPU_WriteReg(MPU, 0xC900, 0xD0);
	MPU_WriteReg(MPU, 0xC901, 0x02);
	MPU_WriteReg(MPU, 0xC902, 0x50);
	MPU_WriteReg(MPU, 0xC903, 0x50);
	MPU_WriteReg(MPU, 0xC904, 0x50);
	MPU_WriteReg(MPU, 0x3500, 0x00);
	MPU_WriteReg(MPU, 0x3A00, 0x55);  //16-bit/pixel
	
	MPU_WriteReg(MPU, 0x1100, 0x00);
	for(i = 0; i < 10000; i++) __NOP();
	MPU_WriteReg(MPU, 0x2900, 0x00);	//MPU on
}


/****************************************************************************************************************************************** 
* 函数名称: NT35510_SetCursor()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void NT35510_SetCursor(uint16_t x, uint16_t y)
{
	MPU_WriteReg(MPU, NT35510_CMD_COUSOR_X,   x>>8);
	MPU_WriteReg(MPU, NT35510_CMD_COUSOR_X+1, x&0xFF);
	MPU_WriteReg(MPU, NT35510_CMD_COUSOR_Y,   y>>8);
	MPU_WriteReg(MPU, NT35510_CMD_COUSOR_Y+1, y&0xFF);
}

/****************************************************************************************************************************************** 
* 函数名称: NT35510_DrawPoint()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void NT35510_DrawPoint(uint16_t x, uint16_t y, uint16_t rgb)
{
	NT35510_SetCursor(x, y);
	
	MPU_WriteReg(MPU, NT35510_CMD_COUSOR_X, rgb);
}

/****************************************************************************************************************************************** 
* 函数名称: NT35510_Clear()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void NT35510_Clear(uint16_t rgb)
{
	uint32_t i, j;
	
	NT35510_SetCursor(0, 0);
	
	MPU_WR_REG(MPU, NT35510_CMD_WR_DATA >> 8);
	MPU_WR_REG(MPU, NT35510_CMD_WR_DATA & 0xFF);
	
	for(i = 0; i < MPU_VPIX; i++)
	{
		for(j = 0; j < MPU_HPIX; j++)
		{
			MPU_WR_DATA(MPU, rgb);
		}
	}
}

static uint32_t MPUDMA_Color;
/****************************************************************************************************************************************** 
* 函数名称: NT35510_DMAClear()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void NT35510_DMAClear(uint16_t color)
{
	DMA_InitStructure DMA_initStruct;
	
	MPUDMA_Color = color;
	
	NT35510_SetCursor(0, 0);
		
	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_HALFWORD;
	DMA_initStruct.Count = MPU_HPIX * MPU_VPIX;
	DMA_initStruct.MemoryAddr = (uint32_t)&MPUDMA_Color;
	DMA_initStruct.MemoryAddrInc = 0;
	DMA_initStruct.PeripheralAddr = (uint32_t)&MPU->DR;
	DMA_initStruct.PeripheralAddrInc = 0;
	DMA_initStruct.Handshake = DMA_CH1_MPUTX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH1, &DMA_initStruct);
	DMA_CH_Open(DMA_CH1);
	
	MPU->SR = MPU_SR_DMAEN_Msk;
	
	MPU->IR = NT35510_CMD_WR_DATA;
}


uint32_t NT35510_Done(void)
{
	if(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE))
	{
		DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
		
		MPU->SR = 0;	// 完成 DMA 操作后要清除DMA使能
		
		return 1;
	}
	else
	{
		return 0;
	}
}
