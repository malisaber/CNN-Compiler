#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include "Utils.h"
#include "Config.h"
#include "Data_Logger.h"
#include "Hardware.h"


class Dependency_Logger
{
private:

	bool Spaced;																		// True if spacing info has been computed.
	bool Scheduled;																		// True if nodes have been scheduled into levels.
	bool Threaded;																		// True if threads have been built.
	bool Mapped;																		// True if nodes have been mapped to hardware.
	bool Compiled;																		// True if scheduling graph has been compiled (reverse edges built).
	bool Timed;																			// True if nodes have been timed.
	bool Allocated;																		// True if data blocks have been allocated.

	SNID_t Dependency_ID_cntr;															// Monotonic counter for scheduling node IDs.
	SNID_t Starting_Point_ID;															// Start sentinel node IDs.
	SNID_t Ending_Point_ID;																// End sentinel node IDs.
	size_t level;																		// Current scheduling level.
	size_t Input_Data_Block_Count;														// Input	Data Block Count
	size_t Weght_Data_Block_Count;														// Weight	Data Block Count
	size_t Outpt_Data_Block_Count;														// Output	Data Block Count
	size_t Psums_Data_Block_Count;														// P-Sums	Data Block Count
	size_t Input_Location_Offset;														// Input	Location Offset
	size_t Weght_Location_Offset;														// Weight	Location Offset
	size_t Outpt_Location_Offset;														// Output	Location Offset
	size_t Psums_Location_Offset;														// P-Sums	Location Offset
	size_t END_Offset;																	// End offset after allocation.
	size_t Last_Time;																	// Latest time stamp used for allocation.
	
	PE_Node Previous_PE_Info[4][16];													// Cached PE info for coverage heuristics.
	bool    Previous_PE_Info_valid;														// Validity flag for cached PE info.

	std::vector<Scheduling_Node> Nodes;													// All scheduling nodes (index by SNID_t::index()).
	std::vector<std::vector<SNID_t>> Scheduled_ID_list;									// Scheduled node IDs grouped by level.
	std::vector<std::vector<Execution_Thread_Info>> Execution_Threads;					// Execution threads for processing elements.
	std::vector<std::vector<Periphral_Thread_Info>> Periphral_Threads;					// Peripheral threads for activation and MPDR.
	std::vector<std::vector<std::vector<Execution_Thread_Info>>> Execution_Threads_OPT;	// Optimized execution threads.
	std::vector<std::vector<std::vector<Ordering_Node>>> Ordering;						// Ordering nodes grouped by level/baseline.
	
	

public:

	// Temporary counters for debug/instrumentation.
	size_t tmp_cntr;
	size_t need_to_copy;

	// Initialize dependency logger state and create start/end nodes.
	Dependency_Logger										();

	// Destroy dependency logger (no explicit resource cleanup required).
	~Dependency_Logger										();



	// Building Dependency Graph, Create a scheduling node without dependencies.
	SNID_t Create_Scheduling_Node							(Scheduling_types type, size_t layer);

	// Create a scheduling node with a single dependency.
	SNID_t Create_Scheduling_Node							(Scheduling_types type, size_t layer, Dependency dep);

	// Create a scheduling node with multiple dependencies.
	SNID_t Create_Scheduling_Node							(Scheduling_types type, size_t layer, std::vector<Dependency> deps);

	// Get the start sentinel node ID.
	SNID_t Get_Starting_Point_Scheduling_Node_Idx			() const;

	// Get the end sentinel node ID.
	SNID_t Get_Ending_Point_Scheduling_Node_Idx				() const;

	// Get the layer index for a scheduling node.
	size_t Get_Layer_of										(SNID_t SNI);

	// Set produced data block ID for a scheduling node.
	bool Set_Produced_Data_Block_ID							(SNID_t SNI, DBID_t PDBID);

	// Add a consumed data block ID to a scheduling node.
	bool Add_Consumed_Data_Block_ID							(SNID_t SNI, DBID_t CDBID);

	// Add a dependency edge to a scheduling node.
	bool Add_Dependency_to_Scheduling_Node					(SNID_t SNI, Dependency dep);

	// Set scheduling info for a node.
	bool Set_Scheduling_Info								(SNID_t SNI, Scheduling_info  Sinfo);

	// Get scheduling info for a node.
	bool Get_Scheduling_Info								(SNID_t SNI, Scheduling_info& Sinfo);

	// Fetch a full scheduling node record.
	bool Fetch_Scheduling_Node_Info							(SNID_t SNI, Scheduling_Node& SNinfo);

	// Replace a scheduling node record.
	bool Change_Scheduling_Node_Info						(SNID_t SNI, Scheduling_Node  SNinfo);

	// Find nodes that depend on a given dependency.
	bool Fetch_Scheduling_Nodes_with_this_Dependency		(std::vector<SNID_t>& NID, Dependency dep);

	// Seting Convolution Layer Information of the Node
	bool Set_CLI											(SNID_t SNI, Conv_Layer_Info  CLinfo);

	// Getting the Convolution Layer Information of the Node
	bool Get_CLI											(SNID_t SNI, Conv_Layer_Info& CLinfo);

	// Print a single scheduling node and its dependencies.
	bool print_Scheduling_Node_Info							(SNID_t SNI);

	// Dump all scheduling nodes to a text file (appends .txt).
	size_t print_file										(std::filesystem::path name);

	// Return number of scheduling nodes stored.
	size_t size												();

	// Scheduling the Dependency Nodes Assign execution levels to all nodes based on dependency satisfaction.
	size_t Schedule_Nodes									(Data_Logger* DL);

	// Thread Making, Build execution and peripheral threads.
	bool Build_Threads										();

	// Optimize execution threads ordering.
	bool Optimizing_Execution_Threads						();

	// Return total thread size across all levels.
	size_t Thread_Size										();

	// Dump thread info to a text file (appends .txt).
	size_t print_Thread_info_file							(std::filesystem::path name);

	// Dump optimized thread info to a text file (appends .txt).
	size_t print_Optimized_Execution_Thread_info_file		(std::filesystem::path name);

	// Mpping, Map scheduled nodes to hardware resources.
	size_t Map												(Data_Logger* DataL, Hardware* HW, std::filesystem::path mappr_Dump_Dir);

	// Dump mapping info to a text file (appends .txt).
	bool print_mapping_file									(std::filesystem::path fname);

	//	Allocating, Get vault assignment for a scheduling node.
	bool Get_Vault_Info										(SNID_t SID,	size_t& Vlt);

	// Compute produced/accessed times for each data block.
	size_t Calculate_Data_Block_Timing						(Data_Logger*	DataL);

	// Allocate data blocks to vault addresses.
	bool Allocte											(Data_Logger*	DataL);

	// Return required space counts and offsets for each data type.
	bool Get_Required_Spaces								(size_t& Icnt,	size_t& Wcnt, size_t& Ocnt, size_t& Pcnt, size_t& Ifst, size_t& Wfst, size_t& Ofst, size_t& Pfst, size_t& Efst) const;
	

	// Export ordering info for code generation.
	void Get_Ordering_Info									(std::vector<std::vector<std::vector<Ordering_Node>>>& Ord) const;

	// Fetch consumed data blocks for a scheduling node.
	void Get_Consumed_Data_Bloks							(SNID_t SID, std::vector<DBID_t>& Cnsmd_DBID);








private:

	// Duplicate a scheduling node and reset selected fields.
	SNID_t	Douplicate_and_cleanse_Scheduling_Node			(SNID_t SNI, SNID_t NSNI);

	// Create the start sentinel scheduling node.
	SNID_t	Create_Starting_Point_Scheduling_Node			();

	// Create the end sentinel scheduling node.
	SNID_t	Create_Ending_Point_Scheduling_Node				();

	// Compute spacing between execution levels for scheduled nodes.
	bool	Calculation_Scheduling_Node_Spaces				();

	// Clear spacing info for all scheduling nodes.
	bool	Clear_Scheduling_Node_Spacing_Info				();

	// Build reverse edges (Condition_for) and mark Compiled.
	void	Compile_Schedule_Nodes							();

	// Check if a scheduling node is satisfied (all deps ready).
	bool	Is_Scheduling_Node_Satisfied					(Data_Logger* DL,	Scheduling_Node& SN);

	// Check if two PE nodes share the same weights.
	bool	Is_PE_Weights_Similar							(SNID_t SNI,		SNID_t BLNode_Idx);

	// Check if PE inputs follow sequential ordering.
	bool	Is_PE_Inputs_Next_Sequencing					(SNID_t SNI,		SNID_t BLNode_Idx);

	// Calculate coverage statistics for PE mapping.
	void	Calculate_PE_Coverage							(PE_Node PEs[4][16], size_t& total_Cover, size_t& vault_Cover);

	// Clear cached Previous_PE_Info and validity flag.
	void	Clear_Previous_PE_Info							();

	// Optimize execution blocks at a given level.
	void	Optimize_PE_Execution_Block						(Data_Logger* DataL, PE_Node PEs[4][16], size_t lvl);

	// Generate execution ordering nodes for a level.
	void	Generate_Execution_Block						(Data_Logger* DataL, size_t lvl);

	// Generate peripheral ordering nodes for a given node/level.
	void	Generate_Peripheral_Block						(Data_Logger* DataL, SNID_t SNI, size_t lvl);

	// Map a level
	void	Map_level										(std::vector<SNID_t>& nodes_ID, std::vector < std::vector<Mapping_Result>>& res);

	// Map a single scheduling node to plane/vault/node indices.
	void	Map_Node										(Data_Logger* DataL, SNID_t node_idx, size_t block, size_t lvl, size_t plane, size_t vlt, bool forced, size_t counter);

	// Mark a scheduling node as mapped.
	void	Mark_as_Mapped									(SNID_t SNI);

	// Assign a time stamp to a node and its produced data.
	void	Time_The_Node_Please							(Data_Logger* DataL, SNID_t SNI, size_t timer);

	// Mark a node as timed.
	void	Mark_as_Timmed									(SNID_t SNI);

	// Fetch first scheduling node that depends on a given dependency at level.
	bool	Fetch_first_Scheduling_Nodes_with_Dependency	(SNID_t& NID, Dependency dep, size_t lvl);

	// Calculate required space counts and offsets for allocation.
	void	Calculate_Required_Spaces						(Data_Logger* DataL);

	// Allocate input blocks (DATA_IN/IZero).
	void	maloc_DATA_IN									(Data_Logger* DataL, size_t Offset, size_t& Next_Offset);

	// Allocate weight blocks.
	void	maloc_WEIGHT									(Data_Logger* DataL, size_t Offset, size_t& Next_Offset);

	// Allocate output blocks.
	void	maloc_DATA_OUT									(Data_Logger* DataL, size_t Offset, size_t& Next_Offset);

	// Allocate partial-sum blocks.
	void	maloc_PSUM										(Data_Logger* DataL, size_t Offset, size_t& Next_Offset);

	// Allocate a specific DATA_IN node, duplicating if needed.
	void	maloc_DATA_IN_Node								(Data_Logger* DataL, size_t vlt,	size_t* SoBI, SNID_t Orig_Node, Dependency Deps, bool IZero_Force);

	// Allocate a specific WEIGHT node, duplicating if needed.
	void	maloc_WEIGHT_Node								(Data_Logger* DataL, size_t vlt,	size_t* SoBI, SNID_t Orig_Node, Dependency Deps);

	// Clear allocation flags for PZero/PSUM blocks.
	void	Clear_Allocation_Flag							(Data_Logger* DataL);

	// Add a new time slot to the allocation matrix.
	void	Addin_New_Time									(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax, size_t time);

	// Get index of a time slot (create if missing).
	size_t	Return_the_Time_idx								(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax, size_t time);

	// Insert a data block into the allocation matrix across its lifetime.
	void	Add_Data_Block_to_Matrix						(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax, Data_Block  DB);

	// Add a new row to the allocation matrix.
	size_t	Add_New_Matric_Row								(std::vector<std::vector<DBID_t>>& matrix);

	// Optimize the allocation matrix (placeholder).
	void	Optimize_Allocation_Matrix						(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax);

	// Assign starting addresses for blocks from the allocation matrix.
	size_t	Setting_Start_Address_of_Allocation_Matrix		(std::vector<std::vector<DBID_t>>  matrix, Data_Logger* DataL, size_t Offset);

	// Pre-reserve rows in the allocation matrix.
	void	Reserve_Rows									(std::vector<std::vector<DBID_t>>& matrix, size_t cnt);

	// Return maximum sequential length of a level.
	size_t	Get_Max_seq_length_of_level						(size_t lvl);









	
	struct WeightKey
	{
		size_t K, C, FH, FW;

		bool operator==(const WeightKey& other) const
		{
			return K == other.K && C == other.C &&
				FH == other.FH && FW == other.FW;
		}
	};

	struct ExecKey
	{
		size_t B, K, C, W, FH, FW;

		bool operator==(const ExecKey& o) const
		{
			return B == o.B && K == o.K && C == o.C &&
				W == o.W && FH == o.FH && FW == o.FW;
		}
	};

	struct WeightHasher
	{
		size_t operator()(const WeightKey& k) const noexcept
		{
			return std::hash<size_t>()(k.K) ^
				(std::hash<size_t>()(k.C) << 1) ^
				(std::hash<size_t>()(k.FH) << 2) ^
				(std::hash<size_t>()(k.FW) << 3);
		}
	};

	struct ExecHasher
	{
		size_t operator()(const ExecKey& k) const noexcept
		{
			return std::hash<size_t>()(k.B) ^
				(std::hash<size_t>()(k.K) << 1) ^
				(std::hash<size_t>()(k.C) << 2) ^
				(std::hash<size_t>()(k.W) << 3) ^
				(std::hash<size_t>()(k.FH) << 4) ^
				(std::hash<size_t>()(k.FW) << 5);
		}
	};
};



	

