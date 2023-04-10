#include <main.h>
#include <gui.h>
#include <graphic/color.h>
#include <font.h>
#include <stdlib.h>
#include <s5pv210/reg-wdg.h>
#include "nanojpeg.h"
#include "data.h"

/*left - ��/��Ƽ�һ��
 * down - ��/��Ƽ�һ��
 * up - ��/��Ƽ�һ��
 * right - ��/��Ƽ�һ��
 * back - ��/��ת�Ƽ�һ��
 * menu - ��/��ת�Ƽ�һ��
 */

//index_y/r/g/l �����ʼ�Ƶơ���ơ��̵��Լ���ת�Ƶĳ���ʱ�����ֱ�Ϊ5s 18s 8s 5s ��������Ϊȫ�ֱ���
int index_y = 5;
int index_r = 18;
int index_g = 8;
int index_l = 5;
int index[2] = {8, 18};	//ÿ���Ƴ���ʱ����м�������˴�Ϊ��ʼ��������һ�����̵ƿ�ʼ��һ���Ӻ�ƿ�ʼ
char status[2] = {'g', 'r'};//��ʼ״̬

void watchdog_born()	//���Ź��������趨����ʱ������Ӧ��ʵ��ʱ��Ϊ1��
{
	writel(S5PV210_WTCNT, 0x3f9c);		//���Ź������Ĵ����������Ź�����ʱ�Ĵ���
	writel(S5PV210_WTCON, 0xff20);		//д��1111_1111_0010_0000
}

void watchdog_dead()	//���Ź����������������Ƶ����һλʱ���ٴε���watchdog_born����
{
	while(1)
	{
		if (readl(S5PV210_WTCNT) <= 0x0001)
		{
			break;
		}
	}
	watchdog_born();
}

void watchdog_dead_500()//���Ź��м�̬����־���Ź���ʱ����ʱ��0.5s��ʱ��
{
	while(1)
	{
		if (readl(S5PV210_WTCNT) <= 0x1fce)
		{
			break;
		}
	}
}

int key_control_seconds(u32_t keyup, u32_t keydown)	//�������ƽ�ͨ�Ƴ���ʱ���ĺ�������Ƶĳ���ʱ����������̵�+�Ƶ�+��ת��
{
	enum {
		KEY_NAME_POWER	= 0x1 << 0,
		KEY_NAME_LEFT	= 0x1 << 1,
		KEY_NAME_DOWN	= 0x1 << 2,
		KEY_NAME_UP		= 0x1 << 3,
		KEY_NAME_RIGHT	= 0x1 << 4,
		KEY_NAME_BACK	= 0x1 << 5,
		KEY_NAME_MENU	= 0x1 << 6,
		};
if (get_key_event(&keyup, &keydown))
	{if (keydown != 0)		//������°���
	{
		if (keydown & KEY_NAME_LEFT)	//����LEFT
		{
			if (index_g < 99 && index_r < 99)	//δ�ﵽ99s���̵ƺͺ��ͬʱ��1
			{
				index_g = index_g + 1;
				index_r = index_r + 1;
				return 0;
			}
		}
		if (keydown & KEY_NAME_DOWN)	//����DOWN
		{
			if (index_g > 5 && index_r > 5)	//δ�ﵽ5s���̵ƺͺ��ͬʱ��1
			{
				index_g = index_g - 1;
				index_r = index_r - 1;
				return 0;
			}
		}
		if (keydown & KEY_NAME_UP)		//����UP
		{
			if (index_y < 99 && index_r < 99)	//δ�ﵽ99s�ͻƵƺͺ��ͬʱ��1
			{
				index_y = index_y + 1;
				index_r = index_r + 1;
				return 1;
			}
		}
		if (keydown & KEY_NAME_RIGHT)		//����RIGHT
		{
			if (index_y > 5 && index_r > 5)		//δ�ﵽ5s�ͻƵƺͺ��ͬʱ��1
			{
				index_y = index_y - 1;
				index_r = index_r - 1;
				return 1;
			}
		}
		if (keydown & KEY_NAME_BACK)		//����BACK
		{
			if (index_r < 99 && index_l < 99)	//δ�ﵽ99s����ת�ƺͺ��ͬʱ��1
			{
				index_l = index_l + 1;
				index_r = index_r + 1;
				return -1;
			}
		}
		if (keydown & KEY_NAME_MENU)		//����MENU
		{
			if ((index_l > 5) && (index_r > 5))		//δ�ﵽ5s����ת�ͺ��ͬʱ��1
			{
				index_l = index_l - 1;
				index_r = index_r - 1;
				return -1;
			}
		}
	}
	}
    }

void renew_text(u32_t c1, u32_t c2, u32_t c3)	//����LCD��Ļ������
{	u32_t bc_w = 0x000000;	//text�ı����Ǻ�ɫ
	if (index_l > 1)
	{
		lcd_print(10, 516, c1, bc_w, "The duration of the left-turn light is");
		lcd_print(10, 532, c1, bc_w, "%02d seconds", index_l);
	}
	else
	{
		lcd_print(10, 516, c1, bc_w, "The duration of the left-turn light is");
		lcd_print(10, 532, c1, bc_w, "%02d second ", index_l);
	}	//��ӡ�����ڵ���ת�Ƶĳ���ʱ��


	if (index_g > 1)
	{
		lcd_print(10, 548, c1, bc_w, "The duration of the green light is %02d seconds", index_g);
	}
	else
	{
		lcd_print(10, 548, c1, bc_w, "The duration of the green light is %02d second ", index_g);
	}	//��ӡ�����ڵ��̵Ƶĳ���ʱ��

	if (index_y > 1)
	{
		lcd_print(10, 564, c2, bc_w, "The duration of the yellow light is %02d seconds", index_y);
	}
	else
	{
		lcd_print(10, 564, c2, bc_w, "The duration of the yellow light is %02d second ", index_y);
	}	//��ӡ�����ڵĻƵƵĳ���ʱ��

	if (index_r > 1)
	{
		lcd_print(10, 580, c3, bc_w, "The duration of the red light is %02d seconds", index_r);
	}
	else
	{
		lcd_print(10, 580, c3, bc_w, "The duration of the red light is %02d second ", index_r);
	}	//��ӡ�����ڵĺ�Ƶĳ���ʱ��

	}

void traffic_light_initial(u32_t *x, u32_t *y, u32_t r, u32_t *x_b, u32_t *y_b, u32_t c_b) //x y r�ǽ�ͨ�Ƶ����� x_b y_b�ǽ�ͨ�Ʊ��������� c_b�Ǳ�������ɫ
{   //initial
    GUI_RectangleFill(x_b[0], y_b[0], x_b[1], y_b[1], c_b);		//background
	GUI_CircleFill(x[0], y[0], r, 0x000000);
	GUI_CircleFill(x[1], y[1], r, 0x000000);
	GUI_CircleFill(x[2], y[2], r, 0x000000);
	GUI_CircleFill(x[3], y[3], r, 0x000000);	//�Ƶ�λ���ڲ�����ʱ��Ϊ��ɫ
}

void traffic_light(u32_t **px, u32_t **py, u32_t *c, u32_t r, u32_t bc_w, int order, u32_t keydown, u32_t keyup)
{	//�ú����ĺ��壺�����������ѡ��һ�齻ͨ��һֱ���ֺ�ƣ�Ȼ������һ�齻ͨ�ư����̵�-�Ƶ�-��ת��˳��仯��

	//�������еĺ��壺����orderΪ0ʱ����2*order��2*order+1 �����¡��ϵƵ���ţ�ͬʱ��order_sub_2��order_sub_2+1 �������ҵƵ����
	//��֮��order_sub_2��order_sub_2+1��������ڱ�����һֱ��Ƶ�������ͨ�ƣ�2*order��2*order+1�����ڱ����С����̵Ʊ�Ϊ�ƵƱ�Ϊ��ת���Ľ�ͨ��
	int order_sub_1 = abs(1 - order);
    int order_sub_2 = 2 * order_sub_1;

    //����ʱ��λ��
    int position[4][2] = {0};
    //�����н�ͨ�Ƶĳ���ʱ��
    int index_for_this_time[4];
    index_for_this_time[0] = index_g; index_for_this_time[1] = index_y; index_for_this_time[2] = index_r; index_for_this_time[3] = index_l;

    index[order] = index_for_this_time[0];	//����order��indexֵ�����̵Ƶĳ���ʱ��
    status[order] = 'g';	//��order��statusֵΪg
    //����ʱ����ʾλ��
    if (order == 0)
    {
        position[0][0] = 504; position[0][1] = 409;
        position[1][0] = 367; position[1][1] = 292;
        position[2][0] = 504; position[2][1] = 180;
        position[3][0] = 643; position[3][1] = 292;

    }
    else
    {
        position[0][0] = 367; position[0][1] = 292;
        position[1][0] = 504; position[1][1] = 409;
        position[2][0] = 643; position[2][1] = 292;
        position[3][0] = 504; position[3][1] = 180;

    }

    while(index[order_sub_1] > 0)	//һֱ������ƵĽ�ͨ�ƹ��ɴ�ѭ��
    {	watchdog_born();
        GUI_CircleFill(px[order_sub_2][0], py[order_sub_2][0], r, c[0]);
        GUI_CircleFill(px[order_sub_2+1][0], py[order_sub_2+1][0], r, c[0]);    //��order��ź����
        //����ʱ����
        lcd_print(position[1][0], position[1][1], c[0], bc_w, "%d", index[order_sub_1]/10);		//ʮλ
        lcd_print(position[1][0]+8, position[1][1], c[0], bc_w, "%d", index[order_sub_1]-(index[order_sub_1]/10)*10);		//��λ
        lcd_print(position[3][0], position[3][1], c[0], bc_w, "%d", index[order_sub_1]/10);		//ʮλ
        lcd_print(position[3][0]+8, position[3][1], c[0], bc_w, "%d", index[order_sub_1]-(index[order_sub_1]/10)*10);		//��λ


        if (index[order] > 0 && status[order] == 'g') //���order��Ӧ�ĵ�Ӧ������ɫ
        {
            GUI_CircleFill(px[order*2][2], py[order*2][2], r, c[2]);
            GUI_CircleFill(px[order*2+1][2], py[order*2+1][2], r, c[2]);    //order����̵���
            lcd_print(position[0][0], position[0][1], c[2], bc_w, "%d", index[order]/10);		//ʮλ
            lcd_print(position[0][0]+8, position[0][1], c[2], bc_w, "%d", index[order]-(index[order]/10)*10);		//��λ
            lcd_print(position[2][0], position[2][1], c[2], bc_w, "%d", index[order]/10);		//ʮλ
            lcd_print(position[2][0]+8, position[2][1], c[2], bc_w, "%d", index[order]-(index[order]/10)*10);		//��λ
            key_control_seconds(keyup, keydown);//���س���ʱ������ʱ��������һ������Ч
            renew_text(c[2], c[1], c[0]);	//���³���ʱ�����ı�
            //mdelay(1000);	//��ʱ
            watchdog_dead();
            index[order]--;	//�Լ�
            move_straight(order, index[order], index_for_this_time[0]);
        }
        else
        {
            if (status[order] == 'g')	//����ɫ��ʱ����
                {
                    GUI_CircleFill(px[order*2][2], py[order*2][2], r, 0x000000);	//�̵�ʱ������󣬽��̵Ƶ�λ���ú�ɫ����
                    GUI_CircleFill(px[order*2+1][2], py[order*2+1][2], r, 0x000000);	//�̵�ʱ������󣬽��̵Ƶ�λ���ú�ɫ����
                    index[order] = index_for_this_time[1];	//�ı�indexΪ��ɫ�ĳ���ʱ��
                    status[order] = 'y';	//��order״̬��Ϊyellow
                }
            if (index[order] > 0 && status[order] == 'y')//���order��Ӧ�ĵ�Ӧ������ɫ
            {
                GUI_CircleFill(px[order*2][1], py[order*2][1], r, c[1]);
                GUI_CircleFill(px[order*2+1][1], py[order*2+1][1], r, c[1]);    //order��Ӧ�ĻƵ���
                lcd_print(position[0][0], position[0][1], c[1], bc_w, "%d", index[order]/10);		//ʮλ
                lcd_print(position[0][0]+8, position[0][1], c[1], bc_w, "%d", index[order]-(index[order]/10)*10);		//��λ
                lcd_print(position[2][0], position[2][1], c[1], bc_w, "%d", index[order]/10);		//ʮλ
                lcd_print(position[2][0]+8, position[2][1], c[1], bc_w, "%d", index[order]-(index[order]/10)*10);		//��λ
                key_control_seconds(keyup, keydown);//���س���ʱ��
                renew_text(c[2], c[1], c[0]);//���³���ʱ�����ı�
                watchdog_dead_500();
                //mdelay(500);//��ʱ
                GUI_CircleFill(px[order*2][1], py[order*2][1], r, 0x000000);	//�Ƶ���˸
                GUI_CircleFill(px[order*2+1][1], py[order*2+1][1], r, 0x000000);	//�Ƶ���˸
                key_control_seconds(keyup, keydown);//���س���ʱ��
                renew_text(c[2], c[1], c[0]);//���³���ʱ�����ı�
                watchdog_dead();
                //mdelay(500);//��ʱ
                index[order]--;//�Լ�
            }
            else
            {
                if (status[order] == 'y')	//����ɫ��ʱ����
                {
                    index[order] = index_for_this_time[3];	//�ı�indexΪ��ת�Ƶĳ���ʱ��
                    status[order] = 'l';					//��order״̬��Ϊleft
                }
                if (index[order] > 0 && status[order] == 'l')//���order��Ӧ�ĵ�Ӧ������ת�ƣ���Ҫע����ǣ��÷�����ת����ʱ��ֱ�е�Ϊ��ƣ�
                {
                GUI_CircleFill(px[order*2][0], py[order*2][0], r, c[0]);
                GUI_CircleFill(px[order*2+1][0], py[order*2+1][0], r, c[0]);    //ֱ�к����
                left_light(px[order*2][3], py[order*2][3], c[2]);
                left_light(px[order*2+1][3], py[order*2+1][3], c[2]);	//��ת��
                //��ʾ��ת�ƶ���
                lcd_print(position[0][0], position[0][1], c[2], bc_w, "%d", index[order]/10);		//ʮλ
                lcd_print(position[0][0]+8, position[0][1], c[2], bc_w, "%d", index[order]-(index[order]/10)*10);		//��λ
                lcd_print(position[2][0], position[2][1], c[2], bc_w, "%d", index[order]/10);		//ʮλ
                lcd_print(position[2][0]+8, position[2][1], c[2], bc_w, "%d", index[order]-(index[order]/10)*10);		//��λ
                key_control_seconds(keyup, keydown);//���س���ʱ��
                renew_text(c[2], c[1], c[0]);//���³���ʱ�����ı�
                watchdog_dead();
                //mdelay(1000);//��ʱ
                index[order]--;//�Լ�
                turn_left(order, index[order], index_for_this_time[3]);
            	if (status[order] == 'l' && index[order] == 0)//����ת�Ƽ�ʱ����
            	{
            		index[order] = index_r;		//�ı�indexΪ��Ƶĳ���ʱ�䣬ע�⣬�����õ���index_r����index_for_this_time��������Ϊ��һ��ѭ����׼��
            		status[order] = 'r';					//��order״̬��Ϊred
            	    left_light(px[order*2][3], py[order*2][3], c[0]);
            	    left_light(px[order*2+1][3], py[order*2+1][3], c[0]);	//��ת��
            	}
                }
            }
        }
        index[order_sub_1]--;	//������ƵĽ�ͨ�������Լ�
      }
    GUI_CircleFill(px[order_sub_2][0], py[order_sub_2][0], r, 0x000000);	//�̵�ʱ������󣬽���Ƶ�λ���ú�ɫ����
    GUI_CircleFill(px[order_sub_2+1][0], py[order_sub_2+1][0], r, 0x000000);	//�̵�ʱ������󣬽���Ƶ�λ���ú�ɫ����

}

void left_light(u32_t x, u32_t y, u32_t c)	//�ú������ڻ�����ת�Ʒ���
{
    GUI_RectangleFill(x+6, y-5, x+5+8, y+13, c);//���ұߵ�������
    GUI_RectangleFill(x+6, y-5, x-5, y+3, c);	//�����
    //��ͷ
    u32_t x_temp = x-5-9;
    u32_t y1_temp = y+1;
    u32_t y2_temp = y-1;
    while(x_temp <= x-5)
    {
    	GUI_RLine(x_temp, y2_temp, y1_temp, c);
    	x_temp ++;
    	y1_temp ++;
    	y2_temp --;
    }
    //���ϲ�����ͨ�����ʵ��ó�
}

void display_car(const void* file, const int size, int x, int y)
{
	unsigned char *img, *imga, *p;
	int w, h;
	struct surface_t * screen;
	struct surface_t * obj;
    struct rect_t dstRect, srcRect;
    int i,j;

	njInit();
	njDecode(file, 2339);

	img = njGetImage();
	w = njGetWidth();
	h = njGetHeight();

	// Convert RGB to RGBA
	imga = malloc(w * h * 4);

	p = imga;
	for(i = 0; i < w * h; i++)
	{
		for(j = 0; j < 3; j++)
		{
			*p++ = *img++;
		}
		*p++ = 0xff;
	}

	screen = s5pv210_screen_surface();

	obj = surface_alloc(imga, w, h, PIXEL_FORMAT_ABGR_8888);

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.w = w;
    srcRect.h = h;
    dstRect.w = w;
    dstRect.h = h;
    dstRect.x = x;
    dstRect.y = y;
    surface_blit(screen, &dstRect, obj, &srcRect, BLEND_MODE_REPLACE);

}

void turn_left(int order, int index, int index_this_time)
{
	if (order == 0)
	{
		if (index == index_this_time/2)
		{
			GUI_RectangleFill(520, 467, 617, 600, 0xD3D3D3);
			GUI_RectangleFill(407, 0, 504, 132, 0xD3D3D3);
			display_car(file1, 1587, 530, 220);
			display_car(file2, 1683, 420, 330);
		}
		if (index == 1)
		{
			GUI_RectangleFill(410, 200, 610, 400, 0xD3D3D3);
			display_car(file1, 1587, 230, 230);
			display_car(file2, 1683, 710, 330);
		}
		if (index == 0)
		{
			GUI_RectangleFill(0, 195, 345-25, 292, 0xD3D3D3);
			GUI_RectangleFill(680+25, 308, 1024, 405, 0xD3D3D3);
		    display_car(file1, 1587, 530, 470);
		    display_car(file2, 1683, 420, 90);
		}

	}
	else
	{
		if (index == index_this_time/2)
		{
			GUI_RectangleFill(0, 405, 345-25, 308, 0xD3D3D3);
			GUI_RectangleFill(680+25, 195, 1024, 292, 0xD3D3D3);
			display_car(file3, 2113, 520, 330);
			display_car(file4, 2166, 384, 230);
		}
		if (index == 1)
		{
			GUI_RectangleFill(353+30, 200, 672-30, 400, 0xD3D3D3);
			display_car(file3, 2113, 520, 90);
			display_car(file4, 2166, 384, 470);
		}
		if (index == 0)
		{
			GUI_RectangleFill(520, 0, 672-30, 132, 0xD3D3D3);
			GUI_RectangleFill(353+30, 467, 504, 600, 0xD3D3D3);
		    display_car(file3, 2113, 190, 330);
		    display_car(file4, 2166, 710, 230);

		}
	}
}

void move_straight(int order, int index, int index_this_time)
{
	if (order == 0)
	{
		if (index == index_this_time/2)
		{
			GUI_RectangleFill(520, 467, 617, 600, 0xD3D3D3);
			GUI_RectangleFill(407, 0, 504, 132, 0xD3D3D3);
			display_car(file1, 1587, 530, 280);
			display_car(file2, 1683, 420, 280);
		}
		if (index == 2)
		{
			GUI_RectangleFill(410, 200, 610, 400, 0xD3D3D3);
			display_car(file1, 1587, 530, 90);
			display_car(file2, 1683, 420, 470);
		}
		if (index == 0)
		{
			GUI_RectangleFill(520, 0, 617, 132, 0xD3D3D3);
			GUI_RectangleFill(407, 467, 504, 600, 0xD3D3D3);
		    display_car(file1, 1587, 530, 470);
		    display_car(file2, 1683, 420, 90);
		}
	}
	else
	{
		if (index == index_this_time/2)
		{
			GUI_RectangleFill(0, 405, 345-25, 308, 0xD3D3D3);
			GUI_RectangleFill(680+25, 195, 1024, 292, 0xD3D3D3);
			display_car(file3, 2113, 450, 330);
			display_car(file4, 2166, 450, 230);
		}
		if (index == 2)
		{
			GUI_RectangleFill(410, 200, 610, 400, 0xD3D3D3);
			display_car(file3, 2113, 710, 330);
			display_car(file4, 2166, 190, 230);
		}
		if (index == 0)
		{
			GUI_RectangleFill(0, 195, 345-25, 292, 0xD3D3D3);
			GUI_RectangleFill(680+25, 308, 1024, 405, 0xD3D3D3);
		    display_car(file3, 2113, 190, 330);
		    display_car(file4, 2166, 710, 230);
		}
	}
}

int tester_Simulated_traffic_lights(int argc, char * argv[])	//������
{	s5pv210_screen_swap();
	s5pv210_screen_flush();
	surface_fill(s5pv210_screen_surface(), 0, 0, BLEND_MODE_REPLACE);

	//�ֱ����졢�ơ��̡���ת4��Բ�ε����ꡢ��ɫ���뾶
	u32_t x[4][4], y[4][4], c[3], r;  //4����ͨ�ƣ�ÿ����4����

	u32_t keyup, keydown;	//���ر���

	//��ʼ����·
	GUI_RectangleFill(0, 405, 1024, 195, 0xD3D3D3);//��210 ��1024
	GUI_RectangleFill(407-25, 0, 617+25, 600, 0xD3D3D3);//��210 ��600
	int count;
	for(count=0;count<7;count++)
	{
		GUI_LineWith(count*50, 300, count*50+40, 300, 10, 0x4DFFFF);//�������
		GUI_LineWith(1024-count*50, 300, 1024-count*50-40, 300, 10, 0x4DFFFF);//�������
	}
	for (count=0;count<6;count++)
	{
		GUI_LineWith(512, count*30, 512, count*30+25, 10, 0x4DFFFF);//��������
		GUI_LineWith(512, 600-count*30, 512, 600-count*30-25, 10, 0x4DFFFF);//��������
	}

	//��ʼ��
	lcd_print(0, 0, 0xFFFFFF, 0x000000, "Attention : the duration of the red light must");
	lcd_print(0, 16, 0xFFFFFF, 0x000000, "be equal to the sum of the green, yellow");
	lcd_print(0, 32, 0xFFFFFF, 0x000000, "and left turn duration.");

    display_car(file1, 1587, 530, 470);
    display_car(file2, 1683, 420, 90);
    display_car(file3, 2113, 190, 330);
    display_car(file4, 2166, 710, 230);

	//��ͨ�Ƶľ��α��������꼰��ɫ
	u32_t x_b[4][2], y_b[4][2], c_b;
	//�ַ������꼰�ַ��ı�����ɫ
	u32_t bc_w;

	//��������ָ�룬���ڴ��ݽ�ͨ�����깹�ɵĶ�ά����
    u32_t *px[4]; u32_t *py[4];

    r = 20;
    c[0] = 0x0000FF;//��
    c[1] = 0x00FFFF;//��
    c[2] = 0x00FF00;//��
    x[0][0] = 437; y[0][0] = 435;
    x[0][1] = 487; y[0][1] = 435;
    x[0][2] = 537; y[0][2] = 435;
    x[0][3] = 587; y[0][3] = 435;	//�²ཻͨ������

    x[1][0] = 437; y[1][0] = 165;
    x[1][1] = 487; y[1][1] = 165;
    x[1][2] = 537; y[1][2] = 165;
    x[1][3] = 587; y[1][3] = 165;	//�ϲཻͨ������

    x[2][0] = 377-25; y[2][0] = 225;
    x[2][1] = 377-25; y[2][1] = 275;
    x[2][2] = 377-25; y[2][2] = 325;
    x[2][3] = 377-25; y[2][3] = 375;   //��ཻͨ������

    x[3][0] = 647+25; y[3][0] = 225;
    x[3][1] = 647+25; y[3][1] = 275;
    x[3][2] = 647+25; y[3][2] = 325;
    x[3][3] = 647+25; y[3][3] = 375;   //�Ҳཻͨ������

    x_b[0][0] = 412; x_b[0][1] = 612; y_b[0][0] = 405; y_b[0][1] = 465;     //�²ཻͨ�Ʊ���
    x_b[1][0] = 412; x_b[1][1] = 612; y_b[1][0] = 135; y_b[1][1] = 195;     //�ϲཻͨ�Ʊ���
    x_b[2][0] = 347-25; x_b[2][1] = 407-25; y_b[2][0] = 200; y_b[2][1] = 400;     //��ཻͨ�Ʊ���
    x_b[3][0] = 617+25; x_b[3][1] = 677+25; y_b[3][0] = 200; y_b[3][1] = 400;     //�Ҳཻͨ�Ʊ���
    c_b = 0x080808;//������ɫ
	bc_w = 0x080808;//����ʱ���ַ�����

    int i;
    for(i=0;i<4;i++)	//��4����ͨ�Ƴ�ʼ��
    {
        traffic_light_initial(x[i], y[i], r, x_b[i], y_b[i], c_b);
        left_light(x[i][3], y[i][3], c[0]);
        px[i] = x[i];
        py[i] = y[i];
    }
//index_r = index_g + index_y + index_l

    int order= 0;
    while (1)
	{
        order = 0;//��һ�����
        traffic_light(px, py, c, r, bc_w, order, keydown, keyup);    //orderΪ0���������Ҵ��̵ƿ�ʼ��orderΪ1�������´��̵ƿ�ʼ
        order = 1;//�ڶ������
        traffic_light(px, py, c, r, bc_w, order, keydown, keyup);
	}
	return 0;
}
