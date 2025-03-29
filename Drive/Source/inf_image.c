#include <inf_image.h>
void display_attention(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 86, Image_Attention_86x78) ;								// 显示感叹号
	LCD_SetColor(LCD_BLACK);
}

void display_no(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 84, Image_NO_84x78) ;												// 显示叉号
	LCD_SetColor(LCD_BLACK);
}

void display_traffic_lights(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 77, 86, Image_traffic_lights_86x77) ;						// 显示红绿灯
	LCD_SetColor(LCD_BLACK);
}

void display_cars(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 86, Image_cars_86x78) ;											// 显示车辆
	LCD_SetColor(LCD_BLACK);
}
void display_ao(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 77, 86, Image_ao_86x77) ;												// 显示地面凹陷
	LCD_SetColor(LCD_BLACK);
}

void display_tu(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 86, Image_tu_86x78) ;												// 显示地面凸起
	LCD_SetColor(LCD_BLACK);
}

void display_aotu(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 77, 86, Image_aotu_86x77) ;											// 显示地面凹凸不平
	LCD_SetColor(LCD_BLACK);
}
