
#ifndef _UTILITIES_H_
#define _UTILITIES_H_


//#include "EventCallBacker.h"
//extern EventCallBacker_I* ECB;
extern unsigned int DMA_ZDBT_info[16];


#define		PLANE_0		0
#define		PLANE_1		1
#define		PLANE_2		2
#define		PLANE_3		3


#define     PE_0		0
#define     PE_1		1
#define     PE_2		2
#define     PE_3		3
#define     PE_4		4
#define     PE_5		5
#define     PE_6		6
#define     PE_7		7
#define     PE_8		8
#define     PE_9		9
#define     PE_10		10
#define     PE_11		11
#define     PE_12		12
#define     PE_13		13
#define     PE_14		14
#define     PE_15		15


#define     STA_0		0
#define     STA_1		1
#define     STA_2		2
#define     STA_3		3
#define     STA_4		4
#define     STA_5		5
#define     STA_6		6
#define     STA_7		7
#define     STA_8		8
#define     STA_9		9
#define     STA_10		10
#define     STA_11		11
#define     STA_12		12
#define     STA_13		13
#define     STA_14		14
#define     STA_15		15


#define     UPA_0		0
#define     UPA_1		1
#define     UPA_2		2
#define     UPA_3		3
#define     UPA_4		4
#define     UPA_5		5
#define     UPA_6		6
#define     UPA_7		7
#define     UPA_8		8
#define     UPA_9		9
#define     UPA_10		10
#define     UPA_11		11
#define     UPA_12		12
#define     UPA_13		13
#define     UPA_14		14
#define     UPA_15		15


#define     LMN_0		0
#define     LMN_1		1
#define     LMN_2		2
#define     LMN_3		3
#define     LMN_4		4
#define     LMN_5		5
#define     LMN_6		6
#define     LMN_7		7
#define     LMN_8		8
#define     LMN_9		9
#define     LMN_10		10
#define     LMN_11		11
#define     LMN_12		12
#define     LMN_13		13
#define     LMN_14		14
#define     LMN_15		15





void DMA_CallBack	(int cntrol_word);








void Bline_Initiate_STA_UPA(	const unsigned int	Capacity,
								const unsigned int	*Control_word,
								const unsigned int	*Counts,
								const unsigned int	*Ivals,
								const unsigned int	*UPA_Inp_base_addr_ptr,
								const unsigned int (*UPA_Wgt_base_addr_ptr)[9],
								const unsigned int	*UPA_Out_base_addr_ptr,
								const unsigned int	*UPA_Acc_base_addr_ptr);


void Bline_Initiate_STA_ECs(	const unsigned int	Capacity,					
								const unsigned int	*Control_word);


void Bline_Initiate_PE_Start(	const unsigned int	Capacity,					
								const unsigned int	*Control_word,
								const S_CONF_STA_info	STA_info);


void Bline_DMA_ZDB_Transfer	(	const unsigned int	(*Control_struct)[2]);


void Bline_Initiate_MPDR	(	const unsigned int	Capacity,
								const unsigned int	*Control_word_H,
								const unsigned int	*Control_word_L,
								const void			*MPDR_base_addr_ptr);






#endif
