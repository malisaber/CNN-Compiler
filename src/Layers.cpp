#include "Layers.h"
#include <cmath>




bool Layer_2D::IsInput								() const
{
	return false;
}


bool Layer_2D::IsOutput								() const
{
	return false;
}


bool Layer_2D::IsMPDR								() const
{
	return false;
}


bool Layer_2D::IsConv								() const
{
	return false;
}


size_t Layer_2D::Get_Layer_ID						() const
{
	return ID; 
}


Conv_Layer_Info Layer_2D::Get_Layer_info			()
{
	return Layer_info;
}


MaxP_Layer_Info Layer_2D::Get_Window_info			() const
{
	return Windo_info;
}


Conv_Layer_Info Layer_2D::Get_Maxes_info			() const
{
	return Maxes_info;
}


bool Layer_2D::Get_previous_output_scheduling_info  (
														Layer_2D* prev_layer)
{
	Conv_Layer_Info prev_map_entity = {};
	Prev_Out_Sch_Info_rdy = prev_layer->Get_output_scheduling_info(Out_Node_Map_PL, Out_Data_Map_PL, prev_map_entity, getting_from_input);
	if (Prev_Out_Sch_Info_rdy)
	{
		if (!Eq_comapre_BHW_K1C2(prev_map_entity, map_entity))
		{
			std::cout << "\tE: Inconsistency in layer sizing, ";
			std::cout << "(this layer ID : " << ID << ")\n";
			std::cout << "\t\tPrev Layer Info:";
			print(prev_map_entity);
			std::cout << "\n\t\tThis Layer Info:";
			print(map_entity);
			std::cout << std::endl;

			Prev_Out_Sch_Info_rdy = false;
		}
	}
	return Prev_Out_Sch_Info_rdy;
}





Input_Layer_2D::Input_Layer_2D						(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														size_t layer_id)
{
	Layer_info = Linfo;
	Maxes_info = Maxes;
	Windo_info = { 3, 3 };

	map_entity.Batch_size   = (unsigned int)std::ceil((double)Layer_info.Batch_size   / (double)Maxes_info.Batch_size);
	map_entity.Channel_size = (unsigned int)std::ceil((double)Layer_info.Channel_size / (double)Maxes_info.Channel_size);
	map_entity.Width_size   = (unsigned int)std::ceil((double)Layer_info.Width_size   / (double)Maxes_info.Width_size);
	map_entity.Height_size  = (unsigned int)std::ceil((double)Layer_info.Height_size  / (double)Maxes_info.Height_size);
	map_entity.Kernel_size  = map_entity.Channel_size;

	
	Out_Data_Map			= new_DBID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size, map_entity.Kernel_size);
	Out_Node_Map			= new_SNID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size, map_entity.Kernel_size);

	ID = layer_id;
	mapped = false;
	Out_Sch_Info_rdy = false;
	getting_from_input = false;
}


Input_Layer_2D::~Input_Layer_2D						()
{
}


bool Input_Layer_2D::IsInput						() const
{
	return true;
}


bool Input_Layer_2D::Map_Data_Blocks_and_Build_Dependencies(
														Data_Logger* DataL,
														Dependency_Logger* DpndL)
{
	if (mapped)
	{
		std::cout << "\tE: This Layer has been Mapped before,\t(Layer ID: " << ID << ")" << std::endl;
		return false;
	}
	else
	{
		mapped = true;
		SNID_t Start_Node_ID = DpndL->Get_Starting_Point_Scheduling_Node_Idx();
		SNID_t End_Node_ID   = DpndL->Get_Ending_Point_Scheduling_Node_Idx();


		std::cout << "\tL" << ID << ": Input Layer;" << std::endl;


		// Preperations 
		// input  format B H W C
		// Output format B H W C	(this C is K)
		unsigned int B, H, W, K;
		


		// Mapping 
		SNID_t SNID;
		DBID_t DBID;
		Data_Block_Info DBInfo{};



		// Dependencies
		// The input Data Blocks are provided by user and should not be a part of the starting Point Node
		// Building an Empty Dependency for the Input Data as it is the Output data
		// Inp_Data_Map	:	B   H   W   K
		for (B = 0; B < map_entity.Batch_size; B++)
		{
			for (H = 0; H < map_entity.Height_size; H++)
			{
				for (W = 0; W < map_entity.Width_size; W++)
				{
					for (K = 0; K < map_entity.Kernel_size; K++)
					{
						Dependency dep{};
						dep.satisfied = false;
						dep.Dependent_ID = Start_Node_ID;
						SNID = DpndL->Create_Scheduling_Node(Sch_Input_Layer, ID, dep);
						
						
						DBInfo.DBT = DBT_DATA_IN;
						DBInfo.ready = true;
						DBInfo.alocated = false;
						DBInfo.to_Generate = true;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs.Batch_size = B;
						DBInfo.Idxs.Kernel_size = 0;
						DBInfo.Idxs.Channel_size = K;
						DBInfo.Idxs.Width_size = W;
						DBInfo.Idxs.Height_size = H;
						DBInfo.Idxs.FiltH_Size = 0;
						DBInfo.Idxs.FiltW_Size = 0;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID, End_Node_ID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);
						
						
						Out_Node_Map[B][H][W][K] = SNID;
						Out_Data_Map[B][H][W][K] = DBID;
					}
				}
			}
		}
		

		Out_Sch_Info_rdy = true;
	}
	return true;
}


bool Input_Layer_2D::Get_output_scheduling_info		(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI)
{
	if (Out_Sch_Info_rdy)
	{
		map_entity_info = map_entity;
		PLONM = Out_Node_Map;
		PLODM = Out_Data_Map;
		GFI = true;
	}
	else
	{
		std::cout << "\tE: calling for the mapping information too early, ";
		std::cout << "\t(this layer ID : " << ID << " is not ready)" << std::endl;
	}
	return Out_Sch_Info_rdy;
}





Conv_MKMC_2D::Conv_MKMC_2D							(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														size_t layer_id)
{
	Layer_info = Linfo;
	Maxes_info = Maxes;
	Windo_info = { 3, 3 };
	
	map_entity.Batch_size		= (unsigned int)std::ceil((double)Layer_info.Batch_size		/ (double)Maxes_info.Batch_size);
	map_entity.Kernel_size		= (unsigned int)std::ceil((double)Layer_info.Kernel_size		/ (double)Maxes_info.Kernel_size);
	map_entity.Channel_size		= (unsigned int)std::ceil((double)Layer_info.Channel_size	/ (double)Maxes_info.Channel_size);
	map_entity.Width_size		= (unsigned int)std::ceil((double)Layer_info.Width_size		/ (double)Maxes_info.Width_size);
	map_entity.Height_size		= (unsigned int)std::ceil((double)Layer_info.Height_size		/ (double)Maxes_info.Height_size);
	map_entity.FiltH_Size		= (unsigned int)std::ceil((double)Layer_info.FiltH_Size		/ (double)Maxes_info.FiltH_Size);
	map_entity.FiltW_Size		= (unsigned int)std::ceil((double)Layer_info.FiltW_Size		/ (double)Maxes_info.FiltW_Size);

	Out_Data_Map				= new_DBID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size, map_entity.Kernel_size);
	Out_Node_Map				= new_SNID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size, map_entity.Kernel_size);
	
	Out_Data_Map_PL				= NULL;
	Out_Node_Map_PL				= NULL;
	

	ID = layer_id;
	mapped = false;
	Out_Sch_Info_rdy = false;
	Prev_Out_Sch_Info_rdy = false;
	getting_from_input = false;
}


Conv_MKMC_2D::~Conv_MKMC_2D							()
{
}


bool Conv_MKMC_2D::IsConv							() const
{
	return true;
}


bool Conv_MKMC_2D::Map_Data_Blocks_and_Build_Dependencies(
														Data_Logger* DataL,
														Dependency_Logger* DpndL)
{
	if (mapped)
	{
		std::cout << "\tE: This Layer has been Mapped before,\t(Layer ID: " << ID << ")" << std::endl;
		return false;
	}
	if (!Prev_Out_Sch_Info_rdy)
	{
		std::cout << "\tE: calling for the mapping information too early, the previous output scheduling info is not ready.";
		std::cout << "\t(this layer ID : " << ID << " is not ready)" << std::endl;
		return false;
	}
	else
	{
		SNID_t Start_Node_ID	= DpndL->Get_Starting_Point_Scheduling_Node_Idx();
		SNID_t End_Node_ID		= DpndL->Get_Ending_Point_Scheduling_Node_Idx();
		
		
		auto Generate_Zero_Data_Blocks = [this](Data_Logger* DataL, Dependency_Logger* DpndL, SNID_t& SNID, DBID_t& DBID)
			{
				Data_Block_Info DBInfo{};
				SNID_t Start_Node_ID	= DpndL->Get_Starting_Point_Scheduling_Node_Idx();
				SNID_t End_Node_ID		= DpndL->Get_Ending_Point_Scheduling_Node_Idx();


				Dependency dep{};
				dep.satisfied = false;
				dep.Dependent_ID = Start_Node_ID;
				SNID = DpndL->Create_Scheduling_Node(Sch_Input_Layer, ID, dep);


				if (getting_from_input)
					DBInfo.DBT = DBT_IZero;
				else
					DBInfo.DBT = DBT_PZero;
				DBInfo.ready = true;
				DBInfo.alocated = false;
				DBInfo.to_Generate = true;
				DBInfo.prod_flag = false;
				DBInfo.cons_flag = false;
				DBInfo.Layer_ID = ID;
				DBInfo.Dims = Maxes_info;
				DBInfo.Idxs = Conv_Layer_Info_NULL;
				DBID = DataL->Creat_Data_Block(DBInfo, SNID, End_Node_ID);
				DpndL->Set_Produced_Data_Block_ID(SNID, DBID);



				//	New Process:
				dep.satisfied = false;
				dep.Dependent_ID = SNID;
				SNID = DpndL->Create_Scheduling_Node(Sch_CNN_Input, ID, dep);
				// Add consumer to the previous Block
				DataL->Add_Consumer(DBID, SNID);
				DpndL->Add_Consumed_Data_Block_ID(SNID, DBID);
				// Data Block
				DBInfo.DBT = DBT_INTERNAL;
				DBInfo.ready = false;
				DBInfo.alocated = false;
				DBInfo.to_Generate = false;
				DBInfo.Layer_ID = ID;
				DBInfo.Dims = Maxes_info;
				DBInfo.Idxs = Conv_Layer_Info_NULL;
				DBID = DataL->Creat_Data_Block(DBInfo, SNID);
				DpndL->Set_Produced_Data_Block_ID(SNID, DBID);
			};
		SNID_t Zero_Start_SNID;
		DBID_t Zero_Start_DBID;
		Generate_Zero_Data_Blocks(DataL, DpndL, Zero_Start_SNID, Zero_Start_DBID);


		SNID_t Zero_End_SNID;
		DBID_t Zero_End_DBID;
		Generate_Zero_Data_Blocks(DataL, DpndL, Zero_End_SNID, Zero_End_DBID);

		// Preperations 
		// input  format B H W C
		// weight format H W C k
		// Output format B H W C	(this C is K)
		// p sum  format B H W C K FH FW
		unsigned int B, H, C, W, K, FH, FW;
		SNID_t****		Wgt_Node_Map			= new_SNID_4D(map_entity.FiltH_Size, map_entity.FiltW_Size,  map_entity.Channel_size, map_entity.Kernel_size);
		DBID_t****		Wgt_Data_Map			= new_DBID_4D(map_entity.FiltH_Size, map_entity.FiltW_Size,  map_entity.Channel_size, map_entity.Kernel_size);
		SNID_t****		Inp_Node_Map			= new_SNID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size,	  map_entity.Channel_size);
		DBID_t****		Inp_Data_Map			= new_DBID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size,	  map_entity.Channel_size);
		SNID_t*******	Psm_Node_Map			= new_SNID_7D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size,   map_entity.Channel_size, map_entity.Kernel_size, map_entity.FiltH_Size / Windo_info.Height_size, map_entity.FiltW_Size / Windo_info.Width_size);
		DBID_t*******	Psm_Data_Map			= new_DBID_7D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size,   map_entity.Channel_size, map_entity.Kernel_size, map_entity.FiltH_Size / Windo_info.Height_size, map_entity.FiltW_Size / Windo_info.Width_size);
		

		unsigned int inpt_block_needed; // number of data block needed for input		
		unsigned int wght_block_needed; // number of data block needed for weight		
		unsigned int pout_block_needed; // number of data block needed for partial sum 
		unsigned int outp_block_needed; // number of data block needed for output

		inpt_block_needed = map_entity.Batch_size * map_entity.Height_size * map_entity.Channel_size * map_entity.Width_size;
		wght_block_needed = map_entity.FiltH_Size * map_entity.FiltW_Size * map_entity.Kernel_size * map_entity.Channel_size;
		outp_block_needed = map_entity.Batch_size * map_entity.Height_size * map_entity.Width_size * map_entity.Kernel_size;
		pout_block_needed = map_entity.Batch_size * map_entity.Kernel_size * map_entity.Channel_size * map_entity.Height_size * map_entity.Width_size * map_entity.FiltH_Size * map_entity.FiltW_Size / 9;

		std::cout << "\tL" << ID << ": Convolution Layer;\t\tNumber of Needed Block for (input, weithg, partial_sum, output) : \t(";
		std::cout << inpt_block_needed << ", " << wght_block_needed << ", " << pout_block_needed << ", " << outp_block_needed << ")" << std::endl;



		// Mapping 
		SNID_t SNID;
		DBID_t DBID;
		Data_Block_Info DBInfo{};

		
		

		// generating data block for Weights
		// The Weights Data Blocks are provided by user and should not be produced by the starting Point Node
		// Wgt_Data_Map	:	FH  FW  C   K
		for (FH = 0; FH < map_entity.FiltH_Size; FH++)
		{
			for (FW = 0; FW < map_entity.FiltW_Size; FW++)
			{
				for (C = 0; C < map_entity.Channel_size; C++)
				{
					for (K = 0; K < map_entity.Kernel_size; K++)
					{
						// We threat weight as an input layer
						Dependency dep{};
						dep.satisfied = false;
						dep.Dependent_ID = Start_Node_ID;
						SNID = DpndL->Create_Scheduling_Node(Sch_Input_Layer, ID, dep);


						DBInfo.DBT = DBT_WGT;
						DBInfo.ready = true;
						DBInfo.alocated = false;
						DBInfo.to_Generate = true;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs.Batch_size = 0;
						DBInfo.Idxs.Kernel_size = K;
						DBInfo.Idxs.Channel_size = C;
						DBInfo.Idxs.Width_size = 0;
						DBInfo.Idxs.Height_size = 0;
						DBInfo.Idxs.FiltH_Size = FH;
						DBInfo.Idxs.FiltW_Size = FW;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID, End_Node_ID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);

						

						//	New Process:
						dep.satisfied = false;
						dep.Dependent_ID = SNID;
						SNID = DpndL->Create_Scheduling_Node(Sch_CNN_Weight, ID, dep);
						// Add consumer to the previous Block
						DataL->Add_Consumer(DBID, SNID);
						DpndL->Add_Consumed_Data_Block_ID(SNID, DBID);
						// Data Block
						DBInfo.DBT = DBT_INTERNAL;
						DBInfo.ready = false;
						DBInfo.alocated = false;
						DBInfo.to_Generate = false;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs = Conv_Layer_Info_NULL;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);


						Wgt_Node_Map[FH][FW][C][K] = SNID;
						Wgt_Data_Map[FH][FW][C][K] = DBID;
					}
				}
			}
		}




		// input Node
		for (B = 0; B < map_entity.Batch_size; B++)
		{
			for (H = 0; H < map_entity.Height_size; H++)
			{
				for (W = 0; W < map_entity.Width_size; W++)
				{
					for (C = 0; C < map_entity.Channel_size; C++)
					{
						DBID = Out_Data_Map_PL[B][H][W][C];

						//	New Process:
						Dependency dep{};
						dep.satisfied = false;
						dep.Dependent_ID = Out_Node_Map_PL[B][H][W][C];
						SNID = DpndL->Create_Scheduling_Node(Sch_CNN_Input, ID, dep);
						// Add consumer to the previous Block
						DataL->Add_Consumer(DBID, SNID);
						DpndL->Add_Consumed_Data_Block_ID(SNID, DBID);
						// Data Block
						DBInfo.DBT = DBT_INTERNAL;
						DBInfo.ready = false;
						DBInfo.alocated = false;
						DBInfo.to_Generate = false;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs = Conv_Layer_Info_NULL;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);


						Inp_Node_Map[B][H][W][C] = SNID;
						Inp_Data_Map[B][H][W][C] = DBID;
					}
				}
			}
		}


		// Building Dependencies for the Partial Sums:
		// each partial sum is a PE process, so it can be written as A PE process which depends on its inputs and weights
		// Psm_Data_Map	:	B   H   W   C  K  FH/wind  FW/wind;
		for (FH = 0; FH < map_entity.FiltH_Size / Windo_info.Height_size; FH++)
		{
			for (FW = 0; FW < map_entity.FiltW_Size / Windo_info.Width_size; FW++)
			{
				for (B = 0; B < map_entity.Batch_size; B++)
				{
					for (H = 0; H < map_entity.Height_size; H++)
					{
						for (W = 0; W < map_entity.Width_size; W++)
						{
							for (C = 0; C < map_entity.Channel_size; C++)
							{
								for (K = 0; K < map_entity.Kernel_size; K++)
								{
									SNID = DpndL->Create_Scheduling_Node(Sch_Process, ID);
									Conv_Layer_Info this_CLI;
									this_CLI.Batch_size		=	B;
									this_CLI.Kernel_size	=	K;
									this_CLI.Channel_size	=	C;
									this_CLI.Width_size		=	W;
									this_CLI.Height_size	=	H;
									this_CLI.FiltH_Size		=	FH;
									this_CLI.FiltW_Size		=	FW;
									DpndL->Set_CLI(SNID, this_CLI);
									Dependency dep{};
									dep.satisfied = false;
									for (unsigned int wind_h = 0; wind_h < Windo_info.Height_size; wind_h++)
									{
										for (unsigned int wind_w = 0; wind_w < Windo_info.Width_size; wind_w++)
										{
											SNID_t this_SNID = Wgt_Node_Map[Windo_info.Height_size * FH + wind_h][Windo_info.Width_size * FW + wind_w][C][K];
											DBID_t this_DBID = Wgt_Data_Map[Windo_info.Height_size * FH + wind_h][Windo_info.Width_size * FW + wind_w][C][K];
											dep.Dependent_ID = this_SNID;
											DataL->Add_Consumer(this_DBID, SNID);
											DpndL->Add_Consumed_Data_Block_ID(SNID, this_DBID);
											DpndL->Add_Dependency_to_Scheduling_Node(SNID, dep);
										}
									}
									
									// First Row
									dep.satisfied = false;
									if (H == 0)
									{
										SNID_t this_SNID = Zero_Start_SNID;
										DBID_t this_DBID = Zero_Start_DBID;
										//Generate_Zero_Data_Block(DataL, DpndL, this_SNID, this_DBID);
										dep.Dependent_ID = this_SNID;
										DataL->Add_Consumer(this_DBID, SNID);
										DpndL->Add_Consumed_Data_Block_ID(SNID, this_DBID);
									}
									else
									{
										dep.Dependent_ID = Inp_Node_Map[B][H - 1][W][C];
										DataL->Add_Consumer(Inp_Data_Map[B][H - 1][W][C], SNID);
										DpndL->Add_Consumed_Data_Block_ID(SNID, Inp_Data_Map[B][H - 1][W][C]);
									}
									DpndL->Add_Dependency_to_Scheduling_Node(SNID, dep);
									
									// Second Row
									dep.Dependent_ID = Inp_Node_Map[B][H][W][C];
									DataL->Add_Consumer(Inp_Data_Map[B][H][W][C], SNID);
									DpndL->Add_Consumed_Data_Block_ID(SNID, Inp_Data_Map[B][H][W][C]);
									DpndL->Add_Dependency_to_Scheduling_Node(SNID, dep);
									
									// Third Row
									if (H == (map_entity.Height_size - 1))
									{
										SNID_t this_SNID = Zero_End_SNID;
										DBID_t this_DBID = Zero_End_DBID;
										//Generate_Zero_Data_Block(DataL, DpndL, this_SNID, this_DBID);
										dep.Dependent_ID = this_SNID;
										DataL->Add_Consumer(this_DBID, SNID);
										DpndL->Add_Consumed_Data_Block_ID(SNID, this_DBID);
									}
									else
									{
										dep.Dependent_ID = Inp_Node_Map[B][H + 1][W][C];
										DataL->Add_Consumer(Inp_Data_Map[B][H + 1][W][C], SNID);
										DpndL->Add_Consumed_Data_Block_ID(SNID, Inp_Data_Map[B][H + 1][W][C]);
									}
									DpndL->Add_Dependency_to_Scheduling_Node(SNID, dep);
									
									// Data Block
									DBInfo.DBT = DBT_INTERNAL;
									DBInfo.ready = false;
									DBInfo.alocated = false;
									DBInfo.to_Generate = false;
									DBInfo.prod_flag = false;
									DBInfo.cons_flag = false;
									DBInfo.Layer_ID = ID;
									DBInfo.Dims = Maxes_info;
									DBInfo.Idxs = Conv_Layer_Info_NULL;
									DBID = DataL->Creat_Data_Block(DBInfo, SNID);
									DpndL->Set_Produced_Data_Block_ID(SNID, DBID);

									Psm_Data_Map[B][H][W][C][K][FH][FW] = DBID;
									Psm_Node_Map[B][H][W][C][K][FH][FW] = SNID;
								}
							}
						}
					}
				}
			}
		}
		


		// Building Dependencies for the Output Data:
		// each output block can be calculated by adding several partial sum result, to be exact, 
		//		"map_entity.Channel * map_entity.FiltH_Size/Windo_info.Height_size * map_entity.FiltW_Size/Windo_info.Width_size"
		// partial sum results 
		// Out_Data_Map	:	B   H   W   K
		for (B = 0; B < map_entity.Batch_size; B++)
		{
			for (H = 0; H < map_entity.Height_size; H++)
			{
				for (W = 0; W < map_entity.Width_size; W++)
				{
					for (K = 0; K < map_entity.Kernel_size; K++)
					{
						SNID = DpndL->Create_Scheduling_Node(Sch_Accumulate, ID);
						Dependency dep = {};
						dep.satisfied = false;
						for (FH = 0; FH < map_entity.FiltH_Size / Windo_info.Height_size; FH++)
						{
							for (FW = 0; FW < map_entity.FiltW_Size / Windo_info.Width_size; FW++)
							{
								for (C = 0; C < map_entity.Channel_size; C++)
								{
									dep.Dependent_ID = Psm_Node_Map[B][H][W][C][K][FH][FW];
									DataL->Add_Consumer(Psm_Data_Map[B][H][W][C][K][FH][FW], SNID);
									DpndL->Add_Consumed_Data_Block_ID(SNID, Psm_Data_Map[B][H][W][C][K][FH][FW]);
									DpndL->Add_Dependency_to_Scheduling_Node(SNID, dep);
								}
							}
						}
						// Data Block
						DBInfo.DBT = DBT_INTERNAL;
						DBInfo.ready = false;
						DBInfo.alocated = false;
						DBInfo.to_Generate = false;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs = Conv_Layer_Info_NULL;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);


						//	New Process:
						dep.Dependent_ID = SNID;
						SNID = DpndL->Create_Scheduling_Node(Sch_Activation, ID, dep);
						// Add consumer to the previous Block
						DataL->Add_Consumer(DBID, SNID);
						DpndL->Add_Consumed_Data_Block_ID(SNID, DBID);
						// Data Block
						DBInfo.DBT = DBT_PSUM;
						DBInfo.ready = false;
						DBInfo.alocated = false;
						DBInfo.to_Generate = false;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs = Conv_Layer_Info_NULL;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);
						Out_Node_Map[B][H][W][K] = SNID;
						Out_Data_Map[B][H][W][K] = DBID;
					}
				}
			}
		}

		Out_Sch_Info_rdy = true;
	}
	return true;
}


bool Conv_MKMC_2D::Get_output_scheduling_info		(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI)
{
	if (Out_Sch_Info_rdy)
	{
		map_entity_info = map_entity;
		PLONM = Out_Node_Map;
		PLODM = Out_Data_Map;
		GFI = false;
	}
	else
	{
		std::cout << "\tE: calling for the mapping information too early, ";
		std::cout << "\t(this layer ID : " << ID << " is not ready)" << std::endl;
	}
	return Out_Sch_Info_rdy;
}





MPDR_MC_2D::MPDR_MC_2D								(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														MaxP_Layer_Info Window,
														size_t layer_id)
{
	Layer_info = Linfo;
	Maxes_info = Maxes;
	Windo_info = Window;

	map_entity.Batch_size   = (unsigned int)std::ceil((double)Layer_info.Batch_size   / (double)Maxes_info.Batch_size);
	map_entity.Channel_size = (unsigned int)std::ceil((double)Layer_info.Channel_size / (double)Maxes_info.Channel_size);
	map_entity.Width_size   = (unsigned int)std::ceil((double)Layer_info.Width_size   / (double)Maxes_info.Width_size);
	map_entity.Height_size  = (unsigned int)std::ceil((double)Layer_info.Height_size  / (double)Maxes_info.Height_size);
	map_entity.Kernel_size  = map_entity.Channel_size;

	Out_Data_Map			= new_DBID_4D(map_entity.Batch_size, map_entity.Height_size / Windo_info.Height_size, map_entity.Width_size / Windo_info.Width_size, map_entity.Channel_size);
	Out_Node_Map			= new_SNID_4D(map_entity.Batch_size, map_entity.Height_size / Windo_info.Height_size, map_entity.Width_size / Windo_info.Width_size, map_entity.Channel_size);
	
	Out_Data_Map_PL			= NULL;
	Out_Node_Map_PL			= NULL;
	

	ID = layer_id;
	mapped = false;
	Out_Sch_Info_rdy = false;
	Prev_Out_Sch_Info_rdy = false;
	getting_from_input = false;
}


MPDR_MC_2D::~MPDR_MC_2D								()
{

}


bool MPDR_MC_2D::IsMPDR								() const
{
	return true;
}


bool MPDR_MC_2D::Map_Data_Blocks_and_Build_Dependencies(
														Data_Logger* DataL,
														Dependency_Logger* DpndL)
{
	if (mapped)
	{
		std::cout << "\tE: This Layer has been Mapped before,\t(Layer ID: " << ID << ")" << std::endl;
		return false;
	}
	if (!Prev_Out_Sch_Info_rdy)
	{
		std::cout << "\tE: calling for the mapping information too early, the previous output scheduling info is not ready.";
		std::cout << "\t(this layer ID : " << ID << " is not ready)" << std::endl;
		return false;
	}
	else 
	{
		mapped = true;


		std::cout << "\tL" << ID << ": Max Pooling Layer;" << std::endl;


		// Preperations 
		// input  format B H W C
		// Output format B H W C
		unsigned int B, H, W, C;
		

		// Mapping 
		SNID_t SNID;
		DBID_t DBID;
		Data_Block_Info DBInfo{};

		

		// Building Dependencies for the Output Data:
		// each output block can be generated from exact 1 input block
		// Out_Data_Map	:	B   H   W   C
		for (B = 0; B < map_entity.Batch_size; B++)
		{
			for (H = 0; H < map_entity.Height_size / Windo_info.Height_size; H++)
			{
				for (W = 0; W < map_entity.Width_size / Windo_info.Width_size; W++)
				{
					for (C = 0; C < map_entity.Channel_size; C++)
					{
						SNID = DpndL->Create_Scheduling_Node(Sch_MPDR, ID);
						Dependency dep{};
						dep.satisfied = false;
						for (unsigned int wind_h = 0; wind_h < Windo_info.Height_size; wind_h++)
						{
							for (unsigned int wind_w = 0; wind_w < Windo_info.Width_size; wind_w++)
							{
								dep.Dependent_ID = Out_Node_Map_PL[B][Windo_info.Height_size*H+ wind_h][Windo_info.Width_size*W+ wind_w][C];
								DBID = Out_Data_Map_PL[B][Windo_info.Height_size * H + wind_h][Windo_info.Width_size * W + wind_w][C];
								DataL->Add_Consumer(DBID, SNID);
								DpndL->Add_Consumed_Data_Block_ID(SNID, DBID);
								DpndL->Add_Dependency_to_Scheduling_Node(SNID, dep);
							}
						}
						// Data Block
						DBInfo.DBT = DBT_PSUM;
						DBInfo.ready = false;
						DBInfo.alocated = false;
						DBInfo.to_Generate = false;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs = Conv_Layer_Info_NULL;
						DBID = DataL->Creat_Data_Block(DBInfo, SNID);
						DpndL->Set_Produced_Data_Block_ID(SNID, DBID);
						Out_Node_Map[B][H][W][C] = SNID;
						Out_Data_Map[B][H][W][C] = DBID;
					}
				}
			}
		}

		Out_Sch_Info_rdy = true;
	}
	return true;
}


bool MPDR_MC_2D::Get_output_scheduling_info			(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI)
{
	if (Out_Sch_Info_rdy)
	{
		map_entity_info = map_entity;
		map_entity_info.Height_size /= Windo_info.Height_size;
		map_entity_info.Width_size /= Windo_info.Width_size;
		PLONM = Out_Node_Map;
		PLODM = Out_Data_Map;
		GFI = false;
	}
	else
	{
		std::cout << "\tE: calling for the mapping information too early, ";
		std::cout << "\t(this layer ID : " << ID << " is not ready)" << std::endl;
	}
	return Out_Sch_Info_rdy;
}





Output_Layer_2D::Output_Layer_2D					(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														size_t layer_id)
{
	Layer_info = Linfo;
	Maxes_info = Maxes;
	Windo_info = {3, 3};

	map_entity.Batch_size   = (unsigned int)std::ceil((double)Layer_info.Batch_size   / (double)Maxes_info.Batch_size);
	map_entity.Channel_size = (unsigned int)std::ceil((double)Layer_info.Channel_size / (double)Maxes_info.Channel_size);
	map_entity.Width_size   = (unsigned int)std::ceil((double)Layer_info.Width_size   / (double)Maxes_info.Width_size);
	map_entity.Height_size  = (unsigned int)std::ceil((double)Layer_info.Height_size  / (double)Maxes_info.Height_size);
	map_entity.Kernel_size  = map_entity.Channel_size;

	
	Out_Data_Map			= new_DBID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size, map_entity.Kernel_size);
	Out_Node_Map			= new_SNID_4D(map_entity.Batch_size, map_entity.Height_size, map_entity.Width_size, map_entity.Kernel_size);
	
	Out_Data_Map_PL			= NULL;
	Out_Node_Map_PL			= NULL;


	ID = layer_id;
	mapped = false;
	Out_Sch_Info_rdy = false;
	Prev_Out_Sch_Info_rdy = false;
	getting_from_input = false;
}


Output_Layer_2D::~Output_Layer_2D					()
{
}


bool Output_Layer_2D::IsOutput						() const
{
	return true;
}


bool Output_Layer_2D::Map_Data_Blocks_and_Build_Dependencies(
														Data_Logger* DataL,
														Dependency_Logger* DpndL)
{
	if (mapped)
	{
		std::cout << "\tE: This Layer has been Mapped before,\t(Layer ID: " << ID << ")" << std::endl;
		return false;
	}
	if (!Prev_Out_Sch_Info_rdy)
	{
		std::cout << "\tE: Previous Layer is not ready,\t(Layer ID: " << ID << std::endl;
		return false;
	}
	else
	{
		mapped = true;
		SNID_t End_Node_ID = DpndL->Get_Ending_Point_Scheduling_Node_Idx();
		DBID_t DBID;
		//Data_Block_Info DBInfo{};



		std::cout << "\tL" << ID << ": Output Layer;" << std::endl;



		// Preperations 
		// input  format B H W C
		// Output format B H W C	(this C is K)
		unsigned int B, H, W, K;



		// Dependencies
		// Building Dependencies for the Input Data, it depends on the output of the previous layer
		// Inp_Data_Map	:	B   H   W   K
		for (B = 0; B < map_entity.Batch_size; B++)
		{
			for (H = 0; H < map_entity.Height_size; H++)
			{
				for (W = 0; W < map_entity.Width_size; W++)
				{
					for (K = 0; K < map_entity.Kernel_size; K++)
					{
						//Dependency dep{};
						//dep.type = Dep_Node;
						//dep.satisfied = false;
						//dep.Dependent_ID = Out_Scheduling_Node_Map_PL[B][H][W][K];
						//SNID = DpndL->Create_Scheduling_Node(Sch_Data, dep);
						//Out_Scheduling_Node_Map[B][H][W][K] = SNID;
						// Above or follow
						Out_Node_Map[B][H][W][K] = Out_Node_Map_PL[B][H][W][K];
						// Changing the info of Data Blocks f this layer
						DBID = Out_Data_Map_PL[B][H][W][K];
						Data_Block DBInfo;
						DataL->Get_Data_Block_Info(DBID, DBInfo);
						DBInfo.DBT = DBT_DATA_OUT;
						DBInfo.ready = false;
						DBInfo.alocated = false;
						DBInfo.to_Generate = false;
						DBInfo.prod_flag = false;
						DBInfo.cons_flag = false;
						DBInfo.Layer_ID = ID;
						DBInfo.Dims = Maxes_info;
						DBInfo.Idxs.Batch_size = B;
						DBInfo.Idxs.Kernel_size = K;
						DBInfo.Idxs.Channel_size = 0;
						DBInfo.Idxs.Width_size = W;
						DBInfo.Idxs.Height_size = H;
						DBInfo.Idxs.FiltH_Size = 0;
						DBInfo.Idxs.FiltW_Size = 0;
						DBInfo.Consumers_ID.push_back(End_Node_ID);
						DataL->Change_Data_Block_info(DBID, DBInfo);
					}
				}
			}
		}



		// Building Dependencies for the End Point 
		// Outputs	:	B   H   W   K
		Scheduling_Node End_Node;
		DpndL->Fetch_Scheduling_Node_Info(End_Node_ID, End_Node);
		Dependency dep{};
		dep.satisfied = false;
		for (B = 0; B < map_entity.Batch_size; B++)
		{
			for (H = 0; H < map_entity.Height_size; H++)
			{
				for (W = 0; W < map_entity.Width_size; W++)
				{
					for (K = 0; K < map_entity.Kernel_size; K++)
					{
						dep.Dependent_ID = Out_Node_Map[B][H][W][K];
						End_Node.Dependencies.push_back(dep);
						End_Node.Consumes_DBID.push_back(Out_Data_Map_PL[B][H][W][K]);
					}
				}
			}
		}
		DpndL->Change_Scheduling_Node_Info(End_Node_ID, End_Node);


		Out_Sch_Info_rdy = true;
	}
	return true;
}


bool Output_Layer_2D::Get_output_scheduling_info	(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI)
{
	if (Out_Sch_Info_rdy)
	{
		map_entity_info = map_entity;
		PLONM = Out_Node_Map;
		PLODM = Out_Data_Map;
		GFI = false;
	}
	else
	{
		std::cout << "\tE: calling for the mapping information too early, ";
		std::cout << "\t(this layer ID : " << ID << " is not ready)" << std::endl;
	}
	return Out_Sch_Info_rdy;
}





NETWORK::NETWORK									()
{
	NET.clear();
	NET.reserve(32);
}


NETWORK::~NETWORK									()
{
}


size_t NETWORK::Add_Layer							(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes)
{
	MaxP_Layer_Info MP_window = { 3, 3 };
	if (ltype == L2D_MPDR_MC)
	{
		std::cout << "E: You need to specify the window size." << std::endl;
		throw("E: You need to specify the window size.");
		return 0;
	}
	return base_Add_Layer(ltype, linfo, lmaxes, MP_window);
}


size_t NETWORK::Add_Layer							(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes,
														MaxP_Layer_Info MP_window)
{
	if ((ltype != L2D_MPDR_MC) && ((MP_window.Height_size != 3)||(MP_window.Width_size != 3)))
	{
		std::cout << "W: This Layer can not take windoe size other than (3, 3),\n\tThe window size is setting to (3, 3)." << std::endl;
		MP_window = { 3, 3 };
	}

	return base_Add_Layer(ltype, linfo, lmaxes, MP_window);
}


bool NETWORK::Build_Network							(
														Data_Logger* DataL,
														Dependency_Logger* DpndL)
{
	size_t OLCntr(0);


	// Check if the First layer is Input layer
	if (!NET[0]->IsInput())
	{
		std::cout << "E: The First Layer of the network should be input layer." << std::endl;
		throw("E: The First Layer of the netork should be input layer.");
		return false;
	}
	

	for (size_t i = 0; i < NET.size(); ++i)
	{	
		OLCntr += NET[i]->IsOutput();
		if (i != 0)
			NET[i]->Get_previous_output_scheduling_info(NET[i - 1].get());
		
		NET[i]->Map_Data_Blocks_and_Build_Dependencies(DataL, DpndL);
	}

	// Check if the Last layer is Output layer
	size_t last = NET.size() - 1;
	if (!NET[last]->IsOutput())
	{
		std::cout << "E: The Last Layer of the network should be output layer." << std::endl;
		throw("E: The Last Layer of the network should be output layer.");
		return false;
	}

	// There Sould be only one Output layer
	if (OLCntr > 1)
	{
		std::cout << "E: There whould be exactly one Output layer in the Network." << std::endl;
		throw("E: There whould be exactly one Output layer in the Network.");
		return false;
	}

	return NET[last]->mapped;
}


Conv_Layer_Info NETWORK::Get_Maxes_info				(
														size_t id)
{
	return NET[id]->Get_Maxes_info();
}


void NETWORK::Get_input_sizes						(
														std::vector<Conv_Layer_Info>& linfo,
														std::vector<Conv_Layer_Info>& minfo,
														std::vector<size_t>& lid)
{
	//size_t inp_cntr(0);

	linfo.clear();
	minfo.clear();
	lid.clear();
	for (size_t i = 0; i < NET.size(); i++)
	{
		if (NET[i]->IsInput())
		{
			linfo.push_back(NET[i]->Get_Layer_info());
			minfo.push_back(NET[i]->Get_Maxes_info());
			lid.push_back(NET[i]->Get_Layer_ID());
		}
	}
}


void NETWORK::Get_weight_sizes						(
														std::vector<Conv_Layer_Info>& linfo,
														std::vector<Conv_Layer_Info>& minfo,
														std::vector<size_t>& lid)
{
	//size_t inp_cntr(0);

	linfo.clear();
	minfo.clear();
	lid.clear();
	for (size_t i = 0; i < NET.size(); i++)
	{
		if (NET[i]->IsConv())
		{
			linfo.push_back(NET[i]->Get_Layer_info());
			minfo.push_back(NET[i]->Get_Maxes_info());
			lid.push_back(NET[i]->Get_Layer_ID());
		}
	}
}


size_t NETWORK::base_Add_Layer						(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes,
														MaxP_Layer_Info MP_window)
{
	size_t this_ID = NET.size();
	switch (ltype)
	{
	case L2D_Input:
		NET.push_back(std::make_unique<Input_Layer_2D>	(linfo, lmaxes, this_ID));
		break;
	case L2D_CONV_MKMC:
		NET.push_back(std::make_unique<Conv_MKMC_2D>	(linfo, lmaxes, this_ID));
		break;
	case L2D_MPDR_MC:
		NET.push_back(std::make_unique<MPDR_MC_2D>		(linfo, lmaxes, MP_window, this_ID));
		break;
	case L2D_Output:
		NET.push_back(std::make_unique<Output_Layer_2D>	(linfo, lmaxes, this_ID));
		break;
	case L2D_Null:
	default:
		std::cout << "E: Wrong Layer Type." << std::endl;
		throw("E: Wrong Layer Type");
		break;
	}
	return this_ID;
}

