

#include <string>
#include <chrono>
#include <iostream>
#include "Compiler.h"



#define __TEST_CASE_4__




// Entry point: builds a test network, schedules/maps/allocates, and runs code generation.
int main()
{
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    bool                                verbose     =   true;
    std::filesystem::path               Dmp_Dir     =   "E:/WSL/Compiler/dump";
    std::filesystem::path               Src_Dir     =   "E:/WSL/Software";
    std::vector<std::filesystem::path>  Inp_names   =   {"E:/WSL/data_gen/IDG/Input_1.bin"};    //  one entry per input layer
    std::vector<std::filesystem::path>  Wgt_names   =   {"E:/WSL/data_gen/WDG/Weight_1.bin"};   //  one entry per CNN   layer


    Compiler    compiler;
    



#ifdef __TEST_CASE_0__

    //                              B     K     C     W     H     F     F
    Conv_Layer_Info L0_info     = { 1,   32,   32,   32,    4,    3,    3};
    Conv_Layer_Info L1_info     = { 1,  128,   32,   32,    4,    3,    3};
    Conv_Layer_Info L2_info     = { 1,    0,  128,   32,    4,    0,    0};
    Conv_Layer_Info L3_info     = { 1,  128,  128,   16,    2,    3,    3};
    Conv_Layer_Info L4_info     = { 1,  128,  128,   16,    2,    3,    3};
    
    //                              B    K     C     W      H     F    F                        
    Conv_Layer_Info Maxes       = { 1,   8,    8,    8,     1,    1,   1 };
    MaxP_Layer_Info Window      = { 2,   2 };

    size_t L0ID  = compiler.Add_Layer_to_Notwork(L2D_Input,      L0_info, Maxes);
    size_t L1ID  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC,  L1_info, Maxes);
    size_t L2ID  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,    L2_info, Maxes, Window);
    size_t L3ID  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC,  L3_info, Maxes);
    size_t L4ID  = compiler.Add_Layer_to_Notwork(L2D_Output,     L4_info, Maxes);


#endif


    
    
#ifdef __TEST_CASE_1__
    
    //                              B    K     C     W     H    F    F
    Conv_Layer_Info L0_info     = { 1,   32,   32,   32,   32,  3,   3 };
    Conv_Layer_Info L1_info     = { 1,   32,   32,   32,   32,  3,   3 };
    Conv_Layer_Info L2_info     = { 1,   0,    32,   32,   32,  0,   0 };
    Conv_Layer_Info L3_info     = { 1,   32,   32,   16,   16,  3,   3 };
    //                              B    K     C     W     H    F    F                        
    Conv_Layer_Info Maxes       = { 1,   16,   16,   16,   1,   1,   1 };
    MaxP_Layer_Info Window      = { 2,   2 };

    size_t L0ID  = compiler.Add_Layer_to_Notwork(L2D_Input,        L0_info, Maxes);
    size_t L1ID  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC,    L1_info, Maxes);
    size_t L2ID  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,      L2_info, Maxes, Window);
    size_t L3ID  = compiler.Add_Layer_to_Notwork(L2D_Output,       L3_info, Maxes);

    
#endif
        
        


#ifdef __TEST_CASE_2__
    //                              B    K     C     W     H    F    F
    Conv_Layer_Info L0_info     = { 1,   32,   32,   128,  128, 3,   3};
    Conv_Layer_Info L1_info     = { 1,   64,   32,   128,  128, 3,   3};
    Conv_Layer_Info L2_info     = { 1,   0,    64,   128,  128, 0,   0};
    Conv_Layer_Info L3_info     = { 1,   128,  64,   64,   64,  3,   3};
    Conv_Layer_Info L4_info     = { 1,   0,    128,  64,   64,  0,   0};
    Conv_Layer_Info L5_info     = { 1,   256,  128,  32,   32,  3,   3};
    Conv_Layer_Info L6_info     = { 1,   0,    256,  32,   32,  0,   0};
    Conv_Layer_Info L7_info     = { 1,   256,  256,  16,   16,  3,   3};
    
    //                              B    K     C     W     H    F    F                        
    Conv_Layer_Info Maxes       = { 1,   16,   16,   16,   1,   1,   1 };
    MaxP_Layer_Info Window      = { 2,   2 };

    size_t L0ID  = compiler.Add_Layer_to_Notwork(L2D_Input,      L0_info, Maxes);
    size_t L1ID  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC,  L1_info, Maxes);
    size_t L2ID  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,    L2_info, Maxes, Window);
    size_t L3ID  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC,  L3_info, Maxes);
    size_t L4ID  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,    L4_info, Maxes, Window);
    size_t L5ID  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC,  L5_info, Maxes);
    size_t L6ID  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,    L6_info, Maxes, Window);
    size_t L7ID  = compiler.Add_Layer_to_Notwork(L2D_Output,     L7_info, Maxes);
    

#endif




#ifdef __TEST_CASE_3__
    //                             B    K     C     W     H    F    F
    Conv_Layer_Info L00_info    = {1,   64,   3,    224,  224, 3,   3}; //  Input   Layer
    Conv_Layer_Info L01_info    = {1,   64,   3,    224,  224, 3,   3}; //  Conv.   Layer #1
    Conv_Layer_Info L02_info    = {1,   64,   64,   224,  224, 3,   3}; //  Conv.   Layer #2
    Conv_Layer_Info L03_info    = {1,   0,    64,   224,  224, 0,   0}; //  Pooling Layer #1
    Conv_Layer_Info L04_info    = {1,   128,  64,   112,  112, 3,   3}; //  Conv.   Layer #3
    Conv_Layer_Info L05_info    = {1,   128,  128,  112,  112, 3,   3}; //  Conv.   Layer #4
    Conv_Layer_Info L06_info    = {1,   0,    128,  112,  112, 0,   0}; //  Pooling Layer #2
    Conv_Layer_Info L07_info    = {1,   256,  128,  56,   56,  3,   3}; //  Conv.   Layer #5
    Conv_Layer_Info L08_info    = {1,   256,  256,  56,   56,  3,   3}; //  Conv.   Layer #6
    Conv_Layer_Info L09_info    = {1,   256,  256,  56,   56,  3,   3}; //  Conv.   Layer #7
    Conv_Layer_Info L10_info    = {1,   0,    256,  56,   56,  0,   0}; //  Pooling Layer #3
    Conv_Layer_Info L11_info    = {1,   512,  256,  28,   28,  3,   3}; //  Conv    Layer #8
    Conv_Layer_Info L12_info    = {1,   512,  512,  28,   28,  3,   3}; //  Conv    Layer #9
    Conv_Layer_Info L13_info    = {1,   512,  512,  28,   28,  3,   3}; //  Conv    Layer #10
    Conv_Layer_Info L14_info    = {1,   0,    512,  28,   28,  0,   0}; //  Pooling Layer #4
    Conv_Layer_Info L15_info    = {1,   512,  512,  14,   14,  3,   3}; //  Conv.   Layer #11
    Conv_Layer_Info L16_info    = {1,   512,  512,  14,   14,  3,   3}; //  Conv.   Layer #12
    Conv_Layer_Info L17_info    = {1,   512,  512,  14,   14,  3,   3}; //  Conv.   Layer #13
    //Conv_Layer_Info L18_info    = {1,   0,    512,  14,   14,  0,   0}; //  Pooling Layer #5
    Conv_Layer_Info L19_info    = {1,   0,    512,  14,   14,  0,   0}; //  Output Layer
                                                    
    //                             B    K     C     W     H    F    F                        
    Conv_Layer_Info Maxes       = {1,   16,   16,   14,   1,   1,   1};
    MaxP_Layer_Info Window      = {2,   2};
    

    size_t L00  = compiler.Add_Layer_to_Notwork(L2D_Input,     L00_info, Maxes);
    size_t L01  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L01_info, Maxes);
    size_t L02  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L02_info, Maxes);
    size_t L03  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,   L03_info, Maxes, Window);
    size_t L04  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L04_info, Maxes);
    size_t L05  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L05_info, Maxes);
    size_t L06  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,   L06_info, Maxes, Window);
    size_t L07  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L07_info, Maxes);
    size_t L08  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L08_info, Maxes);
    size_t L09  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L09_info, Maxes);
    size_t L10  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,   L10_info, Maxes, Window);
    size_t L11  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L11_info, Maxes);
    size_t L12  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L12_info, Maxes);
    size_t L13  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L13_info, Maxes);
    size_t L14  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,   L14_info, Maxes, Window);
    size_t L15  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L15_info, Maxes);
    size_t L16  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L16_info, Maxes);
    size_t L17  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L17_info, Maxes);
    //size_t L18  = compiler.Add_Layer_to_Notwork(L2D_MPDR_MC,   L18_info, Maxes, Window);// remove this line
    size_t L19  = compiler.Add_Layer_to_Notwork(L2D_Output,    L19_info, Maxes);
    

#endif




#ifdef __TEST_CASE_4__
    //                             B    K     C     W     H     F    F
    Conv_Layer_Info L00_info    = {1,   1,    3,    224,  224,  3,   3}; //  Input   Layer
    Conv_Layer_Info L01_info    = {1,   16,   3,    224,  224,  3,   3}; //  Conv.   Layer #15
    Conv_Layer_Info L02_info    = {1,   1,    16,   224,  224,  0,   0}; //  Output Layer
                                                    
    //                             B    K     C     W     H    F    F                        
    Conv_Layer_Info Maxes       = {1,   16,   16,   16,   1,   1,   1};
    MaxP_Layer_Info Window      = {2,   2};
    

    size_t L00  = compiler.Add_Layer_to_Notwork(L2D_Input,     L00_info, Maxes);
    size_t L01  = compiler.Add_Layer_to_Notwork(L2D_CONV_MKMC, L01_info, Maxes);
    size_t L02  = compiler.Add_Layer_to_Notwork(L2D_Output,    L02_info, Maxes);
    

#endif



    compiler.Add_Dump_directory     (Dmp_Dir);
    compiler.Add_Sorce_directory    (Src_Dir);
    compiler.Add_Data_Gen_Directory (Inp_names, Wgt_names);
    compiler.Compile(verbose);
    compiler.Report();



    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Ellapsed Time = " << std::chrono::duration_cast<std::chrono::seconds>(end - begin).count() << "[s]" << std::endl;

    return 0;
}








