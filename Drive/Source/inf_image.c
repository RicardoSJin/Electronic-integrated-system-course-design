#include <inf_image.h>
void display_attention(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 86, Image_Attention_86x78) ;								// ��ʾ��̾��
	LCD_SetColor(LCD_BLACK);
}

void display_no(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 84, Image_NO_84x78) ;												// ��ʾ���
	LCD_SetColor(LCD_BLACK);
}

void display_traffic_lights(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 77, 86, Image_traffic_lights_86x77) ;						// ��ʾ���̵�
	LCD_SetColor(LCD_BLACK);
}

void display_cars(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 86, Image_cars_86x78) ;											// ��ʾ����
	LCD_SetColor(LCD_BLACK);
}
void display_ao(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 77, 86, Image_ao_86x77) ;												// ��ʾ���氼��
	LCD_SetColor(LCD_BLACK);
}

void display_tu(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 78, 86, Image_tu_86x78) ;												// ��ʾ����͹��
	LCD_SetColor(LCD_BLACK);
}

void display_aotu(void)
{
	LCD_SetColor( 0xffFF8753);
	LCD_DrawImage(100, 180, 77, 86, Image_aotu_86x77) ;											// ��ʾ���氼͹��ƽ
	LCD_SetColor(LCD_BLACK);
}
