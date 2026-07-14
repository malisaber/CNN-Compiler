#pragma once


#include <ostream>

// TODO
//Time_t;	//	Time
//MEMIDX_t;	//	Memory Index


struct DBID_t
{
    // Opaque data block ID value (index into Data_Logger storage).
    unsigned long long value;

    // Construct a null/invalid ID (value = 0).
    static constexpr DBID_t Null					() { return DBID_t{ 0 }; }

    // Convert to index for vector access.
    size_t index									() const { return static_cast<size_t>(value); }

    // Compare IDs for equality.
    bool operator==								 (
														const DBID_t& other) const
    {
        return value == other.value;
    }

    // Compare ID against size_t (for bounds checks).
    bool operator <								 (
														const size_t& other) const
    {
        return value < other;
    }

    // Prefix increment: ++id.
    DBID_t& operator++							  () {
        ++value;        // increment first
        return *this;   // return self by reference
    }

    // Postfix increment: id++.
    DBID_t operator++							   (
														int) {
        DBID_t temp = *this; // copy current value
        ++value;             // increment self
        return temp;         // return old value
    }
};

struct SNID_t
{
    // Opaque scheduling node ID value (index into Dependency_Logger storage).
    unsigned long long value;

    // Construct a null/invalid ID (value = 0).
    static constexpr SNID_t Null					() { return SNID_t{ 0 }; }

    // Convert to index for vector access.
    size_t index									() const { return static_cast<size_t>(value); }

    // Compare IDs for equality.
    bool operator==								 (
														const SNID_t& other) const
    {
        return value == other.value;
    }

    // Compare ID against size_t (for bounds checks).
    bool operator <								 (
														const size_t& other) const
    {
        return value < other;
    }

    // Prefix increment: ++id.
    SNID_t& operator++							  () {
        ++value;        // increment first
        return *this;   // return self by reference
    }

    // Postfix increment: id++.
    SNID_t operator++							   (
														int) {
        SNID_t temp = *this; // copy current value
        ++value;             // increment self
        return temp;         // return old value
    }
};

// Stream-print a DBID_t as its index value.
inline std::ostream& operator<<					 (
														std::ostream& os,
														const DBID_t& t)
{
    return os << t.index();
}

// Stream-print an SNID_t as its index value.
inline std::ostream& operator<<					 (
														std::ostream& os,
														const SNID_t& t)
{
    return os << t.index();
}







// Scheduler node types for the dependency graph.
enum Scheduling_types
{
	Sch_Null        = 0,
	Sch_Start       = 1,
	Sch_Input_Layer = 2,
	Sch_CNN_Input   = 3,
	Sch_CNN_Weight  = 4,
	Sch_Process     = 5,
	Sch_Accumulate  = 6,
	Sch_Activation  = 7,
	Sch_MPDR        = 8,
	Sch_End         = 9
};

// Input tensor layout formats (B/H/C/W permutations).
enum inp_Data_formats
{
	//Any Configuration of B, H, C, W
	//my hardware runs on one of these:
	BHWC, BHCW
};

// Weight tensor layout formats (K/C/H/W permutations).
enum wgt_Data_formats
{
	//KCWH, KCHW, KWCH, KWHC, KHCW, KHWC, 
	//CWKH, CWHK, CHKW, CHWK, CKWH, CKHW,
	//WCHK, WCKH, WHKC, WHCK, WKHC, WKCH,
	//HCKW, HCWK, HKWC, HKCW, HWCK, HWKC,
	//my hardware runs on one of these:
	HWCk, HWKC, WHCK, WHKC
};

// Data block kinds used by Data_Logger.
enum Data_Block_Types
{
	DBT_Null        = 0,
	DBT_IZero       = 1,
	DBT_PZero       = 2,
	DBT_DATA_IN     = 3,
	DBT_WGT         = 4,
	DBT_PSUM        = 5,
	DBT_DATA_OUT    = 6,
	DBT_INTERNAL    = 7,
	DBT_RSRV        = 8
};

// Memory hierarchy levels.
enum Memory_Levels
{
	MT_Null         = 0,
    MT_DRAAM_Layer  = 1, 
    MT_LMN_Layer    = 2, 
    MT_PE_mems      = 3, 
    MT_SA_mem       = 4
};

// Target module categories for allocation/mapping.
enum Target_Module_Types
{
	TMT_Null        = 0, 
    TMT_USER        = 1, 
    TMT_DRAM        = 2, 
    TMT_LMN         = 3, 
    TMT_MPDR        = 4, 
    TMT_PE          = 5, 
    TMT_SA          = 6
};

// Layer class identifiers for NETWORK::Add_Layer.
enum Layer_2D_Types
{
    L2D_Null        = 0,
    L2D_Input       = 1,
    L2D_CONV_MKMC   = 2,
    L2D_MPDR_MC     = 3,
    L2D_Output      = 4,
};





