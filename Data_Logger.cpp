#include "Data_Logger.h"



//***************************************//
//*			Public Functions			*//
//***************************************//

Data_Logger::Data_Logger()
{
	Data_Block_ID_cntr = DBID_t::Null();
	DATA.reserve(32768);
	DATA.push_back(Data_Block_NULL);
	DATA[0].ready = true;
	DATA[0].alocated = true;
	DATA[0].to_Generate = false;
	DATA[0].prod_flag = false;
	DATA[0].cons_flag = false;
}


// Destroy the logger (no explicit resource cleanup required).
Data_Logger::~Data_Logger()
{
}


// Create a data block from info only (no explicit producer/consumer).
// Returns the newly assigned DBID.
DBID_t Data_Logger::Creat_Data_Block(Data_Block_Info DBI)
{
	Data_Block_ID_cntr++;
	Data_Block tmp = assign(DBI);
	tmp.ID = Data_Block_ID_cntr;
	DATA.push_back(tmp);
	return tmp.ID;
}


// Create a data block and set its producer scheduling node.
// Returns the newly assigned DBID.
DBID_t Data_Logger::Creat_Data_Block(Data_Block_Info DBI, SNID_t Prod_ID)
{
	Data_Block_ID_cntr++;
	Data_Block tmp = assign(DBI);
	tmp.ID = Data_Block_ID_cntr;
	tmp.Producer_ID = Prod_ID;
	DATA.push_back(tmp);
	return tmp.ID;
}


// Create a data block with producer and a single initial consumer.
// Returns the newly assigned DBID.
DBID_t Data_Logger::Creat_Data_Block(Data_Block_Info DBI, SNID_t Prod_ID, SNID_t Cons_ID)
{
	Data_Block_ID_cntr++;
	Data_Block tmp = assign(DBI);
	tmp.ID = Data_Block_ID_cntr;
	tmp.Producer_ID = Prod_ID;
	tmp.Consumers_ID.push_back(Cons_ID);
	tmp.alocated = false;
	DATA.push_back(tmp);
	return tmp.ID;
}


// Set/replace the producer scheduling node for an existing data block.
// Returns false if DBID is out of range.
bool Data_Logger::Set_Producer_ID_of_DBID(DBID_t DBI, SNID_t NID)
{
	if (DBI < DATA.size())
	{
		DATA[DBI.index()].Producer_ID = NID;
		return true;
	}
	return false;
}


// Append a consumer scheduling node to an existing data block.
// Returns false if DBID is out of range.
bool Data_Logger::Add_Consumer(DBID_t DBI, SNID_t NID)
{
	if (DBI < DATA.size())
	{
		DATA[DBI.index()].Consumers_ID.push_back(NID);
		return true;
	}
	return false;
}


// Clone a data block entry and mark it as a duplicate of the original.
// Returns the new DBID (or Null if DBID is invalid).
DBID_t Data_Logger::Duplicate_Data_Block(DBID_t DBI)
{
	bool found(false);
	DBID_t NDBID = DBID_t::Null();

	if (DBI < DATA.size())
	{
		found = true;
		NDBID = Creat_Data_Block({});
		DATA[NDBID.index()] = DATA[DBI.index()];
		DATA[NDBID.index()].ID = NDBID;
		DATA[NDBID.index()].Douplicate_of = DBI;
	}

	if (NDBID.index() == 70995)
		std::cout << "FUUUUUCKKKK" << std::endl;

	return NDBID;
}


// Retrieve a full data block record by ID.
// Returns true if found, and copies into DB.
bool Data_Logger::Get_Data_Block_Info(DBID_t DBI, Data_Block& DB)
{
	bool found(false);

	if (DBI < DATA.size())
	{
		found = true;
		DB = DATA[DBI.index()];
	}

	return found;
}


// Replace an existing data block record by ID.
// Returns true if updated; DB.ID is overwritten with DBI.
bool Data_Logger::Change_Data_Block_info(DBID_t DBI, Data_Block DB)
{
	bool found(false);
	
	if (DBI < DATA.size())
	{	
		DB.ID = DBI;
		found = true;
		DATA[DBI.index()] = DB;
	}

	return found;
}


// Set the starting block index (SoBI) within the assigned vault.
// Returns false if DBID is out of range.
bool Data_Logger::Set_Starting_Index_of_Data_Block(DBID_t DBI, size_t SoBI)
{
	bool found(false);

	if (DBI < DATA.size())
	{
		found = true;
		DATA[DBI.index()].SoBI = SoBI;
	}

	return found;
}


// Get the starting block index (SoBI) within the assigned vault.
size_t Data_Logger::Get_Starting_Index_of_Data_Block(DBID_t DBI)
{
	return DATA[DBI.index()].SoBI;
}


// Compute effective address (EA) in bytes from vault and starting index.
size_t Data_Logger::GET_EA(DBID_t DBI)
{
	size_t EA(DATA[DBI.index()].SoBI);
	EA += DATA[DBI.index()].Vault * Ava_Vault_Cap;
	return EA * Block_size;
}


// Dump all data blocks into a text file (appends .txt).
// Returns number of data blocks written.
size_t Data_Logger::print_file(std::filesystem::path name)
{
	std::string a_line;
	unsigned int i;

	std::ofstream file_out;
	file_out.open(name);

	for (i = 0; i < DATA.size(); i++)
	{
		if (!PRINT_DBT_INTERNAL && (DATA[i].DBT == DBT_INTERNAL))
			continue;
		
		file_out						<< std::setw(7)	<< ("D" + std::to_string(DATA[i].ID.index()));
		file_out << "\t("				<< std::setw(7)	<< ("D" + std::to_string(DATA[i].Douplicate_of.index()));
		file_out << ")\t->\tType: "		<< std::setw(2) << DATA[i].DBT << ", ";
		file_out << "\tRdy: "			<< std::setw(1) << DATA[i].ready << ", ";
		file_out << "\tAloc: "			<< std::setw(1) << DATA[i].alocated << ", ";
		file_out << "\tDGEN: "			<< std::setw(1) << DATA[i].to_Generate << ", ";
		file_out << "\tVault: "			<< std::setw(3) << DATA[i].Vault << ", ";
		file_out << "\tSA: "			<< std::setw(7) << DATA[i].SoBI << ", ";
		file_out << "\tMNID: "			<< std::setw(3) << DATA[i].MNID << ", ";
		file_out << "\tLayer ID: "		<< std::setw(2) << DATA[i].Layer_ID << ", ";
		file_out << "\tDim #BKCWHFF: ("	<< std::setw(2) << DATA[i].Dims.Batch_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Dims.Kernel_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Dims.Channel_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Dims.Width_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Dims.Height_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Dims.FiltH_Size;
		file_out << ", "				<< std::setw(2) << DATA[i].Dims.FiltW_Size << "), ";
		file_out << "\tIdx #BKCWHFF: ("	<< std::setw(2) << DATA[i].Idxs.Batch_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Idxs.Kernel_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Idxs.Channel_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Idxs.Width_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Idxs.Height_size;
		file_out << ", "				<< std::setw(2) << DATA[i].Idxs.FiltH_Size;
		file_out << ", "				<< std::setw(2) << DATA[i].Idxs.FiltW_Size << "), ";
		
		file_out << "\tP. Time: "		<< std::setw(7) << DATA[i].Produced_Time << ", ";
		file_out << "\tC. Time: "		<< std::setw(7) << DATA[i].Last_Acc_Time << ", ";
		file_out << "\tProducers: "		<< std::setw(7) << DATA[i].Producer_ID << ", ";
		if (DATA[i].Consumers_ID.size() > 0)
			file_out << "\tConsumers: "	<< std::setw(7) << DATA[i].Consumers_ID[0];
		for (size_t idx = 1; idx < DATA[i].Consumers_ID.size(); idx++)
			file_out << ",\t"			<< std::setw(7) << DATA[i].Consumers_ID[idx];
		file_out << std::endl;
	}

	file_out.close();
	return DATA.size();
}


// Return number of data blocks stored (including null).
size_t Data_Logger::size()
{
	return DATA.size();
}


// Get producer node ID for a data block.
SNID_t Data_Logger::Get_Producer_ID_of_DBID(DBID_t DBI)
{
	return DATA[DBI.index()].Producer_ID;
}


// Test allocation flag for a data block.
bool Data_Logger::Is_Allocated(DBID_t DBI)
{
	return DATA[DBI.index()].alocated;
}


// Get data block type (DBT_*).
Data_Block_Types Data_Logger::Get_Type_of_DBID(DBID_t DBI)
{
	return DATA[DBI.index()].DBT;
}


// Copy all consumer node IDs into the provided vector.
void Data_Logger::Get_Consumers_ID_of_DBID(DBID_t DBI, std::vector<SNID_t>& CID)
{
	CID = DATA[DBI.index()].Consumers_ID;
}


// Replace all consumer node IDs with a single consumer.
void Data_Logger::Set_Consumers_ID_of_DBID(DBID_t DBI, SNID_t CID)
{
	DATA[DBI.index()].Consumers_ID.clear();
	DATA[DBI.index()].Consumers_ID.push_back(CID);
}


// Set vault index for a data block.
void Data_Logger::Set_Vault_of_DBID(DBID_t DBI, size_t vlt)
{
	DATA[DBI.index()].Vault = (unsigned int)vlt;
}


// Retrieve vault index for a data block (by reference).
void Data_Logger::Get_Vault_of_DBID(DBID_t DBI, size_t& vlt)
{
	vlt = DATA[DBI.index()].Vault;
}


// Retrieve vault index for a data block (by value).
size_t Data_Logger::Get_Vault_of_DBID(DBID_t DBI)
{
	return DATA[DBI.index()].Vault;
}


// Set produced time (timestamp) for a data block.
// Returns false if DBID is out of range.
bool Data_Logger::Set_Produced_Time(DBID_t DBI, size_t time)
{
	bool found(false);

	if (DBI < DATA.size())
	{
		found = true;
		DATA[DBI.index()].Produced_Time = time;
	}

	return found;
}


// Update last accessed time for a data block (max of old/new).
// Returns false if DBID is out of range.
bool Data_Logger::Set_Accessed_Time(DBID_t DBI, size_t time)
{
	bool found(false);

	if (DBI < DATA.size())
	{
		found = true;
		if (DATA[DBI.index()].Last_Acc_Time == (size_t)-1)
			DATA[DBI.index()].Last_Acc_Time = time;
		else
			DATA[DBI.index()].Last_Acc_Time = std::max(DATA[DBI.index()].Last_Acc_Time, time);
	}

	return found;
}


// Mark a data block as allocated.
// Returns false if DBID is out of range.
bool Data_Logger::Mark_as_Allocated(DBID_t DBI)
{
	bool found(false);

	if (DBI < DATA.size())
	{
		found = true;
		DATA[DBI.index()].alocated = true;
	}

	return found;
}


// Compute distance in blocks between two data blocks by vault/SBI.
size_t Data_Logger::Calculate_Space_Between(DBID_t DBID_1, DBID_t DBID_2)
{
	size_t vlt1 = DATA[DBID_1.index()].Vault;
	size_t vlt2 = DATA[DBID_2.index()].Vault;

	size_t SBI1 = DATA[DBID_1.index()].SoBI;
	size_t SBI2 = DATA[DBID_2.index()].SoBI;

	return (Ava_Vault_Cap * (vlt2 - vlt1)) + (SBI2 - SBI1);
}


//  Retrieve dimension (Max Sizes) of the block
Conv_Layer_Info Data_Logger::Get_Dims(DBID_t DBI)
{
	return DATA[DBI.index()].Dims;
}
 

//  Retrieve dimension indexes of the block
Conv_Layer_Info Data_Logger::Get_Idxs(DBID_t DBI)
{
	return DATA[DBI.index()].Idxs;
}


// is data block generated
bool Data_Logger::is_Generated(DBID_t DBI)
{
	return DATA[DBI.index()].isGenerated;
}


// Mark a data block as generated
void Data_Logger::Mark_as_Generated(DBID_t DBI)
{
	DATA[DBI.index()].isGenerated = true;
}


// Return the layer ID of Data block id
size_t Data_Logger::Get_Layer_ID(DBID_t DBI)
{
	return DATA[DBI.index()].Layer_ID;
}












//***************************************//
//*			Private Functions			*//
//***************************************//














