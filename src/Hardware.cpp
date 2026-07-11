#include "Hardware.h"
#include "Config.h"

// Initialize hardware model with plane count and default 16 vaults.
Hardware::Hardware									(
														unsigned int available_planes)
{
	Available_Planes = available_planes;
	Available_Vaults = 16;

	/*
	for (size_t i = 0; i < 16; i++)
	{
		DRAM[i].ID = 1+i;
		DRAM[i].Vault = (unsigned int)i;
		DRAM[i].type = MT_DRAAM_Layer;
		DRAM[i].Costs = { L1_Storage, L1_Access_to_Top, L1_Access_to_Self, L1_Access_to_Bottom, L1_Change_Vault_Acc_Pnlty };
		DRAM[i].length = 0x10000000;
		DRAM[i].Start_Adress = i * 0x10000000;
		DRAM[i].Content = new size_t[0x1000000];
		for (size_t j = 0; j < 0x1000000; j++)
			DRAM[i].Content[j] = 0;
	}
	DRAM[15].length = 0xFFF0000;

	for (size_t i = 0; i < 16; i++)
	{
		LMNs[i].ID = 17+i;
		LMNs[i].Vault = (unsigned int)i;
		LMNs[i].type = MT_LMN_Layer;
		LMNs[i].Costs = { L2_Storage, L2_Access_to_Top, L2_Access_to_Self, L2_Access_to_Bottom, L2_Change_Vault_Acc_Pnlty };
		LMNs[i].length = 0x1000;
		LMNs[i].Start_Adress = 0xFFFF0000 + i * 0x1000;
		LMNs[i].Content = new size_t[16];
		for (size_t j = 0; j < 16; j++)
			LMNs[i].Content[j] = 0;
	}
	
	for (size_t i = 0; i < 64; i++)
	{
		PEs[i].usable = false;
		if (i < (size_t)16 * available_planes)
			PEs[i].usable = true;
		PEs[i].ID = 1+i;
		PEs[i].Vault = (unsigned int)(i % 16);
		PEs[i].Exe_Cost = PE_Execution_Cost;
		PEs[i].Status = PST_Idle;
		PEs[i].Wgt_Blk_ID[0] = 0;
		PEs[i].Wgt_Blk_ID[1] = 0;
		PEs[i].Wgt_Blk_ID[2] = 0;
		PEs[i].Wgt_Blk_ID[3] = 0;
		PEs[i].Wgt_Blk_ID[4] = 0;
		PEs[i].Wgt_Blk_ID[5] = 0;
		PEs[i].Wgt_Blk_ID[6] = 0;
		PEs[i].Wgt_Blk_ID[7] = 0;
		PEs[i].Wgt_Blk_ID[8] = 0;
		PEs[i].Inp_Blk_ID[0] = 0;
		PEs[i].Inp_Blk_ID[1] = 0;
		PEs[i].Inp_Blk_ID[2] = 0;
		PEs[i].Out_Blk_ID	 = 0;
	}
		
	for (size_t i = 0; i < 64; i++)
	{
		SAs[i].usable = false;
		if (i < (size_t)16 * available_planes)
			SAs[i].usable = true;
		SAs[i].ID = 1 + i;
		SAs[i].Vault = (unsigned int)(i % 16);
		SAs[i].Exe_Cost = SA_Execution_Cost;
		SAs[i].Status = PST_Idle;
		SAs[i].Inp_Blk_ID = 0;
		SAs[i].Str_Blk_ID = 0;
		SAs[i].Out_Blk_ID = 0;
	}
	*/
}


// No-op destructor; no dynamic resources in this class.
Hardware::~Hardware									()
{
}


// Return total available processing elements (planes * 16 vaults).
unsigned int Hardware::Get_Number_of_Available_PE   () const
{
	return 16 * Available_Planes;
}


// Return available plane count.
unsigned int Hardware::Get_Number_of_Available_Planes() const
{
	return Available_Planes;
}


// Return available vault count.
unsigned int Hardware::Get_Number_of_Available_Vaults() const
{
	return Available_Vaults;
}

