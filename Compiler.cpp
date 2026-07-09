#include "Compiler.h"


Compiler::Compiler                          ()
{
	Dpnd_Log_Eng        = new Dependency_Logger();
    Data_Log_Eng        = new Data_Logger();
    Data_Gen_Eng        = new Data_Generator();
    Code_Gen_Eng        = new Code_Generator();
    Network             = new NETWORK();
    HW                  = new Hardware(Ava_Planes);

    Added_Layer         = false;
    Added_Dir           = false;
    Added_Src           = false;
    Compiled            = false;
    All_done            = false;

    CG_Sorc_file        = "";
    DA_Dump_dest        = "";
    DP_Dump_dest        = "";
    CG_Dump_dest        = "";
    CW_Dump_dest        = "";
    DG_Dump_dest        = "";


    Inp_names.clear();
    Wgt_names.clear();
}


Compiler::~Compiler                         ()
{
}


void    Compiler::Compile                   (bool verbose)
{
    if (!Added_Layer)       throw std::runtime_error("No Layers were added to the network;\nPlrase add some layers using ""Add_Layer_to_Notwork"" member.\n");
    if (!Added_Dir)         throw std::runtime_error("No Dump Directory were added;\nPlrase add a Dump directory using ""Add_Dump_directory"" member.\n");
    if (!Added_Src)         throw std::runtime_error("No Source Directory were added;\nPlrase add a Source directory using ""Add_Sorce_directory"" member.\n");

    // Building the network
    Build_Network(verbose);

    //  Scheduling 
    Schedule_Dependencies(verbose);

    // Building Threads
    Build_Threads(verbose);

    // Mapping
    Map(verbose);

    //  Allocation
    Allocate(verbose);

    // Code Geneneration
    Generate_Codes(verbose);
    
    // Marking as compiled
    Compiled = true;
    Rearreng_Data(verbose, Inp_names, Wgt_names);

    // Informing the completion of the compilation
    std::cout << "Compilation is Done!" << std::endl;
    All_done = true;
}


// Rearrenges the dataset and weights
void    Compiler::Rearreng_Data             (bool verbose, std::vector<std::filesystem::path>  Inps, std::vector<std::filesystem::path>  Wgts)
{
    if (!Compiled)  throw std::runtime_error("Please Compile Before re-arrenging the dataset.\n");

    // Data re-arrangement
    if (verbose)    std::cout << "Running Data Re-arrengement Engine ..." << std::endl;
                    Data_Gen_Eng->Get_Inps_info(Network);
                    Data_Gen_Eng->Get_Wgts_info(Network);
                    Data_Gen_Eng->load_input_files (Inps);
                    Data_Gen_Eng->load_Weight_files(Wgts);
                    Data_Gen_Eng->Generate(Data_Log_Eng, DG_Dump_dest);
    if (verbose)    std::cout << "Running Data Re-arrengement Engine Done!" << std::endl << std::endl << std::endl;

}


// Adds layer to the network:
size_t  Compiler::Add_Layer_to_Notwork      (Layer_2D_Types ltype, Conv_Layer_Info linfo, Conv_Layer_Info lmaxes)
{
    Added_Layer = true;
    return Network->Add_Layer(ltype, linfo, lmaxes);
}


// Adds layer to the network:
size_t  Compiler::Add_Layer_to_Notwork      (Layer_2D_Types ltype, Conv_Layer_Info linfo, Conv_Layer_Info lmaxes, MaxP_Layer_Info MP_window)
{
    Added_Layer = true;
    return Network->Add_Layer(ltype, linfo, lmaxes, MP_window);
}


// Adds a Dump location
void    Compiler::Add_Dump_directory        (std::filesystem::path dir)
{
    if (!std::filesystem::exists(dir))  std::filesystem::create_directory(dir);

    DA_Dump_dest = dir / ("Data_Logger");
    DP_Dump_dest = dir / ("Dependency_Logger");
    CG_Dump_dest = dir / ("Code_Gen");
    CW_Dump_dest = dir / ("Code_Gen") / ("Code_Wizard");
    DG_Dump_dest = dir / ("Data_Gen");
    
    std::filesystem::create_directory(DA_Dump_dest);
    std::filesystem::create_directory(DP_Dump_dest);
    std::filesystem::create_directory(CG_Dump_dest);
    std::filesystem::create_directory(CW_Dump_dest);
    std::filesystem::create_directory(DG_Dump_dest);

    Added_Dir = true;
}


// Adds directory for saving generated data files 
void    Compiler::Add_Data_Gen_Directory    (std::vector<std::filesystem::path> Inps, std::vector<std::filesystem::path> Wgts)
{
    Inp_names = Inps;
    Wgt_names = Wgts;
}


// Adds a source directory 
void Compiler::Add_Sorce_directory          (std::filesystem::path src)
{
    CG_Sorc_file    = src;
    Added_Src       = true;
}


// Reports something
void    Compiler::Report                    ()
{
    if (!All_done)       throw std::runtime_error("The compilation has not finished yet.\n");
    
    std::cout << "Size of Data_Log_Eng: " << Data_Log_Eng->size() << std::endl;
    std::cout << "Size of Dpnd_Log_Eng: " << Dpnd_Log_Eng->size() << std::endl;
    std::cout << "Size of Data_Gen_Eng: " << Data_Gen_Eng->Generated_file_size() << std::endl;
    std::cout << "\t\tInputs:  " << Data_Gen_Eng->Generated_Input_files_size() << std::endl;
    std::cout << "\t\tWeights: " << Data_Gen_Eng->Generated_Weight_files_size() << std::endl;
    std::cout << "\t\tZeros:   " << Data_Gen_Eng->Generated_Zero_files_size() << std::endl;
    Data_Log_Eng->print_file                                    (DA_Dump_dest / ("DATA.txt"));
    Dpnd_Log_Eng->print_file                                    (DP_Dump_dest / ("Nodes_Reshaped.txt"));
    Dpnd_Log_Eng->print_Thread_info_file                        (DP_Dump_dest / ("Threads.txt"));
    Dpnd_Log_Eng->print_Optimized_Execution_Thread_info_file    (DP_Dump_dest / ("Threads_opt.txt"));
    Dpnd_Log_Eng->print_mapping_file                            (DP_Dump_dest / ("Mapping.txt"));
    std::cout << std::endl << "Files are Generated" << std::endl << std::endl << std::endl;
}












// Builds the network and its dependencies
void    Compiler::Build_Network             (bool verbose)
{
    bool bit;

    // Building the network
    if (verbose)    std::cout << "Compiling the Network ... " << std::endl;
    bit =           Network->Build_Network(Data_Log_Eng, Dpnd_Log_Eng);
    if (verbose)    std::cout << "Network Compilation Was " << Res_Dec(bit) << std::endl << std::endl << std::endl;
}


// Schedules the dependencies
void    Compiler::Schedule_Dependencies     (bool verbose)
{
    size_t tmp;

    //  Scheduling 
    if (verbose)    std::cout << "Running Scheduling Engine ..." << std::endl;
    if (verbose)    std::cout << "Total Node: " << Dpnd_Log_Eng->size() << std::endl;
    tmp =           Dpnd_Log_Eng->Schedule_Nodes(Data_Log_Eng);
    if (verbose)    std::cout << "Scheduling Engine Done!" << std::endl;
    if (verbose)    std::cout << "Nodes that did not scheduled: " << Dpnd_Log_Eng->size() - tmp << std::endl << std::endl << std::endl;
}


// Builds Executing threads
void    Compiler::Build_Threads             (bool verbose)
{
    bool bit;

    // Building Threads
    if (verbose)    std::cout << "Running Thread Building Engine ..." << std::endl;
    bit =           Dpnd_Log_Eng->Build_Threads();
    if (verbose)    std::cout << "Thread Building Engine Done!" << std::endl;
    if (verbose)    std::cout << "Result of Thread Building Engine: " << Res_Dec(bit) << std::endl;
    if (verbose)    std::cout << "Running Thread Optimization Engine ..." << std::endl;
    bit =           Dpnd_Log_Eng->Optimizing_Execution_Threads();
    if (verbose)    std::cout << "Thread Optimization Engine Done!" << std::endl << std::endl << std::endl;
}


// Maps the Nodes on the actual Hardware
void    Compiler::Map                       (bool verbose)
{
    bool bit;
    size_t tmp;

    // Mapping
    if (verbose)    std::cout << "Running Mapping Engine ..." << std::endl;
    tmp =           Dpnd_Log_Eng->Map(Data_Log_Eng, HW, DP_Dump_dest / ("Mapper_Dump.txt"));
    bit =           Dpnd_Log_Eng->print_mapping_file(DP_Dump_dest / "Mapping_Raw.txt");
    if (verbose)    std::cout << "Mapping Engine Done!" << std::endl;
    if (verbose)    std::cout << "Nodes that did not Mapped: " << Dpnd_Log_Eng->size() - tmp << std::endl;
    if (verbose)    std::cout << "Calculating Timing ..." << std::endl;
    tmp =           Dpnd_Log_Eng->Calculate_Data_Block_Timing(Data_Log_Eng);
    if (verbose)    std::cout << "Timing Calculation Done!" << std::endl;
    if (verbose)    std::cout << "Last Time: " << tmp << std::endl << std::endl << std::endl;
                    Dpnd_Log_Eng->print_file(DP_Dump_dest / "Nodes.txt");
}


// Allocates the memory blocks
void    Compiler::Allocate                  (bool verbose)
{
    bool bit;
    size_t Icnt;	// Input	Data Block Count
    size_t Wcnt;	// Weight	Data Block Count
    size_t Ocnt;	// Output	Data Block Count
    size_t Pcnt;	// P-Sums	Data Block Count
    size_t Ifst;	// Input	Location Offset
    size_t Wfst;	// Weight	Location Offset
    size_t Ofst;	// Output	Location Offset
    size_t Pfst;	// P-Sums	Location Offset
    size_t Efst;	// End  	Location 


    //  Allocation
    if (verbose)    std::cout << "Running Allocation Engine ..." << std::endl;
    bit =           Dpnd_Log_Eng->Allocte(Data_Log_Eng);
    bit =           Dpnd_Log_Eng->Get_Required_Spaces(Icnt, Wcnt, Ocnt, Pcnt, Ifst, Wfst, Ofst, Pfst, Efst);
    if (verbose)    std::cout << std::dec;
    if (verbose)    std::cout << "Number of Input  Data Block Needed: " << std::setw(8) << Icnt << "\t\tStarting At: " << std::setw(8) << Ifst << std::endl;
    if (verbose)    std::cout << "Number of Weight Data Block Needed: " << std::setw(8) << Wcnt << "\t\tStarting At: " << std::setw(8) << Wfst << std::endl;
    if (verbose)    std::cout << "Number of Output Data Block Needed: " << std::setw(8) << Ocnt << "\t\tStarting At: " << std::setw(8) << Ofst << std::endl;
    if (verbose)    std::cout << "Number of P-Sums Data Block Needed: " << std::setw(8) << Pcnt << "\t\tStarting At: " << std::setw(8) << Pfst << std::endl;
    if (verbose)    std::cout << "\t\t\t\t\t\t\tEnding   At: " << std::setw(8) << Efst << std::endl;
    if (verbose)    std::cout << "Allocation Engine Done!" << std::endl << std::endl << std::endl;
}


// Generats the code
void    Compiler::Generate_Codes            (bool verbose)
{
    // Code Geneneration
    if (verbose)    std::cout << "Running Code Generator Engine ..." << std::endl;
                    Code_Gen_Eng->Extract_PE_Execution_Info(Dpnd_Log_Eng);
                    Code_Gen_Eng->Code_Wizard(Dpnd_Log_Eng, Data_Log_Eng, Network, CG_Sorc_file, CG_Dump_dest, CW_Dump_dest);
    if (verbose)    std::cout << "Code Generator Engine Done!" << std::endl << std::endl << std::endl;
}






