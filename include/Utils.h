#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "Types.h"

//	ID = 0 is reserved.



constexpr auto USE_CHATGPT_CODE		= true;
constexpr auto PRINT_DBT_INTERNAL	= false;






struct Conv_Layer_Info
{
	unsigned int Batch_size;	// Batch size (B).
	unsigned int Kernel_size;	// Kernel/output channels (K).
	unsigned int Channel_size;	// Input channels (C).
	unsigned int Width_size;	// Width (W).
	unsigned int Height_size;	// Height (H).
	unsigned int FiltH_Size;	// Filter height (FH).
	unsigned int FiltW_Size;	// Filter width (FW).
};

const Conv_Layer_Info		Conv_Layer_Info_NULL = { 0, 0, 0, 0, 0, 0, 0 };

struct MaxP_Layer_Info
{
	unsigned int Width_size;	// Window width.
	unsigned int Height_size;	// Window height.
};

const MaxP_Layer_Info		MaxP_Layer_Info_NULL = {3, 3};

struct Data_Block_Info
{
	Data_Block_Types DBT;	// Data block type.
	bool ready;				// Ready flag for scheduling.
	bool alocated;			// Allocation flag.
	bool to_Generate;		// Whether block should be generated.
	bool isGenerated;		// is the block generated?
	bool prod_flag;			// Producer flag.
	bool cons_flag;			// Consumer flag.
	size_t Layer_ID;		// Owning layer ID.
	Conv_Layer_Info Dims;	// Dimensions (BKCWHFF).
	Conv_Layer_Info Idxs;	// Indices within dimensions (BKCWHFF).
};

struct Data_Block
{
	DBID_t				ID				= DBID_t::Null();		// Data block ID.
	DBID_t				Douplicate_of	= DBID_t::Null();		// Original block ID if this is a duplicate.
	Data_Block_Types	DBT				= DBT_Null;				// Block type.
	bool				ready			= false;				// Ready flag.
	bool				alocated		= false;				// Allocation flag.
	bool				to_Generate		= false;				// Whether block should be generated.
	bool				isGenerated		= false;				// is the block generated?
	bool				prod_flag		= false;				// Producer flag.
	bool				cons_flag		= false;				// Consumer flag.
	size_t				Layer_ID		= 0;					// Owning layer ID.
	Conv_Layer_Info		Dims			= Conv_Layer_Info_NULL;	// Dimensions (BKCWHFF).
	Conv_Layer_Info		Idxs			= Conv_Layer_Info_NULL;	// Indices within dimensions (BKCWHFF).
	unsigned int		Vault			= 0;					// Assigned vault index.
	unsigned int		MNID			= 0;					// Memory node ID.
	size_t				SoBI			= 0;					// Starting block index within vault.
	size_t				Produced_Time	= 0;					// Production time for lifetime scheduling.
	size_t				Last_Acc_Time	= 0;					// Last access time for lifetime scheduling.
	SNID_t				Producer_ID		= SNID_t::Null();		// Producer scheduling node.
	std::vector<SNID_t> Consumers_ID	= {};					// Consumer scheduling nodes.
};

const Data_Block			Data_Block_NULL;

struct Dependency
{
	bool satisfied;			// Whether dependency is satisfied.
	SNID_t Dependent_ID;	// Dependent scheduling node ID.
};

struct Scheduling_info
{
	unsigned int		Execution_Level_Space_Before;	// Min spacing before this node's level.
	unsigned int		Execution_Level;				// Execution level index.
	unsigned int		Execution_Level_Space_After;	// Min spacing after this node's level.
};

const Scheduling_info		Scheduling_info_NULL = { 0, 0, 0 };

struct Allocation_info
{
	Target_Module_Types Allocated_Target;	// Allocated target module type.
	size_t				Allocated_Plane;	// Allocated plane index.
	size_t				Allocated_Vault;	// Allocated vault index.
	};

const Allocation_info		Allocation_info_NULL = { TMT_Null, 0 , 0 };

struct Scheduling_Node
{
	bool					scheduled		= false;				// Scheduling flags.
	bool					picked			= false;				// Thread selection flag.
	bool					mapped			= false;				// Mapping flag.
	bool					allocated		= false;				// Allocation flag.
	bool					timmed			= false;				// Timing flag.
	Conv_Layer_Info			CLI				= Conv_Layer_Info_NULL;	// The layer info
	SNID_t					ID				= SNID_t::Null();		// Node ID.
	SNID_t					Douplicate_of	= SNID_t::Null();		// Original node if duplicated.
	size_t					layer			= 0;					// Owning layer index.
	Scheduling_info			Sch_Info		= Scheduling_info_NULL;	// Scheduling info (level and spacing).
	Scheduling_types		type			= Sch_Null;				// Node type.
	Allocation_info			Alc_Info		= Allocation_info_NULL;	// Allocation info (plane/vault/target).
	std::vector<Dependency> Dependencies	= {};					// Dependency list.
	std::vector<SNID_t>		Condition_for	= {};					// Reverse edges: nodes that depend on this node.
	DBID_t					Produced_DBID	= DBID_t::Null();		// Produced data block.
	std::vector<DBID_t>		Consumes_DBID	= {};					// Consumed data block.
};

const Scheduling_Node		Scheduling_Node_NULL;

struct Memory_Costs
{
	unsigned int storage_cost;				// Storage cost for this memory level.
	unsigned int access_cost_to_Top;		// Access cost to next higher level.
	unsigned int access_cost_to_Self;		// Access cost within the same level.
	unsigned int access_cost_to_Bottom;		// Access cost to next lower level.
	unsigned int L3_Change_Vault_Acc_Pnlty;	// Vault change penalty.
};

struct Data_Generator_info
{
	size_t ID;					// Generator record ID.
	unsigned int Conv_Layer_ID;	// Associated convolution layer ID.
	DBID_t DataBlock_ID;		// Data block ID to generate.
	Conv_Layer_Info idxes;		// Indices for this block.
	Data_Block_Types type;		// Block type.
	bool generated;				// Generated flag.
	};

const Data_Generator_info	Data_Generator_info_NULL = { 0, 0, };

struct Execution_Thread_Info
{
	SNID_t BaseLine_Idx;			// Baseline node ID for this thread.
	std::vector<SNID_t> Nodes;		// Ordered nodes in this execution thread.
	std::vector<bool> Optimized;	// Per-node optimization flags.
};

struct Periphral_Sequal_Info
{
	SNID_t Node;				// Peripheral node ID.
	std::vector<SNID_t> Lasts;	// Dependency nodes to execute before this node.
};

struct Periphral_Thread_Info
{
	Scheduling_types type;						// Peripheral type (Sch_Activation or Sch_MPDR).
	std::vector<Periphral_Sequal_Info> PSIs;	// Sequential peripheral nodes.
};

struct PE_Node
{
	bool active;					// Active flag.
	size_t baseline;				// Baseline index.
	size_t thread;					// Thread index.
	std::vector<SNID_t> Sequence;	// Execution sequence of scheduling nodes.
};

// Null/default PE node.
const PE_Node				PE_Node_NULL = { false, 0, 0, {} };

struct Ordering_Node
{
	SNID_t ID;				// Scheduling node ID.
	size_t Plane;			// Allocated plane.
	size_t Vault;			// Allocated vault.
	size_t Rsrv1;			// Reserved/extra field.
	Scheduling_types type;	// Node type.
	bool Generated;			// Whether this ordering node has been consumed.
};

const Ordering_Node			Ordering_Node_NULL = { 0, 0, 0, 0, Sch_Null };

struct Ord_Address
{
	bool valid;		// Validity flag.
	size_t lvl;		// Level index.
	size_t bline;	// Baseline index.
	size_t node;	// Node index within baseline.
	size_t pos;		// Position within node sequence.
};

struct CG_PE_Node
{
	bool				Activation;			// Activation flag for this PE node.
	bool				ESI;
	bool				ESO;
	bool				ESP;				// Equal spacing flags (input/output/accumulator).
	size_t				Plane;
	size_t				Vault;				// Plane and vault assignment.
	DBID_t				Weigth_ID[9];		// Weight block IDs (3x3 kernel).
	std::vector<DBID_t> Inputs_ID;			// Input block IDs.
	std::vector<DBID_t> Output_ID;			// Output block IDs.
	std::vector<SNID_t> EXE_Nodes;			// Execution scheduling node IDs.
	std::vector<DBID_t> Acum_DBID;			// Accumulation block IDs.
	size_t				Inp_Seqnc;			// Input spacing sequence.
	size_t				Out_Seqnc;			// Output spacing sequence.
	size_t				Acc_Seqnc;			// Accumulation spacing sequence.
	
};

struct CG_MPDR_Node
{
	bool				valid;		// Validity flag (used while reordering).
	bool				gen;		// Validity flag (used while reordering).
	bool				ESI;
	bool				ESO;		// Equal spacing flags (input/output).
	SNID_t				Node;		// Scheduling node ID.
	size_t				Vault;		// Vault assignment.
	std::vector<DBID_t> Inputs;		// Input block IDs.
	DBID_t				Output;		// Output block ID.
};

struct CG_PE_Sttus
{
	DBID_t				Weigth_ID[9];				// Weight block IDs (3x3 kernel).
	size_t				Input_Start_Index;			// Input start index for streaming.
	size_t				Input_Index_Advancement;	// Input index stride for streaming.
	size_t				Output_Start_Index;			// Output start index for streaming.
	size_t				Output_Index_Advancement;	// Output index stride for streaming.
};

struct Mapping_Result
{
	SNID_t node;
	uint16_t plane;
	uint16_t pe_index;
	uint16_t vault;
};










// Build a Data_Block from a Data_Block_Info record.
Data_Block assign(Data_Block_Info inp);

// Build Conv_Layer_Info from individual dimensions.
Conv_Layer_Info assign(unsigned int B, unsigned int K, unsigned int C, unsigned int W, unsigned int H, unsigned int FH, unsigned int FW);

// Compare Conv_Layer_Info for B/H/W equality and K==C consistency.
bool Eq_comapre_BHW_K1C2(Conv_Layer_Info i1, Conv_Layer_Info i2);

// Print Conv_Layer_Info to stdout.
void print(Conv_Layer_Info inp);

// Print a Dependency to stdout.
void print(Dependency dep);

// Allocate a 7D array of DBID_t with sizes d0..d6.
DBID_t******* new_DBID_7D	(unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6);

// Allocate a 7D array of SNID_t with sizes d0..d6.
SNID_t******* new_SNID_7D	(unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3, unsigned int d4, unsigned int d5, unsigned int d6);

// Allocate a 4D array of DBID_t with sizes d0..d3.
DBID_t****    new_DBID_4D	(unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3);

// Allocate a 4D array of SNID_t with sizes d0..d3.
SNID_t****    new_SNID_4D	(unsigned int d0, unsigned int d1, unsigned int d2, unsigned int d3);

// Construct a Dependency with given satisfied flag and dependent ID.
Dependency Gen_Dependency(bool satisfied, SNID_t Dependent_ID);

// Map baseline count to vault count (heuristic).
size_t calculate_vault_map(size_t bl);

// Return a string for a boolean result ("Suucessfull"/"Unsuucessfull").
std::string Res_Dec(const bool b);

// embedding the level value and baseline value			to the template
std::string embedd			(size_t lvl,	size_t bline);
std::string embedd			(size_t lvl,	size_t bline,		size_t node);
std::string embedd_array	(size_t lvl,											size_t arr_size_d1);
std::string embedd_array	(size_t lvl,	size_t bline,							size_t arr_size_d1);
std::string embedd_array	(size_t lvl,	size_t bline,		size_t node,		size_t arr_size_d1);
std::string embedd_dma_array(size_t lvl,	size_t bline,		size_t node,		size_t arr_size_d1);
std::string embedd_array_2D	(size_t lvl,											size_t arr_size_d1,	size_t arr_size_d2);
std::string embedd_array_2D	(size_t lvl,	size_t bline,							size_t arr_size_d1,	size_t arr_size_d2);


























