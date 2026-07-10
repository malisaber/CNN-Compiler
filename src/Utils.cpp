#include "Utils.h"


// Build a Data_Block from a Data_Block_Info record and reset allocation/timing fields.
Data_Block assign								   (
														Data_Block_Info inp)
{
	Data_Block out{};
	out.DBT = inp.DBT;
	out.ready = inp.ready;
	out.alocated = inp.alocated;
	out.to_Generate = inp.to_Generate;
	out.isGenerated = inp.isGenerated;
	out.prod_flag = inp.prod_flag;
	out.cons_flag = inp.cons_flag;
	out.Layer_ID = inp.Layer_ID;
	out.Dims = inp.Dims;
	out.Idxs = inp.Idxs;
	out.Vault = 0;
	out.MNID = 0;
	out.SoBI = 0;
	out.Produced_Time = 0;
	out.Last_Acc_Time = -1;
	out.Producer_ID = SNID_t::Null();
	out.Consumers_ID.clear();
	return out;
}


// Build Conv_Layer_Info from individual dimensions.
Conv_Layer_Info assign							  (
														unsigned int B,
														unsigned int K,
														unsigned int C,
														unsigned int W,
														unsigned int H,
														unsigned int FH,
														unsigned int FW)
{
	Conv_Layer_Info out{};
	out.Batch_size = B;
	out.Kernel_size = K;
	out.Channel_size = C;
	out.Width_size = W;
	out.Height_size = H;
	out.FiltH_Size = FH;
	out.FiltW_Size = FW;
	return out;
}


// Compare Conv_Layer_Info for B/H/W equality and K==C consistency.
bool Eq_comapre_BHW_K1C2							(
														Conv_Layer_Info i1,
														Conv_Layer_Info i2)
{
	bool Batch = (i1.Batch_size == i2.Batch_size);
	bool Width = (i1.Width_size == i2.Width_size);
	bool Height = (i1.Height_size == i2.Height_size);
	bool diff = (i1.Kernel_size == i2.Channel_size);
	return Batch && Width && Height && diff;
}


// Print Conv_Layer_Info in (BKCWHFF) order.
void print										  (
														Conv_Layer_Info inp)
{
	std::cout << "(BKCWHFF) = (";
	std::cout << inp.Batch_size << ", ";
	std::cout << inp.Kernel_size << ", ";
	std::cout << inp.Channel_size << ", ";
	std::cout << inp.Width_size << ", ";
	std::cout << inp.Height_size << ", ";
	std::cout << inp.FiltH_Size << ", ";
	std::cout << inp.FiltW_Size << ") ";
}


// Print a Dependency (ID and satisfied flag).
void print										  (
														Dependency dep)
{
	std::cout << "ID: " << dep.Dependent_ID << ", \t";
	std::cout << "Sat.: " << dep.satisfied;
}


// Allocate a 7D array of DBID_t with sizes d0..d6.
DBID_t*******	new_DBID_7D						 (
														unsigned int d0,
														unsigned int d1,
														unsigned int d2,
														unsigned int d3,
														unsigned int d4,
														unsigned int d5,
														unsigned int d6)
{
	DBID_t******* out;
	out = new DBID_t * *****[d0];
	for (unsigned int i = 0; i < d0; i++)
		out[i] = new DBID_t * ****[d1];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			out[i][j] = new DBID_t * ***[d2];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				out[i][j][k] = new DBID_t * **[d3];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				for (unsigned int l = 0; l < d3; l++)
					out[i][j][k][l] = new DBID_t * *[d4];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				for (unsigned int l = 0; l < d3; l++)
					for (unsigned int m = 0; m < d4; m++)
						out[i][j][k][l][m] = new DBID_t * [d5];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				for (unsigned int l = 0; l < d3; l++)
					for (unsigned int m = 0; m < d4; m++)
						for (unsigned int n = 0; n < d5; n++)
							out[i][j][k][l][m][n] = new DBID_t[d6];
	return out;
}


// Allocate a 7D array of SNID_t with sizes d0..d6.
SNID_t*******	new_SNID_7D						 (
														unsigned int d0,
														unsigned int d1,
														unsigned int d2,
														unsigned int d3,
														unsigned int d4,
														unsigned int d5,
														unsigned int d6)
{
	SNID_t******* out;
	out = new SNID_t ****** [d0];
	for (unsigned int i = 0; i < d0; i++)
		out[i] = new SNID_t ***** [d1];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			out[i][j] = new SNID_t **** [d2];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				out[i][j][k] = new SNID_t ***[d3];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				for (unsigned int l = 0; l < d3; l++)
					out[i][j][k][l] = new SNID_t **[d4];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				for (unsigned int l = 0; l < d3; l++)
					for (unsigned int m = 0; m < d4; m++)
						out[i][j][k][l][m] = new SNID_t * [d5];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				for (unsigned int l = 0; l < d3; l++)
					for (unsigned int m = 0; m < d4; m++)
						for (unsigned int n = 0; n < d5; n++)
							out[i][j][k][l][m][n] = new SNID_t[d6];
	return out;
}


// Allocate a 4D array of DBID_t with sizes d0..d3.
DBID_t****		new_DBID_4D						 (
														unsigned int d0,
														unsigned int d1,
														unsigned int d2,
														unsigned int d3)
{
	DBID_t**** out;
	out = new DBID_t * **[d0];
	for (unsigned int i = 0; i < d0; i++)
		out[i] = new DBID_t * *[d1];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			out[i][j] = new DBID_t * [d2];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				out[i][j][k] = new DBID_t[d3];
	return out;
}


// Allocate a 4D array of SNID_t with sizes d0..d3.
SNID_t****		new_SNID_4D						 (
														unsigned int d0,
														unsigned int d1,
														unsigned int d2,
														unsigned int d3)
{
	SNID_t**** out;
	out = new SNID_t *** [d0];
	for (unsigned int i = 0; i < d0; i++)
		out[i] = new SNID_t ** [d1];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			out[i][j] = new SNID_t * [d2];
	for (unsigned int i = 0; i < d0; i++)
		for (unsigned int j = 0; j < d1; j++)
			for (unsigned int k = 0; k < d2; k++)
				out[i][j][k] = new SNID_t[d3];
	return out;
}


// Construct a Dependency with given satisfied flag and dependent ID.
Dependency Gen_Dependency						   (
														bool satisfied,
														SNID_t Dependent_ID)
{
	Dependency dep = {};
	dep.satisfied = satisfied;
	dep.Dependent_ID = Dependent_ID;
	return dep;
}


// Map baseline count to vault count (heuristic).
size_t calculate_vault_map						  (
														size_t bl)
{
	size_t out(0);
	switch (bl)
	{
	case 0:
		std::cout << "\tE: What the fuck is this?" << std::endl;
		break;
	case 1:
		out = 1;
		break;
	case 2:
		out = 2;
		break;
	case 3:
	case 4:
		out = 4;
		break;
	case 5:
	case 6:
	case 7:
	case 8:
		out = 8;
		break;
	default:
		out = 16;
		break;
	}
	return out;
}


// Return a string for a boolean result ("Suucessfull"/"Unsuucessfull").
std::string Res_Dec								 (
														const bool b)
{
	if (b)
		return "Suucessfull";
	return "Unsuucessfull";
}

std::string embedd								  (
														size_t lvl,
														size_t bline)
{
	return std::to_string(lvl) + "_bl_" + std::to_string(bline);
}
std::string embedd								  (
														size_t lvl,
														size_t bline,
														size_t node)
{
	return std::to_string(lvl) + "_bl_" + std::to_string(bline) + "_nd_" + std::to_string(node);
}
std::string embedd_array							(
														size_t lvl,
														size_t arr_size_d1)
{
	return std::to_string(lvl) + "[" + std::to_string(arr_size_d1) + "]";
}
std::string embedd_array							(
														size_t lvl,
														size_t bline,
														size_t arr_size_d1)
{
	return std::to_string(lvl) + "_bl_" + std::to_string(bline) + "[" + std::to_string(arr_size_d1) + "]";
}
std::string embedd_array							(
														size_t lvl,
														size_t bline,
														size_t node,
														size_t arr_size_d1)
{
	return std::to_string(lvl) + "_bl_" + std::to_string(bline) + "_nd_" + std::to_string(node) + "[" + std::to_string(arr_size_d1) + "]";
}
std::string embedd_dma_array						(
														size_t lvl,
														size_t bline,
														size_t node,
														size_t arr_size_d1)
{
	return std::to_string(lvl) + "_bl_" + std::to_string(bline) + "_DMA_" + std::to_string(node) + "[" + std::to_string(arr_size_d1) + "]";
}
std::string embedd_array_2D						 (
														size_t lvl,
														size_t arr_size_d1,
														size_t arr_size_d2)
{
	return std::to_string(lvl) + "[" + std::to_string(arr_size_d1) + "][" + std::to_string(arr_size_d2) + "]";
}
std::string embedd_array_2D						 (
														size_t lvl,
														size_t bline,
														size_t arr_size_d1,
														size_t arr_size_d2)
{
	return std::to_string(lvl) + "_bl_" + std::to_string(bline) + "[" + std::to_string(arr_size_d1) + "][" + std::to_string(arr_size_d2) + "]";
}









