#pragma once


#include <filesystem>
#include <iostream>

#include "Utils.h"
#include "Config.h"
#include "Layers.h"
#include "Hardware.h"
#include "Data_Logger.h"
#include "Data_Generator.h"
#include "Code_Generator.h"
#include "Dependency_Logger.h"


class Compiler
{

	Dependency_Logger*					Dpnd_Log_Eng;
	Data_Logger*						Data_Log_Eng;
	Data_Generator*						Data_Gen_Eng;
	Code_Generator*						Code_Gen_Eng;
	NETWORK*							Network;
	Hardware*							HW;

	bool								Added_Lyr;
	bool								Added_Dir;
	bool								Added_Mtr;
	bool								Added_Out;
	bool								Added_Drm;
	bool								Compiled;
	bool								All_done;


	std::filesystem::path				DA_Dump_dest;
	std::filesystem::path				DP_Dump_dest;
	std::filesystem::path				CG_Matr_file;
	std::filesystem::path				CG_Dump_dest;
	std::filesystem::path				CG_Outp_dest;
	std::filesystem::path				DG_DRAM_dest;

	std::vector<std::filesystem::path>	Inp_names;
	std::vector<std::filesystem::path>	Wgt_names;

public:
	Compiler										();
	~Compiler										();

	// Compile 
	void	Compile									(
														bool verbose);

	// Rearrenges the dataset and weights
	void	Rearreng_Data							(
														bool verbose,
														std::vector<std::filesystem::path>	Inps,
														std::vector<std::filesystem::path>	Wgts);

	// Adds layer to the network:
	size_t	Add_Layer_to_Notwork					(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes);

	// Adds layer to the network:
	size_t	Add_Layer_to_Notwork					(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes,
														MaxP_Layer_Info MP_window);

	// Adds a Dump location
	void	Add_Dump_Directory						(
														std::filesystem::path dir);

	// Adds directory for saving generated data files 
	void	Add_Data_Gen_Directory					(
														std::vector<std::filesystem::path>	Inps,
														std::vector<std::filesystem::path>	Wgts);

	// Adds a dram output directory 
	void	Add_Dram_Directory						(
														std::filesystem::path	src);

	// Adds a haal directory 
	void	Add_HAL_Directory						(
														std::filesystem::path	src);

	// Adds an output directory 
	void	Add_Output_Directory					(
														std::filesystem::path	src);
	
	// Reports something
	void	Report									();

private:
	// Buils the network and its dependencies
	void	Build_Network							(
														bool verbose);

	// Schedules the dependencies
	void	Schedule_Dependencies					(
														bool verbose);

	// Building Executing threads
	void	Build_Threads							(
														bool verbose);

	// Maps the Nodes on the actual Hardware
	void	Map										(
														bool verbose);

	// Allocating the memory blocks
	void	Allocate								(
														bool verbose);

	// Generats the code
	void	Generate_Codes							(
														bool verbose);

	//// Initiating Data rearrengement engine
	//void	DR_initiate								();

};
