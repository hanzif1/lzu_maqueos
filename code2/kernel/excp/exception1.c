
#include <xtos.h>

#define CSR_CRMD 0x0
#define CSR_ECFG 0x4
#define CSR_ESTAT 0x5
#define CSR_EENTRY 0xc
#define CSR_TCFG 0x41
#define CSR_TICLR 0x44
#define CSR_CRMD_IE (1UL << 2)
#define CSR_TCFG_EN (1UL << 0)
#define CSR_TCFG_PER (1UL << 1)
#define CSR_TICLR_CLR (1UL << 0)
#define CSR_ECFG_LIE_TI (1UL << 11)
#define CSR_ESTAT_IS_TI (1UL << 11)
#define CC_FREQ 4

void write_char(char,int,int);
void erase_char(int,int);
unsigned long val1 = 0xF4240; 
int aircraft_x = 3;
int aircraft_y = 2;
void timer_interrupt()
{
	clock_interrupt();
}
void draw_aircraft(int center_x, int center_y)
{
	write_char('*', center_x, center_y);
	write_char('*', center_x, center_y - 1); 
	write_char('*', center_x, center_y + 1); 
	write_char('*', center_x - 1, center_y); 
	write_char('*', center_x + 1, center_y);  
	write_char('*', center_x - 2, center_y); 
	write_char('*', center_x + 2, center_y); 
	write_char('*', center_x - 1, center_y+2);
	write_char('*', center_x + 1, center_y+2); 
}

void clear_aircraft(int center_x, int center_y)
{
	erase_char(center_x, center_y);  
	erase_char(center_x, center_y - 1); 
	erase_char(center_x, center_y + 1);
	erase_char(center_x - 1, center_y); 
	erase_char(center_x + 1, center_y); 
	erase_char(center_x - 2, center_y); 
	erase_char(center_x + 2, center_y);
	erase_char(center_x - 1, center_y+2); 
	erase_char(center_x + 1, center_y+2);
}

void clock_interrupt()
{
	clear_aircraft(aircraft_x, aircraft_y);
	aircraft_x++;
	if (aircraft_x >= 100) 
		aircraft_x = 3; 
	draw_aircraft(aircraft_x, aircraft_y);
}
void do_exception()
{
	unsigned int estat;

	estat = read_csr_32(CSR_ESTAT);
	if (estat & CSR_ESTAT_IS_TI)
	{
		write_csr_32(CSR_TICLR_CLR, CSR_TICLR);
		timer_interrupt();
	}
}
void int_on()
{
	unsigned int crmd;

	crmd = read_csr_32(CSR_CRMD);
	write_csr_32(crmd | CSR_CRMD_IE, CSR_CRMD);
}
void excp_init()
{
	unsigned int val;

	val = read_cpucfg(CC_FREQ);
	write_csr_64((unsigned long)val | CSR_TCFG_EN | CSR_TCFG_PER, CSR_TCFG);
	write_csr_64((unsigned long)exception_handler, CSR_EENTRY);
	write_csr_32(CSR_ECFG_LIE_TI, CSR_ECFG);
}