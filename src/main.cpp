

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
	bool								verbose		=	true;
	std::filesystem::path				Net_Filex	=	"";		// Network to compile
	std::filesystem::path				Mtr_Direc	=	"";		//Materials;
	std::filesystem::path				Dmp_Direc	=	"";		//Dump directory";
	std::filesystem::path				Out_Direc	=	"";		//Output directory;
	std::vector<std::filesystem::path>	Inp_names	=	{""};	//{"E:/WSL/data_gen/IDG/Input_1.bin"};	//	one entry per input layer
	std::vector<std::filesystem::path>	Wgt_names	=	{""};	//{"E:/WSL/data_gen/WDG/Weight_1.bin"};	//	one entry per CNN	layer

	// --- Flags / Options ---
	// Boolean flag: --verbose / --no-verbose (or just -v to force true)
	app.add_flag	("-v,--verbose,!--no-verbose",	verbose,	"Enable verbose output");
	app.add_option	("-n,--network",				Net_Filex,	"Network definition file (JSON)"			);
	app.add_option	("-m,--Mterial-dir",			Mtr_Direc,	"Material directory"						);
	app.add_option	("-d,--dump-dir",				Dmp_Direc,	"Dump directory"							);
	app.add_option	("-o,--Output-dir",				Out_Direc,	"Output directory"							);
	app.add_option	("-i,--input",					Inp_names,	"Input layer file(s), one per input layer"	);
	app.add_option	("-w,--weight",					Wgt_names,	"Weight file(s), one per CNN layer"			);
	
	//->required()->check(CLI::ExistingFile);
	//->required()->check(CLI::ExistingDirectory);


	CLI11_PARSE(app, argc, argv);
	


	Compiler	compiler;



	
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	BuildNetworkFromFile			(Net_Filex, compiler);
	compiler.Add_Dump_Directory		(Dmp_Direc);
	compiler.Add_Material_Directory	(Mtr_Direc);
	compiler.Add_Output_Directory	(Out_Direc);
	compiler.Add_Data_Gen_Directory	(Inp_names, Wgt_names);
	compiler.Compile				(verbose);
	compiler.Report					();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();



	
	std::cout << "Ellapsed Time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

	return 0;
}








