#include "Accelerator.h"


#ifndef _UPRINT_H_
#define _UPRINT_H_



void uprint_initiate(unsigned int boud_rate)
{
	UART_reset();
	unsigned int top = 0;
	unsigned int div = 0;

	switch (boud_rate)
	{
	case 600	:	top = C_CLK_PERIODE / 600	;	break;
	case 1200	:	top = C_CLK_PERIODE / 1200	;	break;
	case 2400	:	top = C_CLK_PERIODE / 2400	;	break;
	case 4800	:	top = C_CLK_PERIODE / 4800	;	break;
	case 9600	:	top = C_CLK_PERIODE / 9600	;	break;
	case 12500	:	top = C_CLK_PERIODE / 12500	;	break;
	default		:	top = C_CLK_PERIODE / 9600	;	break;
	}

	for (div = 0; div <= 15; div++)
	{
		if (top < (1 << C_TRx_Unit_Top_len))
			break;
		top = (top >> 1);
	}

	UART_Config conf;
	conf.Tx_enable				= 1;
	conf.Rx_enable				= 1;
	conf.TBE_intr_enable		= 0;
	conf.RBF_intr_enable		= 1;
	conf.Tx_done_intr_enable	= 0;
	conf.Rx_done_intr_enable	= 0;
	conf.Clk_Div				= div;
	conf.TOP					= top;
	
	UART_Initiate(UART_Cont_Word_gen(conf));
}


void uprint(std::string str)
{
	for (unsigned int i = 0; i<length(str); i++)
	{
		while ((*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD >> C_TRx_Unit_Tx_Buff_Full_Flg_pos	) & 0x1){};
		*C_PERIPHERAL_REG_TRx_UNIT_DATA_WORD = str[i]];
	}
}


void uscan(std::string& str)
{
	str = "";
	unsigned int ch;
	while (1)
	{
		while((*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD >> C_TRx_Unit_Rx_Buff_Empty_Flg_pos	) & 0x1) {};
		ch = (*C_PERIPHERAL_REG_TRx_UNIT_DATA_WORD & (0xff));
		str += ch;
		if (ch == 10)
			break;
	}
	return error;
}


void Recieve_Buffer_Full_interrupt_handler()
{
	uprint("Noone listening...\n");
	while(!((*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD >> C_TRx_Unit_Rx_Buff_Empty_Flg_pos	) & 0x1))
		unsigned int tmp = *C_PERIPHERAL_REG_TRx_UNIT_DATA_WORD;
}




#endif
