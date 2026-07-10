#pragma once


#include <vector>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include "Utils.h"
#include "Config.h"
#include "Data_Logger.h"
#include "Dependency_Logger.h"
#include "Layers.h"


#define OVERWRITE	std::filesystem::copy_options::overwrite_existing


class Code_Generator
{
	bool Imported;														// True once ordering info has been imported from Dependency_Logger.
	bool Extracted;														// True once PE/MPDR execution info has been extracted.
	bool Modified;														// True once extracted info has been modified/normalized for generation.

	std::ofstream Data_H_file;											// header of Datafile
	std::ofstream Data_C_file;											// Cpp	 of Datafile

	std::vector<Ord_Address> Ordering_Index;							// Index from scheduling node ID to ordering address	(lvl/bline/node/pos).
	std::vector<std::vector<std::vector<CG_PE_Node>>>		CG_PEs;		// Extracted processing element execution descriptors per level/baseline.
	std::vector<std::vector<std::vector<CG_MPDR_Node>>>		CG_MPDRs;	// Extracted MPDR execution descriptors per level/baseline.
	std::vector<std::vector<std::vector<Ordering_Node>>>	Ordering;	// Scheduling order per level/baseline/node captured from Dependency_Logger.
	
public:

	// Initialize empty generator state.
	Code_Generator									();

	// Destroy generator	(no explicit resource cleanup required).
	~Code_Generator									();


	// Import ordering from Dependency_Logger and build CG_PEs/CG_MPDRs structures.
	bool	Extract_PE_Execution_Info				(
														Dependency_Logger* DpndL);

	// End-to-end generation: modify extracted info, copy template files, and emit platform main/execute/peripheral functions to destination.
	bool	Code_Wizard								(
														Dependency_Logger* DpndL,
														Data_Logger* DataL,
														NETWORK* network,
														std::filesystem::path Src_fname,
														std::filesystem::path dst_fname,
														std::filesystem::path CGCW_Dump_dir);


private:


	// Build execution info for a single PE	(per level/baseline/plane/vault).
	bool	Extract_Single_PE_Execution_Info		(
														Dependency_Logger* DpndL,
														CG_PE_Node& PEInfo,
														size_t lvl,
														size_t baseline,
														size_t plane,
														size_t vault);

	// Build MPDR execution info for a single baseline.
	bool	Extract_MPDR_Execution_Info				(
														Dependency_Logger* DpndL,
														CG_MPDR_Node& MPDRInfo,
														size_t lvl,
														size_t baseline);

	// Link activation/accumulation outputs back into the owning PE entries.
	void	Link_PE_Storage							(
														Dependency_Logger* DpndL,
														size_t lvl,
														size_t baseline);

	// Remove empty baselines from CG_PEs/CG_MPDRs.
	void	Cleans_CG_Vectors						();

	// Normalize PE inputs to match expected scheduling/data patterns.
	void	Modify_PE_Inputs						();

	// Reorder MPDR units across all levels to maximize spatial regularity.
	void	Modify_MPDR_units						(
														Data_Logger* DataL);

	// Reorder MPDR units for a specific level.
	void	Modify_MPDR_units						(
														Data_Logger* DataL,
														size_t lvl);

	// Check if PE inputs/outputs/accumulators are equally spaced in memory.
	void	Equal_Spaced_Check_PE					(
														Data_Logger* DataL,
														size_t lvl,
														size_t bline,
														size_t node,
														bool& ESI,
														bool& ESO,
														bool& ESP);

	// Check if MPDR inputs/outputs are equally spaced in memory for a baseline.
	void	Equal_Spaced_Check_MPDR					(
														Data_Logger* DataL,
														size_t lvl,
														size_t bline);
	
	// Adjust zero-block inputs to the closest duplicates with correct spacing.
	void	Modify_Zero_Blocks					 	(
														Data_Logger* DataL,
														size_t lvl,
														size_t bline,
														size_t node,
														bool& first,
														bool& last);

	// Apply all normalization/spacing/zero-block fixes and dump intermediate files.
	void	Modify_All							 	(
														Data_Logger* DataL,
														std::filesystem::path fname);

	// Emit a spacing diagnostics report file.
	void	Print_Spacing							(
														Data_Logger* DataL,
														std::filesystem::path fname);

	//// Build spacing pattern for PE outputs; returns false if inconsistent.
	//bool	Get_PE_Out_Spacing_Pattern				(Data_Logger* DataL, size_t lvl, size_t bline, size_t node, std::vector<size_t>& pattern);
	//
	//// Build spacing pattern for PE accumulator outputs; returns false if inconsistent.
	//bool	Get_PE_Acc_Spacing_Pattern				(Data_Logger* DataL, size_t lvl, size_t bline, size_t node, std::vector<size_t>& pattern);
	
	// Generate Codes
	void	Generate_Codes						 	(
														Dependency_Logger* DpndL,
														Data_Logger* DataL,
														NETWORK* network,
														std::filesystem::path srce,
														std::filesystem::path fname);

	// Copy template source files from srce to dest	(overwrite if present).
	void	Copy_File								(
														std::filesystem::path srce,
														std::filesystem::path dest);

	// Emit the first part of generated main.cpp	(includes globals and setup).
	void	Generate_Main_P1						(
														std::ofstream& files_out);

	// Emit "Platform_Execute_Layer_<lvl>" function body.
	void	Generate_Platform_Execute_Layer_lvl		(
														Data_Logger* DataL,
														std::ofstream& files_out,
														size_t lvl);

	// Emit "Platform_Execute_Layer_<lvl>_BL<bline>" function body.
	void	Generate_Platform_Execute_Layer_bl	 	(
														Data_Logger* DataL,
														std::ofstream& files_out,
														size_t lvl,
														size_t bline);

	// Emit "Platform_Execute_BseLine" function body.
	void	Generate_Platform_Execute_BseLine		(
														std::ofstream& files_out);

	// Emit "Platform_Peripheral_Layer_<lvl>" function body.
	void	Generate_Platform_Peripheral_Layer_lvl 	(
														Dependency_Logger* DpndL,
														Data_Logger* DataL,
														NETWORK* network,
														std::ofstream& files_out,
														size_t lvl);

	// Emit the tail of generated main.cpp	(interrupt handlers and helpers).
	void	Generate_Main_P2						(
														std::ofstream& files_out);

	// adding necessary data for execution part to the Data.h and making its array
	void	Generage_Data_Blocks_Exe_lvl_bline	 	(
														Data_Logger* DataL,
														size_t lvl,
														size_t bline);

	// adding necessary data for peripheral part to the Data.h and making its array
	void	Generage_Data_Blocks_Peri_lvl			(
														Dependency_Logger* DpndL,
														Data_Logger* DataL,
														NETWORK* network,
														size_t lvl,
														std::vector<std::vector<size_t>> All_BLs);
	
	// adding datablocks
	void	Generage_Data_Blocks_Exe_Baseline		(
														Data_Logger* DataL);
};

