/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "ips096.h"

uint16                   IPS096_penColor     = IPS096_DEFAULT_PENCOLOR;
uint16                   IPS096_backgroundColor      = IPS096_DEFAULT_BGCOLOR;

static ips096_dir_enum          ips096_display_dir  = IPS096_DEFAULT_DISPLAY_DIR;
static ips096_font_size_enum    ips096_display_font = IPS096_DEFAULT_DISPLAY_FONT;
static uint8                    ips096_x_max        = 160;
static uint8                    ips096_y_max        = 80;
static uint8_t IPS096_colorMode = NORMAL;
static uint16_t IPS096_buffer[80][160] = {0};


#define ips096_write_8bit_data(data)    (spi_write_8bit(IPS096_SPI, (data)))
#define ips096_write_16bit_data(data)   (spi_write_16bit(IPS096_SPI, (data)))

static void ips096_write_index (const uint8 dat)
{
    IPS096_DC(0);
    ips096_write_8bit_data(dat);
    IPS096_DC(1);
}


void IPS096_SetRegion (const uint16 x1, const uint16 y1, const uint16 x2, const uint16 y2)
{
    if(ips096_display_dir == IPS096_PORTAIT)
    {
        ips096_write_index(0x2a);                                               // �е�ַ����
        ips096_write_16bit_data(x1 + 24);
        ips096_write_16bit_data(x2 + 24);
        ips096_write_index(0x2b);                                               // �е�ַ����
        ips096_write_16bit_data(y1);
        ips096_write_16bit_data(y2);
        ips096_write_index(0x2c);                                               // ������д
    }
    else if(ips096_display_dir == IPS096_PORTAIT_180)
    {
        ips096_write_index(0x2a);                                               // �е�ַ����
        ips096_write_16bit_data(x1 + 24);
        ips096_write_16bit_data(x2 + 24);
        ips096_write_index(0x2b);                                               // �е�ַ����
        ips096_write_16bit_data(y1);
        ips096_write_16bit_data(y2);
        ips096_write_index(0x2c);                                               // ������д
    }
    else if(ips096_display_dir == IPS096_CROSSWISE)
    {
        ips096_write_index(0x2a);                                               // �е�ַ����
        ips096_write_16bit_data(x1);
        ips096_write_16bit_data(x2);
        ips096_write_index(0x2b);                                               // �е�ַ����
        ips096_write_16bit_data(y1 + 24);
        ips096_write_16bit_data(y2 + 24);
        ips096_write_index(0x2c);                                               // ������д
    }
    else
    {
        ips096_write_index(0x2a);                                               // �е�ַ����
        ips096_write_16bit_data(x1);
        ips096_write_16bit_data(x2);
        ips096_write_index(0x2b);                                               // �е�ַ����
        ips096_write_16bit_data(y1 + 24);
        ips096_write_16bit_data(y2 + 24);
        ips096_write_index(0x2c);                                               // ������д
    }
}


void IPS096_SetDrawColor(IPS096_ColorMode_e mode)
{
    IPS096_colorMode = mode;
}


/*!
 * @brief   Send buffer to ips096
 *
 * @param   void
 * @return  void
 */
void IPS096_SendBuffer()
{
    IPS096_CS(0);
    IPS096_SetRegion(0, 0, 159, 79);

    for (int i = 0; i < 80; i++)
    {
        for (int j = 0; j < 160; j++)
        {
            ips096_write_16bit_data(IPS096_buffer[i][j]);
        }
    }
    IPS096_CS(1);
}

/*!
 * @brief   Clear buffer array
 *
 * @param   void
 * @return  void
 *
 * @note    Use black background color can easily clear the buffer by memset,
 *          but other color cannot and on board flash is not enough for another buffer
 *          to use memcpy, so using color other than black will slower this function.
 */
void IPS096_ClearBuffer()
{
    if (IPS096_backgroundColor == RGB565_BLACK)
    {
        memset(IPS096_buffer, 0, 80 * 160 * sizeof(uint16_t));
    } else
    {
        for (int i = 0; i < 80; i++)
        {
            for (int j = 0; j < 160; j++)
            {
                IPS096_buffer[i][j] = IPS096_backgroundColor;
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ����
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     color           ��ɫ��ʽ RGB565 ���߿���ʹ�� zf_common_font.h �� rgb565_color_enum ö��ֵ��������д��
// ���ز���     void
// ʹ��ʾ��     ips096_draw_point(0, 0, RGB565_RED);            // ���� 0,0 ��һ����ɫ�ĵ�
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void IPS096_DrawPoint(int16 x, int16 y, const uint16 color)
{
    if (x < 160 && y < 80 && x >= 0 && y >= 0)
    {
        if (IPS096_colorMode == XOR)
        {
            if (IPS096_buffer[y][x] == color)
                IPS096_buffer[y][x] = IPS096_backgroundColor;
            else
                IPS096_buffer[y][x] = color;
        } else
            IPS096_buffer[y][x] = color;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ����
// ����˵��     x_start         ����x��������
// ����˵��     y_start         ����y��������
// ����˵��     x_end           ����x������յ�
// ����˵��     y_end           ����y������յ�
// ����˵��     color           ��ɫ��ʽ RGB565 ���߿���ʹ�� zf_common_font.h �� rgb565_color_enum ö��ֵ��������д��
// ���ز���     void
// ʹ��ʾ��     ips096_draw_line(0, 0, 10, 10, RGB565_RED);     // ���� 0,0 �� 10,10 ��һ����ɫ����
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void IPS096_DrawLine(int16 x_start, int16 y_start, int16 x_end, int16 y_end, const uint16 color)
{
    int16 x_dir = (x_start < x_end ? 1 : -1);
    int16 y_dir = (y_start < y_end ? 1 : -1);
    float temp_rate = 0;
    float temp_b = 0;

    do
    {
        if (x_start != x_end)
        {
            temp_rate = (float) (y_start - y_end) / (float) (x_start - x_end);
            temp_b = (float) y_start - (float) x_start * temp_rate;
        } else
        {
            while (y_start != y_end)
            {
                IPS096_DrawPoint(x_start, y_start, color);
                y_start += y_dir;
            }
            break;
        }
        if (func_abs(y_start - y_end) > func_abs(x_start - x_end))
        {
            while (y_start != y_end)
            {
                IPS096_DrawPoint(x_start, y_start, color);
                y_start += y_dir;
                x_start = (int16) (((float) y_start - temp_b) / temp_rate);
            }
        } else
        {
            while (x_start != x_end)
            {
                IPS096_DrawPoint(x_start, y_start, color);
                x_start += x_dir;
                y_start = (int16) ((float) x_start * temp_rate + temp_b);
            }
        }
    } while (0);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ��ʾ�ַ�
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     dat             ��Ҫ��ʾ���ַ�
// ���ز���     void
// ʹ��ʾ��     ips096_show_char(0, 0, 'x');                    // ���� 0,0 дһ���ַ� x
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void IPS096_ShowChar(int16 x, int16 y, const char dat)
{
    uint8 i, j;
    switch (ips096_display_font)
    {
    case IPS096_6X8_FONT:
        for (i = 0; i < 6; i++)
        {
            // �� 32 ��Ϊ��ȡģ�Ǵӿո�ʼȡ�� �ո��� ascii ������� 32
            uint8 temp_top = ascii_font_6x8[dat - 32][i];
            for (j = 0; j < 8; j++)
            {
                if (temp_top & 0x01)
                {
                    IPS096_DrawPoint(x + i, y + j ,IPS096_penColor);
                }
                temp_top >>= 1;
            }
        }
        break;
    case IPS096_8X16_FONT:
        for (i = 0; i < 8; i++)
        {
            // �� 32 ��Ϊ��ȡģ�Ǵӿո�ʼȡ�� �ո��� ascii ������� 32
            uint8 temp_top = ascii_font_8x16[dat - 32][i];
            uint8 temp_bottom = ascii_font_8x16[dat - 32][i + 8];
            for (j = 0; j < 8; j++)
            {
                if (temp_top & 0x01)
                {
                    IPS096_DrawPoint(x + i, y + j ,IPS096_penColor);
                }
                temp_top >>= 1;
            }
            for (j = 0; j < 8; j++)
            {
                if (temp_bottom & 0x01)
                {
                    IPS096_DrawPoint(x + i, y + j + 8,IPS096_penColor);
                }
                temp_bottom >>= 1;
            }
        }
        break;
    default:
        break;
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ��ʾ�ַ���
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     dat             ��Ҫ��ʾ���ַ���
// ���ز���     void
// ʹ��ʾ��     ips096_show_string(0, 0, "seekfree");
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void IPS096_ShowStr(int16 x, int16 y, const char dat[])
{
    uint16 j = 0;
    while (dat[j] != '\0')
    {
        switch (ips096_display_font)
        {
        case IPS096_6X8_FONT:
            IPS096_ShowChar(x + 6 * j, y, dat[j]);
            j++;
            break;
        case IPS096_8X16_FONT:
            IPS096_ShowChar(x + 8 * j, y, dat[j]);
            j++;
            break;
        default:
            break;
        }
    }
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ��ʾ32λ�з��� (ȥ������������Ч��0)
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     dat             ��Ҫ��ʾ�ı��� �������� int32
// ����˵��     num             ��Ҫ��ʾ��λ�� ���10λ  ������������
// ���ز���     void
// ʹ��ʾ��     ips096_show_int(0, 0, x, 3);                    // x ����Ϊ int32 int16 int8 ����
// ��ע��Ϣ     ��������ʾһ�� ��-����   ������ʾһ���ո�
//-------------------------------------------------------------------------------------------------------------------
void IPS096_ShowInt(int16 x, int16 y, const int32 dat, uint8 num)
{
    zf_assert(num > 0);
    zf_assert(num <= 10);

    int32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];

    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num + 1);

    if (num < 10)
    {
        for (; num > 0; num--)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_int_to_str(data_buffer, dat_temp);
    IPS096_ShowStr(x, y, (const char *) &data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ��ʾ32λ�޷��� (ȥ������������Ч��0)
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     dat             ��Ҫ��ʾ�ı��� �������� uint32
// ����˵��     num             ��Ҫ��ʾ��λ�� ���10λ  ������������
// ���ز���     void
// ʹ��ʾ��     ips096_show_uint(0, 0, x, 3);                   // x ����Ϊ uint32 uint16 uint8 ����
// ��ע��Ϣ     ��������ʾһ�� ��-����   ������ʾһ���ո�
//-------------------------------------------------------------------------------------------------------------------
void IPS096_ShowUint(int16 x, int16 y, const uint32 dat, uint8 num)
{
    zf_assert(num > 0);
    zf_assert(num <= 10);

    uint32 dat_temp = dat;
    int32 offset = 1;
    char data_buffer[12];
    memset(data_buffer, 0, 12);
    memset(data_buffer, ' ', num);

    if (num < 10)
    {
        for (; num > 0; num--)
        {
            offset *= 10;
        }
        dat_temp %= offset;
    }
    func_uint_to_str(data_buffer, dat_temp);
    IPS096_ShowStr(x, y, (const char *) &data_buffer);
}

//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ��ʾ������ (ȥ������������Ч��0)
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     dat             ��Ҫ��ʾ�ı��� �������� float �� double
// ����˵��     num             ����λ��ʾ����   ���8λ
// ����˵��     pointnum        С��λ��ʾ����   ���6λ
// ���ز���     void
// ʹ��ʾ��     ips096_show_float(0, 0, x, 2, 3);               // ��ʾ������ ������ʾ 2 λ С����ʾ 3 λ
// ��ע��Ϣ     �ر�ע�⵱����С��������ʾ��ֵ����д���ֵ��һ����ʱ��
//              ���������ڸ��������ȶ�ʧ���⵼�µģ��Ⲣ������ʾ���������⣬
//              �й���������飬�����аٶ�ѧϰ   ���������ȶ�ʧ���⡣
//              ��������ʾһ�� ��-����   ������ʾһ���ո�
//-------------------------------------------------------------------------------------------------------------------
void IPS096_ShowFloat(int16 x, int16 y, const float dat, uint8 num, uint8 pointnum)
{
    zf_assert(num > 0);
    zf_assert(num <= 8);
    zf_assert(pointnum > 0);
    zf_assert(pointnum <= 6);

    float dat_temp = dat;
    float offset = 1.0;
    char data_buffer[17];
    memset(data_buffer, 0, 17);
    memset(data_buffer, ' ', num + pointnum + 2);

    if (num < 10)
    {
        for (; num > 0; num--)
        {
            offset *= 10;
        }
        dat_temp = dat_temp - ((int) dat_temp / (int) offset) * offset;
    }
    func_float_to_str(data_buffer, dat_temp, pointnum);
    IPS096_ShowStr(x, y, data_buffer);
}


bool reversedColor = false;

void IPS096_ModifyColor()
{
    if (reversedColor)
    {
        IPS096_penColor = IPS096_DEFAULT_BGCOLOR;
        IPS096_backgroundColor = IPS096_DEFAULT_PENCOLOR;
    } else
    {
        IPS096_penColor = IPS096_DEFAULT_PENCOLOR;
        IPS096_backgroundColor = IPS096_DEFAULT_BGCOLOR;
    }
}


/*!
 * @brief   Draw circle / disc with 5 choices of section
 *
 * @param   x           Center x of the circle
 * @param   y           Center y of the circle
 * @param   r           Radius
 * @param   color       Color
 * @param   section     See definition in user_ips096.h
 * @return  void
 */
void IPS096_DrawCircle(int16_t x, int16_t y, uint16_t r, const uint16_t color, uint8_t section)
{
    // y = kx, k = 1
    uint16_t x0 = (uint16_t) (r * cos(0.01745 * 45));
    uint16_t fx;

    // x^2 + y^2 = r^2, y->x / x->y
    for (int i = -x0 + 1; i < 0; ++i)
    {
        fx = (uint16_t) sqrt(pow(r, 2) - pow(i, 2));
        if (section & CIRCLE_UPPER_RIGHT)
        {
            IPS096_DrawPoint(x - i, y - fx, color);
            IPS096_DrawPoint(x + fx, y + i, color);
        }
        if (section & CIRCLE_UPPER_LEFT)
        {
            IPS096_DrawPoint(x + i, y - fx, color);
            IPS096_DrawPoint(x - fx, y + i, color);
        }
        if (section & CIRCLE_LOWER_LEFT)
        {
            IPS096_DrawPoint(x + i, y + fx, color);
            IPS096_DrawPoint(x - fx, y - i, color);
        }
        if (section & CIRCLE_LOWER_RIGHT)
        {
            IPS096_DrawPoint(x - i, y + fx, color);
            IPS096_DrawPoint(x + fx, y - i, color);
        }
    }

    // Add support for XOR color mode
    fx = (uint16_t) sqrt(pow(r, 2) - pow(x0, 2));
    if (section & CIRCLE_UPPER_RIGHT)
    {
        if (r > 1)
        {
            IPS096_DrawPoint(x + r, y, color);
            IPS096_DrawPoint(x, y - r, color);
        }
        if (x0 == fx)
            IPS096_DrawPoint(x + x0, y - x0, color);
        else
        {
            IPS096_DrawPoint(x + x0, y - fx, color);
            IPS096_DrawPoint(x + fx, y - x0, color);
        }
    }
    if (section & CIRCLE_UPPER_LEFT)
    {
        if (r > 1)
        {
            IPS096_DrawPoint(x - r, y, color);
            IPS096_DrawPoint(x, y - r, color);
        }
        if (x0 == fx)
            IPS096_DrawPoint(x - x0, y - x0, color);
        else
        {
            IPS096_DrawPoint(x - x0, y - fx, color);
            IPS096_DrawPoint(x - fx, y - x0, color);
        }
    }
    if (section & CIRCLE_LOWER_LEFT)
    {
        if (r > 1)
        {
            IPS096_DrawPoint(x - r, y, color);
            IPS096_DrawPoint(x, y + r, color);
        }
        if (x0 == fx)
            IPS096_DrawPoint(x - x0, y + x0, color);
        else
        {
            IPS096_DrawPoint(x - x0, y + fx, color);
            IPS096_DrawPoint(x - fx, y + x0, color);
        }
    }
    if (section & CIRCLE_LOWER_RIGHT)
    {
        if (r > 1)
        {
            IPS096_DrawPoint(x + r, y, color);
            IPS096_DrawPoint(x, y + r, color);
        }
        if (x0 == fx)
            IPS096_DrawPoint(x + x0, y + x0, color);
        else
        {
            IPS096_DrawPoint(x + x0, y + fx, color);
            IPS096_DrawPoint(x + fx, y + x0, color);
        }
    }
    if (section == CIRCLE_DRAW_ALL)
    {
        IPS096_DrawPoint(x + r, y, color);
        IPS096_DrawPoint(x - r, y, color);
        IPS096_DrawPoint(x, y - r, color);
        IPS096_DrawPoint(x, y + r, color);
    }
}
void IPS096_DrawDisc(int16_t x, int16_t y, uint16_t r, const uint16_t color, uint8_t section)
{
    // y = kx, k = 1
    uint16_t x0 = (uint16_t) (r * cos(0.01745 * 45));
    uint16_t fx;

    // x^2 + y^2 = r^2, y->x / x->y
    for (int i = -x0 + 1; i < 0; ++i)
    {
        fx = (uint16_t) sqrt(pow(r, 2) - pow(i, 2));
        if (section & CIRCLE_UPPER_RIGHT)
        {
            IPS096_DrawLine(x - i, y - fx, x - i, y + i, color);
            IPS096_DrawLine(x + fx, y + i, x - i, y + i, color);
        }
        if (section & CIRCLE_UPPER_LEFT)
        {
            IPS096_DrawLine(x + i, y - fx, x + i, y + i, color);
            IPS096_DrawLine(x - fx, y + i, x + i, y + i, color);
        }
        if (section & CIRCLE_LOWER_LEFT)
        {
            IPS096_DrawLine(x + i, y + fx, x + i, y - i, color);
            IPS096_DrawLine(x - fx, y - i, x + i, y - i, color);
        }
        if (section & CIRCLE_LOWER_RIGHT)
        {
            IPS096_DrawLine(x - i, y + fx, x - i, y - i, color);
            IPS096_DrawLine(x + fx, y - i, x - i, y - i, color);
        }
    }

    // Add support for XOR color mode
    IPS096_DrawPoint(x, y, color);
    if (r != 2)
        IPS096_DrawPoint(x, y, color);

    fx = (uint16_t) sqrt(pow(r, 2) - pow(x0, 2));
    if (section & CIRCLE_UPPER_RIGHT)
    {
        if (r > 1)
        {
            IPS096_DrawLine(x + r, y, x, y, color);
            IPS096_DrawLine(x, y - r, x, y, color);
        }
        if (r > 2)
            IPS096_DrawLine(x, y, x + x0, y - x0, color);
        if (x0 == fx)
            IPS096_DrawPoint(x + x0, y - x0, color);
        else
        {
            IPS096_DrawPoint(x + x0, y - fx, color);
            IPS096_DrawPoint(x + fx, y - x0, color);
            IPS096_DrawPoint(x + x0, y - x0, color);
        }
    }
    if (section & CIRCLE_UPPER_LEFT)
    {
        if (r > 1)
        {
            IPS096_DrawLine(x - r, y, x, y, color);
            IPS096_DrawLine(x, y - r, x, y, color);
        }
        if (r > 2)
            IPS096_DrawLine(x, y, x - x0, y - x0, color);
        if (x0 == fx)
            IPS096_DrawPoint(x - x0, y - x0, color);
        else
        {
            IPS096_DrawPoint(x - x0, y - fx, color);
            IPS096_DrawPoint(x - fx, y - x0, color);
            IPS096_DrawPoint(x - x0, y - x0, color);
        }
    }
    if (section & CIRCLE_LOWER_LEFT)
    {
        if (r > 1)
        {
            IPS096_DrawLine(x - r, y, x, y, color);
            IPS096_DrawLine(x, y + r, x, y, color);
        }
        if (r > 2)
            IPS096_DrawLine(x, y, x - x0, y + x0, color);
        if (x0 == fx)
            IPS096_DrawPoint(x - x0, y + x0, color);
        else
        {
            IPS096_DrawPoint(x - x0, y + fx, color);
            IPS096_DrawPoint(x - fx, y + x0, color);
            IPS096_DrawPoint(x - x0, y + x0, color);
        }
    }
    if (section & CIRCLE_LOWER_RIGHT)
    {
        if (r > 1)
        {
            IPS096_DrawLine(x + r, y, x, y, color);
            IPS096_DrawLine(x, y + r, x, y, color);
        }
        if (r > 2)
            IPS096_DrawLine(x, y, x + x0, y + x0, color);
        if (x0 == fx)
            IPS096_DrawPoint(x + x0, y + x0, color);
        else
        {
            IPS096_DrawPoint(x + x0, y + fx, color);
            IPS096_DrawPoint(x + fx, y + x0, color);
            IPS096_DrawPoint(x + x0, y + x0, color);
        }
    }
    if (section == CIRCLE_DRAW_ALL)
    {
        IPS096_DrawLine(x + r, y, x, y, color);
        IPS096_DrawLine(x - r, y, x, y, color);
        IPS096_DrawLine(x, y - r, x, y, color);
        IPS096_DrawLine(x, y + r, x, y, color);
        IPS096_DrawPoint(x, y, color);
    }
}


/*!
 * @brief   Draw box / filled box
 *
 * @param   x       Starting x
 * @param   y       Starting y
 * @param   width   Box width
 * @param   height  Box height
 * @param   color   Color
 * @return  void
 */
void IPS096_DrawFrame(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t color)
{
    for (int i = x; i < x + width; i++)
    {
        IPS096_DrawPoint(i, y, color);
        IPS096_DrawPoint(i, y + height - 1, color);
    }
    for (int j = y; j < y + height; j++)
    {
        IPS096_DrawPoint(x, j, color);
        IPS096_DrawPoint(x + width - 1, j, color);
    }
}
void IPS096_DrawBox(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t color)
{
    for (int i = x; i < x + width; ++i)
    {
        for (int j = y; j < y + height; ++j)
        {
            IPS096_DrawPoint(i, j, color);
        }
    }
}


/*!
 * @brief   Draw rounded box / filled box
 *
 * @param   x       Starting x
 * @param   y       Starting y
 * @param   width   Box width
 * @param   height  Box height
 * @param   color   Color
 * @param   r       Radius
 * @return  void
 */
void IPS096_DrawRFrame(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t color, uint8_t r)
{
    for (int i = x + r + 1; i < x + width - r - 1; i++)
    {
        IPS096_DrawPoint(i, y, color);
        IPS096_DrawPoint(i, y + height - 1, color);
    }
    for (int j = y + r + 1; j < y + height - r - 1; j++)
    {
        IPS096_DrawPoint(x, j, color);
        IPS096_DrawPoint(x + width - 1, j, color);
    }

    IPS096_DrawCircle(x + r, y + r, r, color, CIRCLE_UPPER_LEFT);
    IPS096_DrawCircle(x + width - 1 - r, y + r, r, color, CIRCLE_UPPER_RIGHT);
    IPS096_DrawCircle(x + r, y + height - 1 - r, r, color, CIRCLE_LOWER_LEFT);
    IPS096_DrawCircle(x + width - 1 - r, y + height - 1 - r, r, color, CIRCLE_LOWER_RIGHT);
}
void IPS096_DrawRBox(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint16_t color, uint8_t r)
{
    IPS096_DrawDisc(x + r, y + r, r, color, CIRCLE_UPPER_LEFT);
    IPS096_DrawDisc(x + width - 1 - r, y + r, r, color, CIRCLE_UPPER_RIGHT);
    IPS096_DrawDisc(x + r, y + height - 1 - r, r, color, CIRCLE_LOWER_LEFT);
    IPS096_DrawDisc(x + width - 1 - r, y + height - 1 - r, r, color, CIRCLE_LOWER_RIGHT);

    IPS096_DrawBox(x + r + 1, y, width - 2 - 2 * r, r + 1, color);
    IPS096_DrawBox(x, y + r + 1, width, height - 2 * r - 2, color);
    IPS096_DrawBox(x + r + 1, y + height - 1 - r, width - 2 - 2 * r, r + 1, color);
}


/*!
 * @brief   Show Binary BMP photo on screen
 *
 * @param   x       Starting x
 * @param   y       Starting y
 * @param   width   Pic width
 * @param   height  Pic height
 * @param   pic     The array of picture(���� ����ʽ ����)
 */
void IPS096_ShowBMP(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t *pic)
{
    uint8_t temp, j;
    uint8_t x0 = x;
    uint8_t *tmp = (uint8_t *) pic;
    uint16_t i, picSize = 0;

    picSize = (width / 8 + ((width % 8) ? 1 : 0)) * height;

    for (i = 0; i < picSize; i++)
    {
        temp = tmp[i];
        for (j = 0; j < 8; j++)
        {
            if (temp & 0x01)
            {
                IPS096_DrawPoint(x, y, IPS096_penColor);
            }
            temp >>= 1;
            x++;

            if ((x - x0) == width)
            {
                x = x0;
                y++;
                break;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------------------------
// �������     IPS096 ��ʾ 8bit �Ҷ�ͼ�� ����ֵ����ֵ
// ����˵��     x               ����x�������� ������Χ [0, ips096_x_max-1]
// ����˵��     y               ����y�������� ������Χ [0, ips096_y_max-1]
// ����˵��     *image          ͼ������ָ��
// ����˵��     width           ͼ��ʵ�ʿ��
// ����˵��     height          ͼ��ʵ�ʸ߶�
// ����˵��     dis_width       ͼ����ʾ��� ������Χ [0, ips096_x_max]
// ����˵��     dis_height      ͼ����ʾ�߶� ������Χ [0, ips096_y_max]
// ����˵��     threshold       ��ֵ����ʾ��ֵ 0-��������ֵ��
// ���ز���     void
// ʹ��ʾ��     ips096_show_gray_image(0, 0, mt9v03x_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void
IPS096_ShowGrayImage(uint16_t x, uint16_t y, const uint8_t *image, uint16_t width, uint16_t height, uint16_t dis_width,
                     uint16_t dis_height, uint8_t threshold)
{
    uint16_t i, j;
    uint16_t color, temp;
    uint32_t width_index, height_index;

    IPS096_CS(0);
    for (j = 0; j < dis_height; j++)
    {
        height_index = j * height / dis_height;
        for (i = 0; i < dis_width; i++)
        {
            width_index = i * width / dis_width;
            temp = *(image + height_index * width + width_index);
            if (threshold == 0)
            {
                color = (0x001f & ((temp) >> 3)) << 11;
                color = color | (((0x003f) & ((temp) >> 2)) << 5);
                color = color | (0x001f & ((temp) >> 3));
                IPS096_DrawPoint(x + i, y + j, color);
            } else if (temp < threshold)
                IPS096_DrawPoint(x + i, y + j, RGB565_BLACK);
            else
                IPS096_DrawPoint(x + i, y + j, RGB565_WHITE);
        }
    }
    IPS096_CS(1);
}


void IPS096_SetDirection (ips096_dir_enum dir)
{
    ips096_display_dir = dir;
    switch(ips096_display_dir)
    {
    case IPS096_PORTAIT:
    case IPS096_PORTAIT_180:
    {
        ips096_x_max = 80;
        ips096_y_max = 160;
    }break;
    case IPS096_CROSSWISE:
    case IPS096_CROSSWISE_180:
    {
        ips096_x_max = 160;
        ips096_y_max = 80;
    }break;
    }
}


void IPS096_SetColor (const uint16 pen, const uint16 bgcolor)
{
    IPS096_penColor = pen;
    IPS096_backgroundColor = bgcolor;
}


//-------------------------------------------------------------------------------------------------------------------
// �������     1.14�� IPSҺ����ʼ��
// ����˵��     void
// ���ز���     void
// ʹ��ʾ��     ips096_init();
// ��ע��Ϣ
//-------------------------------------------------------------------------------------------------------------------
void IPS096_Init (void)
{
#if IPS096_USE_SOFT_SPI
    soft_spi_init(&ips096_spi, 0, IPS096_SOFT_SPI_DELAY, IPS096_SCL_PIN, IPS096_SDA_PIN, SOFT_SPI_PIN_NULL, SOFT_SPI_PIN_NULL);
#else
    spi_init(IPS096_SPI, SPI_MODE0, IPS096_SPI_SPEED, IPS096_SCL_PIN, IPS096_SDA_PIN, SPI_MISO_NULL, SPI_CS_NULL);
#endif

    gpio_init(IPS096_DC_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(IPS096_RST_PIN, GPO, GPIO_LOW, GPO_PUSH_PULL);
    gpio_init(IPS096_CS_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(IPS096_BLK_PIN, GPO, GPIO_HIGH, GPO_PUSH_PULL);

    IPS096_SetDirection(ips096_display_dir);
    IPS096_SetColor(IPS096_penColor, IPS096_backgroundColor);

    IPS096_RST(0);
    system_delay_ms(200);

    IPS096_RST(1);
    system_delay_ms(100);

    IPS096_CS(0);
    ips096_write_index(0x36);
    system_delay_ms(100);
    if(ips096_display_dir == 0)
    {
        ips096_write_8bit_data(0x08);
    }
    else if(ips096_display_dir == 1)
    {
        ips096_write_8bit_data(0xC8);
    }
    else if(ips096_display_dir == 2)
    {
        ips096_write_8bit_data(0x78);
    }
    else
    {
        ips096_write_8bit_data(0xA8);
    }

    ips096_write_index(0x3A);
    ips096_write_8bit_data(0x05);

    ips096_write_index(0xB2);
    ips096_write_8bit_data(0x0C);
    ips096_write_8bit_data(0x0C);
    ips096_write_8bit_data(0x00);
    ips096_write_8bit_data(0x33);
    ips096_write_8bit_data(0x33);

    ips096_write_index(0xB7);
    ips096_write_8bit_data(0x35);

    ips096_write_index(0xBB);
    ips096_write_8bit_data(0x37);

    ips096_write_index(0xC0);
    ips096_write_8bit_data(0x2C);

    ips096_write_index(0xC2);
    ips096_write_8bit_data(0x01);

    ips096_write_index(0xC3);
    ips096_write_8bit_data(0x12);

    ips096_write_index(0xC4);
    ips096_write_8bit_data(0x20);

    ips096_write_index(0xC5);
    ips096_write_8bit_data(0x06);

    ips096_write_index(0xC6);
    ips096_write_8bit_data(0x0F);

    ips096_write_index(0xD0);
    ips096_write_8bit_data(0xA4);
    ips096_write_8bit_data(0xA1);

    ips096_write_index(0xE0);
    ips096_write_8bit_data(0xD0);
    ips096_write_8bit_data(0x04);
    ips096_write_8bit_data(0x0D);
    ips096_write_8bit_data(0x11);
    ips096_write_8bit_data(0x13);
    ips096_write_8bit_data(0x2B);
    ips096_write_8bit_data(0x3F);
    ips096_write_8bit_data(0x54);
    ips096_write_8bit_data(0x4C);
    ips096_write_8bit_data(0x18);
    ips096_write_8bit_data(0x0D);
    ips096_write_8bit_data(0x0B);
    ips096_write_8bit_data(0x1F);
    ips096_write_8bit_data(0x23);

    ips096_write_index(0xE1);
    ips096_write_8bit_data(0xD0);
    ips096_write_8bit_data(0x04);
    ips096_write_8bit_data(0x0C);
    ips096_write_8bit_data(0x11);
    ips096_write_8bit_data(0x13);
    ips096_write_8bit_data(0x2C);
    ips096_write_8bit_data(0x3F);
    ips096_write_8bit_data(0x44);
    ips096_write_8bit_data(0x51);
    ips096_write_8bit_data(0x2F);
    ips096_write_8bit_data(0x1F);
    ips096_write_8bit_data(0x1F);
    ips096_write_8bit_data(0x20);
    ips096_write_8bit_data(0x23);

    ips096_write_index(0x11);
    system_delay_ms(120);

    ips096_write_index(0x29);
    IPS096_CS(1);

    IPS096_ClearBuffer();
    IPS096_SendBuffer();
}
