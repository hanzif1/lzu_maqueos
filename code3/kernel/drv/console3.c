#include <xtos.h>
#define NR_PIX_X 1280
#define NR_PIX_Y 800
#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8
#define NR_CHAR_X (NR_PIX_X / CHAR_WIDTH)
#define NR_CHAR_Y (NR_PIX_Y / CHAR_HEIGHT)
#define NR_BYTE_PIX 4
#define VRAM_BASE (0x40000000UL | DMW_MASK)
#define L7A_I8042_DATA (0x1fe00060UL | DMW_MASK)

extern char fonts[];
int x, y;
unsigned char save_chars[CHAR_HEIGHT * CHAR_WIDTH * NR_BYTE_PIX];
unsigned char *pos_old = (unsigned char *)0;
int isFirst = 1;
char digits_map[] = "0123456789abcdef";
char keys_map[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '`', 0,
    0, 0, 0, 0, 0, 'q', '1', 0, 0, 0, 'z', 's', 'a', 'w', '2', 0,
    0, 'c', 'x', 'd', 'e', '4', '3', 0, 0, 32, 'v', 'f', 't', 'r', '5', 0,
    0, 'n', 'b', 'h', 'g', 'y', '6', 0, 0, 0, 'm', 'j', 'u', '7', '8', 0,
    0, ',', 'k', 'i', 'o', '0', '9', 0, 0, '.', '/', 'l', ';', 'p', '-', 0,
    0, 0, '\'', 0, '[', '=', 0, 0, 0, 0, 13, ']', 0, '\\', 0, 0,
    0, 0, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, '`', 0};

int shift_def = 0;
int caps_def = 0;

void *memcpy(void *dest, const void *src, unsigned int n)
{
    char *d = (char *)dest;
    const char *s = (const char *)src;
    while (n--)
        *d++ = *s++;
    return dest;
}

void write_char(char ascii, int xx, int yy)
{
    char *font_byte;
    int row, col;
    char *pos;

    font_byte = &fonts[(ascii - 32) * CHAR_HEIGHT];
    pos = (char *)(VRAM_BASE + (yy * CHAR_HEIGHT * NR_PIX_X + xx * CHAR_WIDTH) * NR_BYTE_PIX);
    for (row = 0; row < CHAR_HEIGHT; row++, font_byte++)
    {
        for (col = 0; col < CHAR_WIDTH; col++)
        {
            if (*font_byte & (1 << (7 - col)))
            {
                *pos++ = 0;
                *pos++ = 0;
                *pos++ = 255;
                *pos++ = 0;
            }
            else
                pos += NR_BYTE_PIX;
        }
        pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
    }
}
void erase_char(int xx, int yy)
{
    int row, col;
    char *pos;

    pos = (char *)(VRAM_BASE + (yy * CHAR_HEIGHT * NR_PIX_X + xx * CHAR_WIDTH) * NR_BYTE_PIX);
    for (row = 0; row < CHAR_HEIGHT; row++)
    {
        for (col = 0; col < CHAR_WIDTH; col++)
        {
            *pos++ = 0;
            *pos++ = 0;
            *pos++ = 0;
            *pos++ = 0;
        }
        pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
    }
}
void scrup()
{
    int i;
    char *from, *to;

    to = (char *)VRAM_BASE;
    from = (char *)(VRAM_BASE + (CHAR_HEIGHT * NR_PIX_X * NR_BYTE_PIX));
    for (i = 0; i < (NR_PIX_Y - CHAR_HEIGHT) * NR_PIX_X * NR_BYTE_PIX; i++, to++, from++)
        *to = *from;
    for (i = 0; i < NR_CHAR_X; i++)
        erase_char(i, NR_CHAR_Y - 1);
}
void cr_lf()
{
    x = 0;
    if (y < NR_CHAR_Y - 1)
        y++;
    else
        scrup();
}
void del()
{
    if (x)
    {
        x--;
    }
    else if (y)
    {
        y--;
    }
    erase_char(x, y);
}
void move_char(int from_x, int from_y, int to_x, int to_y)
{
    // 计算源和目标字符块的基地址
    char *src_pos_base = (char *)(VRAM_BASE + (from_y * CHAR_HEIGHT * NR_PIX_X + from_x * CHAR_WIDTH) * NR_BYTE_PIX);
    char *dst_pos_base = (char *)(VRAM_BASE + (to_y * CHAR_HEIGHT * NR_PIX_X + to_x * CHAR_WIDTH) * NR_BYTE_PIX);
    // 每一行像素所占的字节数
    int line_bytes = CHAR_WIDTH * NR_BYTE_PIX;
    // 显存中一整行像素所占的字节数 (偏移量)
    int pitch = NR_PIX_X * NR_BYTE_PIX;
    int row;
    // 逐行复制字符的像素数据
    for (row = 0; row < CHAR_HEIGHT; row++)
    {
        char *src = src_pos_base + row * pitch;
        char *dst = dst_pos_base + row * pitch;
        memcpy(dst, src, line_bytes);
    }
}
void printk(char *buf)
{
    char c;
    int nr = 0;
    int size_per_row = CHAR_WIDTH * NR_BYTE_PIX;
    int i; 

    while (buf[nr] != '\0')
        nr++;
    // 恢复光标下方的字符
    if (!isFirst && pos_old)
    {
        for (i = 0; i < CHAR_HEIGHT; i++)
        {
            unsigned char *dst = pos_old + i * NR_PIX_X * NR_BYTE_PIX;
            unsigned char *src = save_chars + i * size_per_row;
            memcpy(dst, src, size_per_row);
        }
    }
    while (nr--)
    {
        c = *buf++;
        if (c > 31 && c < 127)
        {
            // 将光标位置(x,y)到行尾的所有字符向右移动一格
            for (i = NR_CHAR_X - 2; i >= x; i--)
            {
                move_char(i, y, i + 1, y);
            }
            
            unsigned char *pos_check = (unsigned char *)(VRAM_BASE + (y * CHAR_HEIGHT * NR_PIX_X + x * CHAR_WIDTH) * NR_BYTE_PIX);
            if (pos_old && pos_old == pos_check)
            {
                pos_old = (unsigned char *)0;
            }
            erase_char(x, y);
            write_char(c, x, y);
            x++; // 光标右移
            if (x >= NR_CHAR_X) // 检查是否需要换行
                cr_lf();
        }
        else if (c == 10 || c == 13)
            cr_lf();
        else if (c == 127)
            del(); 
        else
            panic("panic: unsurpported char!\n");
    }
    unsigned char *pos = (unsigned char *)(VRAM_BASE + (y * CHAR_HEIGHT * NR_PIX_X + x * CHAR_WIDTH) * NR_BYTE_PIX);
    for (i = 0; i < CHAR_HEIGHT; i++)
    {
        unsigned char *row_ptr = pos + i * NR_PIX_X * NR_BYTE_PIX;
        unsigned char *buf_ptr = save_chars + i * size_per_row;
        memcpy(buf_ptr, row_ptr, size_per_row); // 保存新光标位置下的字符
    }
    pos_old = pos;
    isFirst = 0;
    write_char('_', x, y); // 绘制新光标
}
void panic(char *s)
{
    printk(s);
    while (1)
        ;
}
void con_init()
{
    x = 0;
    y = 0;
}
void print_debug(char *str, unsigned long val)
{
    int i, j;
    char buffer[20];

    printk(str);
    buffer[0] = '0';
    buffer[1] = 'x';
    for (j = 0, i = 17; j < 16; j++, i--)
    {
        buffer[i] = (digits_map[val & 0xfUL]);
        val >>= 4;
    }
    buffer[18] = '\n';
    buffer[19] = '\0';
    printk(buffer);
}
void do_keyboard(unsigned char c)
{
    int size_per_row = CHAR_WIDTH * NR_BYTE_PIX;
    int i;
    //上下左右
    if (c == 0x75 || c == 0x72 || c == 0x6b || c == 0x74){
        unsigned char *pos_addr;
        if (!isFirst && pos_old){
            for (i = 0; i < CHAR_HEIGHT; i++){
                unsigned char *dst = pos_old + i * NR_PIX_X * NR_BYTE_PIX;
                unsigned char *src = save_chars + i * size_per_row;
                memcpy(dst, src, size_per_row);
            }
        }
        else{
            isFirst = 0;
        }
        if (c == 0x75){
            if (y > 0)
                y--;
        }
        else if (c == 0x72){
            if (y < NR_CHAR_Y - 1)
                y++;
        }
        else if (c == 0x6b){
            if (x > 0)
                x--;
            else if (y > 0) {
                y--;
                x = NR_CHAR_X - 1;
            }
        }
        else if (c == 0x74){
            if (x < NR_CHAR_X - 1){
                x++;
            }
            else{
                if (y < NR_CHAR_Y - 1){
                    x = 0;
                    y++;
                }
                else{
                    scrup(); //滚到下一页
                    x = 0;
                }
            }
        }
        //在绘制新光标前，保存新位置 (x, y) 处的字符
        pos_addr = (unsigned char *)(VRAM_BASE + (y * CHAR_HEIGHT * NR_PIX_X + x * CHAR_WIDTH) * NR_BYTE_PIX);
        pos_old = pos_addr;
        for (i = 0; i < CHAR_HEIGHT; i++)
        {
            unsigned char *row_ptr = pos_addr + i * NR_PIX_X * NR_BYTE_PIX;
            unsigned char *buf_ptr = save_chars + i * size_per_row;
            memcpy(buf_ptr, row_ptr, size_per_row);
        }
        write_char('_', x, y); // 绘制新光标
        return;
    }
    //处理正常字符
    char ch = keys_map[c];
    if (ch >= 'a' && ch <= 'z')
    {
        if (caps_def ^ shift_def) 
            ch = ch - 'a' + 'A';
    }
    else
    {
        ch = keys_map[c];
    }
    if (ch)
    {
        char tmp[2];
        tmp[0] = ch;
        tmp[1] = '\0';
        printk(tmp);
    }
}
void keyboard_interrupt()
{
    unsigned char c;
    c = *(volatile unsigned char *)L7A_I8042_DATA;
    //判断是否为方向键
    if (c == 0xe0)
    {
        unsigned char c2 = *(volatile unsigned char *)L7A_I8042_DATA;
        //判断方向键松开
        if (c2 == 0xf0)
        {
            *(volatile unsigned char *)L7A_I8042_DATA;
            return;
        }
        do_keyboard(c2);
        return;
    }
    //判断按键松开
    if (c == 0xf0)
    {
        c = *(volatile unsigned char *)L7A_I8042_DATA;
        if (c == 0x12 || c == 0x59)
        {
            shift_def = 0;
        }
        return;
    }
    if (c == 0x12 || c == 0x59)
    {
        shift_def = 1;
        return;
    }
    if (c == 0x58)
    {
        caps_def = !caps_def;
        return;
    }
    do_keyboard(c);
}
