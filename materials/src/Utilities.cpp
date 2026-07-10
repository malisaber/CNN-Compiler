#include "Utilities.h"
#include "Accelerator.h"



void Bline_Initiate_STA_UPA(	const unsigned int	Capacity,
								const unsigned int	*Control_word,
								const unsigned int	*Counts,
								const unsigned int	*Ivals,
								const unsigned int	*UPA_Inp_base_addr_ptr,
								const unsigned int (*UPA_Wgt_base_addr_ptr)[9],
								const unsigned int	*UPA_Out_base_addr_ptr,
								const unsigned int	*UPA_Acc_base_addr_ptr)
{
	for (unsigned int idx = 0; idx < Capacity; idx++)
	{
		unsigned int pln = (Control_word[idx] >>	4)	& 0xF;
		unsigned int vlt = (Control_word[idx] >>	0)	& 0xF;
		unsigned int Inp_count = (Counts[idx] >>	24)	& 0xFF;
		unsigned int Wgt_count = (Counts[idx] >>	16)	& 0xFF;
		unsigned int Out_count = (Counts[idx] >>	8)	& 0xFF;
		unsigned int Acc_count = (Counts[idx] >>	0)	& 0xFF;
		unsigned int Inp_Ival  = ((Ivals[idx] >>	24)	& 0xFF) << 8;
		unsigned int Wgt_Ival  = ((Ivals[idx] >>	16)	& 0xFF) << 8;
		unsigned int Out_Ival  = ((Ivals[idx] >>	8)	& 0xFF) << 8;
		unsigned int Acc_Ival  = ((Ivals[idx] >>	0)	& 0xFF) << 8;
		PE_INIT_UA_Inp_load_address	(pln,	vlt,	UPA_Inp_base_addr_ptr[idx],	Inp_count,	Inp_Ival);
		PE_INIT_UA_Wgt_load_address	(pln,	vlt,	UPA_Wgt_base_addr_ptr[idx],	Wgt_count,	Wgt_Ival);
		PE_INIT_SA_Out_load_address (pln,	vlt,	UPA_Out_base_addr_ptr[idx],	Out_count,	Out_Ival);
		PE_INIT_SA_Acc_load_address (pln,	vlt,	UPA_Acc_base_addr_ptr[idx],	Acc_count,	Acc_Ival);
	}
}


void Bline_Initiate_STA_ECs(	const unsigned int	Capacity,					
								const unsigned int	*Control_word)
{
	for (unsigned int idx = 0; idx < Capacity; idx++)
	{
		unsigned int cnt = (Control_word[idx] >>	16)	& 0xFFFF;
		unsigned int pln = (Control_word[idx] >>	4)	& 0xF;
		unsigned int vlt = (Control_word[idx] >>	0)	& 0xF;
		PSU_EC_CNTR_start_with_config(pln, STA, vlt, cnt, 1, 1, 1);
	}
}


void Bline_Initiate_PE_Start(	const unsigned int	Capacity,					
								const unsigned int	*Control_word,
								const S_CONF_STA_info STA_info)
{
	for (unsigned int idx = 0; idx < Capacity; idx++)
	{
		unsigned int pln = (Control_word[idx] >>	4)	& 0xF;
		unsigned int vlt = (Control_word[idx] >>	0)	& 0xF;
		PE_CONT_Configure_Update_Start(pln, vlt, STA_info);
	}
}


void Bline_DMA_ZDB_Transfer	(	const unsigned int	(*Control_struct)[2])
{
	DMA_EC_reset_all();
	//	size_t tmp = (Cnt << 24) + (vlt << 20) + (Max << 10);
	for (unsigned int vlt = 0; vlt < 16; vlt++)
	{
		unsigned int tmp = Control_struct[vlt][0];
		unsigned int max = (tmp >> 10) & 0x000003FF;
		if (max != 0)
		{
			//	starting the transfer
			DMA_ZDBT_info[vlt] = (unsigned int)Control_struct[vlt];
			unsigned int cnt = (tmp >> 24);
			unsigned int* arr = (unsigned int*)Control_struct[vlt][1];
			DMA_start_transfer(vlt, vlt * 0x1000000, arr[0], cnt);
			// Enabling the interrupt 
			DMA_EC_CNTR_config(vlt, 1, 1, 0, 1);
			INTH_enable_intr_DMA_done(vlt);
		}
	}
}

void Bline_Initiate_MPDR	(	const unsigned int	Capacity,
								const unsigned int	*Control_word_H,
								const unsigned int	*Control_word_L,
								const void			*MPDR_base_addr_ptr)
{
	// Control word High
	//			field		:	vlt			Col			chan,		rounds		Out_Ival
	//			Hex pos		:	00,			0,			0,			00,			00
	//
	// Control word Low
	//			field		:	R1C1_Ival	R1C2_Ival	R2C1_Ival	R2C2_Ival
	//			Hex pos		:	00,			00,			00,			00
	//
	// Effective Address of DataBlocks
	//			Addr Low	:	Out_EA
	//					+1	:	In0_EA
	//					+2	:	In1_EA
	//					+3	:	In2_EA
	//					+4	:	In3_EA
	const unsigned int (*base_addr)[5] = static_cast<const unsigned int (*)[5]>((void*)MPDR_base_addr_ptr);
	for (unsigned int trd = 0; trd < Capacity; trd++)
	{
		PDR_EC_CNTR_config_start(Control_word_H[trd]);
		MPDR_Initiate_Start		(Control_word_H[trd], Control_word_L[trd], base_addr[trd]);
	}
}








