#pragma once

#include "Utils.h"



const size_t Ava_Planes			= 4;			// Accelerator topology: number of processing planes.
const size_t Ava_Vaults			= 16;			// Accelerator topology: number of memory vaults per plane.
const size_t Ava_Vault_Cap		= 0x100000;		// Capacity of a single vault in blocks (indexable slots).
const size_t Block_size			= 0x100;		// Size of a single data block in bytes.
const size_t Clock_Period		= 2000000000;	// Hardware clock period (Hz).


const size_t Assert_Max_Ival	= 255;




//	L0	:	Null
//	L1	:	DRAAM
//	L2	:	LMN
//	L3	:	PE
//	L4	:	SA

// Storage cost per memory level (abstract units).
const unsigned int	L0_Storage			= -1;
const unsigned int	L1_Storage			= 5;
const unsigned int	L2_Storage			= 10;
const unsigned int	L3_Storage			= 10;
const unsigned int	L4_Storage			= 100;
 
// Access cost from this level to the level above.
const unsigned int	L0_Access_to_Top	= -1;
const unsigned int	L1_Access_to_Top	= -1;
const unsigned int	L2_Access_to_Top	= 100;
const unsigned int	L3_Access_to_Top	= 5;
const unsigned int	L4_Access_to_Top	= 5;
 
// Access cost within the same level.
const unsigned int	L0_Access_to_Self	= -1;
const unsigned int	L1_Access_to_Self	= 260;
const unsigned int	L2_Access_to_Self	= 15;
const unsigned int	L3_Access_to_Self	= -1;
const unsigned int	L4_Access_to_Self	= 1;
 
// Access cost from this level to the level below.
const unsigned int	L0_Access_to_Bottom	= -1;
const unsigned int	L1_Access_to_Bottom	= 100;
const unsigned int	L2_Access_to_Bottom	= -1;
const unsigned int	L3_Access_to_Bottom	= -1;
const unsigned int	L4_Access_to_Bottom = -1;
 
// Penalty for changing vaults at a given level.
const unsigned int	L0_Change_Vault_Acc_Pnlty = 1;
const unsigned int	L1_Change_Vault_Acc_Pnlty = 1;
const unsigned int	L2_Change_Vault_Acc_Pnlty = 4;
const unsigned int	L3_Change_Vault_Acc_Pnlty = 10000;
const unsigned int	L4_Change_Vault_Acc_Pnlty = 10000;


// Execution cost for a processing element (PE).
const unsigned int	PE_Execution_Cost = 1;
// Execution cost for a storage/accumulation unit (SA).
const unsigned int	SA_Execution_Cost = 1;




