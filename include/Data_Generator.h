#pragma once

#include <filesystem>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "Utils.h"
#include "Utils.h"
#include "Layers.h"
#include "Data_Logger.h"



class Data_Generator
{
	size_t									GIDF_cntr;		// Generated data block counter
	size_t									GWDF_cntr;		// Generated data block counter
	size_t									GZDF_cntr;		// Generated data block counter

	size_t									GIDB_cntr;		// Generated data block counter
	size_t									GWDB_cntr;		// Generated data block counter
	size_t									GZDB_cntr;		// Generated data block counter

	std::vector<Conv_Layer_Info>			Inps_linfo;		// layer information of Inputs 
	std::vector<Conv_Layer_Info>			Inps_minfo;		// layer information of Inputs 
	std::vector<size_t>						Inps_lid;		// layer ID			 of Inputs 
	std::vector<Conv_Layer_Info>			Wgts_linfo;		// layer information of Weights 
	std::vector<Conv_Layer_Info>			Wgts_minfo;		// layer information of Weights 
	std::vector<size_t>						Wgts_lid;		// layer ID			 of Weights 

	std::vector<std::filesystem::path>		Inps_Fnames;	// input files' name
	std::vector<std::filesystem::path>		Wgts_Fnames;	// weight files' name

public:
	// Initialize generator state and reserve storage.
	Data_Generator									();
	
	// Destroy generator	(no explicit resource cleanup required).
	~Data_Generator									();

	// Set inputs information getting from network
	void	Get_Inps_info							(
														NETWORK* NET,
														std::vector<size_t>& lids);

	// Set weights information getting from network
	void	Get_Wgts_info							(
														NETWORK* NET
														std::vector<size_t>& lids);

	// Return number of generator input files.
	size_t	Generated_Input_files_size				();

	// Return number of generator Weight stored.
	size_t	Generated_Weight_files_size				();

	// Return number of generator Zero stored.
	size_t	Generated_Zero_files_size				();

	// Return number of generator records stored.
	size_t	Generated_file_size						();

	// Load input files' name 
	void	load_input_files						(
														std::vector<std::filesystem::path> names);

	// Load weight files' name 
	void	load_Weight_files						(
														std::vector<std::filesystem::path> names);

	// Generating files
	void	Generate								(
														Data_Logger* DG,
														std::filesystem::path dram);


private:

	// Reading a binary file of data
	std::vector<uint16_t> Read_binary_file			(
														std::filesystem::path& filename,
														size_t element_size);

	// Convert the bcwh idexed to offset	 ------------ Comiler Side ------------	 ------------ DataSet Side ------------
	size_t	Relative_2_Absolute_idx					(
														size_t b,
														size_t c,
														size_t w,
														size_t h,
														//size_t B,
														size_t C,
														size_t W,
														size_t H);

	// Generate data file for input layer x
	void	Generate_IDF							(
														Data_Logger* DG,
														size_t idx,
														std::filesystem::path dest);

	// Generate Weight data file for CNN layer x
	void	Generate_WDF							(
														Data_Logger* DG,
														size_t idx,
														std::filesystem::path dest);
	
	// Generate Zero data files
	void	Generate_ZDF							(
														Data_Logger* DG,
														std::filesystem::path dest);

	// returning the input value
	size_t	write_input_value						(
														std::ofstream& fid,
														std::vector<uint16_t> data,
														Conv_Layer_Info linfo,
														Conv_Layer_Info Dims,
														Conv_Layer_Info Idxs,
														bool* used);
	
	// returning the weight value 
	size_t	write_Weight_value						(
														std::ofstream& fid,
														std::vector<uint16_t> data,
														Conv_Layer_Info linfo,
														Conv_Layer_Info Dims,
														Conv_Layer_Info Idxs,
														bool* used);

	// Writing a zero data block
	size_t	write_zero_block						(
														std::ofstream& fid,
														Conv_Layer_Info Dims);
};









