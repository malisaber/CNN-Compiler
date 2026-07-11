#pragma once

#include "Utils.h"
#include <vector>

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



