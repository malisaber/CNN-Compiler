#include "Data_Generator.h"




// Initialize generator state and create a reserved null record at index 0.
Data_Generator::Data_Generator						()
{
	GIDF_cntr = 0;
	GWDF_cntr = 0;
	GZDF_cntr = 0;

	GIDB_cntr = 0;
	GWDB_cntr = 0;
	GZDB_cntr = 0;
}


// No-op destructor; storage is owned by STL containers.
Data_Generator::~Data_Generator						()
{

}


// Set inputs information getting from network
void	Data_Generator::Get_Inps_info				(
														NETWORK* NET)
{
	NET->Get_input_sizes(Inps_linfo, Inps_minfo, Inps_lid);
}


// Set weights information getting from network
void	Data_Generator::Get_Wgts_info				(
														NETWORK* NET)
{
	NET->Get_weight_sizes(Wgts_linfo, Wgts_minfo, Wgts_lid);
}


// Return number of generator input files.
size_t Data_Generator::Generated_Input_files_size   ()
{
	return GIDF_cntr;
}


// Return number of generator Weight stored.
size_t Data_Generator::Generated_Weight_files_size  ()
{
	return GWDF_cntr;
}


// Return number of generator Zero stored.
size_t Data_Generator::Generated_Zero_files_size	()
{
	return GZDF_cntr;
}


// Return number of generator records stored.
size_t	Data_Generator::Generated_file_size			()
{
	return GIDF_cntr + GWDF_cntr + GZDF_cntr;
}


// Load input files' name 
void	Data_Generator::load_input_files			(
														std::vector<std::filesystem::path> names)
{
	Inps_Fnames = names;
}


// Load weight files' name 
void	Data_Generator::load_Weight_files			(
														std::vector<std::filesystem::path> names)
{
	Wgts_Fnames = names;
}


// Generate Data files
void	Data_Generator::Generate					(
														Data_Logger* DG,
														std::filesystem::path dest)
{
	// Generate input files:
	for (size_t i = 0; i < Inps_lid.size(); i++)
		Generate_IDF(DG, i, dest);

	// Generate Weight files:
	for (size_t i = 0; i < Inps_lid.size(); i++)
		Generate_WDF(DG, i, dest);

	// Generte Zero files
	Generate_ZDF(DG, dest);
}










// Reading a binary file of input  data
std::vector<uint16_t> Data_Generator::Read_binary_file(
														std::filesystem::path& filename,
														size_t element_size)
{
		std::ifstream fid(filename, std::ios::binary);
		if (!fid) throw std::runtime_error("Could not open file: " + filename.string());

		const size_t count = static_cast<size_t>(element_size);
		std::vector<uint16_t> raw(count);

		fid.read(reinterpret_cast<char*>(raw.data()), count * sizeof(uint16_t));
		//std::cout << fid.gcount() << std::endl;
		if (fid.gcount() != static_cast<std::streamsize>(count * sizeof(uint16_t))) {
			throw std::runtime_error("Unexpected EOF reading " + filename.string());
		}

	return raw; 
}


// Convert the CHW idexed to absolute index
// flat layout, index as img[b*C*FH*FW + c*FH*FW + w*FH + h]
size_t	Data_Generator::Relative_2_Absolute_idx		(
														size_t b,
														size_t c,
														size_t w,
														size_t h,
														//size_t B,
														size_t C,
														size_t W,
														size_t H)
{
	return (b * C * H * W) + (c * H * W) + (w * H) + h;
}


// Generate data file for input layer x
void	Data_Generator::Generate_IDF				(
														Data_Logger* DL,
														size_t idx,
														std::filesystem::path dest)
{
	size_t					cntr		= 0;
	size_t					lid			= Inps_lid[idx];
	Conv_Layer_Info			linfo		= Inps_linfo[idx];
	Conv_Layer_Info			minfo		= Inps_minfo[idx];
	std::filesystem::path	fname		= Inps_Fnames[idx];
	size_t					el_size		= linfo.Batch_size * linfo.Channel_size * linfo.Width_size * linfo.Height_size;
	std::vector<uint16_t>	data		= Read_binary_file(fname, el_size);
	size_t					Data_size	= DL->size();

	
	// for validation purposes
	bool*					used		= new bool [el_size];
	for (size_t i = 0; i < el_size; i++)
		used[i] = false;


	// generating input data blocks
	for (DBID_t id = DBID_t::Null(); id < Data_size; id++)
	{
		// chech if the data is input or never generated before or does not belong to this layer
		if (DL->Get_Type_of_DBID(id) != DBT_DATA_IN)
			continue;
		if (DL->is_Generated(id))
			continue;
		if (DL->Get_Layer_ID(id) != lid)
			continue;

		// extract information refarding the address and indexes of the data block
		Conv_Layer_Info	Idxs = DL->Get_Idxs(id);
		size_t			Addr = DL->GET_EA(id)/Block_size;
		
		// creating the file 
		std::filesystem::path fname	 = dest / ("DRAM_DATA_" + std::to_string(Addr) + ".txt");
		std::ofstream out(fname);
		if (!out) throw std::runtime_error("Could not open output file: " + fname.string());
		out << std::hex << std::uppercase << std::setfill('0');

		// Writing the data into the file, 
		cntr += write_input_value(out, data, linfo, minfo, Idxs, used);

		// Closing the file
		out.close();

		// Mark the data block as generated
		DL->Mark_as_Generated(id);

		// Increase the generated file counter
		GIDF_cntr++;
	}


	// Assertion
	if (cntr != el_size) throw std::runtime_error("Error Here");
	for (size_t i = 0; i < el_size; i++)
		if (!used[i]) throw std::runtime_error("Error Here");

	GIDB_cntr += cntr;
}


// Generate Weight data file for CNN layer x
void	Data_Generator::Generate_WDF				(
														Data_Logger* DL,
														size_t idx,
														std::filesystem::path dest)
{
	size_t					cntr		= 0;
	size_t					lid			= Wgts_lid[idx];
	Conv_Layer_Info			linfo		= Wgts_linfo[idx];
	Conv_Layer_Info			minfo		= Wgts_minfo[idx];
	std::filesystem::path	fname		= Wgts_Fnames[idx];
	size_t					el_size		= linfo.Kernel_size * linfo.Channel_size * linfo.FiltW_Size * linfo.FiltH_Size;
	std::vector<uint16_t>	data		= Read_binary_file(fname, el_size);
	size_t					Data_size	= DL->size();

	
	// for validation purposes
	bool*					used		= new bool [el_size];
	for (size_t i = 0; i < el_size; i++)
		used[i] = false;


	// generating Weight data blocks
	for (DBID_t id = DBID_t::Null(); id < Data_size; id++)
	{
		// chech if the data is weight or never generated before or does not belong to this layer
		if (DL->Get_Type_of_DBID(id) != DBT_WGT)
			continue;
		if (DL->is_Generated(id))
			continue;
		if (DL->Get_Layer_ID(id) != lid)
			continue;

		// extract information refarding the address and indexes of the data block
		Conv_Layer_Info	Idxs = DL->Get_Idxs(id);
		size_t			Addr = DL->GET_EA(id) / Block_size;

		// creating the file 
		std::filesystem::path fname = dest / ("DRAM_DATA_" + std::to_string(Addr) + ".txt");
		std::ofstream out(fname);
		if (!out) throw std::runtime_error("Could not open output file: " + fname.string());
		out << std::hex << std::uppercase << std::setfill('0');

		// Writing the data into the file, 
		cntr += write_Weight_value(out, data, linfo, minfo, Idxs, used);

		// Closing the file
		out.close();

		// Mark the data block as generated
		DL->Mark_as_Generated(id);

		// Increase the generated file counter
		GWDF_cntr++;
	}

	
	// Assertion
	if (cntr != el_size) throw std::runtime_error("Error Here");
	for (size_t i = 0; i < el_size; i++)
		if (!used[i]) throw std::runtime_error("Error Here");

	GWDB_cntr += cntr;
}


// Generate Zero data files
void	Data_Generator::Generate_ZDF				(
														Data_Logger* DL,
														std::filesystem::path dest)
{
	size_t					cntr = 0;
	size_t					Data_size = DL->size();


	// generating Zero data blocks
	for (DBID_t id = DBID_t::Null(); id < Data_size; id++)
	{
		// chech if the data is input and never generated before
		if (!((DL->Get_Type_of_DBID(id) == DBT_IZero) || (DL->Get_Type_of_DBID(id) == DBT_PZero)))
			continue;
		if (DL->is_Generated(id))
			continue;

		// extract information refarding the address and indexes of the data block
		Conv_Layer_Info	Dims  = DL->Get_Dims(id);
		size_t			Addr  = DL->GET_EA(id) / Block_size;

		// creating the file 
		std::filesystem::path fname = dest / ("DRAM_DATA_" + std::to_string(Addr) + ".txt");
		std::ofstream out(fname);
		if (!out) throw std::runtime_error("Could not open output file: " + fname.string());
		out << std::hex << std::uppercase << std::setfill('0');

		// Writing the data into the file, 
		cntr += write_zero_block(out, Dims);

		// Closing the file
		out.close();

		// Mark the data block as generated
		DL->Mark_as_Generated(id);

		// Increase the generated file counter
		GZDF_cntr++;
	}

	GZDB_cntr += cntr;
}


// returning the value 
size_t	Data_Generator::write_input_value			(
														std::ofstream& fid,
														std::vector<uint16_t> data,
														Conv_Layer_Info linfo,
														Conv_Layer_Info Dims,
														Conv_Layer_Info Idxs,
														bool* used)
{
	size_t cntr(0);

	size_t Min_Batch_size   = std::min(Dims.Batch_size,   linfo.Batch_size);
	size_t Min_Channel_size = std::min(Dims.Channel_size, linfo.Channel_size);
	size_t Min_Width_size   = std::min(Dims.Width_size,   linfo.Width_size);
	size_t Min_Height_size  = std::min(Dims.Height_size,  linfo.Height_size);

	// The hardware reads: (least frequent) H x C x W x K (most frequent)
	for (size_t c = 0; c < Min_Channel_size; c++)
		for (size_t w = 0; w < Min_Width_size; w++)
		{
			size_t true_b = Idxs.Batch_size	  * Min_Batch_size    + 0;
			size_t true_c = Idxs.Channel_size * Min_Channel_size  + c;
			size_t true_w = Idxs.Width_size   * Min_Width_size    + w;
			size_t true_h = Idxs.Height_size  * Min_Height_size   + 0;
			size_t offset = Relative_2_Absolute_idx(true_b, true_c, true_w, true_h, linfo.Channel_size, linfo.Width_size, linfo.Height_size);
			fid << std::setw(4) << data[offset] << "\n";
			used[offset]  = true;
			cntr++;
		}

	for (size_t i=cntr; i < Block_size; i++)
		fid << std::setw(4) << 0 << "\n";

	return cntr;
}


// returning the weight value 
size_t	Data_Generator::write_Weight_value			(
														std::ofstream& fid,
														std::vector<uint16_t> data,
														Conv_Layer_Info linfo,
														Conv_Layer_Info Dims,
														Conv_Layer_Info Idxs,
														bool* used)
{
	size_t cntr(0);

	size_t Min_Kernel_size  = std::min(Dims.Kernel_size,  linfo.Kernel_size);
	size_t Min_Channel_size = std::min(Dims.Channel_size, linfo.Channel_size);
	//size_t Min_Width_size   = std::min(Dims.Width_size,   linfo.Width_size);
	//size_t Min_Height_size  = std::min(Dims.Height_size,  linfo.Height_size);

	// The hardware reads: (least frequent) H x C x W x K (most frequent)
	for (size_t c = 0; c < Min_Channel_size; c++)
		for (size_t k = 0; k < Min_Kernel_size; k++)
		{
			size_t true_k = Idxs.Kernel_size  * Min_Kernel_size   + k;
			size_t true_c = Idxs.Channel_size * Min_Channel_size  + c;
			size_t true_w = Idxs.FiltW_Size   * Dims.FiltW_Size   + 0;
			size_t true_h = Idxs.FiltH_Size   * Dims.FiltH_Size   + 0;
			size_t offset = Relative_2_Absolute_idx(true_k, true_c, true_w, true_h, linfo.Channel_size, linfo.FiltW_Size, linfo.FiltH_Size);
			fid << std::setw(4) << data[offset] << "\n";
			used[offset] = true;
			cntr++;
		}

	for (size_t i = cntr; i < Block_size; i++)
		fid << std::setw(4) << 0 << "\n";

	return cntr;
}


// Writing a zero data block
size_t	Data_Generator::write_zero_block			(
														std::ofstream& fid,
														Conv_Layer_Info Dims)
{
	size_t cntr = Dims.Channel_size * Dims.Width_size;

	// The hardware reads: (least frequent) H x C x W x K (most frequent)
	for (size_t i = 0; i < Block_size; i++)
		fid << std::setw(4) << 0 << "\n";

	return cntr;
}



