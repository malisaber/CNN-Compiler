#pragma once



#include <filesystem>
#include <vector>
#include <fstream>
#include <iomanip>
#include "Config.h"
#include "Utils.h"
#include "Hardware.h"

class Data_Logger
{
	DBID_t Data_Block_ID_cntr;		// Monotonic counter for assigning new data block IDs.

	std::vector<Data_Block>	DATA;	// Registry of all data blocks; index matches DBID_t::index().

public:

	// Initialize the data logger and create a reserved null data block at index 0.
	Data_Logger									();
	
	// Destroy the logger (no explicit resource cleanup required).
	~Data_Logger								();

	
	// Create a data block from info only (no explicit producer/consumer).
	DBID_t	Creat_Data_Block					(Data_Block_Info DBI);
	
	// Create a data block and set its producer scheduling node.
	DBID_t	Creat_Data_Block					(Data_Block_Info DBI,	SNID_t Prod_ID);
	
	// Create a data block with producer and a single initial consumer.
	DBID_t	Creat_Data_Block					(Data_Block_Info DBI,	SNID_t Prod_ID,	SNID_t Cons_ID);
	
	// Set/replace the producer scheduling node for an existing data block.
	bool	Set_Producer_ID_of_DBID				(DBID_t DBI,			SNID_t NID);
	
	// Append a consumer scheduling node to an existing data block.
	bool	Add_Consumer						(DBID_t DBI,			SNID_t NID);
	
	// Clone a data block entry and mark it as a duplicate of the original.
	DBID_t	Duplicate_Data_Block				(DBID_t DBI);
	
	// Retrieve a full data block record by ID.
	bool	Get_Data_Block_Info					(DBID_t DBI, Data_Block& DB);
	
	// Replace an existing data block record by ID.
	bool	Change_Data_Block_info				(DBID_t DBI, Data_Block  DB);
	
	// Set the starting block index (SoBI) within the assigned vault.
	bool	Set_Starting_Index_of_Data_Block	(DBID_t DBI, size_t SoBI);
	
	// Get the starting block index (SoBI) within the assigned vault.
	size_t	Get_Starting_Index_of_Data_Block	(DBID_t DBI);
	
	// Compute effective address (EA) in bytes from vault and starting index.
	size_t	GET_EA								(DBID_t DBI);
	
	// Dump all data blocks into a text file (appends .txt).
	size_t	print_file							(std::filesystem::path name);
	
	// Return number of data blocks stored (including null).
	size_t	size								();
	
	// Get producer node ID for a data block.
	SNID_t	Get_Producer_ID_of_DBID				(DBID_t DBI);
	
	// Test allocation flag for a data block.
	bool	Is_Allocated						(DBID_t DBI);
	
	// Get data block type (DBT_*).
	Data_Block_Types Get_Type_of_DBID			(DBID_t DBI);
	
	// Copy all consumer node IDs into the provided vector.
	void	Get_Consumers_ID_of_DBID			(DBID_t DBI, std::vector<SNID_t>& CID);
	
	// Replace all consumer node IDs with a single consumer.
	void	Set_Consumers_ID_of_DBID			(DBID_t DBI, SNID_t CID);
	
	// Set vault index for a data block.
	void	Set_Vault_of_DBID					(DBID_t DBI, size_t  vlt);
	
	// Retrieve vault index for a data block (by reference).
	void	Get_Vault_of_DBID					(DBID_t DBI, size_t& vlt);
	
	// Retrieve vault index for a data block (by value).
	size_t	Get_Vault_of_DBID					(DBID_t DBI);
	
	// Set produced time (timestamp) for a data block.
	bool	Set_Produced_Time					(DBID_t DBI, size_t  time);
	
	// Update last accessed time for a data block (max of old/new).
	bool	Set_Accessed_Time					(DBID_t DBI, size_t  time);
	
	// Mark a data block as allocated.
	bool	Mark_as_Allocated					(DBID_t DBI);
	
	// Compute distance in blocks between two data blocks by vault/SBI.
	size_t	Calculate_Space_Between				(DBID_t DBID_1, DBID_t DBID_2);
	
	//  Retrieve dimension (Max Sizes) of the block 
	Conv_Layer_Info	Get_Dims					(DBID_t DBI);
	
	//  Retrieve dimension indexes of the block 
	Conv_Layer_Info	Get_Idxs					(DBID_t DBI);
	
	// is data block generated
	bool	is_Generated						(DBID_t DBI);
	
	// Mark a data block as generated
	void	Mark_as_Generated					(DBID_t DBI);

	// Return the layer ID of Data block id
	size_t	Get_Layer_ID						(DBID_t DBI);

private:

	//void Memory_content_Check() {};



};



