

#include <string>
#include <chrono>
#include <iostream>
#include "Compiler.h"
#include "CLI11.hpp"







// Entry point: builds a test network, schedules/maps/allocates, and runs code generation.
int main(int argc, char** argv)
{
	CLI::App app{"CNN Compiler for custom CNN accelerator"};

	// Defaults (same as your original values)
    bool                                verbose     =   true;
    std::filesystem::path               Dmp_Dir     =   "";		//"E:/WSL/Compiler/dump";
    std::filesystem::path               Src_Dir     =   "";		//"E:/WSL/Software";
    std::vector<std::filesystem::path>  Inp_names   =   {""};	//{"E:/WSL/data_gen/IDG/Input_1.bin"};    //  one entry per input layer
    std::vector<std::filesystem::path>  Wgt_names   =   {""};	//{"E:/WSL/data_gen/WDG/Weight_1.bin"};   //  one entry per CNN   layer

    // --- Flags / Options ---
    // Boolean flag: --verbose / --no-verbose (or just -v to force true)
    app.add_flag("-v,--verbose,!--no-verbose", verbose, "Enable verbose output");

    // Single path options
    app.add_option("-d,--dump-dir", Dmp_Dir, "Dump directory")
        ->check(CLI::ExistingDirectory)	//	optional: validate path exists
        ->required();					//	Required input argument

    app.add_option("-s,--src-dir", Src_Dir, "Source directory")
        ->check(CLI::ExistingDirectory)	// optional: validate path exists
        ->required();					//	Required input argument

    // Vector of paths: repeatable, e.g. -i file1.bin -i file2.bin
    app.add_option("-i,--input", Inp_names, "Input layer file(s), one per input layer")
        ->check(CLI::ExistingFile)		// optional: validate each file exists
        ->required();					//	Required input argument

    app.add_option("-w,--weight", Wgt_names, "Weight file(s), one per CNN layer")
        ->check(CLI::ExistingFile)		// optional: validate each file exists
        ->required();					//	Required input argument

    CLI11_PARSE(app, argc, argv);
    


    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    Compiler    compiler;




    compiler.Add_Dump_directory     (Dmp_Dir);
    compiler.Add_Sorce_directory    (Src_Dir);
    compiler.Add_Data_Gen_Directory (Inp_names, Wgt_names);
    compiler.Compile(verbose);
    compiler.Report();



    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Ellapsed Time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

    return 0;
}








