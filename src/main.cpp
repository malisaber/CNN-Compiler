

#include <string>
#include <chrono>
#include <iostream>
#include "CLI11.hpp"
#include "Compiler.h"
#include "Network_Loader.h"







// Entry point: builds a test network, schedules/maps/allocates, and runs code generation.
int main											(
														int argc,
														char** argv)
{
	CLI::App app{"CNN Compiler for custom CNN accelerator"};
 
	// Defaults (same as your original values)
	int									verbose		=	0;
	size_t								report		=	0;
	bool								help_report	=	false;
	bool								rearrange	=	false;
	std::filesystem::path				Net_Filex	=	"";		// Network to compile
	std::filesystem::path				Hal_Direc	=	"";		//Materials;
	std::filesystem::path				Dmp_Direc	=	"";		//Dump directory";
	std::filesystem::path				Out_Direc	=	"";		//Output directory;
	std::filesystem::path				Drm_Direc	=	"";		//Output directory;
	std::vector<std::filesystem::path>	Inp_names	=	{""};	//{"E:/WSL/data_gen/IDG/Input_1.bin"};	//	one entry per input layer
	std::vector<std::filesystem::path>	Wgt_names	=	{""};	//{"E:/WSL/data_gen/WDG/Weight_1.bin"};	//	one entry per CNN	layer

	// --- Flags / Options ---
	// Boolean flag: --verbose / --no-verbose (or just -v to force true)
	app.add_option	("-v,--verbose", 				verbose, 		"Verbosity level"																);
	app.add_option	("-p,--report",					report,			"Generate Reports, for more info, run -help-report"								);
	app.add_flag	("-a,--data-rearrange",			rearrange,		"run Data Rearrengment Engine to generate the necessary files for simulation"	);
	app.add_option	("-n,--network",				Net_Filex,		"Network definition file (JSON)"												);
	app.add_option	("-l,--hal-dir",				Hal_Direc,		"Hardware Abstraction Layer directory"											);
	app.add_option	("-d,--dump-dir",				Dmp_Direc,		"Dump directory"																);
	app.add_option	("-o,--Output-dir",				Out_Direc,		"Output directory"																);
	app.add_option	("-r,--dram-dir",				Drm_Direc,		"Output DRAM directory"															);
	app.add_option	("-i,--input",					Inp_names,		"Input layer file(s), one per input layer"										);
	app.add_option	("-w,--weight",					Wgt_names,		"Weight file(s), one per CNN layer"												);
	app.add_flag	("--help-report", 				help_report, 	"Show detailed report help"														);


	CLI11_PARSE(app, argc, argv);
	
	//->required()->check(CLI::ExistingFile);
	//->required()->check(CLI::ExistingDirectory);
	


	if (help_report)
	{
    	std::cout << std::endl << "REPORT HELP";
		std::cout << std::endl << "===========";
		std::cout << std::endl << "	";
		std::cout << std::endl << "-p, --report";
		std::cout << std::endl << "    Generate report file of each internal part of the compiler.";
		std::cout << std::endl << "	";
		std::cout << std::endl << "-p 1   -> Generate Nodes                   information report file";
		std::cout << std::endl << "-p 2   -> Generate Nodes   (Reshaped)      information report file";
		std::cout << std::endl << "-p 4   -> Generate Data Blocks             information report file";
		std::cout << std::endl << "-p 8   -> Generate Generatable Data files  information report file";
		std::cout << std::endl << "-p 16  -> Generate Threads                 information report file";
		std::cout << std::endl << "-p 32  -> Generate Threads (Optimized)     information report file";
		std::cout << std::endl << "-p 64  -> Generate Mapping (Raw)           information report file";
		std::cout << std::endl << "-p 128 -> Generate Mapping                 information report file";
		std::cout << std::endl << "-p 256 -> Generate Spacing                 information report file";
		std::cout << std::endl << "	";
		std::cout << std::endl << "or any combination of those for generating multiple report files";
		std::cout << std::endl;
    	std::exit(0);
	};
	
	Compiler	compiler;

	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	BuildNetworkFromFile			(Net_Filex, compiler);
	compiler.Add_HAL_Directory		(Hal_Direc);
	compiler.Add_Dump_Directory		(Dmp_Direc);
	compiler.Add_Output_Directory	(Out_Direc);
	compiler.Add_Dram_Directory		(Drm_Direc);
	compiler.Add_Data_Gen_Directory	(Inp_names, Wgt_names);
	compiler.Compile				(verbose, rearrange);
	compiler.Report					(report);
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();



	
	std::cout << "Ellapsed Time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

	return 0;
}








