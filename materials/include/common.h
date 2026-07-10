#ifndef	_COMMON_H_
#define _COMMON_H_
//////////////////////////////
//	Peripheral Address Map	//
//////////////////////////////


#define C_CLK_PERIODE						1000000000



#define PLANE_CNT							2



#define C_PERIPHERAL_REG_BASE_ADD			0XFFFF0000
#define C_PERIPHERAL_PLAN_BASE_ADD			0XFFFF0200
#define C_PERIPHERAL_PLAN_INTERVAL			0X100


			
#define C_DMAx_Address_Excess_val			0X000		
#define C_MPDR_Address_Excess_val			0X010		
#define C_TIMR_Address_Excess_val			0X020		
#define C_DMEV_Address_Excess_val			0X060		
#define C_MPEV_Address_Excess_val			0X0A0		
#define C_COST_Address_Excess_val			0X0E0		
#define C_TRxU_Address_Excess_val			0X0F0		
#define C_INTH_Address_Excess_val			0X100		
			

#define C_CONF_OFFSET_ADDRESS_Excess_val	0X00
#define C_INIT_OFFSET_ADDRESS_Excess_val	0X40
#define C_PECO_OFFSET_ADDRESS_Excess_val	0X50
#define C_EVNT_OFFSET_ADDRESS_Excess_val	0X90








/*******	DMAs CONTROL	*******/
volatile unsigned int* const C_PERIPHERAL_REG_DMA_READ_ADDRESS							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMAx_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_DMA_WRITE_ADDRESS							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMAx_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_DMA_TRANS_COUNT							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMAx_Address_Excess_val	+	0X08);
volatile unsigned int* const C_PERIPHERAL_REG_DMA_CONTROL								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMAx_Address_Excess_val	+	0X0C);



/*******	MPDR CONTROL	*******/
volatile unsigned int* const C_PERIPHERAL_REG_MPDR_DATABLOCK_ADDRESS					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPDR_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_MPDR_CONFIG								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPDR_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_MPDR_CONTTOL								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPDR_Address_Excess_val	+	0X08);

		
		
/*******	TIMER CONTROL	*******/		
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_0_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_0_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_1_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X08);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_1_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X0C);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_2_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X10);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_2_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X14);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_3_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X18);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_3_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X1C);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_4_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X20);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_4_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X24);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_5_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X28);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_5_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X2C);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_6_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X30);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_6_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X34);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_7_CONTROL							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X38);
volatile unsigned int* const C_PERIPHERAL_REG_TIMER_7_VALUE								= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TIMR_Address_Excess_val	+	0X3C);
		
		
		
/*******	DMA	EVENT COUNTER	*******/		
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_0_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_1_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_2_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X08);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_3_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X0C);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_4_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X10);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_5_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X14);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_6_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X18);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_7_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X1C);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_8_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X20);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_9_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X24);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_10_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X28);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_11_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X2C);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_12_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X30);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_13_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X34);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_14_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X38);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_DMA_15_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_DMEV_Address_Excess_val	+	0X3C);
		
		
		
/*******	MPDR	EVENT COUNTER	*******/		
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_0_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_1_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_2_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X08);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_3_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X0C);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_4_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X10);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_5_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X14);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_6_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X18);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_7_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X1C);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_8_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X20);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_9_DONE					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X24);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_10_DONE				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X28);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_11_DONE				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X2C);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_12_DONE				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X30);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_13_DONE				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X34);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_14_DONE				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X38);
volatile unsigned int* const C_PERIPHERAL_REG_EVENT_COUNTER_MPDR_15_DONE				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_MPEV_Address_Excess_val	+	0X3C);



/*******	CONTROL_REGISTER	*******/		
volatile unsigned int* const C_PERIPHERAL_REG_CONTROL_REGISTE							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_COST_Address_Excess_val	+	0X00);



/*******	USART	*******/	
volatile unsigned int* const C_PERIPHERAL_REG_TRx_UNIT_DATA_WORD						= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TRxU_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_TRx_UNIT_CONT_WORD						= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_TRxU_Address_Excess_val	+	0X04);



/*******	INTERRUPT HANDLER	*******/		
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_PC						= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_TIMER					= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X00);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_MPDR						= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_DMA						= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X04);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_0				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X08);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X0C);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X10);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ENABLE_PSU_PLANE_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X14);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ADDRESS							= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X18);
volatile unsigned int* const C_PERIPHERAL_REG_INTERRUPT_ACKNOWLEDGE						= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_REG_BASE_ADD		+	C_INTH_Address_Excess_val	+	0X1C);



//	PLANE	0	:	
/*******	CONFIG HOLDER	*******/
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X00);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X04);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X08);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_1_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X0C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_2_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X10);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_2_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X14);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_2_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X18);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_2_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X1C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_3_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X20);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_3_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X24);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_3_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X28);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_3_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X2C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_4_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X30);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_4_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X34);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_4_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X38);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_CONFIG_HOLDER_PE_4_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_CONF_OFFSET_ADDRESS_Excess_val + 0X3C);

/*******	SAUs INITIATION	*******/
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_VALUE			= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_INIT_OFFSET_ADDRESS_Excess_val + 0X00);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_BIAS_CONTROL			= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_INIT_OFFSET_ADDRESS_Excess_val + 0X04);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_INIT_OFFSET_ADDRESS_Excess_val + 0X08);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_SAU_INITIATE_ADDRESS_POINT_CNTR	= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_INIT_OFFSET_ADDRESS_Excess_val + 0X0C);


/*******	PE CONTROL	*******/
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X00);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X04);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X08);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_1_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X0C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_2_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X10);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_2_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X14);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_2_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X18);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_2_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X1C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_3_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X20);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_3_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X24);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_3_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X28);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_3_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X2C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_4_1				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X30);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_4_2				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X34);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_4_3				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X38);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_PEs_CONTROL_PE_4_4				= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_PECO_OFFSET_ADDRESS_Excess_val + 0X3C);


/*******	EVENT COUNTER	*******/
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_1_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X00);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_2_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X04);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_3_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X08);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_1_4_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X0C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_2_1_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X10);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_2_2_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X14);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_2_3_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X18);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_2_4_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X1C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_3_1_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X20);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_3_2_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X24);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_3_3_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X28);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_3_4_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X2C);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_4_1_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X30);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_4_2_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X34);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_4_3_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X38);
volatile unsigned int* const C_PERIPHERAL_PLANE_0_REG_EVENT_COUNTER_PSU_4_4_DONE		= reinterpret_cast< volatile unsigned int* >	(C_PERIPHERAL_PLAN_BASE_ADD		+	C_EVNT_OFFSET_ADDRESS_Excess_val + 0X3C);



//////////////////////
//	Bit Positions	//
//////////////////////

/*******	DMAs CONTROL	*******/
#define	C_DMA_Address_pos						0
#define	C_DMA_Start_pos							31

/*******	MPDRs CONTROL	*******/
#define	C_MPDR_Target_pos						0
#define	C_MPDR_IA_Wen_pos						3
#define	C_MPDR_CA_Wen_pos						4
#define	C_MPDR_BA_Wen_pos						5
#define	C_MPDR_Unit_Addres_pos					28
#define	C_MPDR_Max_Colm_pos						0
#define	C_MPDR_Max_Chan_pos						4
#define	C_MPDR_Load_pos							8
#define	C_MPDR_Keep_pos							9
#define	C_MPDR_Start_pos						10

/*******	TIMER CONTROL	*******/
#define	C_Timer_Top_pos							0
#define	C_Timer_Clk_Div_pos						20
#define	C_Timer_Int_clear_pos					28
#define	C_Timer_Int_Enable_pos					29
#define	C_Timer_Init_pos						30
#define	C_Timer_Enable_pos						31
#define	C_Timer_Val_pos							0
#define	C_Timer_Int_Val_pos						31

/*******	EVENT COUNTER	*******/
#define	C_Event_Val_pos							0
#define	C_Event_Max_pos							11
#define	C_Event_Event_Source_pos				22	//	Source:		0:PE DONE,		1:STA DONE,		2:UPA DONE,		3:RESERVED
#define	C_Event_Event_Value_pos					24
#define	C_Event_Int_Value_pos					25
#define	C_Event_Sensitity_pos					26	//Sensitivity:	0:falling edge,	1:rising edge
#define	C_Event_Int_Clear_pos					27
#define	C_Event_Int_Enabel_pos					28
#define	C_Event_Stuck_at_Top_pos				29
#define	C_Event_Init_pos						30
#define	C_Event_Enable_pos						31

/*******	CONTROL_REGISTER	*******/
#define	C_Accelerator_Plane_0_Normal_pos		0
#define	C_Accelerator_Plane_1_Normal_pos		1
#define	C_Accelerator_Plane_2_Normal_pos		2
#define	C_Accelerator_Plane_3_Normal_pos		3
#define	C_Accelerator_Connect_pos				31

/*******	USART	*******/
#define	C_TRx_Unit_Top_pos						0
#define C_TRx_Unit_Top_len						16
#define	C_TRx_Unit_Rx_Buff_Full_Flg_pos			16
#define	C_TRx_Unit_Rx_Buff_Empty_Flg_pos		17
#define	C_TRx_Unit_Tx_Buff_Full_Flg_pos			18
#define	C_TRx_Unit_Tx_Buff_Empty_Flg_pos		19
#define	C_TRx_Unit_Clk_Div_pos					20
#define	C_TRx_Unit_Interrupt_clear_pos			25
#define	C_TRx_Unit_Rx_Data_Received_Intr_en_pos	26
#define	C_TRx_Unit_Tx_Data_Sent_Intr_en_pos		27
#define	C_TRx_Unit_Rx_Buff_Full_Intr_en_pos		28
#define	C_TRx_Unit_Tx_Buff_Empty_Intr_en_pos	29
#define	C_TRx_Unit_Rx_Enable_pos				30
#define	C_TRx_Unit_Tx_Enable_pos				31
#define	C_TRx_Unit_DATA_0_pos					0
#define	C_TRx_Unit_DATA_1_pos					8
#define	C_TRx_Unit_DATA_2_pos					16
#define	C_TRx_Unit_DATA_3_pos					24
#define	C_TRx_Unit_Rx_DORE_Flg_pos				31


/*******	INTERRUPT HANDLER	*******/
#define	C_INT_PC_Req_pos						0	//	Highest Priority
#define	C_INT_TBE_pos							1
#define	C_INT_RBF_pos							2
#define	C_INT_TXD_pos							3
#define	C_INT_RXD_pos							4
#define	C_INT_Timer_0_pos						8
#define	C_INT_Timer_1_pos						9
#define	C_INT_Timer_2_pos						10
#define	C_INT_Timer_3_pos						11
#define	C_INT_Timer_4_pos						12
#define	C_INT_Timer_5_pos						13
#define	C_INT_Timer_6_pos						14
#define	C_INT_Timer_7_pos						15

#define	C_INT_MPDR_0_Done_pos					0
#define	C_INT_MPDR_1_Done_pos					1
#define	C_INT_MPDR_2_Done_pos					2
#define	C_INT_MPDR_3_Done_pos					3
#define	C_INT_MPDR_4_Done_pos					4
#define	C_INT_MPDR_5_Done_pos					5
#define	C_INT_MPDR_6_Done_pos					6
#define	C_INT_MPDR_7_Done_pos					7
#define	C_INT_MPDR_8_Done_pos					8
#define	C_INT_MPDR_9_Done_pos					9
#define	C_INT_MPDR_10_Done_pos					10
#define	C_INT_MPDR_11_Done_pos					11
#define	C_INT_MPDR_12_Done_pos					12
#define	C_INT_MPDR_13_Done_pos					13
#define	C_INT_MPDR_14_Done_pos					14
#define	C_INT_MPDR_15_Done_pos					15

#define	C_INT_DMA_0_Done_pos					16
#define	C_INT_DMA_1_Done_pos					17
#define	C_INT_DMA_2_Done_pos					18
#define	C_INT_DMA_3_Done_pos					19
#define	C_INT_DMA_4_Done_pos					20
#define	C_INT_DMA_5_Done_pos					21
#define	C_INT_DMA_6_Done_pos					22
#define	C_INT_DMA_7_Done_pos					23
#define	C_INT_DMA_8_Done_pos					24
#define	C_INT_DMA_9_Done_pos					25
#define	C_INT_DMA_10_Done_pos					26
#define	C_INT_DMA_11_Done_pos					27
#define	C_INT_DMA_12_Done_pos					28
#define	C_INT_DMA_13_Done_pos					29
#define	C_INT_DMA_14_Done_pos					30
#define	C_INT_DMA_15_Done_pos					31

#define	C_INT_PSU_1_1_Done_pos					0
#define	C_INT_PSU_1_2_Done_pos					1
#define	C_INT_PSU_1_3_Done_pos					2
#define	C_INT_PSU_1_4_Done_pos					3
#define	C_INT_PSU_2_1_Done_pos					4
#define	C_INT_PSU_2_2_Done_pos					5
#define	C_INT_PSU_2_3_Done_pos					6
#define	C_INT_PSU_2_4_Done_pos					7
#define	C_INT_PSU_3_1_Done_pos					8
#define	C_INT_PSU_3_2_Done_pos					9
#define	C_INT_PSU_3_3_Done_pos					10
#define	C_INT_PSU_3_4_Done_pos					11
#define	C_INT_PSU_4_1_Done_pos					12
#define	C_INT_PSU_4_2_Done_pos					13
#define	C_INT_PSU_4_3_Done_pos					14
#define	C_INT_PSU_4_4_Done_pos					15
#define	C_INT_ACK_pos							0
#define	C_INT_NEXT_code_pos						0
#define	C_INT_THIS_code_pos						8
#define	C_INT_NEXT_Address_pos					16
#define	C_INT_THIS_Address_pos					24

/*******	CONFIG HOLDER	*******/
#define	C_Channel_MAX_pos						0
#define	C_Column_MAX_pos						4
#define	C_Kernel_MAX_pos						8
#define	C_ZPad_MAX_pos							12
#define	C_WFM_Numeric_pos						22
#define	C_IFM_Numeric_pos						23
#define	C_Computation_Size_pos					24
#define	C_Shift_Count_pos						25
#define	C_FSM_SEL_pos							30

/*******	SAUs INITIATION	*******/
#define	C_SAU_Bias_Value_pos					0
#define	C_SAU_Bias_Kernel_Add_pos				0
#define	C_SAU_Bias_Wen_pos						31
#define	C_SUU_Address_Point_pos					0
#define	C_SUU_Address_Point_Target_Add_pos		0
#define	C_SUU_Address_Point_Unit_Add_pos		16
#define	C_SUU_Address_Point_SA_UAb_pos			28
#define	C_SUU_Address_Point_Interval_Wen_pos	29
#define	C_SUU_Address_Point_Count_Wen_pos		30
#define	C_SUU_Address_Point_Base_Wen_pos		31





/*******	PE CONTROL	*******/
#define	C_UPA_Status_pos						0
#define	C_Update_WFM_pos						3
#define	C_Update_IFM_pos						4
#define	C_Pause_UPA_pos							5
#define	C_Update_Store_Base_Address_pos			8
#define	C_Update_load_Base_Address_pos			9
#define	C_Store_Row_pos							10	//	store internal Buffer
#define	C_Enable_Activation_pos					11
#define	C_Save_Row_pos							12	//	save it in the inrternal Buffer
#define	C_Bias_Accumulation_Enable_pos			13	//	Accumulate with Bias
#define	C_PEout_Accumulation_Enable_pos			14	//	Accumulate with PE output
#define	C_Buffer_Accumulation_Enable_pos		15	//	Accumulate with Internal Buffer
#define	C_Load_Row_pos							16	//	preload internal Buffer
#define	C_Pause_STA_pos							17
#define	C_ACK_STA_pos							18
#define	C_AUTOMATIC_STA_pos						19
//#define	C_AUTOMATIC_STA_CNTR_pos				20
//#define	C_Inc_Row_0_pos							24
//#define	C_Inc_Row_1_pos							25
//#define	C_Inc_Row_2_pos							26
#define	C_Start_PE_pos							27
#define	C_Pause_PE_pos							28





//	Function Decleration
extern	"C"	{	void EXT_INT_0_handler();	}	//	Interrupt 0
extern	"C"	{	void EXT_INT_1_handler();	}	//	Interrupt 1
extern	"C"	{	void EXT_INT_2_handler();	}	//	Interrupt 2
extern	"C"	{	void EXT_INT_3_handler();	}	//	Interrupt 3
extern	"C"	{	void EXT_INT_4_handler();	}	//	Interrupt 4
extern	"C"	{	void EXT_INT_5_handler();	}	//	Interrupt 5
extern	"C"	{	void EXT_INT_6_handler();	}	//	Interrupt 6
extern	"C"	{	void EXT_INT_7_handler();	}	//	Interrupt 7
extern	"C"	{	void EXT_INT_8_handler();	}	//	Interrupt 8
extern	"C"	{	void EXT_INT_9_handler();	}	//	Interrupt 9
extern	"C"	{	void EXT_INT_10_handler();	}	//	Interrupt 10
extern	"C"	{	void EXT_INT_11_handler();	}	//	Interrupt 11
extern	"C"	{	void EXT_INT_12_handler();	}	//	Interrupt 12
extern	"C"	{	void EXT_INT_13_handler();	}	//	Interrupt 13
extern	"C"	{	void EXT_INT_14_handler();	}	//	Interrupt 14
extern	"C"	{	void EXT_INT_15_handler();	}	//	Interrupt 15

extern	"C"	{	void Enable_GINT();			}	//	Enable Global  The Interrupt
extern	"C"	{	void Disable_GINT();		}	//	Enable Disable The Interrupt


/*	HOW TO USE THESE 
		unsigned int* addr1 = reinterpret_cast< volatile unsigned int* >0xDEADBEEF;
		unsigned int* addr2 = reinterpret_cast< volatile unsigned int* >0xDEADBEEF;
		unsigned int value = *addr1;
		*addr2 = value;
		*/


enum				E_FSM_Src		{	FSM_CNN_1=0,	FSM_CNN_2=1, 	FSM_MLP_1=2,	FSM_MLP_2=3};
unsigned int		E_Shift_cnt;
enum				E_FH_Arith		{	HALF=0,			FULL=1	};
enum				E_IFM_NS		{	IUNSIGN=0,		ISIGN=1	};
enum				E_WFM_NS		{	WUNSIGN=0,		WSIGN=1	};
unsigned int		E_Zpad_max;
unsigned int		E_Kern_max;
unsigned int		E_Colm_max;
unsigned int		E_Chnl_max;
unsigned int		E_Rows_max;
unsigned int		E_Cntr_max;


struct	S_PE_cofig
{
	E_FSM_Src		FSM_Src		=	FSM_CNN_1;
	unsigned int 	E_Shift_cnt	=	0;
	E_FH_Arith		FH_Arith	=	FULL;
	E_IFM_NS		IFM_NS		=	IUNSIGN;
	E_WFM_NS		WFM_NS		=	WUNSIGN;
	unsigned int 	E_Zpad_max	=	3;
	unsigned int 	E_Kern_max	=	15;
	unsigned int 	E_Colm_max	=	15;
	unsigned int 	E_Chnl_max	=	15;
};

struct	S_Addressing_Info
{
	unsigned int base_add	=	0;
	unsigned int count		=	0;
	unsigned int interval	=	0;
};

struct	S_CONF_STA_info
{
	unsigned int update_store_address;
	unsigned int update_load_address;
	unsigned int store_enable;
	unsigned int load_enable;
	unsigned int activation_enable;
	unsigned int save_on_buffer;
	unsigned int accumulate_with_bias;
	unsigned int accumulate_with_OBM;
	unsigned int accumulate_with_buffer;
	unsigned int Automatic;
};

enum	E_PSU_EC_target		{	PEs=0,		STA=1,		UPA=2};

struct	S_UPA_Inp_Addressing_Info
{
	unsigned int base_add	=	0;
	unsigned int interval	=	0;
};

struct	UART_Config
{
	unsigned int Tx_enable;
	unsigned int Rx_enable;
	unsigned int TBE_intr_enable;
	unsigned int RBF_intr_enable;
	unsigned int Tx_done_intr_enable;
	unsigned int Rx_done_intr_enable;
	unsigned int Clk_Div;
	unsigned int TOP;
};

unsigned int UART_Cont_Word_gen(UART_Config conf)
{
	unsigned int wrod = 0;
	word |= Tx_enable				<< C_TRx_Unit_Tx_Enable_pos;
	word |= Rx_enable				<< C_TRx_Unit_Rx_Enable_pos;
	word |= TBE_intr_enable			<< C_TRx_Unit_Tx_Buff_Empty_Intr_en_pos;
	word |= RBF_intr_enable			<< C_TRx_Unit_Rx_Buff_Full_Intr_en_pos;
	word |= Tx_done_intr_enable		<< C_TRx_Unit_Tx_Data_Sent_Intr_en_pos;
	word |= Rx_done_intr_enable		<< C_TRx_Unit_Rx_Data_Received_Intr_en_pos;
	word |= 1						<< C_TRx_Unit_Interrupt_clear_pos;
	word |= (Clk_Div	& 0xF)		<< C_TRx_Unit_Clk_Div_pos;
	word |= (TOP		& 0xFFFF)	<< C_TRx_Unit_Top_pos;
}



#endif


