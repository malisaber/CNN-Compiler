#pragma once

#include "Utils.h"
#include <vector>


/*******	PE CONTROL	*******/
#define	C_UPA_Status_pos						0
#define	C_Update_WFM_pos						3
#define	C_Update_IFM_pos						4
#define	C_Pause_UPA_pos							5
#define	C_Update_Store_Base_Address_pos			8
#define	C_Update_load_Base_Address_pos			9
#define	C_Store_Row_pos							10	//	store internal Buffer
#define	C_Enable_Activation_pos					11
#define	C_Save_Row_pos							12	//	save it in the inrternal Buffer
#define	C_Bias_Accumulation_Enable_pos			13	//	Accumulate with Bias
#define	C_PEout_Accumulation_Enable_pos			14	//	Accumulate with PE output
#define	C_Buffer_Accumulation_Enable_pos		15	//	Accumulate with Internal Buffer
#define	C_Load_Row_pos							16	//	preload internal Buffer
#define	C_Pause_STA_pos							17
#define	C_ACK_STA_pos							18
#define	C_AUTOMATIC_STA_pos						19
#define	C_Start_PE_pos							27
#define	C_Pause_PE_pos							28



class Hardware
{
	unsigned int Available_Planes;	// Number of processing planes available.
	unsigned int Available_Vaults;	// Number of memory vaults available.
	
public:

	// Construct hardware model with a given plane count.
	Hardware										(
														unsigned int available_planes);
	
	// Destroy hardware model	(no explicit resource cleanup required).
	~Hardware										();
	
	
	// Return total number of available PEs	(planes * vaults).
	unsigned int Get_Number_of_Available_PE			() const;
	
	// Return number of available planes.
	unsigned int Get_Number_of_Available_Planes		() const;
	
	// Return number of available vaults.
	unsigned int Get_Number_of_Available_Vaults		() const;

private:

};



