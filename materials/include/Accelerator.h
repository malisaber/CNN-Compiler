#include "common.h"

#ifndef _ACCELERATOR_H_
#define _ACCELERATOR_H_



//class ACC_DMA
//{
//private:
//	/* data */
//	//C_PERIPHERAL_REG_DMA_READ_ADDRESS
	//C_PERIPHERAL_REG_DMA_WRITE_ADDRESS
	//C_PERIPHERAL_REG_DMA_TRANS_COUNT
	//C_PERIPHERAL_REG_DMA_CONTROL
	//DMA Address			TARGET
	//		0			DMA of LMN(1,1)
	//		1			DMA of LMN(1,1)
	//		2			DMA of LMN(1,1)
	//		3			DMA of LMN(1,1)
	//		4			DMA of LMN(1,2)
	//		5			DMA of LMN(1,2)
	//		6			DMA of LMN(1,2)
	//		7			DMA of LMN(1,2)
	//		8			DMA of LMN(2,1)
	//		9			DMA of LMN(2,1)
	//		10			DMA of LMN(2,1)
	//		11			DMA of LMN(2,1)
	//		12			DMA of LMN(2,2)
	//		13			DMA of LMN(2,2)
	//		14			DMA of LMN(2,2)
	//		15			DMA of LMN(2,2)
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	Control
	//	O	|	31	|	30		29		28		27		26		25		24		23		22		21	 	20	 	19		18		17		16	 |
	//	N	| start |======================================================= RESERVED =======================================================|
	//	T	
	//	R	
	//	O	|	15		14		13		12		11		10		9		8		7		6		5	|	4		3		2		1		0	 |
	//	L	|======================================= RESERVED ======================================|============= DMA Address ==============|
	//
	//		DMA Address			TARGET
	//				0			LMN(1,1)->DMA(0)
	//				1			LMN(1,1)->DMA(1)
	//				2			LMN(1,1)->DMA(2)
	//				3			LMN(1,1)->DMA(3)
	//				4			LMN(1,2)->DMA(4)
	//				5			LMN(1,2)->DMA(5)
	//				6			LMN(1,2)->DMA(6)
	//				7			LMN(1,2)->DMA(7)
	//				8			LMN(2,1)->DMA(8)
	//				9			LMN(2,1)->DMA(9)
	//				10			LMN(2,1)->DMA(10)
	//				11			LMN(2,1)->DMA(11)
	//				12			LMN(2,2)->DMA(12)
	//				13			LMN(2,2)->DMA(13)
	//				14			LMN(2,2)->DMA(14)
	//				15			LMN(2,2)->DMA(15)
	//				----------------------------
	//	NOTE:
	//		if the actual number of DMAs is less than 4 and 16 in MMNs and GMN
	//		respectively, the starting addresses for each MMNs remain constant.
	//		Additionally, any references or addresses pointing to DMAs beyond
	//		the existing ones won't have any impact on the system.
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void DMA_check_address(unsigned int DMA_add);
//	void DMA_reset_all();
//public:
//	ACC_DMA(){DMA_reset_all();};
//	~ACC_DMA(){DMA_reset_all();};
//	unsigned int DMA_start_transfer(unsigned int DMA_add, unsigned int read_add, unsigned int write_add, unsigned int cnt);
//	unsigned int DMA_status(unsigned int DMA_add, unsigned int& status);
//};

void DMA_reset_all						()
{
	*C_PERIPHERAL_REG_DMA_READ_ADDRESS = 0;
	*C_PERIPHERAL_REG_DMA_WRITE_ADDRESS= 0;
}

void DMA_wait_for_done					(unsigned int DMA_add)
{
	unsigned int val;
	while(true)
	{
		val = *(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + DMA_add);
		val &= (1 << C_Event_Event_Value_pos);
		if (val)
			break;
	}
}

void DMA_start_transfer					(unsigned int DMA_add, unsigned int read_add, unsigned int write_add, unsigned int cnt)
{
	DMA_wait_for_done(DMA_add);
	*C_PERIPHERAL_REG_DMA_READ_ADDRESS = read_add;
	*C_PERIPHERAL_REG_DMA_WRITE_ADDRESS= write_add;
	*C_PERIPHERAL_REG_DMA_TRANS_COUNT  = cnt;
	*C_PERIPHERAL_REG_DMA_CONTROL = (1 << C_DMA_Start_pos) | (DMA_add << 0);
}



// ACC_MPDR

void MPDR_reset_all						()
{
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	=	0;	
	*C_PERIPHERAL_REG_MPDR_CONFIG				=	0;	
	*C_PERIPHERAL_REG_MPDR_CONTTOL				=	0;	
}

void MPDR_wait_for_done					(unsigned int MPDR_add)
{
	unsigned int val;
	while(true)
	{
		val = *(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + MPDR_add);
		val &= (1 << C_Event_Event_Value_pos);
		if (val)
			break;
	}
}

void MPDR_Initiate						(unsigned int MPDR_add, S_Addressing_Info R1C1, S_Addressing_Info R1C2, S_Addressing_Info R2C1, S_Addressing_Info R2C2, S_Addressing_Info out)
{
	unsigned int val;

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (0 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C1.base_add;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_BA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C1.count;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_CA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C1.interval;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_IA_Wen_pos);

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (1 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C2.base_add;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_BA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C2.count;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_CA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C2.interval;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_IA_Wen_pos);

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (2 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C1.base_add;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_BA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C1.count;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_CA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C1.interval;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_IA_Wen_pos);

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (3 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C2.base_add;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_BA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C2.count;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_CA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C2.interval;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_IA_Wen_pos);

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (7 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= out.base_add;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_BA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= out.count;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_CA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= out.interval;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_IA_Wen_pos);
}

void MPDR_Start							(unsigned int MPDR_add, unsigned int Max_Colm, unsigned int Max_Chan, unsigned int keep)
{
	unsigned int val =	(MPDR_add <<	C_MPDR_Unit_Addres_pos) |
						(1 << C_MPDR_Start_pos) | 
						(keep << C_MPDR_Keep_pos) | 
						(1 << C_MPDR_Load_pos) | 
						(Max_Chan << C_MPDR_Max_Chan_pos) | 
						(Max_Colm << C_MPDR_Max_Colm_pos);
	*C_PERIPHERAL_REG_MPDR_CONTTOL = val;
}

void MPDR_Stop							(unsigned int MPDR_add)
{
	*C_PERIPHERAL_REG_MPDR_CONTTOL = 0;
}

void MPDR_Initiate_Start				(unsigned int H_Ivals,	unsigned int L_Ivals,	const unsigned int* BAs)
{
	//field			:	vlt			Col			chan,		rounds		Out_Ival
	//Hex pos		:	00,			0,			0,			00,			00	

	unsigned int MPDR_add	= ((H_Ivals >> 24) & 0xFF);
	unsigned int Max_colm	= ((H_Ivals >> 20) & 0xF);
	unsigned int Max_chan	= ((H_Ivals >> 16) & 0xF);
	unsigned int Outs_Ival	= ((H_Ivals >>  0) & 0xFF) << 8;
	unsigned int R1C1_Ival	= ((L_Ivals >> 24) & 0xFF) << 8;
	unsigned int R1C2_Ival	= ((L_Ivals >> 16) & 0xFF) << 8;
	unsigned int R2C1_Ival	= ((L_Ivals >>  8) & 0xFF) << 8;
	unsigned int R2C2_Ival	= ((L_Ivals >>  0) & 0xFF) << 8;

	unsigned int cnt = ((Max_colm+1) * (Max_chan+1))-1;
	unsigned int val;
	
	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (1 << C_MPDR_BA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= BAs[0];
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (7 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= BAs[1];
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (0 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= BAs[2];
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= BAs[3];
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (2 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= BAs[4];
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (3 << C_MPDR_Target_pos);

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (1 << C_MPDR_IA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C1_Ival;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (0 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R1C2_Ival;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C1_Ival;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (2 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= R2C2_Ival;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (3 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= Outs_Ival;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (7 << C_MPDR_Target_pos);

	val = (MPDR_add << C_MPDR_Unit_Addres_pos)	| (1 << C_MPDR_CA_Wen_pos);
	*C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS	= cnt;
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (0 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (1 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (2 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (3 << C_MPDR_Target_pos);
	*C_PERIPHERAL_REG_MPDR_CONFIG				= val | (7 << C_MPDR_Target_pos);


	val =	(MPDR_add 	<<	C_MPDR_Unit_Addres_pos) |
			(1			<< C_MPDR_Start_pos) | 
			(1			<< C_MPDR_Keep_pos) | 
			(1			<< C_MPDR_Load_pos) | 
			(Max_chan	<< C_MPDR_Max_Chan_pos) | 
			(Max_colm	<< C_MPDR_Max_Colm_pos);
	*C_PERIPHERAL_REG_MPDR_CONTTOL = val;
}



//class ACC_TIMERs
//{
//private:
//	/* data */
//	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_TIMER_0_CONTROL
	//	Control 	(Write Only)
	//	O	|	31	|	30	 |	29	 |	28	 |	27		26		25		24	|	23		22		21	 	20	 | 	19		18		17		16	 |
	//	N	|Enable |= init =| INT E | INT C |========= RESERVED ===========|=========== Clk Div ============|========= TOP(19:16) ==========|
	//	T	
	//	R	
	//	O	|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0	 |
	//	L	|========================================================== TOP(15:0) ===========================================================|
	//
	//			clk Div
	//				0	:	clk
	//				1	:	clk / 2
	//					:	
	//				i	:	clk / (2**i)
	//					:	
	//				15	:	clk / 65536
	//
	//------------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_TIMER_0_VALUE
	//	Value		(Read Only)
	//	a	|	31	|	30		29		28		27		26		25		24		23		22		21		20	 |	19		18		17		16	 |
	//	l	| INT V |======================================= RESERVED =======================================|========= Val(19:16) ==========|
	//	u	 
	//	e	
	//		|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0	 |
	//		|========================================================== Val(15:0) ===========================================================|
	//
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void TIMER_check_address(unsigned int timer_add);
//	void TIMER_reset_all();
//public:
//	ACC_TIMERs(){TIMER_reset_all();};
//	~ACC_TIMERs(){TIMER_reset_all();};
//	unsigned int TIMER_config(unsigned int timer_add, unsigned int top, unsigned int clk_dev, unsigned int intr_en);
//	unsigned int TIMER_start(unsigned int timer_add);
//	unsigned int TIMER_stop(unsigned int timer_add);
//	unsigned int TIMER_restart(unsigned int timer_add);
//	unsigned int TIMER_get_value(unsigned int timer_add, unsigned int& value, unsigned int& intr_status);
//};

void TIMER_reset_all					()
{
	for (unsigned int i=0; i<16; i++)
		*(C_PERIPHERAL_REG_TIMER_0_CONTROL+i) = 0;
}

void TIMER_config						(unsigned int timer_add, unsigned int top, unsigned int clk_dev, unsigned int intr_en)
{
	//	clk_dev:
	//		n	->	clk_timer	=	clk_system / (2**n);
	unsigned int tmp = 0;
	tmp |= (1 << C_Timer_Init_pos);
	tmp |= (intr_en << C_Timer_Int_Enable_pos);
	tmp |= (1 << C_Timer_Int_clear_pos);
	tmp |= ((clk_dev & 0xFFFF) << C_Timer_Clk_Div_pos);
	tmp |= top & 0XFFFFF;
	*(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add) = tmp;
}

void TIMER_start						(unsigned int timer_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add);
	tmp |= (1 << C_Timer_Enable_pos);
	*(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add) = tmp;
}

void TIMER_stop							(unsigned int timer_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add);
	tmp &= ~(1 << C_Timer_Enable_pos);
	*(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add) = tmp;
}

void TIMER_restart						(unsigned int timer_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add);
	tmp |= (1 << C_Timer_Int_clear_pos);
	tmp |= (1 << C_Timer_Init_pos);
	*(C_PERIPHERAL_REG_TIMER_0_CONTROL + 2*timer_add) = tmp;
}

void TIMER_get_value					(unsigned int timer_add, unsigned int& value, unsigned int& intr_status)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_TIMER_0_VALUE + 2*timer_add);
	value = tmp & 0xFFFFF;
	intr_status = ((tmp & (1 << C_Timer_Int_Val_pos)) >> C_Timer_Int_Val_pos);
}



//class ACC_DMA_EC
//{
//private:
//	/* data */
//	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE
	//	Control
	//	O	|	31	|	30	 |	29	 |	28	 |	27	 |	26	|	25	|	24	 |	23		22		21	 	20	 	19		18		17		16	 |
	//	N	|Enable |= init =| Stuck | INT E | INT C | SENS | INT V | Evnt V |========================== MAX(11:4) ==========================|
	//	T	
	//	R	
	//	O	|	15		14		13		12	|	11		10		9		8		7		6		5		4		3		2		1		0	 |
	//	L	|========== MAX(11:4) ==========|============================================= VAL ==============================================|
	//
	//		Sensitivity		:		sensitive to
	//				0		:		falling edge
	//				1		:		rising edge
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void DMA_EC_check_address(unsigned int SEC_add);
//	void DMA_EC_reset_all();
//public:
//	ACC_DMA_EC(){DMA_EC_reset_all();};
//	~ACC_DMA_EC(){DMA_EC_reset_all();};
//	unsigned int DMA_EC_CNTR_config(unsigned int SEC_add, unsigned int max, unsigned int sens_level, unsigned int repeat, unsigned int intr_en);
//	unsigned int DMA_EC_CNTR_start(unsigned int SEC_add);
//	unsigned int DMA_EC_CNTR_stop(unsigned int SEC_add);
//	unsigned int DMA_EC_CNTR_restart(unsigned int SEC_add);
//	unsigned int DMA_EC_CNTR_get_value(unsigned int SEC_add, unsigned int& value, unsigned int& event_status, unsigned int& intr_status);
//};

void DMA_EC_reset_all					()
{
	for (unsigned int i=0; i<16; i++)
		*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE+i) = 0;
}

void DMA_EC_reset						(unsigned int EC_add)
{
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE+EC_add) = 0;
}

void DMA_EC_CNTR_config					(unsigned int EC_add, unsigned int max, unsigned int sense_level, unsigned int repeat, unsigned int intr_en)
{
	//	sense level:
	//			0	->	faling Edge
	//			1	->	rising Edge
	unsigned int tmp = 0;
	tmp |= ((max & 0xFF) << C_Event_Max_pos);
	tmp |= (sense_level << C_Event_Sensitity_pos);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (intr_en << C_Event_Int_Enabel_pos);
	tmp |= (repeat << C_Event_Stuck_at_Top_pos);
	tmp |= (1 << C_Event_Init_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add) = tmp;
}

void DMA_EC_CNTR_start					(unsigned int EC_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add);
	tmp |= (1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add) = tmp;
}

void DMA_EC_CNTR_stop					(unsigned int EC_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add);
	tmp &= ~(1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add) = tmp;
}

void DMA_EC_CNTR_restart				(unsigned int EC_add)
{
	 unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (1 << C_Event_Init_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add) = tmp;
}

void DMA_EC_CNTR_get_value				(unsigned int EC_add, unsigned int& value, unsigned int& event_status, unsigned int& intr_status)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add);
	value = tmp % (1 << C_Event_Max_pos);
	event_status = ((tmp & (1 << C_Event_Event_Value_pos)) >> C_Event_Event_Value_pos); 
	intr_status = ((tmp & (1 << C_Event_Int_Value_pos)) >> C_Event_Int_Value_pos);
}

void PDR_EC_CNTR_config_start			(unsigned int H_Cont_Word)
{
	//	sense level:
	//			0	->	faling Edge
	//			1	->	rising Edge

	unsigned int EC_add		= ((H_Cont_Word >> 24) & 0xFF);
	unsigned int TOP		= ((H_Cont_Word >>  8) & 0xFF);

	unsigned int tmp = 0;
	tmp |= (TOP << C_Event_Max_pos);
	tmp |= (1 << C_Event_Sensitity_pos);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (1 << C_Event_Int_Enabel_pos);
	tmp |= (0 << C_Event_Stuck_at_Top_pos);
	tmp |= (1 << C_Event_Init_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add) = tmp;
	tmp |= (1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE + EC_add) = tmp;
}



//class ACC_MPDR_EC
//{
//private:
//	/* data */
//public:
//	ACC_MPDR_EC(/* args */);
//	~ACC_MPDR_EC();
//};
//
//ACC_MPDR_EC::ACC_MPDR_EC(/* args */)
//{
//}
//
//ACC_MPDR_EC::~ACC_MPDR_EC()
//{
//}

void MPDR_EC_reset_all					()
{
	for (unsigned int i=0; i<16; i++)
		*(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE+i) = 0;
}

void MPDR_EC_reset_all					(unsigned int EC_add)
{
	*(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE+EC_add) = 0;
}

void MPDR_EC_CNTR_config				(unsigned int EC_add, unsigned int max, unsigned int sense_level, unsigned int repeat, unsigned int intr_en)
{
	//	sense level:
	//			0	->	faling Edge
	//			1	->	rising Edge
	unsigned int tmp = 0;
	tmp |= ((max & 0xFFF) << C_Event_Max_pos);
	tmp |= (sense_level << C_Event_Sensitity_pos);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (intr_en << C_Event_Int_Enabel_pos);
	tmp |= (repeat << C_Event_Stuck_at_Top_pos);
	tmp |= (1 << C_Event_Init_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add) = tmp;
}

void MPDR_EC_CNTR_start					(unsigned int EC_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add);
	tmp |= (1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add) = tmp;
}

void MPDR_EC_CNTR_stop					(unsigned int EC_add)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add);
	tmp &= ~(1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add) = tmp;
}

void MPDR_EC_CNTR_restart				(unsigned int EC_add)
{
	 unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (1 << C_Event_Init_pos);
	*(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add) = tmp;
}

void MPDR_EC_CNTR_get_value				(unsigned int EC_add, unsigned int& value, unsigned int& event_status, unsigned int& intr_status)
{
	unsigned int tmp = *(C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE + EC_add);
	value = tmp % (1 << C_Event_Max_pos);
	event_status = ((tmp & (1 << C_Event_Event_Value_pos)) >> C_Event_Event_Value_pos); 
	intr_status = ((tmp & (1 << C_Event_Int_Value_pos)) >> C_Event_Int_Value_pos);
}



//class ACC_CONTROL_REG
//{
//private:
//	/* data */
//	//----------------------------------------------------------------------------------------------------------------------------------------
//	//	C_PERIPHERAL_REG_CONTROL_REGISTE
//	//	Control 	(Write Only)
//	//	O	|	31	|	30	|	29		28		27		26		25		24		23		22		21	 	20	 	19		18		17		16	 |
//	//	N	|Normal |Connect|=================================================== RESERVED ===================================================|
//	//	T	
//	//	R	
//	//	O	|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0	 |
//	//	L	|========================================================== RESERVED ============================================================|
//	//
//	//----------------------------------------------------------------------------------------------------------------------------------------
//public:
//	ACC_CONTROL_REG(){CONT_REG_ACC_reset(); CONT_REG_DRAM_disconnect();};
//	~ACC_CONTROL_REG(){CONT_REG_ACC_reset(); CONT_REG_DRAM_disconnect();};
//	void CONT_REG_ACC_normal();
//	void CONT_REG_ACC_reset();
//	void CONT_REG_DRAM_connect();
//	void CONT_REG_DRAM_disconnect();
//};

inline void CONT_REG_ACC_Plane_normal	(unsigned int plane)
{
	*C_PERIPHERAL_REG_CONTROL_REGISTE = *C_PERIPHERAL_REG_CONTROL_REGISTE | (1 << plane);
}

inline void CONT_REG_ACC_Plane_reset	(unsigned int plane)
{
	*C_PERIPHERAL_REG_CONTROL_REGISTE = *C_PERIPHERAL_REG_CONTROL_REGISTE & (~(1 << plane));
}

inline void CONT_REG_DRAM_connect		()
{
	*C_PERIPHERAL_REG_CONTROL_REGISTE = *C_PERIPHERAL_REG_CONTROL_REGISTE | (1 << C_Accelerator_Connect_pos);
}

inline void CONT_REG_DRAM_disconnect	()
{
	*C_PERIPHERAL_REG_CONTROL_REGISTE = *C_PERIPHERAL_REG_CONTROL_REGISTE & (~(1 << C_Accelerator_Connect_pos));
}



//class ACC_USART
//{
//private:
//	/* data */
//public:
//	ACC_USART(/* args */);
//	~ACC_USART();
//};
//
//ACC_USART::ACC_USART(/* args */)
//{
//}
//
//ACC_USART::~ACC_USART()
//{
//}
//

void UART_reset							()
{
	*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD = 0;
}

void UART_Initiate						(unsigned int Cont_Word)
{
	*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD = Cont_Word;
}

void UART_read_flags					(unsigned int& TBE_flg, unsigned int& TBF_flg, unsigned int& RBE_flg, unsigned int& RBF_flg)
{
	unsigned int word = *C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD;
	TBE_flg = (word >> C_TRx_Unit_Tx_Buff_Empty_Flg_pos	) & 0x1;
	TBF_flg = (word >> C_TRx_Unit_Tx_Buff_Full_Flg_pos	) & 0x1;
	RBE_flg = (word >> C_TRx_Unit_Rx_Buff_Empty_Flg_pos	) & 0x1;
	RBF_flg = (word >> C_TRx_Unit_Rx_Buff_Full_Flg_pos	) & 0x1;
}

void UART_write_data					(unsigned int Data_Word)
{
	*C_PERIPHERAL_REG_TRx_UNIT_DATA_WORD = Data_Word;
}

void UART_read_data						(unsigned int& Data_Word, unsigned int& DOR_error)
{
	Data_Word = *C_PERIPHERAL_REG_TRx_UNIT_DATA_WORD;
	DOR_error = Data_Word >> C_TRx_Unit_Rx_DORE_Flg_pos;
	Data_Word = Data_Word & 0Xff;
}

void UART_read_TBF_flag					(unsigned int& TBF_flg)
{
	TBF_flg = (*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD >> C_TRx_Unit_Tx_Buff_Full_Flg_pos	) & 0x1;
}

void UART_read_RBE_flag					(unsigned int& RBE_flg)
{
	TBF_flg = (*C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD >> C_TRx_Unit_Rx_Buff_Empty_Flg_pos	) & 0x1;
}





//class ACC_INTR_HANDLER
//{
//private:
//	/* data */
//	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_0
	//	INT ENABLE 0
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|= DMA 4,4 Rdy =|= DMA 4,3 Rdy =|= DMA 4,2 Rdy =|= DMA 4,1 Rdy =|= DMA 3,4 Rdy =|= DMA 3,3 Rdy =|= DMA 3,2 Rdy =|= DMA 3,1 Rdy =|
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|= DMA 2,4 Rdy =|= DMA 2,3 Rdy =|= DMA 2,2 Rdy =|= DMA 2,1 Rdy =|= DMA 1,4 Rdy =|= DMA 1,3 Rdy =|= DMA 1,2 Rdy =|= DMA 1,1 Rdy =|
	//	A
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	|=== TIMER 7 ===|=== TIMER 6 ===|=== TIMER 5 ===|=== TIMER 4 ===|=== TIMER 3 ===|=== TIMER 2 ===|=== TIMER 1 ===|=== TIMER 0 ===|
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	0	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|=== PC  Req ===|
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_1
	//	INT ENABLE 1
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|STA 1,4,4 Done |STA 1,4,3 Done |STA 1,4,2 Done |STA 1,4,1 Done |STA 1,3,4 Done |STA 1,3,3 Done |STA 1,3,2 Done |STA 1,3,1 Done |
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|STA 1,2,4 Done |STA 1,2,3 Done |STA 1,2,2 Done |STA 1,2,1 Done |STA 1,1,4 Done |STA 1,1,3 Done |STA 1,1,2 Done |STA 1,1,1 Done |
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	| PE 1,4,4 Done | PE 1,4,3 Done | PE 1,4,2 Done | PE 1,4,1 Done | PE 1,3,4 Done | PE 1,3,3 Done | PE 1,3,2 Done | PE 1,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	1	| PE 1,2,4 Done | PE 1,2,3 Done | PE 1,2,2 Done | PE 1,2,1 Done | PE 1,1,4 Done | PE 1,1,3 Done | PE 1,1,2 Done | PE 1,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_2
	//	INT ENABLE 2
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	|UPA 1,4,4 Done |UPA 1,4,3 Done |UPA 1,4,2 Done |UPA 1,4,1 Done |UPA 1,3,4 Done |UPA 1,3,3 Done |UPA 1,3,2 Done |UPA 1,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	2	|UPA 1,2,4 Done |UPA 1,2,3 Done |UPA 1,2,2 Done |UPA 1,2,1 Done |UPA 1,1,4 Done |UPA 1,1,3 Done |UPA 1,1,2 Done |UPA 1,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_3
	//	INT ENABLE 3
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|STA 2,4,4 Done |STA 2,4,3 Done |STA 2,4,2 Done |STA 2,4,1 Done |STA 2,3,4 Done |STA 2,3,3 Done |STA 2,3,2 Done |STA 2,3,1 Done |
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|STA 2,2,4 Done |STA 2,2,3 Done |STA 2,2,2 Done |STA 2,2,1 Done |STA 2,1,4 Done |STA 2,1,3 Done |STA 2,1,2 Done |STA 2,1,1 Done |
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	| PE 2,4,4 Done | PE 2,4,3 Done | PE 2,4,2 Done | PE 2,4,1 Done | PE 2,3,4 Done | PE 2,3,3 Done | PE 2,3,2 Done | PE 2,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	3	| PE 2,2,4 Done | PE 2,2,3 Done | PE 2,2,2 Done | PE 2,2,1 Done | PE 2,1,4 Done | PE 2,1,3 Done | PE 2,1,2 Done | PE 2,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_4
	//	INT ENABLE 4
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	|UPA 2,4,4 Done |UPA 2,4,3 Done |UPA 2,4,2 Done |UPA 2,4,1 Done |UPA 2,3,4 Done |UPA 2,3,3 Done |UPA 2,3,2 Done |UPA 2,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	4	|UPA 2,2,4 Done |UPA 2,2,3 Done |UPA 2,2,2 Done |UPA 2,2,1 Done |UPA 2,1,4 Done |UPA 2,1,3 Done |UPA 2,1,2 Done |UPA 2,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_5
	//	INT ENABLE 5
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|STA 3,4,4 Done |STA 3,4,3 Done |STA 3,4,2 Done |STA 3,4,1 Done |STA 3,3,4 Done |STA 3,3,3 Done |STA 3,3,2 Done |STA 3,3,1 Done |
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|STA 3,2,4 Done |STA 3,2,3 Done |STA 3,2,2 Done |STA 3,2,1 Done |STA 3,1,4 Done |STA 3,1,3 Done |STA 3,1,2 Done |STA 3,1,1 Done |
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	| PE 3,4,4 Done | PE 3,4,3 Done | PE 3,4,2 Done | PE 3,4,1 Done | PE 3,3,4 Done | PE 3,3,3 Done | PE 3,3,2 Done | PE 3,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	5	| PE 3,2,4 Done | PE 3,2,3 Done | PE 3,2,2 Done | PE 3,2,1 Done | PE 3,1,4 Done | PE 3,1,3 Done | PE 3,1,2 Done | PE 3,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_6
	//	INT ENABLE 6
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	|UPA 3,4,4 Done |UPA 3,4,3 Done |UPA 3,4,2 Done |UPA 3,4,1 Done |UPA 3,3,4 Done |UPA 3,3,3 Done |UPA 3,3,2 Done |UPA 3,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	6	|UPA 3,2,4 Done |UPA 3,2,3 Done |UPA 3,2,2 Done |UPA 3,2,1 Done |UPA 3,1,4 Done |UPA 3,1,3 Done |UPA 3,1,2 Done |UPA 3,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_7
	//	INT ENABLE 7
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|STA 4,4,4 Done |STA 4,4,3 Done |STA 4,4,2 Done |STA 4,4,1 Done |STA 4,3,4 Done |STA 4,3,3 Done |STA 4,3,2 Done |STA 4,3,1 Done |
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|STA 4,2,4 Done |STA 4,2,3 Done |STA 4,2,2 Done |STA 4,2,1 Done |STA 4,1,4 Done |STA 4,1,3 Done |STA 4,1,2 Done |STA 4,1,1 Done |
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	| PE 4,4,4 Done | PE 4,4,3 Done | PE 4,4,2 Done | PE 4,4,1 Done | PE 4,3,4 Done | PE 4,3,3 Done | PE 4,3,2 Done | PE 4,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	7	| PE 4,2,4 Done | PE 4,2,3 Done | PE 4,2,2 Done | PE 4,2,1 Done | PE 4,1,4 Done | PE 4,1,3 Done | PE 4,1,2 Done | PE 4,1,1 Done |
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ENABLE_8
	//	INT ENABLE 8
	//	N	|		31		|		30		|		29		|		28		|		27		|		26		|		25		|		24		|
	//	T	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//		
	//	E	|		23		|		22		|		21		|		20		|		19		|		18		|		17		|		16		|
	//	N	|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|== RESERVED ===|
	//	A	
	//	B	|		15		|		14		|		13		|		12		|		11		|		10		|		9		|		8		|
	//	L	|UPA 4,4,4 Done |UPA 4,4,3 Done |UPA 4,4,2 Done |UPA 4,4,1 Done |UPA 4,3,4 Done |UPA 4,3,3 Done |UPA 4,3,2 Done |UPA 4,3,1 Done |
	//	E	
	//		|		7		|		6		|		5		|		4		|		3		|		2		|		1		|		0		|
	//	8	|UPA 4,2,4 Done |UPA 4,2,3 Done |UPA 4,2,2 Done |UPA 4,2,1 Done |UPA 4,1,4 Done |UPA 4,1,3 Done |UPA 4,1,2 Done |UPA 4,1,1 Done |
	//		
	//	
	//	Interrupt Priority:
	//		INT_EN_0_b0	> INT_EN_0_b1 > ... > INT_EN_0_b31 > INT_EN_1_b0 > > INT_EN_1_b1 > ... > INT_EN_8_b0 > INT_EN_8_b1 > ... > INT_EN_8_b31
	//		Higher		>																										 > LOWER
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ADDRESS		
	//	INT ADDRESS
	//	N	|	31		30		29		28		27		26		25		24		23		22		21	 	20	 	19		18		17		16	 |
	//	T	|==================== Previous Int Address =====================|========================= INT Address ==========================|
	//		
	//	A	
	//	D	|	15		14		13		12		11		10		9		8		7		6		5		4		3	|	2		1		0	 |
	//	D	|============================================== RESERVED ===============================================|== INT Address Decode ==|
	//
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_REG_INTERRUPT_ACKNOWLEDGE	
	//	INT ACKNOWLEDGE
	//	N	|	31		30		29		28		27		26		25		24		23		22		21	 	20	 	19		18		17		16	 |
	//	T	|========================================================== RESERVED ============================================================|
	//		
	//	A	
	//	C	|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1	|	0	 |
	//	K	|====================================================== RESERVED =======================================================|= ACK  =|
	//
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	RESERVED
	//	E	|	31		30		29		28		27		26		25		24		23		22		21		20		19		18		17		16	 |
	//	S	|=========================================================== RESERVED ===========================================================|
	//	E	 
	//	R	
	//	V	|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0	 |
	//	E	|=========================================================== RESERVED ===========================================================|
	//	D
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void INTH_reset_all();
//public:
//	ACC_INTR_HANDLER(){INTH_reset_all();};
//	~ACC_INTR_HANDLER(){INTH_reset_all();};
//	void INTH_enable_intr_pc();
//	void INTH_enable_intr_timer(unsigned int timer_add);
//	void INTH_enable_intr_DMA_done(unsigned int dma_add);
//	void INTH_enable_PE_done(unsigned int plane, unsigned int pe_add);
//	void INTH_enable_STA_done(unsigned int plane, unsigned int pe_add);
//	void INTH_enable_UPA_done(unsigned int plane, unsigned int pe_add);
//	
//	void INTH_disable_intr_pc();
//	void INTH_disable_intr_timer(unsigned int timer_add);
//	void INTH_disable_intr_DMA_done(unsigned int dma_add);
//	void INTH_disable_PE_done(unsigned int plane, unsigned int pe_add);
//	void INTH_disable_STA_done(unsigned int plane, unsigned int pe_add);
//	void INTH_disable_UPA_done(unsigned int plane, unsigned int pe_add);
//
//	void INTH_get_intr_address(unsigned int& intr_address, unsigned int& intr_prev_address);
//};

void INTH_reset_all						()
{
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC			) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER		) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR		) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA			) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0	) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_1	) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_2	) = 0;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_3	) = 0;
}

void INTH_enable_intr_pc				()
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC | (1 << C_INT_PC_Req_pos);
}

void INTH_enable_intr_TBE				()
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC | (1 << C_INT_TBE_pos);
}

void INTH_enable_intr_RBF				()
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC | (1 << C_INT_RBF_pos);
}

void INTH_enable_intr_Tx_Done			()
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC | (1 << C_INT_TXD_pos);
}

void INTH_enable_intr_Rx_Done			()
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC | (1 << C_INT_RXD_pos);
}

void INTH_enable_intr_timer				(unsigned int timer_add)
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER | (1 << (C_INT_Timer_0_pos+timer_add));
}

void INTH_enable_intr_timer_group		(unsigned int timer_group)
{
	unsigned int group = timer_group & 0xFF;
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER | (group << C_INT_Timer_0_pos);
}

void INTH_enable_intr_MPDR_done			(unsigned int mpdr_add)
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR | (1 << (C_INT_MPDR_0_Done_pos+mpdr_add));
}

void INTH_enable_intr_MPDR_done_group	(unsigned int mpdr_group)
{
	unsigned int group = mpdr_group & 65535;
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR | (group << C_INT_MPDR_0_Done_pos);
}

void INTH_enable_intr_DMA_done			(unsigned int dma_add)
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA | (1 << (C_INT_DMA_0_Done_pos+dma_add));
}

void INTH_enable_intr_DMA_done_group	(unsigned int dma_group)
{
	unsigned int group = dma_group & 65535;
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA | (group << C_INT_DMA_0_Done_pos);
}

void INTH_enable_PSU_done				(unsigned int plane, unsigned int pe_add)
{
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) = *(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) | (1 << (C_INT_PSU_1_1_Done_pos+pe_add));
}

void INTH_enable_PSU_done_group			(unsigned int plane, unsigned int pe_group)
{
	unsigned int group = pe_group & 0xFFFF;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) = *(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) | (group << C_INT_PSU_1_1_Done_pos);
}

void INTH_disable_intr_pc				()
{
	 *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC & (~(1 << C_INT_PC_Req_pos));
}

void INTH_disable_intr_TBE				()
{
	 *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC & (~(1 << C_INT_TBE_pos));
}

void INTH_disable_intr_RBF				()
{
	 *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC & (~(1 << C_INT_RBF_pos));
}

void INTH_disable_Tx_Done				()
{
	 *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC & (~(1 << C_INT_TXD_pos));
}

void INTH_disable_Rx_Done				()
{
	 *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC & (~(1 << C_INT_RXD_pos));
}

void INTH_disable_intr_timer			(unsigned int timer_add)
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER & (~(1 << (C_INT_Timer_0_pos+timer_add)));
}

void INTH_disable_intr_timer_group		(unsigned int timer_group)
{
	unsigned int group = timer_group & 0xFF;
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER & (~(group << C_INT_Timer_0_pos));
}

void INTH_disable_intr_MPDR_done		(unsigned int mpdr_add)
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR & (~(1 << (C_INT_MPDR_0_Done_pos+mpdr_add)));
}

void INTH_disable_intr_MPDR_done_group	(unsigned int mpdr_group)
{
	unsigned int group = mpdr_group & 0xFFFF;
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR & (~(group << C_INT_MPDR_0_Done_pos));
}

void INTH_disable_intr_DMA_done			(unsigned int dma_add)
{
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA & (~(1 << (C_INT_DMA_0_Done_pos+dma_add)));
}

void INTH_disable_intr_DMA_done_group	(unsigned int dma_group)
{
	unsigned int group = dma_group & 0xFFFF;
	*C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA = *C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA & (~(group << C_INT_DMA_0_Done_pos));
}

void INTH_disable_PSU_done				(unsigned int plane, unsigned int pe_add)
{
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) = *(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) & (~(1 << (C_INT_PSU_1_1_Done_pos+pe_add)));
}

void INTH_disable_PSU_done_group		(unsigned int plane, unsigned int pe_group)
{
	unsigned int group = pe_group & 0xFFFF;
	*(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) = *(C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0+plane) & (~(group << C_INT_PSU_1_1_Done_pos));
}

inline 
void INTH_get_intr_address				(unsigned int& next_intr_address, unsigned int& this_intr_address, unsigned int& next_intr_code, unsigned int& this_intr_code)
{
	unsigned int value = *(C_PERIPHERAL_REG_INTERRUPT_ADDRESS);
	next_intr_address	=	(value >> C_INT_NEXT_Address_pos)	& 0x000000FF;
	this_intr_address	=	(value >> C_INT_THIS_Address_pos)	& 0x000000FF;
	next_intr_code		=	(value >> C_INT_NEXT_code_pos)		& 0x0000000F;
	this_intr_code		=	(value >> C_INT_THIS_code_pos)		& 0x0000000F;
}

inline 
void INTH_get_intr_address				(unsigned int& this_intr_address, unsigned int& this_intr_code)
{
	unsigned int value = *(C_PERIPHERAL_REG_INTERRUPT_ADDRESS);
	this_intr_address	=	(value >> C_INT_THIS_Address_pos)	& 0x000000FF;
	this_intr_code		=	(value >> C_INT_THIS_code_pos)		& 0x0000000F;
}



//class CONF_HOLDER
//{
//private:
//	/* data */
//	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_1_H
	//		
	//	H	|	63		62		61		60		59		58		57		56		55		54		53		52		51		50		49		48	 |
	//	I	|====================================================	RESERVED	=============================================================|
	//	G	
	//	H	
	//	E	|	47		46		45	|	44	|	43		42		41		40		39	|	38	|	37	|	36	|	35		34	|	33		32	 |
	//	R	| RSRVD |=== FSM Sel ===| PESrc |============= Shift Count =============|= F/H =|IFM_NS |WFM_NS |= Address sel =|= Zero pad max =|
	//		
	//		
	//	---------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_1_L	
	//		
	//	L	|	31		30		29		28	|	27		26		25		24	|	23		22		21		20	|	19		18		17		16	 |
	//	O	|========== Kernel Max =========|========== Column Max =========|========= Channel Max =========|=========== Row Max ============|
	//	W	
	//	E	
	//	E	|	15		14		13		12	|	11		10		9		8	|	7		6		5		4	|	3		2	|	1		0	 |
	//		|========= counter Max =========|=========== Bank Min ==========|=========== Bank Max ==========|== GBank Min ==|== GBank Max ===|
	//		
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void CONFH_check_address(unsigned int pe_add);
//	void CONFH_reset_all();
//	void plane_add;
//public:
//	CONF_HOLDER(unsigned int PBA){CONFH_reset_all(); plane_add = PBA; };
//	~CONF_HOLDER(){CONFH_reset_all();};
//	unsigned int CONFH_set_conf(unsigned int pe_add, S_PE_cofig cnf);
//};

void CONFH_reset_all					(unsigned int plane_add)
{
	for (unsigned int i=0; i<16; i++)
		*(C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + i) = 0;
}

void CONFH_set_conf						(unsigned int plane_add, unsigned int pe_add, S_PE_cofig cnf)
{
	unsigned int val = 0;
	val	|=	(cnf.E_Chnl_max		<<	C_Channel_MAX_pos		);
	val	|=	(cnf.E_Colm_max		<<	C_Column_MAX_pos		);
	val	|=	(cnf.E_Kern_max		<<	C_Kernel_MAX_pos		);
	val	|=	(cnf.E_Zpad_max		<<	C_ZPad_MAX_pos			);
	val	|=	(cnf.WFM_NS			<<	C_WFM_Numeric_pos		);
	val	|=	(cnf.IFM_NS			<<	C_IFM_Numeric_pos		);
	val	|=	(cnf.FH_Arith		<<	C_Computation_Size_pos	);
	val	|=	(cnf.E_Shift_cnt	<<	C_Shift_Count_pos		);
	val	|=	(cnf.FSM_Src		<<	C_FSM_SEL_pos			);
	*(C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = val;
}

void CONFH_refresh						(unsigned int plane_add)
{
	CONT_REG_ACC_Plane_reset(plane_add);
	CONT_REG_ACC_Plane_normal(plane_add);
}



//class PE_INIT
//{
//private:
//	/* data */
//	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_VALUE
	//	Store Agent BIAS Value
	//	A	|	31		30		29		28		27		26		25		24		23		22		21	 	20	 	19		18		17		16	 |
	//	B	|=========================================================== RESERVED ===========================================================|
	//	V	
	//		
	//		|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0	 |
	//		|========================================================= Bias Value ===========================================================|
	//
	//------------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_CONTROL
	//	Store Agent BIAS Control
	//	A	|	31	|	30		29		28		27		26		25		24		23		22		21		20		19		18		17		16	 |
	//	B	|= Wen =|======================================================= RESERVED =======================================================|
	//	C	 
	//		
	//		|	15		14		13		12		11		10		9		8		7		6		5		4	|	3		2		1		0	 |
	//		|========================================== RESERVED ===========================================|======== Kernel Address ========|
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT
	//	Address Point
	//	P	|	31		30		29		28		27		26		25		24		23		22		21	 	20	 	19		18		17		16	 |
	//		|=========================================================== RESERVED ===========================================================|
	//		
	//		
	//		|	15		14		13		12		11		10		9		8		7		6		5		4		3		2		1		0	 |
	//		|======================================================== Address Value =========================================================|
	//
	//------------------------------------------------------------------------------------------------------------------------------------------
	//	C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR
	//	Address Point Control
	//	P	|	31	|	30	|	29	|	28	|	27		26		25		24		23		22		21		20	 |	19		18		17		16	 |
	//	C	| B Wen | C Wen | I Wen |SA/ ~UA|=========================== RESERVED ===========================|======== Unit Address =========|
	//		
	//		
	//		|	15		14		13		12		11		10		9		8		7		6		5		4	 |	3		2		1		0	 |
	//		|================================================== RESERVED ====================================|======= Target Address ========|
	//
	//			TARGGET	ADDRESS	(Update Agent)
	//						0		:	WEIGHT MEMORY BANK #(1,1)
	//						1		:	WEIGHT MEMORY BANK #(1,2)
	//						2		:	WEIGHT MEMORY BANK #(1,3)
	//						3		:	WEIGHT MEMORY BANK #(2,1)
	//						4		:	WEIGHT MEMORY BANK #(2,2)
	//						5		:	WEIGHT MEMORY BANK #(2,3)
	//						6		:	WEIGHT MEMORY BANK #(3,1)
	//						7		:	WEIGHT MEMORY BANK #(3,2)
	//						8		:	WEIGHT MEMORY BANK #(3,3)
	//						9		:	RESERVED
	//						10		:	RESERVED
	//						11		:	RESERVED
	//						12		:	INPUT MEMORY BANK #4
	//						13		:	INPUT MEMORY BANK #3
	//						14		:	INPUT MEMORY BANK #2
	//						15		:	INPUT MEMORY BANK #1
	//------------------------------------------------------------------------
	//			TARGGET	ADDRESS	(Store Agent)
	//						0		:	LOAD	ROW from
	//						1		:	STORE	ROW at
	//						2		:	RESERVED
	//						3		:	RESERVED
	//						4		:	RESERVED
	//						5		:	RESERVED
	//						6		:	RESERVED
	//						7		:	RESERVED
	//						8		:	RESERVED
	//						9		:	RESERVED
	//						10		:	RESERVED
	//						11		:	RESERVED
	//						12		:	RESERVED
	//						13		:	RESERVED
	//						14		:	RESERVED
	//						15		:	RESERVED
	//------------------------------------------------------------------------
	//			Unit Address			row		col
	//						0000	:	1		1
	//						0001	:	1		2
	//						0010	:	1		3
	//						0011	:	1		4
	//						0100	:	2		1
	//						0101	:	2		2
	//						0110	:	2		3
	//						0111	:	2		4
	//						1000	:	3		1
	//						1001	:	3		2
	//						1010	:	3		3
	//						1011	:	3		4
	//						1100	:	4		1
	//						1101	:	4		2
	//						1110	:	4		3
	//						1111	:	4		4
	//------------------------------------------------------------------------
	//	Initialization procedure:
	//		BIASes:
	//			1)	SET		Unit Address					@		C_PERIPHERAL_REG_SAU_INITIATE_ADDRESS_POINT_CNTR
	//			2)	PUT		Bias Value						@		C_PERIPHERAL_REG_SAU_INITIATE_BIAS_VALUE
	//			3)	SET		Kernel Address &	Wen			@		C_PERIPHERAL_REG_SAU_INITIATE_BIAS_CONTROL
	//			4)	repeat 2 and 3			until you initiate all kernel biases for unit r,c
	//			5)	repeat 1, 2, 3 and 4	until you initiate all kernel biases for all units
	//			
	//		ASSRESSes:
	//			1)	PUT		Address Value					@		C_PERIPHERAL_REG_SAU_INITIATE_ADDRESS_POINT
	//			2)	SET		different part of				@		C_PERIPHERAL_REG_SAU_INITIATE_ADDRESS_POINT_CNTR
	//			3)	repeat 1 and 2 for all Target Address of SU and CU in each Unit Address. (total of 64 times)
	//			
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void PE_INIT_check_address(unsigned int pe_add);
//	void PE_INIT_reset_all();
//	void plane_add;
//public:
//	PE_INIT(unsigned int PBA){PE_INIT_reset_all(); plane_add = PBA;};
//	~PE_INIT(){PE_INIT_reset_all();};
//	void PE_INIT_SA_load_bias_data(unsigned int pe_add, unsigned int biases[16], unsigned int count);
//	void PE_INIT_SA_load_address  (unsigned int pe_add, S_addressing_info load_add,   S_addressing_info store_add);
//	void PE_INIT_UA_load_address  (unsigned int pe_add, S_addressing_info wgt_add[9], S_addressing_info inp_add[4]);
//};

void PE_INIT_reset_all					(unsigned int plane_add)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_VALUE			+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = 0;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_CONTROL		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = 0;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = 0;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = 0;
}

void PE_INIT_SA_load_bias_data			(unsigned int plane_add, unsigned int pe_add, unsigned int biases[16], unsigned int count)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = (pe_add << C_SUU_Address_Point_Unit_Add_pos);
	for (unsigned int i=0; i< count; i++)
	{
		*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_VALUE		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = biases[i];
		*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_CONTROL	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL)) = (1 << C_SAU_Bias_Wen_pos) | (i << C_SAU_Bias_Kernel_Add_pos);
	}
}

void PE_INIT_SA_load_address 			(unsigned int plane_add, unsigned int pe_add, S_Addressing_Info load_add,   S_Addressing_Info store_add)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_add.base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_add.count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_add.interval;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_add.base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_add.count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_add.interval;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
}

void PE_INIT_UA_load_address 			(unsigned int plane_add, unsigned int pe_add, S_Addressing_Info wgt_add[9], S_Addressing_Info inp_add)
{
	for (unsigned int i=0; i<9; i++)
	{
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT)									=   wgt_add[i].base_add;
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR)								=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(i << C_SUU_Address_Point_Target_Add_pos);

		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT)									=   wgt_add[i].count;
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR)								=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(i << C_SUU_Address_Point_Target_Add_pos);

		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT)	   								=   wgt_add[i].interval;
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR) 							=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(i << C_SUU_Address_Point_Target_Add_pos);
	}
	
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   inp_add.base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(15 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   inp_add.count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(15 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   inp_add.interval;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(15 << C_SUU_Address_Point_Target_Add_pos);
	
}

void PE_INIT_SA_load_address 			(unsigned int plane_add,				unsigned int pe_add, 
										unsigned int load_base_add,				unsigned int load_Count,	unsigned int load_Ival,   
										unsigned int store_base_add,			unsigned int store_Count,	unsigned int store_Ival)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_Count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_Ival;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_Count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_Ival;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
}

void PE_INIT_UA_Inp_load_address		(unsigned int plane_add,				unsigned int pe_add,   
										unsigned int Input_base_add,			unsigned int Input_Count,	unsigned int Input_Ival)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   Input_base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(15 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   Input_Count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(15 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   Input_Ival;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(15 << C_SUU_Address_Point_Target_Add_pos);
	
}

void PE_INIT_UA_Wgt_load_address		(unsigned int plane_add,				unsigned int pe_add,
										const unsigned int* Weight_base_add,	unsigned int Weight_Count,	unsigned int Weight_Ival)
{
	*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT)										=   Weight_Count;
	for (unsigned int i=0; i<9; i++)
	{
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR)								=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(i << C_SUU_Address_Point_Target_Add_pos);
	}


	*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT)	   									=   Weight_Ival;
	for (unsigned int i=0; i<9; i++)
	{
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR) 							=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(i << C_SUU_Address_Point_Target_Add_pos);
	}

	for (unsigned int i=0; i<9; i++)
	{
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT)									=   Weight_base_add[i];
		*(plane_add + C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR)								=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(0 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(i << C_SUU_Address_Point_Target_Add_pos);		
	}
}

void PE_INIT_SA_Out_load_address 		(unsigned int plane_add,				unsigned int pe_add, 
										unsigned int store_base_add,			unsigned int store_Count,	unsigned int store_Ival)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_Count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   store_Ival;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(1 << C_SUU_Address_Point_Target_Add_pos);
}

void PE_INIT_SA_Acc_load_address 		(unsigned int plane_add,				unsigned int pe_add, 
										unsigned int load_base_add,				unsigned int load_Count,	unsigned int load_Ival)
{
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_base_add;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (1 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_Count;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(1 << C_SUU_Address_Point_Count_Wen_pos) |
																												(0 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   load_Ival;
	*(C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	+ (plane_add * C_PERIPHERAL_PLAN_INTERVAL))	=   (0 << C_SUU_Address_Point_Base_Wen_pos) |
																												(0 << C_SUU_Address_Point_Count_Wen_pos) |
																												(1 << C_SUU_Address_Point_Interval_Wen_pos) |
																												(1 << C_SUU_Address_Point_SA_UAb_pos) |
																												(pe_add << C_SUU_Address_Point_Unit_Add_pos) |
																												(0 << C_SUU_Address_Point_Target_Add_pos);
	
}



//class PE_CONT
//{
//private:
//	/* data */
//	//	C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1
//	//------------------------------------------------------------------------------------------------------------------------------------
//	//	31				30				29				28				27				26				25				24
//	//	RSRVD,			RSRVD,			RSRVD,			PAUSE_PEs,		CMD_PEs_start,	CMD_inc_Rows2,	CMD_inc_Rows1,	CMD_inc_Rows0
//	//------------------------------------------------------------------------------------------------------------------------------------
//	//	23				22				21				20				19				18				17				16
//	//	RSRVD,			RSRVD,			RSRVD,			RSRVD,			RSRVD,			RSRVD,			PAUSE_STA,		CMD_STA_load	
//	//------------------------------------------------------------------------------------------------------------------------------------
//	//	15				14				13				12				11				10				9				8
//	//	CMD_STA_MEM_en,	CMD_STA_OBM_en,	CMD_STA_BIS_en,	CMD_STA_save,	CMD_STA_active,	CMD_STA_store,	CMD_STA_load_UA,CMD_STA_stor_UA,  
//	//------------------------------------------------------------------------------------------------------------------------------------
//	//	7				6				5				4				3				2				1				0
//	//	RSRVD,			RSRVD,			PAUSE_UPA,		CMD_UPA_Up_IFM,	CMD_UPA_Up_WFM,	RSRVD,			CMD_UPA_status1,CMD_UPA_status0
//	//------------------------------------------------------------------------------------------------------------------------------------
//	void PE_CONT_check_address(unsigned int pe_add);
//	void PE_CONT_reset_all();
//	void plane_add;
//public:
//	PE_CONT(unsigned int PBA){PE_CONT_reset_all(); plane_add = PBA;};
//	~PE_CONT(){PE_CONT_reset_all();};
//	void PE_CONT_PE_start(unsigned int pe_add);
//	void PE_CONT_PE_stop(unsigned int pe_add);
//	void PE_CONT_PE_pause(unsigned int pe_add);
//	void PE_CONT_PE_resume(unsigned int pe_add);
//	void PE_CONT_PE_inc_rows(unsigned int pe_add, unsigned int row0, unsigned int row1, unsigned int row2);
//
//	void PE_CONT_STA_pause(unsigned int pe_add);
//	void PE_CONT_STA_resume(unsigned int pe_add);
//	void PE_CONT_STA_load_config(unsigned int pe_add, S_CONF_STA_info info);
//
//	void PE_CONT_UPA_pause(unsigned int pe_add);
//	void PE_CONT_UPA_resume(unsigned int pe_add);
//	void PE_CONT_UPA_start_updating_IFM(unsigned int pe_add);
//	void PE_CONT_UPA_start_updating_WFM(unsigned int pe_add);
//	void PE_CONT_UPA_stop_updating_IFM (unsigned int pe_add);
//	void PE_CONT_UPA_stop_updating_WFM (unsigned int pe_add);
//};

void PE_CONT_reset_all					(unsigned int plane_add)
{
	for(unsigned int i=0; i<16; i++)
		*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + i) = 0;
}

void PE_CONT_PE_pause					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |= (1 << C_Pause_PE_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_PE_resume					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(1 << C_Pause_PE_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_PE_start					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |= (1 << C_Start_PE_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_PE_stop					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(1 << C_Start_PE_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_STA_pause					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |= (1 << C_Pause_STA_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_STA_resume					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(1 << C_Pause_STA_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_STA_load_config			(unsigned int plane_add, unsigned int pe_add, S_CONF_STA_info info)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(   (1 << C_Update_Store_Base_Address_pos) | 
				(1 << C_Update_load_Base_Address_pos) |
				(1 << C_Store_Row_pos) |
				(1 << C_Enable_Activation_pos) |
				(1 << C_Save_Row_pos) |
				(1 << C_Bias_Accumulation_Enable_pos) |
				(1 << C_PEout_Accumulation_Enable_pos) |
				(1 << C_Buffer_Accumulation_Enable_pos) |
				(1 << C_Load_Row_pos) |
				(1 << C_AUTOMATIC_STA_pos));
	tmp |=  (   (info.update_store_address << C_Update_Store_Base_Address_pos) | 
				(info.update_load_address << C_Update_load_Base_Address_pos) |
				(info.store_enable << C_Store_Row_pos) |
				(info.activation_enable << C_Enable_Activation_pos) |
				(info.save_on_buffer << C_Save_Row_pos) |
				(info.accumulate_with_bias << C_Bias_Accumulation_Enable_pos) |
				(info.accumulate_with_OBM << C_PEout_Accumulation_Enable_pos) |
				(info.accumulate_with_buffer << C_Buffer_Accumulation_Enable_pos) |
				(info.load_enable << C_Load_Row_pos) |
				(info.Automatic << C_AUTOMATIC_STA_pos));
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_STA_ACK					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |=  (1 << C_ACK_STA_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_pause					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |= (1 << C_Pause_UPA_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_resume					(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(1 << C_Pause_UPA_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_start_updating_IFM		(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |= (1 << C_Update_IFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_start_updating_WFM		(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp |= (1 << C_Update_WFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_stop_updating_IFM		(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(1 << C_Update_IFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_stop_updating_WFM		(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	tmp &= ~(1 << C_Update_WFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_UPA_start_updating			(unsigned int plane_add, unsigned int pe_add)
{
	unsigned int lod = *(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add);
	unsigned int tmp;
	tmp = lod | (1 << C_Update_WFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = lod;
	tmp = lod | (1 << C_Update_IFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}

void PE_CONT_Configure_Update_Start		(unsigned int plane_add, unsigned int pe_add, S_CONF_STA_info info)
{
	unsigned int lod;
	unsigned int tmp;
	// Config
	lod	=	(	(info.update_store_address << C_Update_Store_Base_Address_pos) | 
				(info.update_load_address << C_Update_load_Base_Address_pos) |
				(info.store_enable << C_Store_Row_pos) |
				(info.activation_enable << C_Enable_Activation_pos) |
				(info.save_on_buffer << C_Save_Row_pos) |
				(info.accumulate_with_bias << C_Bias_Accumulation_Enable_pos) |
				(info.accumulate_with_OBM << C_PEout_Accumulation_Enable_pos) |
				(info.accumulate_with_buffer << C_Buffer_Accumulation_Enable_pos) |
				(info.load_enable << C_Load_Row_pos) |
				(info.Automatic << C_AUTOMATIC_STA_pos));
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;

	/// Update
	tmp =	lod	| (1 << C_Update_WFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = lod;
	tmp	= 	lod | (1 << C_Update_IFM_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;

	// Start
	tmp	=	tmp	| (1 << C_Start_PE_pos);
	*(C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1 + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + pe_add) = tmp;
}



//class PSU_EC
//{
//private:
//	/* data */
//	//	C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PEs_1_1_DONE
	//----------------------------------------------------------------------------------------------------------------------------------------
	//	Control
	//	O	|	31	|	30	 |	29	 |	28	 |	27	 |	26	|	25	|	24	 |	23		22		21	 	20	 	19		18		17		16	 |
	//	N	|Enable |= init =| Stuck | INT E | INT C | SENS | INT V | Evnt V |========================== MAX(11:4) ==========================|
	//	T	
	//	R	
	//	O	|	15		14		13		12	|	11		10		9		8		7		6		5		4		3		2		1		0	 |
	//	L	|========== MAX(11:4) ==========|============================================= VAL ==============================================|
	//
	//		Sensitivity		:		sensitive to
	//				0		:		falling edge
	//				1		:		rising edge
	//----------------------------------------------------------------------------------------------------------------------------------------
//	void PSU_EC_check_address(unsigned int SEC_add);
//	void PSU_EC_reset_all();
//	void plane_add;
//public:
//	PSU_EC(unsigned int PBA){PSU_EC_reset_all(); plane_add = PBA;};
//	~PSU_EC(){PSU_EC_reset_all();};
//	unsigned int PSU_EC_CNTR_config(E_PSU_EC_target targ, unsigned int SEC_add, unsigned int max, unsigned int sens_level, unsigned int repeat, unsigned int intr_en);
//	unsigned int PSU_EC_CNTR_start(E_PSU_EC_target targ, unsigned int SEC_add);
//	unsigned int PSU_EC_CNTR_stop(E_PSU_EC_target targ, unsigned int SEC_add);
//	unsigned int PSU_EC_CNTR_restart(E_PSU_EC_target targ, unsigned int SEC_add);
//	unsigned int PSU_EC_CNTR_get_value(E_PSU_EC_target targ, unsigned int SEC_add, unsigned int& value, unsigned int& event_status, unsigned int& intr_status);
//};

void PSU_EC_reset_all					(unsigned int plane_add)
{
	for (unsigned int i=0; i<16; i++)
		*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + i) = 0;
}

void PSU_EC_reset						(unsigned int plane_add, unsigned int SEC_add)
{
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = 0;
}

void PSU_EC_CNTR_start_with_config		(unsigned int plane_add, E_PSU_EC_target targ, unsigned int SEC_add, unsigned int max, unsigned int sens_level, unsigned int repeat, unsigned int intr_en)
{
	unsigned int tmp = 0;
	tmp |= ((max & 0xFFF) << C_Event_Max_pos);
	tmp |= (sens_level << C_Event_Sensitity_pos);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (intr_en << C_Event_Int_Enabel_pos);
	tmp |= (repeat << C_Event_Stuck_at_Top_pos);
	tmp |= (1 << C_Event_Init_pos);
	tmp |= (targ << C_Event_Event_Source_pos);
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = tmp;
	tmp |= (1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = tmp;
}

void PSU_EC_CNTR_config					(unsigned int plane_add, E_PSU_EC_target targ, unsigned int SEC_add, unsigned int max, unsigned int sens_level, unsigned int repeat, unsigned int intr_en)
{
	unsigned int tmp = 0;
	tmp |= ((max & 0xFFF) << C_Event_Max_pos);
	tmp |= (sens_level << C_Event_Sensitity_pos);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (intr_en << C_Event_Int_Enabel_pos);
	tmp |= (repeat << C_Event_Stuck_at_Top_pos);
	tmp |= (1 << C_Event_Init_pos);
	tmp |= (targ << C_Event_Event_Source_pos);
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = tmp;
}

void PSU_EC_CNTR_start					(unsigned int plane_add, unsigned int SEC_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add);
	tmp |= (1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = tmp;
}

void PSU_EC_CNTR_stop					(unsigned int plane_add, unsigned int SEC_add)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add);
	tmp &= ~(1 << C_Event_Enable_pos);
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = tmp;
}

void PSU_EC_CNTR_restart				(unsigned int plane_add, unsigned int SEC_add)
{
	 unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add);
	tmp |= (1 << C_Event_Int_Clear_pos);
	tmp |= (1 << C_Event_Init_pos);
	*(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add) = tmp;
}

void PSU_EC_CNTR_get_value				(unsigned int plane_add, unsigned int SEC_add, unsigned int& value, unsigned int& event_status, unsigned int& intr_status, unsigned int& Source)
{
	unsigned int tmp = *(C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE + (plane_add * C_PERIPHERAL_PLAN_INTERVAL) + SEC_add);
	value = tmp % (1 << C_Event_Max_pos);
	event_status = ((tmp & (1 << C_Event_Event_Value_pos)) >> C_Event_Event_Value_pos); 
	intr_status = ((tmp & (1 << C_Event_Int_Value_pos)) >> C_Event_Int_Value_pos);
	Source =  ((tmp & (3 << C_Event_Event_Source_pos)) >> C_Event_Event_Source_pos); 
}


#endif

