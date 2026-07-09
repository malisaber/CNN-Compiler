#include "Dependency_Logger.h"



//***************************************//
//*			Public Functions			*//
//***************************************//

// Initialize dependency logger state, create sentinel nodes, and reset counters.
Dependency_Logger::Dependency_Logger()
{
	Spaced = false;
	Scheduled = false;
	Threaded = false;
	Mapped = false;
	Compiled = false;
	Timed = false;
	Allocated = false;
	Dependency_ID_cntr = SNID_t::Null();
	level = 0;
	Nodes.reserve(32768);
	Nodes.push_back({});
	Nodes[0].type = Sch_Null;
	Nodes[0].scheduled	= true;
	Nodes[0].allocated	= true;
	Nodes[0].picked = true;
	Nodes[0].mapped = true;
	Ordering.clear();
	Scheduled_ID_list.clear();
	Execution_Threads.clear();
	Periphral_Threads.clear();
	

	Execution_Threads_OPT.clear();

	Previous_PE_Info_valid = false;
	Clear_Previous_PE_Info();
	Starting_Point_ID = Create_Starting_Point_Scheduling_Node();
	Ending_Point_ID = Create_Ending_Point_Scheduling_Node();
	tmp_cntr = 0;


	Input_Data_Block_Count	= 0;
	Weght_Data_Block_Count	= 0;
	Outpt_Data_Block_Count	= 0;
	Psums_Data_Block_Count	= 0;

	Input_Location_Offset	= 0;
	Weght_Location_Offset	= 0;
	Outpt_Location_Offset	= 0;
	Psums_Location_Offset	= 0;
	END_Offset				= 0;

	need_to_copy			= 0;
	Last_Time				= 0;
}


// No-op destructor; state is owned by STL containers.
Dependency_Logger::~Dependency_Logger()
{
}


// Create a scheduling node without dependencies.
// Returns the newly assigned SNID.
SNID_t Dependency_Logger::Create_Scheduling_Node(Scheduling_types type, size_t layer)
{
	Dependency_ID_cntr++;
	Scheduling_Node tmp;
	tmp.scheduled	= false;
	tmp.picked		= false;
	tmp.mapped		= false;
	tmp.allocated	= false;
	tmp.timmed		= false;
	tmp.ID = Dependency_ID_cntr;
	tmp.layer = layer;
	tmp.Sch_Info = Scheduling_info_NULL;
	tmp.type = type;
	tmp.Alc_Info = Allocation_info_NULL;
	tmp.Dependencies.reserve(12);
	tmp.Condition_for.clear();
	tmp.Produced_DBID = DBID_t::Null();
	tmp.Consumes_DBID.clear();
	Nodes.push_back(tmp);
	return Dependency_ID_cntr;
}


// Create a scheduling node with a single dependency.
// Returns the newly assigned SNID.
SNID_t Dependency_Logger::Create_Scheduling_Node(Scheduling_types type, size_t layer, Dependency dep)
{
	Dependency_ID_cntr++;
	Scheduling_Node tmp;
	tmp.scheduled = false;
	tmp.picked = false;
	tmp.mapped = false;
	tmp.allocated = false;
	tmp.timmed = false;
	tmp.ID = Dependency_ID_cntr;
	tmp.layer = layer;
	tmp.Sch_Info = Scheduling_info_NULL;
	tmp.type = type;
	tmp.Alc_Info = Allocation_info_NULL;
	tmp.Dependencies.push_back(dep);
	tmp.Condition_for.clear();
	tmp.Produced_DBID = DBID_t::Null();
	tmp.Consumes_DBID.clear();
	Nodes.push_back(tmp);
	return Dependency_ID_cntr;
}


// Create a scheduling node with multiple dependencies.
// Returns the newly assigned SNID.
SNID_t Dependency_Logger::Create_Scheduling_Node(Scheduling_types type, size_t layer, std::vector<Dependency> deps)
{
	Dependency_ID_cntr++;
	Scheduling_Node tmp;
	tmp.scheduled = false;
	tmp.picked = false;
	tmp.mapped = false;
	tmp.allocated = false;
	tmp.timmed = false;
	tmp.ID = Dependency_ID_cntr;
	tmp.layer = layer;
	tmp.Sch_Info = Scheduling_info_NULL;
	tmp.type = type;
	tmp.Alc_Info = Allocation_info_NULL;
	tmp.Dependencies = deps;
	tmp.Condition_for.clear();
	tmp.Produced_DBID = DBID_t::Null();
	tmp.Consumes_DBID.clear();
	Nodes.push_back(tmp);
	return Dependency_ID_cntr;
}


// Get the start sentinel scheduling node ID.
SNID_t Dependency_Logger::Get_Starting_Point_Scheduling_Node_Idx() const
{
	return Starting_Point_ID;
}


// Get the end sentinel scheduling node ID.
SNID_t Dependency_Logger::Get_Ending_Point_Scheduling_Node_Idx() const
{
	return Ending_Point_ID;
}


// Get the layer index for a scheduling node.
size_t Dependency_Logger::Get_Layer_of(SNID_t SNI)
{
	return Nodes[SNI.index()].layer;
}


// Set the produced data block ID for a scheduling node.
bool Dependency_Logger::Set_Produced_Data_Block_ID(SNID_t SNI, DBID_t PDBID)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		Nodes[SNI.index()].Produced_DBID = PDBID;
	}
	return found;
}


// Add a consumed data block ID to a scheduling node.
bool Dependency_Logger::Add_Consumed_Data_Block_ID(SNID_t SNI, DBID_t CDBID)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		Nodes[SNI.index()].Consumes_DBID.push_back(CDBID);
	}
	return found;
}


// Add a dependency edge to a scheduling node.
bool Dependency_Logger::Add_Dependency_to_Scheduling_Node(SNID_t SNI, Dependency dep)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
			found = true;
			Nodes[SNI.index()].Dependencies.push_back(dep);
	}
	return found;
}


// Set scheduling info for a node.
bool Dependency_Logger::Set_Scheduling_Info(SNID_t SNI, Scheduling_info Sinfo)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		Nodes[SNI.index()].Sch_Info = Sinfo;
	}
	return found;
}


// Get scheduling info for a node.
bool Dependency_Logger::Get_Scheduling_Info(SNID_t SNI, Scheduling_info& Sinfo)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		Sinfo = Nodes[SNI.index()].Sch_Info;
	}
	return found;
}


// Fetch a full scheduling node record.
bool Dependency_Logger::Fetch_Scheduling_Node_Info(SNID_t SNI, Scheduling_Node& SNinfo)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		SNinfo = Nodes[SNI.index()];
	}
	return found;
}


// Replace a scheduling node record and preserve its ID.
bool Dependency_Logger::Change_Scheduling_Node_Info(SNID_t SNID, Scheduling_Node SNinfo)
{
	bool found(false);
	if (SNID.index() < Nodes.size())
	{
		found = true;
		Nodes[SNID.index()] = SNinfo;
		Nodes[SNID.index()].ID = SNID;
	}
	return found;
}


// Find nodes that depend on a given dependency.
// If compiled, uses Condition_for cache; otherwise scans all nodes.
bool Dependency_Logger::Fetch_Scheduling_Nodes_with_this_Dependency(std::vector<SNID_t>& NID, Dependency dep)
{
	NID.clear();
	bool found(false);

	if (Compiled)
	{
		NID = Nodes[dep.Dependent_ID.index()].Condition_for;
		found = true;
	}
	else
	{
		tmp_cntr++;
		unsigned int j;
		for (SNID_t i= SNID_t::Null(); i < Nodes.size(); i++)
		{
			for (j = 0; j < Nodes[i.index()].Dependencies.size(); j++)
			{
				if (Nodes[i.index()].Dependencies[j].Dependent_ID == dep.Dependent_ID)
				{
					NID.push_back(i);
					found = true;
					break;
				}
			}
		}
	}
	return found;
}


// Seting Convolution Layer Information of the Node
bool Dependency_Logger::Set_CLI(SNID_t SNI, Conv_Layer_Info CLinfo)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		Nodes[SNI.index()].CLI = CLinfo;
	}
	return found;
}


// Getting the Convolution Layer Information of the Node
bool Dependency_Logger::Get_CLI(SNID_t SNI, Conv_Layer_Info& CLinfo)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		CLinfo = Nodes[SNI.index()].CLI;
	}
	return found;
}


// Print a scheduling node and its dependencies to stdout.
bool Dependency_Logger::print_Scheduling_Node_Info(SNID_t SNI)
{
	bool found(false);
	if (SNI < Nodes.size())
	{
		found = true;
		Scheduling_Node sn = Nodes[SNI.index()];
		std::cout << "Scheduling Node #"			<< SNI;
		std::cout << ", \tType: "					<< sn.type;
		std::cout << ", \tNumber of Dependencies: " << sn.Dependencies.size() << std::endl;
		for (unsigned int i = 0; i < sn.Dependencies.size(); i++)
		{
			std::cout << "\t";
			print(sn.Dependencies[i]);
			std::cout << std::endl;
		}
	}
	return found;
}


// Dump all scheduling nodes to a text file (appends .txt).
// Returns number of nodes written.
size_t Dependency_Logger::print_file(std::filesystem::path name)
{
	unsigned int i, j;

	std::ofstream file_out;
	file_out.open(name);

	file_out				<< std::setw(8) << "Node";
	file_out << "\t("		<< std::setw(8) << "Doup Of";
	file_out << ")\t#"		<< std::setw(1) << "T";
	file_out << "\t\t("		<< std::setw(1) << "S";
	file_out << ", "		<< std::setw(2) << "EL";
	file_out << ", "		<< std::setw(2) << "SL";
	file_out << ", "		<< std::setw(2) << "SR";
	file_out << ")\t\t("	<< std::setw(1) << "P";
	file_out << ", "		<< std::setw(1) << "M";
	file_out << ", "		<< std::setw(1) << "A";
	file_out << ")\t("		<< std::setw(2) << "P";
	file_out << ", "		<< std::setw(2) << "V";
	file_out << ")\t -> "	<< std::endl;


	for (i = 1; i < Nodes.size(); i++)
	{
		file_out			<< std::setw(8) << ("N" + std::to_string(Nodes[i].ID.index()));
		file_out << "\t("	<< std::setw(8) << ("N" + std::to_string(Nodes[i].Douplicate_of.index()));
		file_out << ")\t#"	<< std::setw(1) << Nodes[i].type;
		file_out << "\t\t("	<< std::setw(1) << Nodes[i].scheduled;
		file_out << ", "	<< std::setw(2) << Nodes[i].Sch_Info.Execution_Level;
		file_out << ", "	<< std::setw(2) << Nodes[i].Sch_Info.Execution_Level_Space_Before;
		file_out << ", "	<< std::setw(2) << Nodes[i].Sch_Info.Execution_Level_Space_After;
		file_out << ")\t\t("<< std::setw(1) << Nodes[i].picked;
		file_out << ", "	<< std::setw(1) << Nodes[i].mapped;
		file_out << ", "	<< std::setw(1) << Nodes[i].allocated;
		file_out << ")\t("	<< std::setw(2) << Nodes[i].Alc_Info.Allocated_Plane;
		file_out << ", "	<< std::setw(2) << Nodes[i].Alc_Info.Allocated_Vault;
		file_out << ")\t -> ";
		for (j = 0; j < Nodes[i].Dependencies.size(); j++)
		{
			file_out << std::setw(7) << ("N" + std::to_string(Nodes[i].Dependencies[j].Dependent_ID.index())) << ", \t";
		}
		file_out << std::endl;
	}
	
	file_out.close();
	return Nodes.size();
}


// Return number of scheduling nodes stored.
size_t Dependency_Logger::size()
{
	return Nodes.size();
}


// Schedule dependency nodes level-by-level until no further nodes are ready.
// Computes spacing info and compiles reverse edges.
size_t Dependency_Logger::Schedule_Nodes(Data_Logger* DL)
{
	size_t tot(0);
	unsigned int level(0);
	for (size_t i = 0; i < Scheduled_ID_list.size(); i++)
		Scheduled_ID_list[i].clear();
	Scheduled_ID_list.clear();

	while (true)
	{
		
		Scheduled_ID_list.push_back({});
		bool has(false);
		for (size_t i = 0; i < Nodes.size(); i++)
		{
			if (Is_Scheduling_Node_Satisfied(DL, Nodes[i]))
			{
				tot++;
				has = true;
				Scheduled_ID_list[level].push_back(Nodes[i].ID);
				Nodes[i].Sch_Info.Execution_Level = level; //Nodes[i].scheduled = true;
				Nodes[i].Sch_Info.Execution_Level_Space_Before = -1;
				Nodes[i].Sch_Info.Execution_Level_Space_After  = -1;
			}
		}
		
		
		for (size_t i = 0; i < Scheduled_ID_list[level].size(); i++)
		{
			SNID_t SNode = Scheduled_ID_list[level][i];
			Nodes[SNode.index()].scheduled = true;
		}
		if (has)
		{
			std::cout << "\033[2K\r" << std::flush;
			std::cout << "\tBuilding Level #" << level;
			std::cout << "\t\tNode of this Level: " << std::setw(6) << Scheduled_ID_list[level].size();
			std::cout << "\t\tProgress: " << (tot * 100) / (size() - 1) << "%" << std::endl;
		}
		else
		{
			if (Scheduled_ID_list[Scheduled_ID_list.size() - 1].size() == 0)
				Scheduled_ID_list.pop_back();
			break;
		}
		level++;
	}

	size_t scheduled_cntr(1);
	for (size_t i = 0; i < Scheduled_ID_list.size(); i++)
		scheduled_cntr += Scheduled_ID_list[i].size();

	Scheduled = true;
	std::cout << "\tCompiling ...";
	Clear_Scheduling_Node_Spacing_Info();
	Calculation_Scheduling_Node_Spaces();
	Compile_Schedule_Nodes();
	std::cout << "\033[2K\r" << std::flush;
	std::cout << "\tCompilation Complete." << std::endl;
	return scheduled_cntr;
}


// Build execution and peripheral threads from scheduled nodes.
// Groups Sch_Process nodes by similar weights, and Sch_Activation/Sch_MPDR by type.
bool Dependency_Logger::Build_Threads()
{
	//	Sch_Null,		Sch_Start,		Sch_Input,		Sch_Process,	Sch_PE_Store,	Sch_Accumulate,		
	//	********,		*********,		*********,		***********,	************,	**************,	
	//	
	//	Sch_Acc_Store,	Sch_Activation,	Sch_Act_Store,	Sch_MPDR,		Sch_MPDR_Store,	Sch_Stop,
	//	*************,	**************,	*************,	********,		**************,	********,			
	//	
	// PE_Node;
	// SA_Node;
	// MPDR_Node;
	// Transfer_Node;

	//	Sch_Process:
	Execution_Threads.clear();
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		std::cout << "\033[2K\r" << std::flush;
		std::cout << "\tBuilding Execution Threads:\tLeveL#" << lvl;
		std::vector<Execution_Thread_Info> EXE_Threads;
		EXE_Threads.clear();
		for (size_t pt = 0; pt < Scheduled_ID_list[lvl].size(); pt++)
		{
			SNID_t SNode = Scheduled_ID_list[lvl][pt];
			if (Nodes[SNode.index()].picked)
				continue;
			if (Nodes[SNode.index()].type != Sch_Process)
				continue;
			if (EXE_Threads.size() == 0)
			{
				Execution_Thread_Info ETInfo;
				Nodes[SNode.index()].picked = true;
				ETInfo.BaseLine_Idx = SNode;
				ETInfo.Nodes.clear();
				ETInfo.Nodes.push_back(SNode);
				ETInfo.Optimized.push_back(false);
				EXE_Threads.push_back(ETInfo);
				continue;
			}
			bool Added(false);
			for (size_t TBLCntr = 0; TBLCntr < EXE_Threads.size(); TBLCntr++)// Thread BaseLine Counter
			{
				if (Is_PE_Weights_Similar(SNode, EXE_Threads[TBLCntr].BaseLine_Idx))
				{
					Nodes[SNode.index()].picked = true;
					EXE_Threads[TBLCntr].Nodes.push_back(SNode);
					EXE_Threads[TBLCntr].Optimized.push_back(false);
					Added = true;
					break;
				}
			}
			if (!Added)
			{
				Nodes[SNode.index()].picked = true;
				Execution_Thread_Info ETInfo;
				ETInfo.BaseLine_Idx = SNode;
				ETInfo.Nodes.clear();
				ETInfo.Nodes.push_back(SNode);
				ETInfo.Optimized.push_back(false);
				EXE_Threads.push_back(ETInfo);
			}
		}
		Execution_Threads.push_back(EXE_Threads);
	}
	std::cout << "\033[2K\r" << std::flush;
	std::cout << "\tBuilding Execution Threads:\tDone!" << std::endl;

	// Sch_Activation, Sch_MPDR
	Periphral_Threads.clear();
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		std::cout << "\033[2K\r" << std::flush;
		std::cout << "\tBuilding Periphral Threads:\tLeveL#" << lvl;
		std::vector<Periphral_Thread_Info> ACC_Threads;
		ACC_Threads.clear();
		for (size_t pt = 0; pt < Scheduled_ID_list[lvl].size(); pt++)
		{
			SNID_t SNode = Scheduled_ID_list[lvl][pt];
			if (Nodes[SNode.index()].picked)
				continue;
			if ((Nodes[SNode.index()].type != Sch_Activation) && (Nodes[SNode.index()].type != Sch_MPDR))
				continue;
			if (ACC_Threads.size() == 0)
			{
				Nodes[SNode.index()].picked = true;
				Periphral_Thread_Info STInfo;
				Periphral_Sequal_Info SSInfo = {};
				SSInfo.Lasts.clear();
				for (size_t i = 0; i< Nodes[SNode.index()].Dependencies.size(); i++)
					SSInfo.Lasts.push_back(Nodes[SNode.index()].Dependencies[i].Dependent_ID);
				SSInfo.Node = SNode;
				STInfo.type = Nodes[SNode.index()].type;
				STInfo.PSIs.push_back(SSInfo);
				ACC_Threads.push_back(STInfo);
				continue;
			}
			bool Added(false);
			for (size_t PTCntr = 0; PTCntr < ACC_Threads.size(); PTCntr++) // Peripheral Type Counter
			{
				if (ACC_Threads[PTCntr].type == Nodes[SNode.index()].type)
				{
					Nodes[SNode.index()].picked = true;
					Periphral_Sequal_Info SSInfo = {};
					SSInfo.Lasts.clear();
					for (size_t i = 0; i < Nodes[SNode.index()].Dependencies.size(); i++)
						SSInfo.Lasts.push_back(Nodes[SNode.index()].Dependencies[i].Dependent_ID);
					SSInfo.Node = SNode;
					ACC_Threads[PTCntr].PSIs.push_back(SSInfo);
					Added = true;
					break;
				}
			}
			if (!Added)
			{
				Nodes[SNode.index()].picked = true;
				Periphral_Thread_Info STInfo;
				Periphral_Sequal_Info SSInfo = {};
				SSInfo.Lasts.clear();
				for (size_t i = 0; i < Nodes[SNode.index()].Dependencies.size(); i++)
					SSInfo.Lasts.push_back(Nodes[SNode.index()].Dependencies[i].Dependent_ID);
				SSInfo.Node = SNode;
				STInfo.type = Nodes[SNode.index()].type;
				STInfo.PSIs.push_back(SSInfo);
				ACC_Threads.push_back(STInfo);
			}
		}
		Periphral_Threads.push_back(ACC_Threads);
	}
	std::cout << "\033[2K\r" << std::flush;
	std::cout << "\tBuilding Periphral Threads:\tDone!" << std::endl;

	Threaded = true;
	return true;
}


// Optimize execution threads by reordering nodes with sequential inputs.
// Builds Execution_Threads_OPT per level.
bool Dependency_Logger::Optimizing_Execution_Threads()
{
	//Doing for each levels
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		size_t reorder_cntr(0);
		std::vector<std::vector<Execution_Thread_Info>> Execution_Threads_Level_OPT;
		std::cout << "\033[2K\r" << std::flush;
		std::cout << "\tOptimizing Execution Threads -> \tLeveL: " << lvl;

		// Doing for each baseline:
		for (size_t bl = 0; bl < Execution_Threads[lvl].size(); bl++)
		{
			std::cout << "\033[2K\r" << std::flush;
			std::cout << "\tOptimizing Execution Threads -> \tLeveL: " << lvl << ",\tThread:" << bl;
			
			//	Reordering Execution Nodes in this baseline
			std::vector<Execution_Thread_Info> O1_EXE_Threads;
			while (true)
			{
				if ((reorder_cntr % 100) == 0)
				{
					std::cout << "\033[2K\r" << std::flush;
					std::cout << "\tOptimizing Execution Threads -> \tLeveL: " << lvl << ",\tThread:" << bl << ",\tReordered:" << reorder_cntr;
				}
				Execution_Thread_Info opt_thread;
				for (size_t node = 0; node < Execution_Threads[lvl][bl].Nodes.size(); node++)
				{
					if (Execution_Threads[lvl][bl].Optimized[node])
						continue;
					if (opt_thread.Nodes.size() == 0)
					{
						reorder_cntr++;
						opt_thread.BaseLine_Idx = Execution_Threads[lvl][bl].Nodes[node];
						opt_thread.Nodes.push_back(Execution_Threads[lvl][bl].Nodes[node]);
						Execution_Threads[lvl][bl].Optimized[node] = true;
					}
					else if (Is_PE_Inputs_Next_Sequencing(opt_thread.BaseLine_Idx, Execution_Threads[lvl][bl].Nodes[node]))
					{
						reorder_cntr++;
						Execution_Threads[lvl][bl].Optimized[node] = true;
						opt_thread.BaseLine_Idx = Execution_Threads[lvl][bl].Nodes[node];
						opt_thread.Nodes.push_back(Execution_Threads[lvl][bl].Nodes[node]);
						continue;
					}
				}
				if (opt_thread.Nodes.size() != 0)
					O1_EXE_Threads.push_back(opt_thread);
				else
					break;
			}


			// Concatinating Compatible threads this each baseline
			std::vector<Execution_Thread_Info> O2_EXE_Threads;
			for (size_t Trd1 = 0; Trd1 < O1_EXE_Threads.size(); Trd1++)
			{
				if (O1_EXE_Threads[Trd1].Nodes.size() == 0)
					continue;
				for (size_t Trd2 = 0; Trd2 < O1_EXE_Threads.size(); Trd2++)
				{
					if (Trd1 == Trd2)
						continue;
					if (O1_EXE_Threads[Trd2].Nodes.size() == 0)
						continue;
					size_t Trd1_last_node = O1_EXE_Threads[Trd1].Nodes.size() - 1;
					if (Is_PE_Inputs_Next_Sequencing(O1_EXE_Threads[Trd1].Nodes[Trd1_last_node], O1_EXE_Threads[Trd2].Nodes[0]))
					{
						for (size_t node = 0; node < O1_EXE_Threads[Trd2].Nodes.size(); node++)
							O1_EXE_Threads[Trd1].Nodes.push_back(O1_EXE_Threads[Trd2].Nodes[node]);
						O1_EXE_Threads[Trd2].Nodes.clear();
						O1_EXE_Threads[Trd2].Optimized.clear();
					}
				}
			}
			for (size_t Trd1 = 0; Trd1 < O1_EXE_Threads.size(); Trd1++)
				if (O1_EXE_Threads[Trd1].Nodes.size() != 0)
					O2_EXE_Threads.push_back(O1_EXE_Threads[Trd1]);

			Execution_Threads_Level_OPT.push_back(O2_EXE_Threads);
		}

		// Storing Thread Info of this level
		Execution_Threads_OPT.push_back(Execution_Threads_Level_OPT);
	}
	std::cout << "\033[2K\r" << std::flush;
	std::cout << "\tOptimizing Execution Threads:\tDone!" << std::endl;

	return true;
}


// Return total number of nodes across execution and peripheral threads.
size_t Dependency_Logger::Thread_Size()
{
	

	size_t tot(3);
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		size_t this_size(0);
		for (size_t node = 0; node < Execution_Threads[lvl].size(); node++)
			this_size += Execution_Threads[lvl][node].Nodes.size();
		tot += this_size;
	}
	
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		size_t this_size(0);
		for (size_t node = 0; node < Periphral_Threads[lvl].size(); node++)
			this_size += Periphral_Threads[lvl][node].PSIs.size();
		tot += this_size;
	}
	
	//for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	//{
	//	size_t this_size(0);
	//	for (size_t node = 0; node < Storation_Threads[lvl].size(); node++)
	//		this_size += Storation_Threads[lvl][node].PSIs.size();
	//	tot += this_size;
	//}
	
	//for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	//{
	//	size_t this_size(0);
	//	for (size_t node = 0; node < Transfers_Threads[lvl].size(); node++)
	//		this_size += Transfers_Threads[lvl][node].PSIs.size();
	//	tot += this_size;
	//}
	
	return tot;
}


// Dump execution/peripheral thread info to a text file (appends .txt).
size_t Dependency_Logger::print_Thread_info_file(std::filesystem::path name)
{
	std::ofstream file_out;
	file_out.open(name);

	size_t tot(3);
	//std::cout << "\tNumber of Levels: " << Scheduled_ID_list.size() << std::endl;
	file_out  << "\tNumber of Levels: " << Scheduled_ID_list.size() << std::endl;

	//std::cout << "\t\tExecution_Threads:\t(";
	file_out  << "\t\tExecution_Threads:\t(";
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		size_t this_size(0);
		for (size_t node = 0; node < Execution_Threads[lvl].size(); node++)
			this_size += Execution_Threads[lvl][node].Nodes.size();
		//std::cout <<                 Execution_Threads[lvl].size() << "(" <<                  this_size << "), ";
		file_out  << std::setw(5) << Execution_Threads[lvl].size() << "(" <<  std::setw(5) << this_size << "),\t";
		tot += this_size;
	}
	//std::cout << "\b\b)" << std::endl;
	file_out  << ")"     << std::endl;

	//std::cout << "\t\tPeriphral_Threads:\t(";
	file_out  << "\t\tPeriphral_Threads:\t(";
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		size_t this_size(0);
		for (size_t node = 0; node < Periphral_Threads[lvl].size(); node++)
			this_size +=  Periphral_Threads[lvl][node].PSIs.size();
		//std::cout <<                 Periphral_Threads[lvl].size() << "(" <<                  this_size << "), ";
		file_out  << std::setw(5) << Periphral_Threads[lvl].size() << "(" <<  std::setw(5) << this_size << "),\t";
		tot += this_size;
	}
	//std::cout << "\b\b)" << std::endl;
	file_out  << ")"     << std::endl;

	////std::cout << "\t\tStoration_Threads:\t(";
	//file_out  << "\t\tStoration_Threads:\t(";
	//for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	//{
	//	size_t this_size(0);
	//	for (size_t node = 0; node < Storation_Threads[lvl].size(); node++)
	//		this_size +=  Storation_Threads[lvl][node].PSIs.size();
	//	//std::cout <<                 Storation_Threads[lvl].size() << "(" <<                  this_size << "), ";
	//	file_out  << std::setw(5) << Storation_Threads[lvl].size() << "(" <<  std::setw(5) << this_size << "),\t";
	//	tot += this_size;
	//}
	////std::cout << "\b\b)" << std::endl;
	//file_out  << ")"     << std::endl;

	////std::cout << "\t\tTransfers_Threads:\t(";
	//file_out  << "\t\tTransfers_Threads:\t(";
	//for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	//{
	//	size_t this_size(0);
	//	for (size_t node = 0; node < Transfers_Threads[lvl].size(); node++)
	//		this_size +=  Transfers_Threads[lvl][node].PSIs.size();
	//	//std::cout <<                 Transfers_Threads[lvl].size() << "(" <<                  this_size << "), ";
	//	file_out  << std::setw(5) << Transfers_Threads[lvl].size() << "(" <<  std::setw(5) << this_size << "),\t";
	//	tot += this_size;
	//}
	////std::cout << "\b\b)" << std::endl << std::endl;
	//file_out  << ")"     << std::endl << std::endl;

	file_out.close();
	return tot;
}


// Dump optimized execution thread info to a text file (appends .txt).
size_t Dependency_Logger::print_Optimized_Execution_Thread_info_file(std::filesystem::path name)
{
	std::ofstream file_out;
	file_out.open(name);

	size_t tot(0);
	for (size_t lvl = 0; lvl < Scheduled_ID_list.size(); lvl++)
	{
		size_t this_size(0);
		if (Execution_Threads_OPT[lvl].size() == 0)
			continue;
		file_out << std::endl << "Level: " << lvl << std::endl;
		for (size_t bl = 0; bl < Execution_Threads_OPT[lvl].size(); bl++)
		{
			file_out << "\tBaseline @" << bl << std::endl;
			for (size_t trds = 0; trds < Execution_Threads_OPT[lvl][bl].size(); trds++)
			{

				this_size = Execution_Threads_OPT[lvl][bl][trds].Nodes.size();
				file_out << "\t\tThread %" << trds << ",\t#" << this_size << std::endl;
				tot += this_size;
			}
				
		}
	}
	

	file_out.close();
	return tot;
}


// Map scheduled nodes onto hardware planes/vaults and build ordering.
// Returns number of nodes successfully mapped.
size_t Dependency_Logger::Map(Data_Logger* DataL, Hardware* HW, std::filesystem::path mappr_Dump_Dir)
{
	std::ofstream file_out;
	file_out.open(mappr_Dump_Dir); 

	size_t Planes = HW->Get_Number_of_Available_Planes();
	size_t Vaults = HW->Get_Number_of_Available_Vaults();
	size_t tot(1);
	PE_Node PEs[4][16];

	for (size_t i = 0; i < 4; i++)
		for (size_t j = 0; j < 16; j++)
			PEs[i][j] = PE_Node_NULL;

	if (Planes > 4)
	{
		std::cout << "\tE: Error in Hardware configuration of mapper:" << std::endl;
		std::cout << "\t\t Number of Planes (" << Planes << ") succeeds 4, setting to 4";
		Planes = 4;
	}
	if (Vaults != 16)
	{
		std::cout << "\tE: Error in Hardware configuration of mapper:" << std::endl;
		std::cout << "\t\t Number of Vaults (" << Planes << ") should be 16." << std::endl;
		std::cout << "\t\t Other Configuration does not support yet, , setting to 16";
		Vaults = 16;
	}

	Ordering.clear();
	Ordering.reserve(Execution_Threads.size()+1);
	for (size_t tmp = 0; tmp < Scheduled_ID_list.size(); tmp++)
	{
		Ordering.push_back({});
		Ordering[tmp].clear();
	}
	Ordering[0].push_back({});
	Ordering_Node order = {};


	// Waiting For Start
	level = 0;
	SNID_t Start_Point_Idx = Get_Starting_Point_Scheduling_Node_Idx();
	Map_Node(DataL, Start_Point_Idx, 0, 0, 0, 0, false, 0);


	//	Body
	//	Do Mapping on each Threads
	//	Map Each thread on a single vault
	//	
	//	Execution_Threads
	//	Periphral_Threads
	for (size_t lvl = 0; lvl < Execution_Threads.size(); lvl++)
	{
		// printing info
		std::cout << "\tMapping Level #" << std::setw(4) << lvl << std::endl;
		level = lvl;
		
		// if this thread contains Execution Unit
		if (Execution_Threads[lvl].size() > 0)
		{
			if (!USE_CHATGPT_CODE)
			{
				Clear_Previous_PE_Info();
				size_t Baselines	= Execution_Threads_OPT[lvl].size();		//	Different Weigth
				size_t Threads		= Execution_Threads_OPT[lvl][0].size();		//	Different input sequence in each weight
				size_t Sequence		= Execution_Threads_OPT[lvl][0][0].Nodes.size();
				size_t map_Vault	= calculate_vault_map(Baselines);
				size_t mul_Vault	= Vaults / map_Vault;

				file_out << "\n\n";
				file_out << "\t";
				file_out << std::setw(4) << "" << "\t";
				file_out << std::setw(4) << "" << "\t";
				file_out << std::setw(4) << "" << "\t";
				file_out << std::setw(4) << Baselines << ",\t";
				file_out << std::setw(4) << Threads << ",\t";
				file_out << std::setw(4) << Sequence << std::endl;
				file_out << "\t";
				file_out << std::setw(4) << "lvl" << ",\t";
				file_out << std::setw(4) << "P" << ",\t";
				file_out << std::setw(4) << "pes" << ",\t";
				file_out << std::setw(4) << "bls" << ",\t";
				file_out << std::setw(4) << "trd" << ",\t";
				file_out << std::setw(4) << "seq" << std::endl;

				size_t BSLN = Baselines;
				size_t BLEX = 0;
				size_t rnd(0);
				size_t pes(0);
				size_t trd(0);
				size_t bls(0);
				bool   add(false);

				while (BSLN > 0)
				{
					bool last(false);
					map_Vault = calculate_vault_map(BSLN);

					for (size_t i = 0; i < 4; i++)
						for (size_t j = 0; j < 16; j++)
							PEs[i][j] = PE_Node_NULL;

					for (size_t pln = 0; pln < Threads; pln += mul_Vault)
					{
						add = false;
						for (size_t m = 0; m < mul_Vault; m++)
						{
							for (size_t i = 0; i < std::min(map_Vault, BSLN); i++)
							{
								pes = m * map_Vault + i;
								bls = BLEX + i;
								trd = pln + m;
								if (trd >= Threads)
								{
									last = true;
									break;
								}

								size_t p = ((pln / mul_Vault) % Planes);
								size_t seq = Execution_Threads_OPT[lvl][bls][trd].Nodes.size();

								file_out << "\t";
								file_out << std::setw(4) << lvl << ",\t";
								file_out << std::setw(4) << p << ",\t";
								file_out << std::setw(4) << pes << ",\t";
								file_out << std::setw(4) << bls << ",\t";
								file_out << std::setw(4) << trd << ",\t";
								file_out << std::setw(4) << seq << std::endl;

								PEs[p][pes].active = true;
								PEs[p][pes].baseline = bls;
								PEs[p][pes].thread = trd;
								PEs[p][pes].Sequence = Execution_Threads_OPT[lvl][bls][trd].Nodes;
							}
							if (last)
								break;
						}
						if (((pln / mul_Vault) % Planes) == (Planes - 1))
						{
							add = true;
							rnd++;
							Optimize_PE_Execution_Block(DataL, PEs, lvl);
							for (size_t i = 0; i < 4; i++)
								for (size_t j = 0; j < 16; j++)
									PEs[i][j] = PE_Node_NULL;
						}
					}

					BLEX = BLEX + std::min(map_Vault, BSLN);
					BSLN = BSLN - std::min(map_Vault, BSLN);
					if (!add)
					{
						rnd++;
						Optimize_PE_Execution_Block(DataL, PEs, lvl);
						for (size_t i = 0; i < 4; i++)
							for (size_t j = 0; j < 16; j++)
								PEs[i][j] = PE_Node_NULL;
					}
					add = false;
				}

				Generate_Execution_Block(DataL, lvl);
			}
			else
			{
				// Initialization
				std::vector<SNID_t> Execution_Thread_level;
				Execution_Thread_level.reserve(Execution_Threads[lvl].size() * Execution_Threads[lvl][0].Nodes.size());
				std::vector<std::vector<Mapping_Result>> Level_mapping_result;
				for (size_t bline = 0; bline < Execution_Threads_OPT[lvl].size(); bline++)
				{
					for (size_t node = 0; node < Execution_Threads_OPT[lvl][bline].size(); node++)
					{
						for (size_t seq = 0; seq < Execution_Threads_OPT[lvl][bline][node].Nodes.size(); seq++)
						{
							Execution_Thread_level.push_back(Execution_Threads_OPT[lvl][bline][node].Nodes[seq]);
						}
					}
				}

				// Calling the Mapper
				Map_level(Execution_Thread_level, Level_mapping_result);

				// Post processing
				for (size_t bline = 0; bline < Level_mapping_result.size(); bline++)
				{
					size_t block_idx = Ordering[level].size();
					Ordering[level].push_back({});
					Ordering[level][block_idx].reserve(Execution_Thread_level.size());

					for (size_t idx = 0; idx < Level_mapping_result[bline].size(); idx++)
					{
						size_t pln = Level_mapping_result[bline][idx].plane;
						size_t vlt = Level_mapping_result[bline][idx].vault;
						SNID_t this_node_idx = Level_mapping_result[bline][idx].node;
						Map_Node(DataL, this_node_idx, block_idx, lvl, pln, vlt, true, 0);
					}
				}
			}
		}

		// if this Thread contains any peripheral
		if (Periphral_Threads[lvl].size() > 0)
		{
			for (size_t trd = 0; trd < Periphral_Threads[lvl].size(); trd++)
			{
				Ordering[level].reserve(2 * Periphral_Threads[lvl][trd].PSIs.size());
				for (size_t node = 0; node < Periphral_Threads[lvl][trd].PSIs.size(); node++)
				{
					Generate_Peripheral_Block(DataL, Periphral_Threads[lvl][trd].PSIs[node].Node, lvl);
				}
			}
		}
	}
	

	// Do mapping for each dependencies of the End node by calling Map_Node function of End Point;
	// These Nodes are not processing element type, so they are not mapped before.
	// it is highly possible that these dependencies are Output data of last layer.
	// So, Fuck them very hard. fucccccccccccccccck this life.
	SNID_t End_Point_Idx = Get_Ending_Point_Scheduling_Node_Idx();
	size_t End_Point_Lvl = Nodes[End_Point_Idx.index()].Sch_Info.Execution_Level;
	level = Scheduled_ID_list.size() - 1;
	Ordering[level].push_back({});
	Map_Node(DataL, End_Point_Idx, 0, End_Point_Lvl, 0, 0, false, 0);
	


	Mapped = true;
	size_t out(0);
	for (size_t idx = 0; idx < Nodes.size(); idx++)
		out += (size_t)(Nodes[idx].mapped);
	return out;
}


// Dump mapping and ordering info to a text file (appends .txt).
bool Dependency_Logger::print_mapping_file(std::filesystem::path fname)
{
	std::ofstream file_out;
	file_out.open(fname);

	if (Mapped)
	{
		for (size_t ord_lvl = 0; ord_lvl < Ordering.size(); ord_lvl++)
		{
			if (Ordering[ord_lvl].size() > 0)
			{
				file_out << std::endl;
				file_out << "Ordering Level\t\t\t#" << std::setw(6) << ord_lvl << std::endl;
			}
			
			for (size_t ord_blk = 0; ord_blk < Ordering[ord_lvl].size(); ord_blk++)
			{
				if (Ordering[ord_lvl][ord_blk].size() > 0)
				{
					file_out << std::endl;
					file_out << "Execution Block\t\t\t#"			<< std::setw(6) << ord_blk << std::endl;
				}

				for (size_t ord_nod = 0; ord_nod < Ordering[ord_lvl][ord_blk].size(); ord_nod++)
				{
					// Get This Ordering Node
					Ordering_Node Ord_Node = Ordering[ord_lvl][ord_blk][ord_nod];
					// Print something for each type of ordering
					switch (Ord_Node.type)
					{
					case Sch_Start:
						file_out << "Waiting For Start Command" << std::endl;
						break;
					case Sch_Process:
						file_out << "Start Processing Unit\t#"	<< std::setw(6) << Ord_Node.ID;
						file_out << "\t\t\t\t\t\t("				<< std::setw(2) << Ord_Node.Plane;
						file_out << ", "						<< std::setw(2) << Ord_Node.Vault << ")" << std::endl;
						break;
					case Sch_Accumulate:
						file_out << "Start Accumulate Unit\t#"	<< std::setw(6) << Ord_Node.ID;
						file_out << "\t\t\t\t\t\t("				<< std::setw(2) << Ord_Node.Plane;
						file_out << ", "						<< std::setw(2) << Ord_Node.Vault << ")" << std::endl;
						break;
					case Sch_Activation:
						file_out << "Start Activation Unit\t#"	<< std::setw(6) << Ord_Node.ID;
						file_out << "\t\t\t\t\t\t("				<< std::setw(2) << Ord_Node.Plane;
						file_out << ", "						<< std::setw(2) << Ord_Node.Vault << ")" << std::endl;
						break;
					case Sch_MPDR:
						file_out << "Start Max Pooling Unit\t#"	<< std::setw(6) << Ord_Node.ID;
						file_out << "\t\t\t\t\t\t("				<< std::setw(2) << "-";
						file_out << ", "						<< std::setw(2) << Ord_Node.Vault << ")" << std::endl;
						break;
					case Sch_End:
						file_out << "End of Execution"			<< std::endl;
						break;
					default:
						// Print Wrong Node
						//	* Sch_CNN_Input
						//	* Sch_CNN_Weight
						//	* Sch_Null
						//	* Sch_Input_Layer
						std::cout << "Someting Went Wrong in mapper, Node: "	<< Ord_Node.ID << std::endl;
						file_out << "Wrong Node, lol"							<< std::endl;
						break;
					}
				}
			}
		}
	}

	file_out.close();
	return Mapped;
}


// Get allocated vault for a scheduling node.
bool Dependency_Logger::Get_Vault_Info(SNID_t SID, size_t& Vlt)
{
	bool found(false);
	if (SID < Nodes.size())
	{
		found = true;
		Vlt = Nodes[SID.index()].Alc_Info.Allocated_Vault;
	}
	return found;
}


// Compute produced/last-access times for data blocks from ordering.
// Returns the last time index.
size_t Dependency_Logger::Calculate_Data_Block_Timing(Data_Logger* DataL)
{
	if (!Mapped)
		return false;

	size_t timer(0);
	bool Advance(false);

	for (size_t dim0 = 0; dim0 < Ordering.size(); dim0++)
	{
		Advance = false;
		for (size_t dim1 = 0; dim1 < Ordering[dim0].size(); dim1++)
		{
			for (size_t dim2 = 0; dim2 < Ordering[dim0][dim1].size(); dim2++)
			{
				// This Ordering Node
				Ordering_Node Ord_Node = Ordering[dim0][dim1][dim2];
				// The alloction time should dvnce only t each level.
				// This behaviour originated from lack of loops and feedback
				// in each layars
				switch (Ord_Node.type)
				{
				case Sch_Activation:
				case Sch_MPDR:
					Advance = true;
					break;
				default:
					break;
				}

				// The Scheduling Index of this node
				SNID_t SNID = Ord_Node.ID;

				// Timing this Scheduling Node
				Time_The_Node_Please(DataL, SNID, timer);
			}
		}
		if (Advance)
			timer++;
	}

	Timed = true;
	Last_Time = timer;
	return timer;
}


// Allocate data blocks into vault address space.
bool Dependency_Logger::Allocte(Data_Logger* DataL)
{
	Calculate_Required_Spaces(DataL);
	need_to_copy = 0;

	if (!Timed)
		return false;

	size_t Ifst = 1;
	size_t Wfst = 1;
	size_t Ofst = 1;
	size_t Pfst = 1;
	size_t Efst = 1;
	
	std::cout << "\tMapping Input Data Blocks ..." << std::endl;
	maloc_DATA_IN (DataL, Ifst, Wfst);
	
	std::cout << "\tMapping Weight Blocks ..." << std::endl;
	maloc_WEIGHT  (DataL, Wfst, Ofst);
	
	std::cout << "\tMapping Output Data Blocks ..." << std::endl;
	maloc_DATA_OUT(DataL, Ofst, Pfst);
	
	std::cout << "\tMapping Partial Result Blocks ..." << std::endl;
	maloc_PSUM    (DataL, Pfst, Efst);


	Input_Location_Offset	= Ifst;
	Weght_Location_Offset	= Wfst;
	Outpt_Location_Offset	= Ofst;
	Psums_Location_Offset	= Pfst;
	END_Offset				= Efst;

	Allocated = true;
	return Allocated;
}


// Return required space counts and offsets for each data type.
bool Dependency_Logger::Get_Required_Spaces(size_t& Icnt, size_t& Wcnt, size_t& Ocnt, size_t& Pcnt, size_t& Ifst, size_t& Wfst, size_t& Ofst, size_t& Pfst, size_t& Efst) const
{
	Icnt = Input_Data_Block_Count;
	Wcnt = Weght_Data_Block_Count;
	Ocnt = Outpt_Data_Block_Count;
	Pcnt = Psums_Data_Block_Count;
	Ifst = Input_Location_Offset;
	Wfst = Weght_Location_Offset;
	Ofst = Outpt_Location_Offset;
	Pfst = Psums_Location_Offset;
	Efst = END_Offset;
	if (!Allocated)
		std::cout << "\tE: Allocation is NOT done yet." << std::endl;
	return Allocated;
}


// Export ordering info for code generation.
void Dependency_Logger::Get_Ordering_Info(std::vector<std::vector<std::vector<Ordering_Node>>>& Ord) const
{
	Ord = Ordering;
}


// Fetch consumed data block IDs for a scheduling node.
void Dependency_Logger::Get_Consumed_Data_Bloks(SNID_t SID, std::vector<DBID_t>& Cnsmd_DBID)
{
	Cnsmd_DBID = Nodes[SID.index()].Consumes_DBID;
}










//***************************************//
//*			Private Functions			*//
//***************************************//

// Duplicate a scheduling node, clear its dependency state, and attach to a new origin.
// Returns the new node ID.
SNID_t Dependency_Logger::Douplicate_and_cleanse_Scheduling_Node(SNID_t SNI, SNID_t NSNI)
{
	bool found(false);
	SNID_t NSNID = SNID_t::Null();

	if (SNI < Nodes.size())
	{
		found = true;
		NSNID = Create_Scheduling_Node({}, 0);
		Nodes[NSNID.index()] = Nodes[SNI.index()];
		Nodes[NSNID.index()].ID = NSNID;
		Nodes[NSNID.index()].Douplicate_of = SNI;

		bool last(false);
		std::vector<SNID_t> Conds = Nodes[SNI.index()].Condition_for;
		Nodes[SNI  .index()].Condition_for.clear();
		Nodes[NSNID.index()].Condition_for.clear();
		for (size_t Cnd_cntr = 0; Cnd_cntr < Conds.size(); Cnd_cntr++)
		{
			SNID_t cnod_node = Conds[Cnd_cntr];
			if (!last)
			{
				Nodes[SNI.index()].Condition_for.push_back(cnod_node);
				if (cnod_node == NSNI)
				{
					last = true;
					Nodes[SNI.index()].Condition_for.pop_back();
				}
			}
			
			if (last)
				Nodes[NSNID.index()].Condition_for.push_back(cnod_node);
		}



		for (size_t Dep_cntr = 0; Dep_cntr < Nodes[NSNID.index()].Dependencies.size(); Dep_cntr++)
			Nodes[Nodes[NSNID.index()].Dependencies[Dep_cntr].Dependent_ID.index()].Condition_for.push_back(NSNID);

		for (size_t Cnd_cntr = 0; Cnd_cntr < Nodes[NSNID.index()].Condition_for.size(); Cnd_cntr++)
			for (size_t Dep_cntr = 0; Dep_cntr < Nodes[	Nodes[NSNID.index()].Condition_for[Cnd_cntr].index()].Dependencies.size(); Dep_cntr++)
				if (Nodes[Nodes[NSNID.index()].Condition_for[Cnd_cntr].index()].Dependencies[Dep_cntr].Dependent_ID == SNI)
				{
					Nodes[Nodes[NSNID.index()].Condition_for[Cnd_cntr].index()].Dependencies[Dep_cntr].Dependent_ID = NSNID;
					break;
				}
	}

	return NSNID;
}


// Create the start sentinel scheduling node.
SNID_t Dependency_Logger::Create_Starting_Point_Scheduling_Node()
{
	Dependency_ID_cntr++;
	Scheduling_Node tmp;
	tmp.ID = Dependency_ID_cntr;
	tmp.type = Sch_Start;
	tmp.scheduled = false;
	tmp.picked = true;
	tmp.mapped = false;
	tmp.allocated = false;
	tmp.timmed = false;
	tmp.Dependencies.reserve(1);
	Nodes.push_back(tmp);
	return Dependency_ID_cntr;
}


// Create the end sentinel scheduling node.
SNID_t Dependency_Logger::Create_Ending_Point_Scheduling_Node()
{
	Dependency_ID_cntr++;
	Scheduling_Node tmp;
	tmp.ID = Dependency_ID_cntr;
	tmp.type = Sch_End;
	tmp.scheduled = false;
	tmp.picked = true;
	tmp.mapped = false;
	tmp.allocated = false;
	tmp.timmed = false;
	tmp.Dependencies.reserve(1);
	Nodes.push_back(tmp);
	return Dependency_ID_cntr;
}


// Compute spacing between execution levels for scheduled nodes.
bool Dependency_Logger::Calculation_Scheduling_Node_Spaces()
{
	if (!Scheduled)
		return false;

	for (size_t node = 0; node < Scheduled_ID_list[0].size(); node++)
	{
		Scheduling_info node_Sch_Info;
		Get_Scheduling_Info(Scheduled_ID_list[0][node], node_Sch_Info);
		node_Sch_Info.Execution_Level_Space_Before = 0;
		Set_Scheduling_Info(Scheduled_ID_list[0][node], node_Sch_Info);
	}

	for (size_t lvl = 1; lvl < Scheduled_ID_list.size(); lvl++)
	{
		for (size_t node = 0; node < Scheduled_ID_list[lvl].size(); node++)
		{
			Scheduling_Node SNInfo;
			Fetch_Scheduling_Node_Info(Scheduled_ID_list[lvl][node], SNInfo);

			for (size_t dep = 0; dep < SNInfo.Dependencies.size(); dep++)
			{
				
				Scheduling_info prev_node_Sch_Info;
				Get_Scheduling_Info(SNInfo.Dependencies[dep].Dependent_ID, prev_node_Sch_Info);
				unsigned int diff;
				diff = SNInfo.Sch_Info.Execution_Level - prev_node_Sch_Info.Execution_Level - 1;
				SNInfo.Sch_Info.Execution_Level_Space_Before = std::min(SNInfo.Sch_Info.Execution_Level_Space_Before, diff);
				prev_node_Sch_Info.Execution_Level_Space_After = std::min(prev_node_Sch_Info.Execution_Level_Space_After, diff);
				Set_Scheduling_Info(SNInfo.Dependencies[dep].Dependent_ID, prev_node_Sch_Info);
				
			}

			Set_Scheduling_Info(SNInfo.ID, SNInfo.Sch_Info);
		}
	}

	size_t end = Scheduled_ID_list.size() - 1;
	for (size_t node = 0; node < Scheduled_ID_list[end].size(); node++)
	{
		Scheduling_info node_Sch_Info;
		Get_Scheduling_Info(Scheduled_ID_list[end][node], node_Sch_Info);
		node_Sch_Info.Execution_Level_Space_After = 0;
		Set_Scheduling_Info(Scheduled_ID_list[end][node], node_Sch_Info);
	}

	Spaced = true;
	return true;
}


// Clear spacing info for all scheduling nodes.
bool Dependency_Logger::Clear_Scheduling_Node_Spacing_Info()
{
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		Nodes[i].Sch_Info.Execution_Level_Space_Before = -1;
		Nodes[i].Sch_Info.Execution_Level_Space_After = -1;
	}

	Spaced = false;
	return true;
}


// Build reverse edges (Condition_for) for all nodes and mark Compiled.
void Dependency_Logger::Compile_Schedule_Nodes()
{
	for (size_t idx = 0; idx < Nodes.size(); idx++)
	{
		for (size_t dep_idx = 0; dep_idx < Nodes[idx].Dependencies.size(); dep_idx++)
		{
			
			SNID_t nod = Nodes[idx].Dependencies[dep_idx].Dependent_ID;
			Nodes[nod.index()].Condition_for.push_back(Nodes[idx].ID);
		}
	}
	Compiled = true;
}


// Check if a scheduling node is satisfied (all dependencies ready).
bool Dependency_Logger::Is_Scheduling_Node_Satisfied(Data_Logger* DL, Scheduling_Node& SN)
{
	bool Sat(true);
	if (SN.scheduled)
		return false;
	if (SN.type == Sch_Null)
		return false;
	for (size_t i = 0; i < SN.Dependencies.size(); i++)
	{
		if (SN.Dependencies[i].satisfied)
			continue;
		else
		{
			Scheduling_Node SN_info;
			Fetch_Scheduling_Node_Info(SN.Dependencies[i].Dependent_ID, SN_info);
			SN.Dependencies[i].satisfied = SN_info.scheduled;
			Sat = Sat && SN_info.scheduled;
		}
		if (!Sat)
			break;
	}
	return Sat;
}


// Check whether two Sch_Process nodes share the same weight blocks.
bool Dependency_Logger::Is_PE_Weights_Similar(SNID_t SID, SNID_t BLNode_Idx)
{
	// the first 9 dependencies point to Weights 
	// Other 3 are for the input. 
	int sim_cntr(0);
	for (size_t from = 0; from < 9; from++)
	{
		for (size_t to = 0; to < 9; to++)
		{
			if (!(Nodes[SID.index()].Dependencies[from].Dependent_ID == Nodes[BLNode_Idx.index()].Dependencies[to].Dependent_ID))
				continue;
			sim_cntr++;
			break;
		}
	}
	if (sim_cntr == 9)
		return true;
	return false;
}


// Check whether inputs of SID follow sequentially after BLNode_Idx inputs.
bool Dependency_Logger::Is_PE_Inputs_Next_Sequencing(SNID_t SID, SNID_t BLNode_Idx)
{
	// the first 9 dependencies point to Weights 
	// Other 3 are for the input. 
	if (!(Nodes[SID.index()].Dependencies[10].Dependent_ID == Nodes[BLNode_Idx.index()].Dependencies[9].Dependent_ID))
		return false;
	if (!(Nodes[SID.index()].Dependencies[11].Dependent_ID == Nodes[BLNode_Idx.index()].Dependencies[10].Dependent_ID))
		return false;
	return true;
}


// Calculate total and per-vault coverage of PE assignments.
void Dependency_Logger::Calculate_PE_Coverage(PE_Node PEs[4][16], size_t& total_Cover, size_t& vault_Cover)
{
	total_Cover = 0;
	for (unsigned int i = 0; i < 4; i++)
		for (unsigned int j = 0; j < 16; j++)
			total_Cover += (PEs[i][j].active);
	
	vault_Cover = 0;
	for (unsigned int j = 0; j < 16; j++)
		for (unsigned int i = 0; i < 4; i++)
			if (PEs[i][j].active)
			{
				vault_Cover++;
				break;
			}
}


// Reset cached Previous_PE_Info records.
void Dependency_Logger::Clear_Previous_PE_Info()
{
	Previous_PE_Info_valid = false;
	for (size_t pln = 0; pln < 4; pln++)
		for (size_t vlt = 0; vlt < 16; vlt++)
			Previous_PE_Info[pln][vlt] = PE_Node_NULL;
}


// TODO
// Optimize execution block ordering for a level.
void Dependency_Logger::Optimize_PE_Execution_Block(Data_Logger* DataL, PE_Node PEs[4][16], size_t lvl)
{
	for (size_t pln = 0; pln < 4; pln++)
		for (size_t vlt = 0; vlt < 16; vlt++)
			Previous_PE_Info[pln][vlt] = PEs[pln][vlt];
	Previous_PE_Info_valid = true;
	Generate_Execution_Block(DataL, lvl);
}


// Generate execution ordering nodes for a given level.
void Dependency_Logger::Generate_Execution_Block(Data_Logger* DataL, size_t lvl)
{
	size_t block_idx = Ordering[level].size();
	if (Previous_PE_Info_valid)
	{
		Ordering[level].push_back({});
		Ordering[level][block_idx].reserve(Ava_Planes * Ava_Vaults * 7 * Previous_PE_Info[0][0].Sequence.size());
		for (size_t pln = 0; pln < 4; pln++)
			for (size_t vlt = 0; vlt < 16; vlt++)
				if (Previous_PE_Info[pln][vlt].active)
					for (size_t node = 0; node < Previous_PE_Info[pln][vlt].Sequence.size(); node++)
					{
						SNID_t this_node_idx = Previous_PE_Info[pln][vlt].Sequence[node];
						Map_Node(DataL, this_node_idx, block_idx, lvl, pln, vlt, true, 0);
					}
	}
	Previous_PE_Info_valid = false;
	Clear_Previous_PE_Info();
}


// Generate peripheral ordering nodes for activation/MPDR dependencies.
void Dependency_Logger::Generate_Peripheral_Block(Data_Logger* DataL, SNID_t SID, size_t lvl)
{
	Ordering[level].push_back({});
	size_t block_idx = Ordering[level].size() - 1;
	DBID_t CDBID = Nodes[SID.index()].Consumes_DBID[0];
	SNID_t PSNID = DataL->Get_Producer_ID_of_DBID(CDBID);

	if (Nodes[SID.index()].type == Sch_Activation)
	{
		CDBID = Nodes[PSNID.index()].Consumes_DBID[0];
		PSNID = DataL->Get_Producer_ID_of_DBID(CDBID);
	}
	
	size_t plane = Nodes[PSNID.index()].Alc_Info.Allocated_Plane;
	size_t vault = Nodes[PSNID.index()].Alc_Info.Allocated_Vault;

	Map_Node(DataL, SID, block_idx, lvl, plane, vault, true, 0);
}


// Map a level
void Dependency_Logger::Map_level(std::vector<SNID_t>& nodes_ID, std::vector<std::vector<Mapping_Result>>& res)
{
	constexpr uint16_t PLANES = 4;
	constexpr uint16_t VAULTS = 16;

	res.clear();

	// round-robin plane per vault
	uint16_t next_plane[VAULTS] = { 0 };

	// cache execution group -> assigned PE
	std::unordered_map<ExecKey, std::pair<uint16_t, uint16_t>, ExecHasher> exec_to_pe;

	for (const auto& node : nodes_ID)
	{
		Scheduling_Node The_node = Nodes[node.index()];

		if (The_node.type == Sch_Null)
			continue;

		const auto& cli = The_node.CLI;

		WeightKey wkey{ cli.Kernel_size, cli.Channel_size,
					   cli.FiltH_Size, cli.FiltW_Size };

		size_t vault = WeightHasher{}(wkey) % VAULTS;
		uint16_t pe_index = static_cast<uint16_t>(vault);
		 
		ExecKey ekey{ cli.Batch_size, cli.Kernel_size,
					 cli.Channel_size, cli.Width_size,
					 cli.FiltH_Size, cli.FiltW_Size };

		uint16_t plane;

		auto it = exec_to_pe.find(ekey);
		if (it == exec_to_pe.end())
		{
			plane = next_plane[vault]++;
			exec_to_pe.emplace(ekey, std::make_pair(plane, pe_index));
		}
		else
		{
			plane = it->second.first;
		}

		uint16_t which_plane = plane / PLANES;
		if (which_plane >= res.size())
			res.push_back({});

		Mapping_Result mr({});
		mr.node = The_node.ID;
		mr.pe_index = pe_index;
		mr.plane = plane % PLANES;
		mr.vault = static_cast<uint16_t>(vault);
		
		res[which_plane].push_back(mr);
	}
}


// Map a single scheduling node to plane/vault/node and update ordering.
void Dependency_Logger::Map_Node(Data_Logger* DataL, SNID_t node_idx, size_t block, size_t lvl, size_t plane, size_t vlt, bool forced, size_t counter)
{
	bool nxt_forced = false;
	Ordering_Node order({});
	order.ID = node_idx;
	order.Plane = plane;
	order.Vault = vlt;
	order.Rsrv1 = counter;
	order.type = Nodes[node_idx.index()].type;
	order.Generated = false;
	

	if (lvl < 0)
		return;
	if (!forced)
		if (Nodes[node_idx.index()].mapped)
			return;
	switch (Nodes[node_idx.index()].type)
	{
	case Sch_Null:
		break;
	case Sch_Start:
		order.Plane = 0;
		order.Vault = 0;
		order.Rsrv1 = 0;
		//order.Original = node_idx;
		Ordering[level][block].push_back(order);
		Nodes[node_idx.index()].Alc_Info.Allocated_Target	= TMT_USER;
		Nodes[node_idx.index()].Alc_Info.Allocated_Plane	= 0;
		Nodes[node_idx.index()].Alc_Info.Allocated_Vault	= 0;
		break;
	case Sch_Input_Layer:
		for (size_t dc = 0; dc < Nodes[node_idx.index()].Dependencies.size(); dc++)
			Map_Node(DataL, Nodes[node_idx.index()].Dependencies[dc].Dependent_ID, block, lvl - 1, plane, vlt, nxt_forced, dc);
		Nodes[node_idx.index()].Alc_Info.Allocated_Target	= TMT_Null;
		Nodes[node_idx.index()].Alc_Info.Allocated_Plane	= plane;
		Nodes[node_idx.index()].Alc_Info.Allocated_Vault	= vlt;
		break;
	case Sch_CNN_Input:
		for (size_t dc = 0; dc < Nodes[node_idx.index()].Dependencies.size(); dc++)
			Map_Node(DataL, Nodes[node_idx.index()].Dependencies[dc].Dependent_ID, block, lvl - 1, plane, vlt, nxt_forced, dc);
		Nodes[node_idx.index()].Alc_Info.Allocated_Target = TMT_USER;
		Nodes[node_idx.index()].Alc_Info.Allocated_Plane = plane;
		Nodes[node_idx.index()].Alc_Info.Allocated_Vault = vlt;
		break;
	case Sch_CNN_Weight:
		for (size_t dc = 0; dc < Nodes[node_idx.index()].Dependencies.size(); dc++)
			Map_Node(DataL, Nodes[node_idx.index()].Dependencies[dc].Dependent_ID, block, lvl - 1, plane, vlt, nxt_forced, dc);
		Nodes[node_idx.index()].Alc_Info.Allocated_Target = TMT_USER;
		Nodes[node_idx.index()].Alc_Info.Allocated_Plane = plane;
		Nodes[node_idx.index()].Alc_Info.Allocated_Vault = vlt;
		break;
	case Sch_Process:
	case Sch_Accumulate:
	case Sch_Activation:
	case Sch_MPDR:
	case Sch_End:
		if (Nodes[node_idx.index()].type == Sch_Process)
		{
			if (lvl != level)
			{
				std::cout << "\tE: A previous process node has bot been Mapped. Breaking the Mapping process" << std::endl;
				std::cout << "\t\tCalling Mapping Process on level #" << level << ", This Level #" << lvl << std::endl;
				break;
			}
			nxt_forced = true;
		}
		for (size_t dc = 0; dc < Nodes[node_idx.index()].Dependencies.size(); dc++)
			Map_Node(DataL, Nodes[node_idx.index()].Dependencies[dc].Dependent_ID, block, lvl - 1, plane, vlt, nxt_forced, dc);
		Ordering[level][block].push_back(order);
		Nodes[node_idx.index()].Alc_Info.Allocated_Target	= TMT_USER;
		Nodes[node_idx.index()].Alc_Info.Allocated_Plane	= plane;
		Nodes[node_idx.index()].Alc_Info.Allocated_Vault	= vlt;
		break;
	}
	Mark_as_Mapped(node_idx);
}


// Mark a scheduling node as mapped.
void Dependency_Logger::Mark_as_Mapped(SNID_t SNI)
{
	Nodes[SNI.index()].mapped = true;
}


// Assign produced/accessed times for a node and its data blocks.
void Dependency_Logger::Time_The_Node_Please(Data_Logger* DataL, SNID_t SNI, size_t timer)
{
	if (Nodes[SNI.index()].timmed)
		return;

	// The Data Block That this node produced 
	DBID_t DBID = Nodes[SNI.index()].Produced_DBID;
	// If the Data Block index is valid, set the production time 
	if (!(DBID == DBID_t::Null()))
		DataL->Set_Produced_Time(DBID, timer);
	// Now, we should set the consumption time for the consumed block
	for (size_t consumers = 0; consumers < Nodes[SNI.index()].Consumes_DBID.size(); consumers++)
		DataL->Set_Accessed_Time(Nodes[SNI.index()].Consumes_DBID[consumers], timer);

	// Now, time its dependencies
	for (size_t idx = 0; idx < Nodes[SNI.index()].Dependencies.size(); idx++)
		Time_The_Node_Please(DataL, Nodes[SNI.index()].Dependencies[idx].Dependent_ID, timer);

	// marking this node as timmed
	Mark_as_Timmed(SNI);
}


// Mark a scheduling node as timed.
void Dependency_Logger::Mark_as_Timmed(SNID_t SNI)
{
	Nodes[SNI.index()].timmed = true;
}


// Find the first scheduling node in a level that depends on dep.
bool Dependency_Logger::Fetch_first_Scheduling_Nodes_with_Dependency(SNID_t& NID, Dependency dep, size_t lvl)
{
	bool found(false);
	if (Compiled)
	{
		NID = Nodes[dep.Dependent_ID.index()].Condition_for[0];
		found = true;
	}
	else
	{
		tmp_cntr++;
		size_t i, j;
		for (i = 0; i < Scheduled_ID_list[lvl].size(); i++)
		{
			SNID_t SNode = Scheduled_ID_list[lvl][i];
			for (j = 0; j < Nodes[SNode.index()].Dependencies.size(); j++)
			{
				if (Nodes[SNode.index()].Dependencies[j].Dependent_ID == dep.Dependent_ID)
				{
					NID = SNode;
					found = true;
					break;
				}
			}
			if (found)
				break;
		}
	}
	return found;
}


// Compute required counts and offsets for input/weight/output/psum blocks.
void Dependency_Logger::Calculate_Required_Spaces(Data_Logger* DataL)
{
	size_t inps(0);
	size_t wgts(0);
	size_t outs(0);
	size_t psms(0);

	for (DBID_t DBI = DBID_t::Null(); DBI < DataL->size(); DBI++)
	{
		Data_Block DB;
		DataL->Get_Data_Block_Info(DBI, DB);
		switch (DB.DBT)
		{
		case DBT_IZero:
		case DBT_DATA_IN:
			inps++;
			break;
		case DBT_WGT:
			wgts++;
			break;
		case DBT_DATA_OUT:
			outs++;
			break;
		case DBT_PZero:
		case DBT_PSUM:
			psms++;
			break;
		default:
		//	* DBT_Null:
		//	* DBT_RSRV:
		//	* DBT_INTERNAL:
			break;
			break;
		}
	}

	Input_Data_Block_Count = inps;
	Weght_Data_Block_Count = wgts;
	Outpt_Data_Block_Count = outs;
	Psums_Data_Block_Count = psms;
}


// Allocate input (DATA_IN/IZero) blocks with sequential addresses.
void Dependency_Logger::maloc_DATA_IN (Data_Logger* DataL, size_t Offset, size_t& Next_Offset)
{
	size_t SoBI[Ava_Vaults] = {};
	for (size_t i = 0; i < Ava_Vaults; i++)
		SoBI[i] = Offset;
	
	for (size_t dim0 = 0; dim0 < Ordering.size(); dim0++)
	{
		for (size_t dim1 = 0; dim1 < Ordering[dim0].size(); dim1++)
		{
			for (size_t dim2 = 0; dim2 < Ordering[dim0][dim1].size(); dim2++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[dim0][dim1][dim2];
				// pass if the type of ordering node is not "Sch_CNN_Input"
				if (Ord_Node.type != Sch_Process)
					continue;
				// Some other initialization 
				SNID_t Order_SNID = Ord_Node.ID;
				// now it's time to allocte all of its dependencies
				size_t vlt = Nodes[Order_SNID.index()].Alc_Info.Allocated_Vault;
				//std::cout << "Vlt: " << vlt << "\t(N" << Order_SNID << ")\t";
				for (size_t Dep_Cntr = 0; Dep_Cntr < Nodes[Order_SNID.index()].Dependencies.size(); Dep_Cntr++)
					maloc_DATA_IN_Node(DataL, vlt, SoBI, Order_SNID, Nodes[Order_SNID.index()].Dependencies[Dep_Cntr], false);
				//std::cout << std::endl;
			}
		}
	}
	
	Next_Offset = 0;
	for (size_t i = 0; i < Ava_Vaults; i++)
		Next_Offset = std::max(Next_Offset, SoBI[i]);
	Next_Offset++;
	Next_Offset = ((Next_Offset / 8) * 8) + 8;
}


// Allocate weight blocks with sequential addresses.
void Dependency_Logger::maloc_WEIGHT  (Data_Logger* DataL, size_t Offset, size_t& Next_Offset)
{
	size_t SoBI[Ava_Vaults] = {};
	for (size_t i = 0; i < Ava_Vaults; i++)
		SoBI[i] = Offset;

	for (size_t dim0 = 0; dim0 < Ordering.size(); dim0++)
	{
		for (size_t dim1 = 0; dim1 < Ordering[dim0].size(); dim1++)
		{
			for (size_t dim2 = 0; dim2 < Ordering[dim0][dim1].size(); dim2++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[dim0][dim1][dim2];
				// pass if the type of ordering node is not "Sch_Process"
				if (Ord_Node.type != Sch_Process)
					continue;
				// Some other initialization 
				SNID_t Order_SNID = Ord_Node.ID;
				// now it's time to allocte all of its dependencies
				size_t vlt = Nodes[Order_SNID.index()].Alc_Info.Allocated_Vault;
				//std::cout << "Vlt: " << vlt << "\t(N" << Order_SNID << ")\t";
				for (size_t Dep_Cntr = 0; Dep_Cntr < Nodes[Order_SNID.index()].Dependencies.size(); Dep_Cntr++)
					maloc_WEIGHT_Node(DataL, vlt, SoBI, Order_SNID, Nodes[Order_SNID.index()].Dependencies[Dep_Cntr]);
				//std::cout << std::endl;
			}
		}
	}
	
	Next_Offset = 0;
	for (size_t i = 0; i < Ava_Vaults; i++)
		Next_Offset = std::max(Next_Offset, SoBI[i]);
	Next_Offset++;
	Next_Offset = ((Next_Offset / 8) * 8) + 8;
}


// Allocate output blocks with sequential addresses.
void Dependency_Logger::maloc_DATA_OUT(Data_Logger* DataL, size_t Offset, size_t& Next_Offset)
{
	size_t SoBI[Ava_Vaults] = {};
	for (size_t i = 0; i < Ava_Vaults; i++)
		SoBI[i] = Offset;
	
	for (size_t dim0 = 0; dim0 < Ordering.size(); dim0++)
	{
		for (size_t dim1 = 0; dim1 < Ordering[dim0].size(); dim1++)
		{
			for (size_t dim2 = 0; dim2 < Ordering[dim0][dim1].size(); dim2++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[dim0][dim1][dim2];
				// pass if the type of ordering node is not "Sch_CNN_Weight"
				if ((Ord_Node.type != Sch_MPDR) && (Ord_Node.type != Sch_Activation))
					continue;
				// Some other initialization 
				SNID_t Order_SNID = Ord_Node.ID;
				DBID_t Prdcd_DBID = Nodes[Order_SNID.index()].Produced_DBID;
				// pass if the type of Produced date block is not Output data
				if (DataL->Get_Type_of_DBID(Prdcd_DBID) != DBT_DATA_OUT)
					continue;
				// allocating it, because each data Block produced only once
				// it it allocated before, generating error
				if (DataL->Is_Allocated(Prdcd_DBID))
				{
					std::cout << "\tE: Error occured while allocatign Output Data Blocks" << std::endl;
					std::cout << "\t\tIt has been allocated before. What the Fuck? " << std::endl << std::endl;
				}
				// its allocation part
				size_t vlt = Nodes[Order_SNID.index()].Alc_Info.Allocated_Vault;
				DataL->Set_Vault_of_DBID(Prdcd_DBID, vlt);
				DataL->Set_Starting_Index_of_Data_Block(Prdcd_DBID, SoBI[vlt]++);
				DataL->Mark_as_Allocated(Prdcd_DBID);
			}
		}
	}
	
	Next_Offset = 0;
	for (size_t i = 0; i < Ava_Vaults; i++)
		Next_Offset = std::max(Next_Offset, SoBI[i]);
	Next_Offset++;
	Next_Offset = ((Next_Offset / 8) * 8) + 8;
}


// Allocate partial-sum blocks with sequential addresses.
void Dependency_Logger::maloc_PSUM    (Data_Logger* DataL, size_t Offset, size_t& Next_Offset)
{
	/*
	Programmer's Log:
		the "A" key in my laptop's keyboard fell when I tried to debug this motehr fucker, Fuckkkkkkkkkkkkk
		now, I have 4 missing key from my keyboard, I think they srarted to migrate to somwhere else, a promiss land, 
		where each key can be pressed without stress, fuck, 
	*/

	// Simple Allocator, 
	
	size_t								SoBI[Ava_Vaults] = {};
	
	// Simple, plane
	for (size_t i = 0; i < Ava_Vaults; i++)
		SoBI[i] = Offset;

	// 1- Allocate its inputs
	std::cout << "\tConstructing Data Block Usage Matrix for: ";
	for (size_t lvl = 0; lvl < Ordering.size(); lvl++)
	{
	
		std::cout << std::endl << "\t\tLevel: " << std::setw(3) << lvl + 1 << "/" << std::setw(3) << Ordering.size() << ",\t\t";
	
		size_t ival = (Ordering[lvl].size() / 100) + 1;
		for (size_t bline = 0; bline < Ordering[lvl].size(); bline++)
		{
			if ((bline % ival) == 0)
			{
				std::cout << "\033[2K\r"	<< std::flush;
				std::cout << "\t\tLevel: "	<< std::setw(3) << lvl + 1		<< "/" << std::setw(3) << Ordering.size() << ",\t\t";
				std::cout << "Baseline: "	<< std::setw(6) << bline + 1	<< "/" << std::setw(6) << Ordering[lvl].size() << ",\t\t";
			}
	
			for (size_t node = 0; node < Ordering[lvl][bline].size(); node++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[lvl][bline][node];
				size_t vlt = Ord_Node.Vault;
				// pass if the type of ordering node is not "Sch_Process"
				if ((Ord_Node.type != Sch_Process) )
					continue;
				
				// it should take a step back to Sch_Process's dependencies.
				for (size_t Dep_cntr = 0; Dep_cntr < Nodes[Ord_Node.ID.index()].Dependencies.size(); Dep_cntr++)
				{
					// Some other initialization 
					SNID_t Order_SNID = Nodes[Ord_Node.ID.index()].Dependencies[Dep_cntr].Dependent_ID;
					// Now we can have the same procedure as Sch_MPDR, for each consumed data do the same:
					for (size_t Cnsmd_Cntr = 0; Cnsmd_Cntr < Nodes[Order_SNID.index()].Consumes_DBID.size(); Cnsmd_Cntr++)
					{
						// Initializing the Cnsmd_DBID 
						DBID_t Cnsmd_DBID = Nodes[Order_SNID.index()].Consumes_DBID[Cnsmd_Cntr];
						Data_Block_Types Cnsmd_DBID_type = DataL->Get_Type_of_DBID(Cnsmd_DBID);
						// pass if the type of consumed date block is not Partial Result or Partial Zero
						if ((Cnsmd_DBID_type != DBT_PZero) && (Cnsmd_DBID_type != DBT_PSUM))
							continue;

						if (!DataL->Is_Allocated(Cnsmd_DBID))
						{
							// Now we hit the million dolor Question, "WHAT SHOULD I DO NEXT?"
							// Get its info for no reason
							Data_Block  DB;
							DataL->Get_Data_Block_Info(Cnsmd_DBID, DB);
							// allocate it 
							DB.SoBI = SoBI[vlt]++;
							DB.Vault = (unsigned int)vlt;
							DB.alocated = true;
							// writeign it back
							DataL->Change_Data_Block_info(Cnsmd_DBID, DB);
						}

						// if it does allocated before, 
						else
						{
							// if the type of consumed date block is Partial Zero
							if (Cnsmd_DBID_type == DBT_PZero)
							{

								// 1- Douplicating the data Block 
								Cnsmd_DBID = DataL->Duplicate_Data_Block(Cnsmd_DBID);
								// 2- Douplicating the Scheduling Node
								Order_SNID = Douplicate_and_cleanse_Scheduling_Node(Order_SNID, Ord_Node.ID);
								// 3- Changing the consumed data of that node to the new data block 
								Nodes[Order_SNID.index()].Consumes_DBID[0] = Cnsmd_DBID;
								// 4- Setting its vault to the Node vault 
								DataL->Set_Consumers_ID_of_DBID(Cnsmd_DBID, Order_SNID);
								// 5- Changing the strting Index of the new Data
								// Get its info for no reason
								Data_Block  DB;
								DataL->Get_Data_Block_Info(Cnsmd_DBID, DB);
								// allocate it 
								DB.SoBI = SoBI[vlt]++;
								DB.Vault = (unsigned int)vlt;
								DB.alocated = true;
								// writeign it back
								DataL->Change_Data_Block_info(Cnsmd_DBID, DB);
							}
						}

					}
				}
			}
		}
	
		std::cout << "\033[2K\r"	<< std::flush;
		std::cout << "\t\tLevel: "	<< std::setw(3) << lvl + 1 << "/" << std::setw(3) << Ordering.size() << ", \tDone!";
	}
	std::cout << std::endl;
	
	// 2- Allocate its output
	for (size_t lvl = 0; lvl < Ordering.size(); lvl++)
	{
		for (size_t bline = 0; bline < Ordering[lvl].size(); bline++)
		{
			for (size_t node = 0; node < Ordering[lvl][bline].size(); node++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[lvl][bline][node];
				size_t vlt = Ord_Node.Vault;
				// pass if the type of ordering node is not "Sch_Process"
				if ((Ord_Node.type != Sch_Process))
					continue;

				// now we should move forward
				SNID_t ACC_node = Nodes[Ord_Node.ID.index()].Condition_for[0];
				SNID_t ACT_node = Nodes[ACC_node.index()].Condition_for[0];
				SNID_t NXT_node = Nodes[ACT_node.index()].Condition_for[0];

				if (Nodes[NXT_node.index()].type != Sch_MPDR)
					continue;

				if (Nodes[ACT_node.index()].allocated)
					continue;

				Data_Block  DB;
				DBID_t Cnsmd_DBID = Nodes[ACT_node.index()].Produced_DBID;
				DataL->Get_Data_Block_Info(Cnsmd_DBID, DB);
				// allocate it 
				DB.SoBI = SoBI[vlt]++;
				DB.Vault = (unsigned int)vlt;
				DB.alocated = true;
				// writeign it back
				DataL->Change_Data_Block_info(Cnsmd_DBID, DB);
				// mark as allocated
				Nodes[ACT_node.index()].allocated = true;
			}
		}
	}

	Next_Offset = 0;
	for (size_t i = 0; i < Ava_Vaults; i++)
		Next_Offset = std::max(Next_Offset, SoBI[i]);
	Next_Offset++;
	Next_Offset = ((Next_Offset / 8) * 8) + 8;
	

	

	// Allocator with save space mode 
	/*
	// TODO: adding n
	// The Far More Complex algorithm
	size_t SoBI[Ava_Vaults] = {};
	std::vector<std::vector<DBID_t>>	DBIDUM[Ava_Vaults]; // Data Block ID Usage Mtrix
	std::vector<size_t>					Time_Axes[Ava_Vaults];
	
	// Initializing 
	std::cout << "\tInitializing Allocation Engine ..." << std::endl;
	for (size_t vlt = 0; vlt < Ava_Vaults; vlt++)
	{
		DBIDUM[vlt]   .clear();
		Time_Axes[vlt].clear();
		for (size_t time = 0; time <= Last_Time; time++)
		{
			Addin_New_Time(DBIDUM[vlt], Time_Axes[vlt], time);
			Reserve_Rows(DBIDUM[vlt], Psums_Data_Block_Count/ Ava_Vaults);
		}
	}
	
	//	Construcing the Data Block ID Usage Mtrix
	std::cout << "\tConstructing Data Block Usage Matrix for: ";
	for (size_t lvl = 0; lvl < Ordering.size(); lvl++)
	{
		if (Ordering[lvl].size() > 0)
			std::cout << std::endl << "\t\tLevel: " << std::setw(3) << lvl +1 << "/" << std::setw(3) << Ordering.size() << ",\t\t";
		
		size_t ival = (Ordering[lvl].size() / 100) + 1;
		for (size_t bline = 0; bline < Ordering[lvl].size(); bline++)
		{
			if ((bline % ival) == 0)
			{
				std::cout << "\033[2K\r" << std::flush;
				std::cout << "\t\tLevel: "	<< std::setw(3) << lvl + 1		<< "/" << std::setw(3) << Ordering.size() << ",\t\t";
				std::cout << "Baseline: "	<< std::setw(6) << bline + 1	<< "/" << std::setw(6) << Ordering[lvl].size() << ",\t\t";
			}

			// inputs
			for (size_t node = 0; node < Ordering[lvl][bline].size(); node++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[lvl][bline][node];
				size_t vlt = Ord_Node.Vault;
				
				// if the layer is Sch_Process
				if (Ord_Node.type == Sch_Process)
				{
					// it should take a step back to Sch_Process's dependencies.
					for (size_t Dep_cntr = 0; Dep_cntr < Nodes[Ord_Node.ID.index()].Dependencies.size(); Dep_cntr++)
					{
						// Some other initialization 
						SNID_t Order_SNID = Nodes[Ord_Node.ID.index()].Dependencies[Dep_cntr].Dependent_ID;
						// Now we can have the same procedure as Sch_MPDR, for each consumed data do the same:
						for (size_t Cnsmd_Cntr = 0; Cnsmd_Cntr < Nodes[Order_SNID.index()].Consumes_DBID.size(); Cnsmd_Cntr++)
						{
							// Initializing the Cnsmd_DBID 
							DBID_t Cnsmd_DBID = Nodes[Order_SNID.index()].Consumes_DBID[Cnsmd_Cntr];
							//if (Cnsmd_DBID.index() == 21653)
							//	std::cout << "Here is 21653" << std::endl;
							Data_Block_Types Cnsmd_DBID_type = DataL->Get_Type_of_DBID(Cnsmd_DBID);
							// if the type of consumed date block is Partial Result
							// Pass if it already allocated 
							if (!DataL->Is_Allocated(Cnsmd_DBID))
							{
								// Now we hit the million dolor Question, "WHAT SHOULD I DO NEXT?"
								// Get its info for no reason
								Data_Block  DB;
								DataL->Get_Data_Block_Info(Cnsmd_DBID, DB);
								DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
								// Adding this Block ID to the matrix
								Add_Data_Block_to_Matrix(DBIDUM[vlt], Time_Axes[vlt], DB);
								// Maybe it works, Maybe not, Who chairs?, Nothing mattress.
								DataL->Mark_as_Allocated(Cnsmd_DBID);
							}
							// if it does allocated before, 
							else
							{
								// if the type of consumed date block is Partial Zero
								if (Cnsmd_DBID_type == DBT_PZero)
								{

									// 1- Douplicating the data Block 
									Cnsmd_DBID = DataL->Duplicate_Data_Block(Cnsmd_DBID);
									// 2- Douplicating the Scheduling Node
									Order_SNID = Douplicate_and_cleanse_Scheduling_Node(Order_SNID, Ord_Node.ID);
									// 3- Changing the consumed data of that node to the new data block 
									Nodes[Order_SNID.index()].Consumes_DBID[0] = Cnsmd_DBID;
									// 4- Setting its vault to the Node vault 
									DataL->Set_Consumers_ID_of_DBID(Cnsmd_DBID, Order_SNID);
									// 5- Changing the strting Index of the new Data
									Data_Block  DB;
									DataL->Get_Data_Block_Info(Cnsmd_DBID, DB);
									DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
									// 6- allocation
									// Adding this Block ID to the matrix
									Add_Data_Block_to_Matrix(DBIDUM[vlt], Time_Axes[vlt], DB);
									DataL->Mark_as_Allocated(Cnsmd_DBID);
								}
							}
						}
					}
				}
			}
			
			// outputs
			for (size_t node = 0; node < Ordering[lvl][bline].size(); node++)
			{
				// some initialization
				Ordering_Node Ord_Node = Ordering[lvl][bline][node];
				size_t vlt = Ord_Node.Vault;

				// if the layer is Sch_Process
				if (Ord_Node.type == Sch_Process)
				{
					
					SNID_t ACC_node = Nodes[Ord_Node.ID.index()].Condition_for[0];
					SNID_t ACT_node = Nodes[ACC_node.index()].Condition_for[0];
					SNID_t NXT_node = Nodes[ACT_node.index()].Condition_for[0];

					if (Nodes[NXT_node.index()].type != Sch_MPDR)
						continue;

					if (Nodes[ACT_node.index()].allocated)
						continue;
										
					// Now we hit the million dolor Question, "WHAT SHOULD I DO NEXT?"
					// Get its info for no reason
					Data_Block  DB;
					DBID_t Cnsmd_DBID = Nodes[ACT_node.index()].Produced_DBID;
					DataL->Get_Data_Block_Info(Cnsmd_DBID, DB);
					DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
					// Adding this Block ID to the matrix
					Add_Data_Block_to_Matrix(DBIDUM[vlt], Time_Axes[vlt], DB);
					// Maybe it works, Maybe not, Who chairs?, Nothing mattress.
					DataL->Mark_as_Allocated(Cnsmd_DBID);
					Nodes[ACT_node.index()].allocated = true;
				}
			}
		}
	}
	std::cout << std::endl;
	// Working Like a charm at its first try 
	
	// Optimizing   the Data Block ID Usage Mtrix
	std::cout << "\tOptimizing Data Block Usage Matrix ...";
	for (size_t vlt = 0; vlt < Ava_Vaults; vlt++)
		Optimize_Allocation_Matrix(DBIDUM[vlt], Time_Axes[vlt]);
	std::cout << "\b\b\bDone!" << std::endl;
	
	// setting Starting Address
	Clear_Allocation_Flag(DataL);
	std::cout << "\tCalculating Starting Address of each Blocks." << std::endl;
	for (size_t vlt = 0; vlt < Ava_Vaults; vlt++)
		SoBI[vlt] = Setting_Start_Address_of_Allocation_Matrix(DBIDUM[vlt], DataL, Offset);

	Next_Offset = 0;
	for (size_t i = 0; i < Ava_Vaults; i++)
		Next_Offset = std::max(Next_Offset, SoBI[i]);
	Next_Offset++;
	Next_Offset = ((Next_Offset / 8) * 8) + 8;
	*/
}


// Allocate a single input node, duplicating data if vault mismatch.
void Dependency_Logger::maloc_DATA_IN_Node(Data_Logger* DataL, size_t vlt, size_t* SoBI, SNID_t Orig_Node, Dependency Deps, bool IZero_Force)
{
	// Initializations
	SNID_t Dep_SNID = Deps.Dependent_ID;
	DBID_t Cnsmd_DBID = Nodes[Dep_SNID.index()].Consumes_DBID[0];
	Data_Block_Types Cnsmd_DBID_Type = DataL->Get_Type_of_DBID(Cnsmd_DBID);
	// it should returns if the node is not Sch_CNN_Input, and its consumed data block is not DBT_IZero or DBT_DATA_IN
	if (Nodes[Dep_SNID.index()].type != Sch_CNN_Input)
		return;
	if (Cnsmd_DBID_Type == DBT_PZero)
		return;
	if (Cnsmd_DBID_Type == DBT_PSUM)
		return;

	if (!DataL->Is_Allocated(Cnsmd_DBID))
	{
		//std::cout << Cnsmd_DBID << ",\t\t";
		DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
		DataL->Set_Starting_Index_of_Data_Block(Cnsmd_DBID, SoBI[vlt]++);
		DataL->Mark_as_Allocated(Cnsmd_DBID);
	}
	// if it allocated before, Duplicating data and allocate the new Data, if the block nd the Node are located in the different vault 
	else
		// After a long night, I found out that it is not enough to just douplicate the data, and the dependency Node should be revised too, fuck me.
	{
		// Trget Vault is the input parmmeter "vlt", and the dta vault cn be found like:
		size_t Data_vlt;
		DataL->Get_Vault_of_DBID(Cnsmd_DBID, Data_vlt);
		// return if they are in the same vault
		if ((Data_vlt == vlt) && (Cnsmd_DBID_Type != DBT_IZero))	// TODO: Optimize the DataBlock Usage
			return;

		// Here is the place where the magic happens
		// 1- Douplicating the data Block 
		DBID_t old_Cnsmd_DBID = Cnsmd_DBID;
		Cnsmd_DBID = DataL->Duplicate_Data_Block(Cnsmd_DBID);
		// 2- Douplicating the Scheduling Node
		SNID_t old_Dep_SNID = Dep_SNID;
		Dep_SNID = Douplicate_and_cleanse_Scheduling_Node(Dep_SNID, Orig_Node);
		// 3- Changing the consumed data of that node to the new data block 
		Nodes[Dep_SNID.index()].Consumes_DBID[0] = Cnsmd_DBID;
		// 4- Setting its vault to the Node vault 
		DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
		// 5- Changing the strting Index of the new Data
		DataL->Set_Starting_Index_of_Data_Block(Cnsmd_DBID, SoBI[vlt]++);
		// 6- To be sure,
		DataL->Mark_as_Allocated(Cnsmd_DBID);
	}

}


// Allocate a single weight node, duplicating data if vault mismatch.
void Dependency_Logger::maloc_WEIGHT_Node(Data_Logger* DataL, size_t vlt, size_t* SoBI, SNID_t Orig_Node, Dependency Deps)
{
	// Initializations
	SNID_t Dep_SNID = Deps.Dependent_ID;
	DBID_t Cnsmd_DBID = Nodes[Dep_SNID.index()].Consumes_DBID[0];
	Data_Block_Types Cnsmd_DBID_Type = DataL->Get_Type_of_DBID(Cnsmd_DBID);
	// it should returns if the node is not Sch_CNN_Weight
	if (Nodes[Dep_SNID.index()].type != Sch_CNN_Weight)
		return;

	if (!DataL->Is_Allocated(Cnsmd_DBID))
	{
		//std::cout << Cnsmd_DBID << ",\t\t";
		DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
		DataL->Set_Starting_Index_of_Data_Block(Cnsmd_DBID, SoBI[vlt]++);
		DataL->Mark_as_Allocated(Cnsmd_DBID);
	}
	// if it allocated before, Duplicating data and allocate the new Data, if the block nd the Node are located in the different vault 
	else
	{
		// Trget Vault is the input parmmeter "vlt", and the dta vault cn be found like:
		size_t Data_vlt;
		DataL->Get_Vault_of_DBID(Cnsmd_DBID, Data_vlt);
		// return if they are in the same vault
		if (Data_vlt == vlt)
		{
			//std::cout << Cnsmd_DBID << ",\t\t";
			return;
		}
		// Here is the place where the magic happens
		// 1- Douplicating the data Block 
		DBID_t old_Cnsmd_DBID = Cnsmd_DBID;
		Cnsmd_DBID = DataL->Duplicate_Data_Block(Cnsmd_DBID);
		//std::cout << "!" << Cnsmd_DBID << "(" << old_Cnsmd_DBID << "),\t";
		//std::cout << "New Data, DBID: " << Cnsmd_DBID << ",\tA copy of:" << old_Cnsmd_DBID << std::endl;
		// 2- Douplicating the Scheduling Node
		SNID_t old_Dep_SNID = Dep_SNID;
		Dep_SNID = Douplicate_and_cleanse_Scheduling_Node(Dep_SNID, Orig_Node);
		// 3- Changing the consumed data of that node to the new data block 
		Nodes[Dep_SNID.index()].Consumes_DBID[0] = Cnsmd_DBID;
		// 4- Setting its vault to the Node vault 
		DataL->Set_Consumers_ID_of_DBID(Cnsmd_DBID, Dep_SNID);
		DataL->Set_Vault_of_DBID(Cnsmd_DBID, vlt);
		// 5- Changing the strting Index of the new Data
		DataL->Set_Starting_Index_of_Data_Block(Cnsmd_DBID, SoBI[vlt]++);
		// 6- To be sure,
		DataL->Mark_as_Allocated(Cnsmd_DBID);
	}

}


// Clear allocation flags for PZero and PSUM blocks.
void Dependency_Logger::Clear_Allocation_Flag(Data_Logger* DataL)
{
	for (DBID_t idx = DBID_t::Null(); idx < DataL->size(); idx++)
	{
		Data_Block DB;
		DataL->Get_Data_Block_Info(idx, DB);
		switch (DB.DBT)
		{
		case DBT_PZero:
		case DBT_PSUM:
			DB.alocated = false;
			DataL->Change_Data_Block_info(idx, DB);
			break;
		}
	}
}


// Add a new time slot to the allocation matrix.
void   Dependency_Logger::Addin_New_Time(std::vector<std::vector<DBID_t>>& Mx, std::vector<size_t>& Ax, size_t time)
{
	size_t lst = Mx.size();
	Ax.push_back(time);
	Mx.push_back({});
	for (size_t idx = 0; idx < Mx[0].size(); idx++)
		Mx[lst].push_back(DBID_t::Null());
}


// Get index of a time slot (create if missing).
size_t Dependency_Logger::Return_the_Time_idx(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax, size_t time)
{
	bool found(false);
	size_t idx;
	for (idx = 0; idx < Ax.size(); idx++)
	{
		if (Ax[idx] == time)
		{
			found = true;
			break;
		}
	}
	if (!found)
	{
		Addin_New_Time(matrix, Ax, time);
		idx = Ax.size() - 1;
	}
	return idx;
}


// Insert a data block into the allocation matrix across its lifetime.
void   Dependency_Logger::Add_Data_Block_to_Matrix(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax, Data_Block DB)
{
	// initialization
	size_t SoLT = DB.Produced_Time;
	size_t EoLT = DB.Last_Acc_Time;
	size_t Diff = EoLT - SoLT + 1;		//	TODO: What if i change the 1 to 2?
	size_t* Times_Idx = new size_t[Diff];

	// extrcting the corresponded indexes to the desired time
	for (size_t t = 0; t < Diff; t++)
		Times_Idx[t] = Return_the_Time_idx(matrix, Ax, EoLT + t);

	// check for enmpty spaces in that time period
	bool	inserted(false);
	size_t	idx;
	for (idx = 0; idx < matrix[0].size(); idx++)
	{
		inserted = true;
		for (size_t t = 0; t < Diff; t++)
			inserted &= (matrix[Times_Idx[t]][idx] == DBID_t::Null());
		if (inserted)
			break;
	}

	// if there were non, create one
	if (!inserted)
		idx = Add_New_Matric_Row(matrix);

	// inserting data in those sluts
	for (size_t t = 0; t < Diff; t++)
		matrix[Times_Idx[t]][idx] = DB.ID;
}


// Add a new row to the allocation matrix.
size_t Dependency_Logger::Add_New_Matric_Row(std::vector<std::vector<DBID_t>>& matrix)
{
	for (size_t idx = 0; idx < matrix.size(); idx++)
		matrix[idx].push_back(DBID_t::Null());
	return matrix[0].size() - 1;
}


// TODO
// Optimize the allocation matrix (placeholder).
void   Dependency_Logger::Optimize_Allocation_Matrix(std::vector<std::vector<DBID_t>>& matrix, std::vector<size_t>& Ax)
{
	// TODO
	return;
}


// Assign starting addresses for blocks from the allocation matrix.
size_t Dependency_Logger::Setting_Start_Address_of_Allocation_Matrix(std::vector<std::vector<DBID_t>> matrix, Data_Logger* DataL, size_t Offset)
{
	if (matrix.size() == 0)
	{
		Offset++;
		Offset = ((Offset / 8) * 8) + 8;
		return Offset;
	}

	for (size_t time = 0; time < matrix.size(); time++)
	{
		for (size_t idx = 0; idx < matrix[time].size(); idx++)
		{
			// check if the entery is not empty
			if (matrix[time][idx] == DBID_t::Null())
				continue;

			// now it's time to assign the start index of the 
			Data_Block  DB;
			DataL->Get_Data_Block_Info(matrix[time][idx], DB);

			// if it allocated before and we fetch it again in another time of it's  lifetime
			if (DB.alocated)
				continue;

			DB.SoBI = Offset + idx;
			DB.alocated = true;
			DataL->Change_Data_Block_info(matrix[time][idx], DB);

			// clear the block from the matrix, for the rest of it's lifetime
			// or we cn simplly pass the allocated data blocks
		}
	}

	Offset += matrix[0].size();
	Offset = ((Offset / 8) * 8) + 8;
	return Offset;
}


// Pre-reserve rows in the allocation matrix.
void   Dependency_Logger::Reserve_Rows(std::vector<std::vector<DBID_t>>& matrix, size_t cnt)
{
	for (size_t idx = 0; idx < matrix.size(); idx++)
		matrix[idx].reserve(cnt);
}


// Return maximum sequential length of a level.
size_t Dependency_Logger::Get_Max_seq_length_of_level(size_t lvl)
{
	size_t out = 0;
	for (size_t bl = 0; bl < Execution_Threads[lvl].size(); bl++)
		out = std::max(out, Execution_Threads[lvl][bl].Nodes.size());
	return out;
}










