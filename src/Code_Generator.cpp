#include "Code_Generator.h"


//***************************************//
//*			Public Functions			*//
//***************************************//

// Initialize Code_Generator state.
// - Clears ordering/PE/index containers (CG_MPDRs starts empty by default).
// - Resets Imported/Extracted/Modified flags.
// - Resets Max_PZero_per_Vault.
Code_Generator::Code_Generator								()
{
	Ordering.clear();
	CG_PEs.clear();
	Ordering_Index.clear();

	Imported = false;
	Extracted = false;
	Modified = false;
}


// No-op destructor; state is owned by STL containers and released automatically.
Code_Generator::~Code_Generator								()
{
}


// Import ordering from Dependency_Logger and build PE/MPDR execution descriptors.
// - Pulls Ordering from DpndL and sizes Ordering_Index for all scheduling nodes.
// - For each level/baseline, builds CG_PEs by plane/vault and then links storage.
// - Builds CG_MPDRs per baseline if an MPDR node exists.
// - Marks Extracted and removes empty baselines.
// Returns true on successful extraction.
bool Code_Generator::Extract_PE_Execution_Info				(Dependency_Logger* DpndL)
{
	DpndL->Get_Ordering_Info(Ordering);
	Imported = true;
	CG_PEs  .reserve(Ordering.size());
	CG_MPDRs.reserve(Ordering.size());
	Ordering_Index.reserve(DpndL->size());
	for (size_t i = 0; i < DpndL->size(); i++)
		Ordering_Index.push_back({0, 0, 0});
	

	// For each Level in orderin
	for (size_t lvl = 0; lvl < Ordering.size(); lvl++)
	{
		// initiate the CG_PEs and CG_MPDRs
		CG_PEs  .push_back({});
		CG_MPDRs.push_back({});
		CG_PEs[lvl]  .reserve(Ordering[lvl].size());
		CG_MPDRs[lvl].reserve(Ordering[lvl].size());
		
		// loop on each baseline
		for (size_t baseline = 0; baseline < Ordering[lvl].size(); baseline++)
		{
			// initiate again
			if (Ordering[lvl][baseline].size() > 0)
			{
				CG_PEs[lvl]  .push_back({});
				CG_MPDRs[lvl].push_back({});
				CG_PEs[lvl][baseline]  .reserve(Ava_Planes * Ava_Vaults);
				CG_MPDRs[lvl][baseline].reserve(Ava_Planes * Ava_Vaults);
			}
			else
				continue;
			CG_PE_Node PEInfo;
			// building the processing element info, each PE in each vault and plane has a dedicated PE_info
			for (size_t plane = 0; plane < Ava_Planes; plane++)
				for (size_t vault = 0; vault < Ava_Vaults; vault++)
					if (Extract_Single_PE_Execution_Info(DpndL, PEInfo, lvl, baseline, plane, vault))
						CG_PEs[lvl][baseline].push_back(PEInfo);
			// Now its time to link the storings and Activations to the PE info
			Link_PE_Storage(DpndL, lvl, baseline);
			// Now building the MPDR Execution Info
			CG_MPDR_Node MPDRInfo;
			if (Extract_MPDR_Execution_Info(DpndL, MPDRInfo, lvl, baseline))
				CG_MPDRs[lvl][baseline].push_back(MPDRInfo);
		}
	}

	Extracted = true;
	Cleans_CG_Vectors();
	return true;
}


// Generate executable platform code from extracted scheduling/data info.
// - Requires Extracted; returns false if extraction was not performed.
// - Normalizes internal structures (Modify_All) and writes spacing dumps.
// - Copies template runtime files into dest and emits main.cpp.
// - Generates per-level execute/peripheral functions and per-baseline bodies.
// Returns true on successful generation.
bool Code_Generator::Code_Wizard							(Dependency_Logger* DpndL, Data_Logger* DataL, NETWORK* network, std::filesystem::path Src_fname, std::filesystem::path dst_fname, std::filesystem::path CGCW_Dump_dir)
{
	if (!Extracted)
		return false;

	// Modifing the structure of extracted information to simpify the code generation
	Modify_All(DataL, CGCW_Dump_dir);
	
	// Generate Codes.
	Generate_Codes(DpndL, DataL, network, Src_fname, dst_fname);

	return true;
}







//***************************************//
//*			Private Functions			*//
//***************************************//

// Populate PEInfo with scheduling nodes, inputs, and weights for a plane/vault.
// - Initializes PEInfo fields and reserves vector capacities.
// - Scans Ordering for Sch_Process nodes matching plane/vault and not generated.
// - Marks ordering nodes as Generated and records Ordering_Index mapping.
// - Captures input DBIDs from Sch_CNN_Input dependencies.
// - On the first node, captures weight DBIDs from Sch_CNN_Weight dependencies.
// Returns true if at least one execution node was captured.
bool Code_Generator::Extract_Single_PE_Execution_Info		(Dependency_Logger* DpndL, CG_PE_Node& PEInfo, size_t lvl, size_t baseline, size_t plane, size_t vault)
{
	Ordering_Node Ord_Node;
	SNID_t ONID;
	size_t Rsrv;
	PEInfo.Activation = false;
	PEInfo.ESI = false;
	PEInfo.ESO = false;
	PEInfo.ESP = false;
	PEInfo.Plane = plane;
	PEInfo.Vault = vault;
	PEInfo.Weigth_ID[0] = DBID_t::Null();
	PEInfo.Weigth_ID[1] = DBID_t::Null();
	PEInfo.Weigth_ID[2] = DBID_t::Null();
	PEInfo.Weigth_ID[3] = DBID_t::Null();
	PEInfo.Weigth_ID[4] = DBID_t::Null();
	PEInfo.Weigth_ID[5] = DBID_t::Null();
	PEInfo.Weigth_ID[6] = DBID_t::Null();
	PEInfo.Weigth_ID[7] = DBID_t::Null();
	PEInfo.Weigth_ID[8] = DBID_t::Null();
	PEInfo.Inputs_ID.clear(); 
	PEInfo.Output_ID.clear(); 
	PEInfo.EXE_Nodes.clear(); 
	PEInfo.Acum_DBID.clear();
	PEInfo.Inp_Seqnc = 0;
	PEInfo.Out_Seqnc = 0;
	PEInfo.Acc_Seqnc = 0;
	PEInfo.Inputs_ID.reserve(378);
	PEInfo.Output_ID.reserve(128);
	PEInfo.EXE_Nodes.reserve(128);
	PEInfo.Acum_DBID.reserve(128);
	
	if (!Imported)
		return false;
	
	for (size_t node = 0; node < Ordering[lvl][baseline].size(); node++)
	{
		Ord_Node = Ordering[lvl][baseline][node];
		ONID = Ord_Node.ID;
		Rsrv = Ord_Node.Rsrv1;
		//	Check for proper value
		if (Ord_Node.Generated)				continue;
		if (Ord_Node.Plane != plane)		continue;
		if (Ord_Node.Vault != vault)		continue;
		if (Ord_Node.type != Sch_Process)	continue;
		// initating
		Ordering_Index[ONID.index()] = { true, lvl ,baseline ,CG_PEs[lvl][baseline].size() , PEInfo.EXE_Nodes.size() };
		// Marking as Generaated
		Ordering[lvl][baseline][node].Generated = true;
		// Getting info of this Scheduling Node
		Scheduling_Node SNinfo;
		DpndL->Fetch_Scheduling_Node_Info(ONID, SNinfo);
		// Execution Node	
		PEInfo.EXE_Nodes.push_back(ONID);
		PEInfo.Output_ID.push_back(DBID_t::Null());
		PEInfo.Acum_DBID.push_back(DBID_t::Null());
		// Inputs, adding the weight of this baseline
		for (size_t idx = 0; idx < SNinfo.Dependencies.size(); idx++)
		{
			// checking its dependencies
			Scheduling_Node Dep_SNinfo;
			DpndL->Fetch_Scheduling_Node_Info(SNinfo.Dependencies[idx].Dependent_ID, Dep_SNinfo);
			// Pass if its not the weight layer
			if (Dep_SNinfo.type != Sch_CNN_Input)
				continue;
			// if it is the weight layer, It Should do something, that i do not know
			PEInfo.Inputs_ID.push_back(Dep_SNinfo.Consumes_DBID[0]);
		}
		// Weights
		if (PEInfo.EXE_Nodes.size() == 1)
		{
			// adding the weight of this baseline
			for (size_t idx = 0; idx < SNinfo.Dependencies.size(); idx++)
			{
				// checking its dependencies
				Scheduling_Node Dep_SNinfo;
				DpndL->Fetch_Scheduling_Node_Info(SNinfo.Dependencies[idx].Dependent_ID, Dep_SNinfo);
				// Pass if its not the weight layer
				if (Dep_SNinfo.type != Sch_CNN_Weight)
					continue;
				// if its is the weight layer, It Should do something, that i do not know
				PEInfo.Weigth_ID[idx] = Dep_SNinfo.Consumes_DBID[0];
			}
		}
	}

	return (PEInfo.EXE_Nodes.size() > 0);
}


// Locate a Sch_MPDR node in the ordering and build MPDRInfo inputs/outputs.
// - Finds the first non-generated Sch_MPDR node in the baseline.
// - Marks it Generated, sets Node/Vault/Output, and collects input DBIDs.
// Returns true if an MPDR node was found.
bool Code_Generator::Extract_MPDR_Execution_Info			(Dependency_Logger* DpndL, CG_MPDR_Node& MPDRInfo, size_t lvl, size_t baseline)
{
	// Initializations
	MPDRInfo.valid = true;
	MPDRInfo.gen = false;
	MPDRInfo.ESI = false;
	MPDRInfo.ESO = false;
	MPDRInfo.Node = SNID_t::Null();
	MPDRInfo.Vault = 0;
	MPDRInfo.Inputs.clear();
	MPDRInfo.Output = DBID_t::Null();
	Ordering_Node Ord_Node;
	SNID_t ONID;
	size_t Rsrv;
	bool found(false);
	// For each nodes 
	for (size_t node = 0; node < Ordering[lvl][baseline].size(); node++)
	{
		// Some initializations
		Ord_Node = Ordering[lvl][baseline][node];
		ONID = Ord_Node.ID;
		Rsrv = Ord_Node.Rsrv1;
		size_t plane = Ord_Node.Plane;
		size_t vault = Ord_Node.Vault;
		// Checkign The orderign type, pass if it is Generated before
		if (Ord_Node.Generated)
			continue;
		// Pass if it isn't Sch_MPDR
		if (Ord_Node.type != Sch_MPDR)
			continue;
		// Marking this node as Generated
		Ordering[lvl][baseline][node].Generated = true;
		found = true;
		// Now, its time to Link the node
		Scheduling_Node MPDR_SNinfo;
		DpndL->Fetch_Scheduling_Node_Info(ONID, MPDR_SNinfo);
		// Some Settings
		MPDRInfo.Node   = ONID;
		MPDRInfo.Vault  = Ord_Node.Vault;
		MPDRInfo.Output = MPDR_SNinfo.Produced_DBID;
		// Now its time to Accumulate previous Blocks, Since the first block is always Zero Block, the first dependency should treat differently
		for (size_t Dep_Cntr = 0; Dep_Cntr < MPDR_SNinfo.Dependencies.size(); Dep_Cntr++)
		{
			Scheduling_Node Prv_SNinfo;
			DpndL->Fetch_Scheduling_Node_Info(MPDR_SNinfo.Dependencies[Dep_Cntr].Dependent_ID, Prv_SNinfo);
			MPDRInfo.Inputs.push_back(Prv_SNinfo.Produced_DBID);
		}
	}
	return found;
}


// Link activation and accumulation outputs back into CG_PEs entries.
// - Finds Sch_Activation nodes and marks them Generated.
// - Walks to the dependent Sch_Accumulate node and its Sch_Process deps.
// - Uses Ordering_Index to locate the owning CG_PE_Node and fills Output_ID
//   and Acum_DBID (Null for the first dep), and sets Activation on last dep.
// Throws if the execution node is missing from Ordering_Index.
void Code_Generator::Link_PE_Storage						(Dependency_Logger* DpndL, size_t lvl, size_t baseline)
{
	Ordering_Node Ord_Node;
	SNID_t ONID;
	size_t Rsrv;
	// For each nodes 
	for (size_t node = 0; node < Ordering[lvl][baseline].size(); node++)
	{
		// Some initializations
		Ord_Node = Ordering[lvl][baseline][node];
		ONID = Ord_Node.ID;
		Rsrv = Ord_Node.Rsrv1;
		size_t plane = Ord_Node.Plane;
		size_t vault = Ord_Node.Vault;
		// Checkign The orderign type,  pass if it is Generated before
		if (Ord_Node.Generated)
			continue;
		// Pass if it isn't Activation, We skiped if it was is Accumulate, because it can be accessed from the activation node
		if (Ord_Node.type != Sch_Activation)
			continue;
		// Marking this node as Generated
		Ordering[lvl][baseline][node].Generated = true;
		// Now, its time to Link the node
		Scheduling_Node Act_SNinfo;
		DpndL->Fetch_Scheduling_Node_Info(ONID, Act_SNinfo);
		// Produced Data Block
		DBID_t Prod_DB = Act_SNinfo.Produced_DBID;
		// now, moving to the Sch_Accumulate Node
		Scheduling_Node Acc_SNinfo;
		DpndL->Fetch_Scheduling_Node_Info(Act_SNinfo.Dependencies[0].Dependent_ID, Acc_SNinfo);
		// Now its time to Accumulate previous Blocks, Since the first block is always Zero Block, the first dependency should treat differently
		for (size_t Dep_Cntr = 0; Dep_Cntr < Acc_SNinfo.Dependencies.size(); Dep_Cntr++)
		{
			Scheduling_Node Exe_SNinfo;
			DpndL->Fetch_Scheduling_Node_Info(Acc_SNinfo.Dependencies[Dep_Cntr].Dependent_ID, Exe_SNinfo);
			Ord_Address Exe_Ord_Addr = Ordering_Index[Exe_SNinfo.ID.index()];
			if (!Exe_Ord_Addr.valid)
			{
				std::cout << "E: Something went wrong, Even I can not say what went wrong, Sorry." << std::endl;
				throw("E: Something went wrong, Even I can not say what went wrong, Sorry.");
				return;
			}
			CG_PEs[Exe_Ord_Addr.lvl][Exe_Ord_Addr.bline][Exe_Ord_Addr.node].Output_ID[Exe_Ord_Addr.pos] = Prod_DB;
			CG_PEs[Exe_Ord_Addr.lvl][Exe_Ord_Addr.bline][Exe_Ord_Addr.node].Acum_DBID[Exe_Ord_Addr.pos] = (Dep_Cntr == 0) ? DBID_t::Null() : Prod_DB;
			CG_PEs[Exe_Ord_Addr.lvl][Exe_Ord_Addr.bline][Exe_Ord_Addr.node].Activation = (Dep_Cntr == (Acc_SNinfo.Dependencies.size() - 1)) ? true : false;
		}
	}
}


// Remove empty baselines from CG_PEs and CG_MPDRs vectors.
// Levels are retained even if empty (level erase code is commented out).
void Code_Generator::Cleans_CG_Vectors						()
{
	for (size_t lvl = CG_PEs.size(); lvl-- > 0; )
	{
		for (size_t bline = CG_PEs[lvl].size(); bline-- > 0; )
			if (CG_PEs[lvl][bline].empty())
				CG_PEs[lvl].erase(CG_PEs[lvl].begin() + bline);
	}


	for (size_t lvl = CG_MPDRs.size(); lvl-- > 0; )
	{
		for (size_t bline = CG_MPDRs[lvl].size(); bline-- > 0; )
			if (CG_MPDRs[lvl][bline].empty())
				CG_MPDRs[lvl].erase(CG_MPDRs[lvl].begin() + bline);
	}
}


// Normalize PE input list by keeping the first two inputs, then every third input.
// Overwrites Inputs_ID for each PE and sets Modified = true.
void Code_Generator::Modify_PE_Inputs						()
{
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	{
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
		{
			for (size_t node = 0; node < CG_PEs[lvl][bline].size(); node++)
			{
				// it's time to do some magic
				std::vector<DBID_t> Inputs_ID;
				Inputs_ID.push_back(CG_PEs[lvl][bline][node].Inputs_ID[0]);
				Inputs_ID.push_back(CG_PEs[lvl][bline][node].Inputs_ID[1]);
				
				for (size_t pos = 2; pos < CG_PEs[lvl][bline][node].Inputs_ID.size(); pos+=3)
				{
					Inputs_ID.push_back(CG_PEs[lvl][bline][node].Inputs_ID[pos]);
				}

				CG_PEs[lvl][bline][node].Inputs_ID = Inputs_ID;
			}
		}
	}
	Modified = true;
}


// Apply MPDR reordering for all levels to create evenly spaced chains.
void Code_Generator::Modify_MPDR_units						(Data_Logger* DataL)
{
	for (size_t lvl = 0; lvl < CG_MPDRs.size(); lvl++)
		Modify_MPDR_units(DataL, lvl);
}


// Reorder MPDR baselines into chains by minimal output spacing within same vault.
// - Finds the minimum spacing between outputs across baselines.
// - Builds chains by appending nodes with same spacing and vault.
// - Rebuilds CG_MPDRs[lvl] with the grouped chains.
void Code_Generator::Modify_MPDR_units						(Data_Logger* DataL, size_t lvl)
{
	if (CG_MPDRs[lvl].empty())
		return;

	// a new container
	std::vector<std::vector<CG_MPDR_Node>> CG_MPDRs_New;

	while(true)
	{
		// some initialization
		size_t min_spaced(-1);
		// finding the smallest interval
		for (size_t bline1 = 0; bline1 < (CG_MPDRs[lvl].size()-1); bline1++)
		{
			if (!CG_MPDRs[lvl][bline1][0].valid)
				continue;

			for (size_t bline2 = bline1+1; bline2 < CG_MPDRs[lvl].size(); bline2++)
			{
				if (!CG_MPDRs[lvl][bline2][0].valid)
					continue;
				min_spaced = std::min(min_spaced, DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline1][0].Output, CG_MPDRs[lvl][bline2][0].Output));
				if (min_spaced == 1)
					break;
			}
			if (min_spaced == 1)
				break;
		}
		//build chanes with the same intervals 
		bool chane_Added(true);
		while (chane_Added)
		{
			chane_Added = false;
			// A loop to find a chain
			std::vector<CG_MPDR_Node> CG_MPDRs_thread;
			for (size_t bline = 0; bline < CG_MPDRs[lvl].size(); bline++)
			{
				if (!CG_MPDRs[lvl][bline][0].valid)
					continue;
				CG_MPDRs_thread.push_back(CG_MPDRs[lvl][bline][0]);
				CG_MPDRs[lvl][bline][0].valid = false;
				break;
			}
			// initializing a loop condition
			bool added(true);
			// loop on baselines to find the next element of th chain
			while (added)
			{
				added = false;
				// loop on other baselines 
				for (size_t bline = 0; bline < CG_MPDRs[lvl].size(); bline++)
				{
					// pass of not valid
					if (!CG_MPDRs[lvl][bline][0].valid)
						continue;
					// if the node have the same interval
					if ((DataL->Calculate_Space_Between(CG_MPDRs_thread.back().Output, CG_MPDRs[lvl][bline][0].Output) == min_spaced) &&
						(DataL->Get_Vault_of_DBID(CG_MPDRs_thread.back().Output) == DataL->Get_Vault_of_DBID(CG_MPDRs[lvl][bline][0].Output)))
					{
						//adding this to the chain
						CG_MPDRs_thread.push_back(CG_MPDRs[lvl][bline][0]);
						CG_MPDRs[lvl][bline][0].valid = false;
						// extending the while loop at least one iteration 
						added = true;
						chane_Added = true;
					}
				}
			}
			// adding this chane to the baselines
			if (!CG_MPDRs_thread.empty())
				CG_MPDRs_New.push_back(CG_MPDRs_thread);
		}
		// now, we aaded all chanes with the minimum interva, what should i do next?
		// you are right, finding the next minimum
		// but first, a way out 
		bool ended(true);
		for (size_t bline = 0; bline < CG_MPDRs[lvl].size(); bline++)
			ended &= !CG_MPDRs[lvl][bline][0].valid;
		if (ended)
			break;
	}

	// replace this new container with the old one
	CG_MPDRs[lvl] = CG_MPDRs_New;
}


// Check equal spacing for PE inputs, outputs, and accumulators.
// - Uses spacing between the first two elements as reference.
// - Sets ESI/ESO/ESP to true only if all subsequent gaps match.
void Code_Generator::Equal_Spaced_Check_PE					(Data_Logger* DataL, size_t lvl, size_t bline, size_t node, bool& ESI, bool& ESO, bool& ESP)
{	
	std::vector<DBID_t> Inputs_ID = CG_PEs[lvl][bline][node].Inputs_ID;
	std::vector<DBID_t> Output_ID = CG_PEs[lvl][bline][node].Output_ID;
	std::vector<DBID_t> Acum_DBID = CG_PEs[lvl][bline][node].Acum_DBID;

	size_t Inp = DataL->Calculate_Space_Between(Inputs_ID[0], Inputs_ID[1]);
	size_t Out = DataL->Calculate_Space_Between(Output_ID[0], Output_ID[1]);
	size_t Acc = DataL->Calculate_Space_Between(Acum_DBID[0], Acum_DBID[1]);

	ESI = true;
	ESO = true;
	ESP = true;
	
	for (size_t idx = 1; idx < Inputs_ID.size(); idx++)
		ESI &= (DataL->Calculate_Space_Between(Inputs_ID[idx - 1], Inputs_ID[idx]) == Inp);
	
	for (size_t idx = 1; idx < Output_ID.size(); idx++)
		ESO &= (DataL->Calculate_Space_Between(Output_ID[idx - 1], Output_ID[idx]) == Out);
	
	for (size_t idx = 1; idx < Acum_DBID.size(); idx++)
		ESP &= (DataL->Calculate_Space_Between(Acum_DBID[idx - 1], Acum_DBID[idx]) == Acc);
}


// Check equal spacing for MPDR inputs/outputs across nodes of a baseline.
// - Compares spacing of outputs and each input slot across nodes.
// - Writes ESI/ESO flags into every MPDR node in the baseline.
void Code_Generator::Equal_Spaced_Check_MPDR				(Data_Logger* DataL, size_t lvl, size_t bline)
{
	bool ESO(true);
	bool ESI(true);

	size_t Out = 0;
	size_t In0 = 0;
	size_t In1 = 0;
	size_t In2 = 0;
	size_t In3 = 0;


	if (CG_MPDRs[lvl][bline].size() > 1)
	{
		Out = DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][0].Output,		CG_MPDRs[lvl][bline][1].Output);
		In0 = DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][0].Inputs[0],	CG_MPDRs[lvl][bline][1].Inputs[0]);
		In1 = DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][0].Inputs[1],	CG_MPDRs[lvl][bline][1].Inputs[1]);
		In2 = DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][0].Inputs[2],	CG_MPDRs[lvl][bline][1].Inputs[2]);
		In3 = DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][0].Inputs[3],	CG_MPDRs[lvl][bline][1].Inputs[3]);

		for (size_t node = 2; node < CG_MPDRs[lvl][bline].size(); node++)
		{
			ESO &= (DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][node - 1].Output,		CG_MPDRs[lvl][bline][node].Output)		== Out);
			ESI	&= (DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][node - 1].Inputs[0],	CG_MPDRs[lvl][bline][node].Inputs[0])	== In0);
			ESI	&= (DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][node - 1].Inputs[1],	CG_MPDRs[lvl][bline][node].Inputs[1])	== In1);
			ESI	&= (DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][node - 1].Inputs[2],	CG_MPDRs[lvl][bline][node].Inputs[2])	== In2);
			ESI	&= (DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][node - 1].Inputs[3],	CG_MPDRs[lvl][bline][node].Inputs[3])	== In3);
		}
	}

	for (size_t node = 0; node < CG_MPDRs[lvl][bline].size(); node++)
	{
		CG_MPDRs[lvl][bline][node].ESO = ESO;
		CG_MPDRs[lvl][bline][node].ESI = ESI;
	}

	// Assertion
	if ((Out > Assert_Max_Ival) || (In0 > Assert_Max_Ival) || (In1 > Assert_Max_Ival) || (In2 > Assert_Max_Ival) || (In3 > Assert_Max_Ival))
	{
		std::cout << "\tE: The maximum supported spacing is Exceeded!";
		throw("\tE: The maximum supported spacing is Exceeded!");
		return;
	}
}


// Adjust first/last zero input blocks by walking duplicate chains to match spacing.
// - Searches duplicate chain for a DBID that matches spacing to its neighbor.
// - Updates Inputs_ID[0] and Inputs_ID[last] if better matches are found.
// - Sets first/last flags when replacements are applied.
void Code_Generator::Modify_Zero_Blocks						(Data_Logger* DataL, size_t lvl, size_t bline, size_t node, bool& first, bool& last)
{
	DBID_t DBID;
	first = false;
	last  = false;
	std::vector<DBID_t> Inputs_ID = CG_PEs[lvl][bline][node].Inputs_ID;
	size_t Diff = DataL->Calculate_Space_Between(Inputs_ID[1], Inputs_ID[2]);

	// Modifing its first Zero Data Block 
	DBID = Inputs_ID[0];
	while (true)
	{
		Data_Block DB;
		DataL->Get_Data_Block_Info(DBID, DB);
		if (DataL->Calculate_Space_Between(DBID, Inputs_ID[1]) == Diff)
		{
			CG_PEs[lvl][bline][node].Inputs_ID[0] = DBID;
			first = true;
			break;
		}

		if (DB.Douplicate_of == DBID_t::Null())
			break;

		DBID = DB.Douplicate_of;
	}

	// Modifing its last  Zero Data Block 
	DBID = Inputs_ID[Inputs_ID.size()-1];
	while (true)
	{
		Data_Block DB;
		DataL->Get_Data_Block_Info(DBID, DB);
		if (DataL->Calculate_Space_Between(Inputs_ID[Inputs_ID.size() - 2], DBID) == Diff)
		{
			CG_PEs[lvl][bline][node].Inputs_ID[Inputs_ID.size() - 1] = DBID;
			last = true;
			break;
		}

		if (DB.Douplicate_of == DBID_t::Null())
			break;

		DBID = DB.Douplicate_of;
	}
}


// Apply all normalization steps and spacing checks, dump spacing diagnostics,
// and compute Max_PZero_per_Vault.
// - Modify_PE_Inputs + Modify_MPDR_units.
// - For each PE: spacing check, optional zero-block fix, set ESI/ESO/ESP,
//   and set input spacing (Inp_Seqnc).
// - For each MPDR baseline: set ESI/ESO.
// - Print spacing reports and compute Max_PZero_per_Vault.
void Code_Generator::Modify_All								(Data_Logger* DataL, std::filesystem::path fname)
{
	bool PE_print_en	(true);
	bool MPDR_print_en	(true);

	Modify_PE_Inputs();
	Modify_MPDR_units(DataL);

	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	{
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
		{
			for (size_t node = 0; node < CG_PEs[lvl][bline].size(); node++)
			{
				bool ESI(false);
				bool ESO(false);
				bool ESP(false);

				Equal_Spaced_Check_PE(DataL, lvl, bline, node, ESI, ESO, ESP);

				bool first(false);
				bool last (false);
				if (!ESI)
					Modify_Zero_Blocks(DataL, lvl, bline, node, first, last);

				CG_PEs[lvl][bline][node].ESI = ESI || (first && last);
				CG_PEs[lvl][bline][node].ESO = ESO;
				CG_PEs[lvl][bline][node].ESP = ESP;
				CG_PEs[lvl][bline][node].Inp_Seqnc = DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Inputs_ID[0], CG_PEs[lvl][bline][node].Inputs_ID[1]);
				CG_PEs[lvl][bline][node].Out_Seqnc = DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Output_ID[0], CG_PEs[lvl][bline][node].Output_ID[1]);
				CG_PEs[lvl][bline][node].Acc_Seqnc = DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Acum_DBID[0], CG_PEs[lvl][bline][node].Acum_DBID[1]);

				// Assertion
				if ((CG_PEs[lvl][bline][node].Inp_Seqnc > Assert_Max_Ival) || (CG_PEs[lvl][bline][node].Out_Seqnc > Assert_Max_Ival) || (CG_PEs[lvl][bline][node].Acc_Seqnc > Assert_Max_Ival))
				{
					std::cout << "\tE: The maximum supported spacing is Exceeded!";
					throw("\tE: The maximum supported spacing is Exceeded!");
					return;
				}
			}
		}
	}

	for (size_t lvl = 0; lvl < CG_MPDRs.size(); lvl++)
	{
		for (size_t bline = 0; bline < CG_MPDRs[lvl].size(); bline++)
		{
			Equal_Spaced_Check_MPDR(DataL, lvl, bline);
		}
	}

	Print_Spacing(DataL, fname);
}


// Write detailed spacing diagnostics for PE/MPDR blocks into report files.
// - fname_PE.txt: detailed per-output PE spacing.
// - fname_MPDR.txt: detailed MPDR spacing.
// - fname_PE_Summary.txt: condensed PE summary.
void Code_Generator::Print_Spacing							(Data_Logger* DataL, std::filesystem::path fname)
{
	std::ofstream files_out_PE;
	std::ofstream files_out_MPDR;
	std::ofstream files_out_PESum;

	files_out_PE	.open(fname / "CG_PE.txt");
	files_out_MPDR	.open(fname / "CG_MPDR.txt");
	files_out_PESum	.open(fname / "CG_PE_Summary.txt");
	

	files_out_PE	<< "\t( "			<< std::setw(3) << "P";
	files_out_PE	<< ", "				<< std::setw(3) << "V";
	files_out_PE	<< ")\t("			<< std::setw(4) << "lvl";
	files_out_PE	<< ","				<< std::setw(4) << "bsl";
	files_out_PE	<< ","				<< std::setw(4) << "nod";
	files_out_PE	<< ")\t("			<< std::setw(4) << "ESI";
	files_out_PE	<< ","				<< std::setw(4) << "ESO";
	files_out_PE	<< ","				<< std::setw(4) << "ESP";
	files_out_PE	<< ")"				<< std::endl;

	files_out_MPDR	<< "\t( "			<< std::setw(3) << "-";
	files_out_MPDR	<< ", "				<< std::setw(3) << "V";
	files_out_MPDR	<< ")\t("			<< std::setw(4) << "lvl";
	files_out_MPDR	<< ","				<< std::setw(4) << "bsl";
	files_out_MPDR	<< ","				<< std::setw(4) << "nod";
	files_out_MPDR	<< ")\t("			<< std::setw(4) << "ESI";
	files_out_MPDR	<< ","				<< std::setw(4) << "ESO";
	files_out_MPDR	<< ")"				<< std::endl;

	files_out_PESum	<< "\t( "			<< std::setw(3) << "P";
	files_out_PESum	<< ", "				<< std::setw(3) << "V";
	files_out_PESum	<< ")\t("			<< std::setw(4) << "lvl";
	files_out_PESum	<< ","				<< std::setw(4) << "bsl";
	files_out_PESum	<< ","				<< std::setw(4) << "nod";
	files_out_PESum	<< ")\t("			<< std::setw(4) << "ESI";
	files_out_PESum	<< ","				<< std::setw(4) << "ESO";
	files_out_PESum	<< ","				<< std::setw(4) << "ESP";
	files_out_PESum	<< ")"				<< std::endl;


	for (size_t lvl=0; lvl< CG_PEs.size(); lvl++)
	{
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
		{
			for (size_t node = 0; node < CG_PEs[lvl][bline].size(); node++)
			{
				size_t pln = CG_PEs[lvl][bline][node].Plane;
				size_t vlt = CG_PEs[lvl][bline][node].Vault;

				size_t idx;
				for (idx = 0; idx < CG_PEs[lvl][bline][node].Output_ID.size(); idx++)
				{
					files_out_PE	<< "\t( "			<< std::setw(3) << pln;
					files_out_PE	<< ", "				<< std::setw(3) << vlt;
					files_out_PE	<< ")\t( "			<< std::setw(3) << lvl;
					files_out_PE	<< ", "				<< std::setw(3) << bline;
					files_out_PE	<< ", "				<< std::setw(3) << node;
					files_out_PE	<< ")\t( "			<< std::setw(3) << CG_PEs[lvl][bline][node].ESI;
					files_out_PE	<< ", "				<< std::setw(3) << CG_PEs[lvl][bline][node].ESO;
					files_out_PE	<< ", "				<< std::setw(3) << CG_PEs[lvl][bline][node].ESP;
					files_out_PE	<< ")\t";

					files_out_PE	<< "\t@Inp\tB1:"	<< std::setw(6) << CG_PEs[lvl][bline][node].Inputs_ID[idx];
					files_out_PE	<< "\t("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
					files_out_PE	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
					files_out_PE	<< ")\tDiff: "		<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Inputs_ID[idx], CG_PEs[lvl][bline][node].Inputs_ID[idx + 1]);
					

					files_out_PE	<< " \t\t@Out\tB1:" << std::setw(6) << CG_PEs[lvl][bline][node].Output_ID[idx];
					files_out_PE	<< "\t("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Output_ID[idx]);
					files_out_PE	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Output_ID[idx]);
					files_out_PE	<< ")\tDiff: ";
					if (idx < (CG_PEs[lvl][bline][node].Output_ID.size()-1))
						files_out_PE					<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Output_ID[idx], CG_PEs[lvl][bline][node].Output_ID[idx + 1]);
					else
						files_out_PE << 0;

					files_out_PE	<< " \t\t@Acc\tB1:"	<< std::setw(6) << CG_PEs[lvl][bline][node].Acum_DBID[idx];
					files_out_PE	<< "\t("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Acum_DBID[idx]);
					files_out_PE	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Acum_DBID[idx]);
					files_out_PE	<< ")\tDiff: ";
					if (idx < (CG_PEs[lvl][bline][node].Output_ID.size()-1))
						files_out_PE					<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Acum_DBID[idx], CG_PEs[lvl][bline][node].Acum_DBID[idx + 1]);
					else
						files_out_PE << 0;

					files_out_PE << std::endl;
				}



				files_out_PE		<< "\t( "				<< std::setw(3) << pln;
				files_out_PE		<< ", "					<< std::setw(3) << vlt;
				files_out_PE		<< ")\t( "				<< std::setw(3) << lvl;
				files_out_PE		<< ", "					<< std::setw(3) << bline;
				files_out_PE		<< ", "					<< std::setw(3) << node;
				files_out_PE		<< ")\t( "				<< std::setw(3) << CG_PEs[lvl][bline][node].ESI;
				files_out_PE		<< ", "					<< std::setw(3) << CG_PEs[lvl][bline][node].ESO;
				files_out_PE		<< ", "					<< std::setw(3) << CG_PEs[lvl][bline][node].ESP;
				files_out_PE		<< ") \t\t@Inp\tB1:"	<< std::setw(6) << CG_PEs[lvl][bline][node].Inputs_ID[idx];
				files_out_PE		<< "\t("				<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
				files_out_PE		<< ", "					<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
				files_out_PE		<< ")\tDiff: "			<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Inputs_ID[idx], CG_PEs[lvl][bline][node].Inputs_ID[idx + 1]) << std::endl;
					

				idx++;
				files_out_PE		<< "\t( "				<< std::setw(3) << pln;
				files_out_PE		<< ", "					<< std::setw(3) << vlt;
				files_out_PE		<< ")\t( "				<< std::setw(3) << lvl;
				files_out_PE		<< ", "					<< std::setw(3) << bline;
				files_out_PE		<< ", "					<< std::setw(3) << node;
				files_out_PE		<< ")\t( "				<< std::setw(3) << CG_PEs[lvl][bline][node].ESI;
				files_out_PE		<< ", "					<< std::setw(3) << CG_PEs[lvl][bline][node].ESO;
				files_out_PE		<< ", "					<< std::setw(3) << CG_PEs[lvl][bline][node].ESP;
				files_out_PE		<< ") \t\t@Inp\tB1:"	<< std::setw(6) << CG_PEs[lvl][bline][node].Inputs_ID[idx];
				files_out_PE		<< "\t("				<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
				files_out_PE		<< ", "					<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
				files_out_PE		<< ")\tDiff: "			<< 0 << std::endl << std::endl << std::endl;
			}
		}
	}
	
	for (size_t lvl = 0; lvl < CG_MPDRs.size(); lvl++)
	{
		for (size_t bline = 0; bline < CG_MPDRs[lvl].size(); bline++)
		{
			for (size_t node = 0; node < CG_MPDRs[lvl][bline].size(); node++)
			{
				files_out_MPDR	<< "\t( "			<< std::setw(3) << "-";
				files_out_MPDR	<< ", "				<< std::setw(3) << "-";
				files_out_MPDR	<< ")\t( "			<< std::setw(3) << lvl;
				files_out_MPDR	<< ", "				<< std::setw(3) << bline;
				files_out_MPDR	<< ", "				<< std::setw(3) << node;
				files_out_MPDR	<< ")\t( "			<< std::setw(3) << CG_MPDRs[lvl][bline][node].ESI;
				files_out_MPDR	<< ", "				<< std::setw(3) << CG_MPDRs[lvl][bline][node].ESO;
				files_out_MPDR	<< ")\t";
				for (size_t idx = 0; idx < CG_MPDRs[lvl][bline][node].Inputs.size(); idx++)
				{
					files_out_MPDR	<< "\t\tBI: "	<< std::setw(6) << CG_MPDRs[lvl][bline][node].Inputs[idx];
					files_out_MPDR	<< " ("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_MPDRs[lvl][bline][node].Inputs[idx]);
					files_out_MPDR	<< ", "			<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_MPDRs[lvl][bline][node].Inputs[idx]);
					files_out_MPDR	<< ") ";
				}
				files_out_MPDR	<< ")\t\tBO: "		<< std::setw(6) << CG_MPDRs[lvl][bline][node].Output;
				files_out_MPDR	<< " ("				<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_MPDRs[lvl][bline][node].Output);
				files_out_MPDR	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_MPDRs[lvl][bline][node].Output);
				files_out_MPDR	<< ")\tDiff: ";
				if (node < (CG_MPDRs[lvl][bline].size() - 1))
					files_out_MPDR					<< DataL->Calculate_Space_Between(CG_MPDRs[lvl][bline][node].Output, CG_MPDRs[lvl][bline][node+1].Output);
				else
					files_out_MPDR	<< 0;
				files_out_MPDR		<< "\t"			<< std::endl;
			}

			files_out_MPDR << std::endl;
		}
	}
	
	for (size_t lvl=0; lvl< CG_PEs.size(); lvl++)
	{
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
		{
			for (size_t node = 0; node < CG_PEs[lvl][bline].size(); node++)
			{
				size_t pln = CG_PEs[lvl][bline][node].Plane;
				size_t vlt = CG_PEs[lvl][bline][node].Vault;

				size_t idx = 0;
				
				files_out_PESum	<< "\t( "			<< std::setw(3) << pln;
				files_out_PESum	<< ", "				<< std::setw(3) << vlt;
				files_out_PESum	<< ")\t( "			<< std::setw(3) << lvl;
				files_out_PESum	<< ", "				<< std::setw(3) << bline;
				files_out_PESum	<< ", "				<< std::setw(3) << node;
				files_out_PESum	<< ")\t( "			<< std::setw(3) << CG_PEs[lvl][bline][node].ESI;
				files_out_PESum	<< ", "				<< std::setw(3) << CG_PEs[lvl][bline][node].ESO;
				files_out_PESum	<< ", "				<< std::setw(3) << CG_PEs[lvl][bline][node].ESP;
				files_out_PESum	<< ")\t\t@Inp\tB1:"	<< std::setw(6) << CG_PEs[lvl][bline][node].Inputs_ID[idx];
				files_out_PESum	<< "\t("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
				files_out_PESum	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
				files_out_PESum	<< ")\tDiff: "		<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Inputs_ID[idx], CG_PEs[lvl][bline][node].Inputs_ID[idx + 1]);
				files_out_PESum	<< " \t\t@Out\tB1:" << std::setw(6) << CG_PEs[lvl][bline][node].Output_ID[idx];
				files_out_PESum	<< "\t("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Output_ID[idx]);
				files_out_PESum	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Output_ID[idx]);
				files_out_PESum	<< ")\tDiff: "		<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Output_ID[idx], CG_PEs[lvl][bline][node].Output_ID[idx + 1]);
				files_out_PESum	<< " \t\t@Acc\tB1:"	<< std::setw(6) << CG_PEs[lvl][bline][node].Acum_DBID[idx];
				files_out_PESum	<< "\t("			<< std::setw(2) << DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Acum_DBID[idx]);
				files_out_PESum	<< ", "				<< std::setw(4) << DataL->Get_Starting_Index_of_Data_Block(CG_PEs[lvl][bline][node].Acum_DBID[idx]);
				files_out_PESum	<< ")\tDiff: "		<< DataL->Calculate_Space_Between(CG_PEs[lvl][bline][node].Acum_DBID[idx], CG_PEs[lvl][bline][node].Acum_DBID[idx + 1]);
				files_out_PESum	<< std::endl;
			}
			files_out_PESum		<< std::endl;
		}
		files_out_PESum			<< std::endl;
	}

	files_out_PE.close();
	files_out_MPDR.close();
	files_out_PESum.close();
}


// Generate Codes
void Code_Generator::Generate_Codes							(Dependency_Logger* DpndL, Data_Logger* DataL, NETWORK* network, std::filesystem::path srce, std::filesystem::path dest)
{
	// Generating the initialization functions, every thing shoud kept simple as fuck.
	// copying the basic files
	
	Copy_File(srce, dest);


	// Generating Main Fucntion 
	std::ofstream files_main;
	files_main .open(dest / ("main.cpp"));
	Data_H_file.open(dest / ("Data.h"));
	Data_C_file.open(dest / ("Data.cpp"));
	Generate_Main_P1(files_main);
	

	// Generate "Platform_Execute_Layer" function
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		if (!CG_PEs[lvl].empty())
			Generate_Platform_Execute_Layer_lvl(DataL, files_main, lvl);

	// Generate "Platform_Peripheral_Layer" function
	for (size_t lvl = 0; lvl < CG_MPDRs.size(); lvl++)
		if (!CG_MPDRs[lvl].empty())
			Generate_Platform_Peripheral_Layer_lvl(DpndL, DataL, network, files_main, lvl);

	//// Generate "Platform_Execute_Layer_node" function
	//for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	//	for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
	//		if (!CG_PEs[lvl][bline].empty())
	//			Generate_Platform_Execute_Layer_bl(DataL, files_main, lvl, bline);

	// Generate "Platform_Execute_BseLine" function
	Generate_Platform_Execute_BseLine(files_main);


	// Generate "Platform_Execute" neccessary data block
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
				Generage_Data_Blocks_Exe_lvl_bline(DataL, lvl, bline);

	// Generate "Platform_Execute" Vectors of data
	Generage_Data_Blocks_Exe_Baseline(DataL);

	// Last part of the main function
	Generate_Main_P2(files_main);


	files_main .close();
	Data_H_file.close();
	Data_C_file.close();
}


// Copy required template source files from srce to dest (overwrite existing).
// Files: Utilities.h, common.h, Accelerator.h/.cpp, EventCallBacker.h/.cpp.
void Code_Generator::Copy_File								(std::filesystem::path srce, std::filesystem::path dest)
{
	// coppying the Utilities.h file
	std::filesystem::copy_file(	srce / ("Utilities.h"),			dest / ("Utilities.h"),				OVERWRITE);

	// coppying the Utilities.h file
	std::filesystem::copy_file(	srce / ("Utilities.cpp"),		dest / ("Utilities.cpp"),			OVERWRITE);

	// coppying the common.h file
	std::filesystem::copy_file(	srce / ("common.h"),			dest / ("common.h"),				OVERWRITE);

	// coppying the Accelerator.h file
	std::filesystem::copy_file(	srce / ("Accelerator.h"),		dest / ("Accelerator.h"),			OVERWRITE);

	// coppying the Accelerator.cpp file
	std::filesystem::copy_file(	srce / ("Accelerator.cpp"),		dest / ("Accelerator.cpp"),			OVERWRITE);

	// coppying the uprint.h file
	std::filesystem::copy_file(	srce / ("uprint.h"),			dest / ("uprint.h"),				OVERWRITE);

	//// coppying the EventCallBacker.h file
	//std::filesystem::copy_file(	srce + "/EventCallBacker.h",	dest + "/EventCallBacker.h",	OVERWRITE);
	//
	//// coppying the EventCallBacker.cpp file
	//std::filesystem::copy_file(srce + "/EventCallBacker.cpp",	dest + "/EventCallBacker.cpp",	OVERWRITE);
}


// Emit the first portion of generated main.cpp.
// - Includes, macros, and forward declarations for handlers and layer funcs.
// - Global DMA arrays/counters sized by Max_PZero_per_Vault.
// - main(): initialize counters, callbacks, system control, interrupts, timer,
//   and calls per-level execute/peripheral functions before looping forever.
void Code_Generator::Generate_Main_P1						(std::ofstream& files_out)
{
	Data_H_file	<< "#include \"common.h\""														<< std::endl << std::endl << std::endl;
	Data_C_file	<< "#include \"Data.h\""														<< std::endl << std::endl << std::endl;
	files_out	<< "#include <new>"																<< std::endl;
	files_out	<< "#include \"Data.h\""														<< std::endl;
	files_out	<< "#include \"uprint.h\""														<< std::endl;
	files_out	<< "#include \"Utilities.h\""													<< std::endl;
	files_out	<< "#include \"Accelerator.h\""													<< std::endl << std::endl << std::endl;
	//files_out	<< "#include \"EventCallBacker.h\""												<< std::endl;
	files_out	<< "void intr_PC		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_TBE		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_RBF		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_TXD		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_RXD		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_Timer		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_MPDR		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_DMA		(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_PLANE_0	(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_PLANE_1	(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_PLANE_2	(int intr_addr);"										<< std::endl;
	files_out	<< "void intr_PLANE_3	(int intr_addr);"										<< std::endl;
	files_out	<< "void (*funcPtrArray[12])(int);"												<< std::endl << std::endl << std::endl;
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	{
		if (!CG_PEs[lvl].empty())
			files_out << "void Platform_Execute_Layer_" << lvl << " ();"						<< std::endl;
	}
	files_out	<< ""																			<< std::endl << std::endl;
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	{
		if (!CG_MPDRs[lvl].empty())
			files_out << "void Platform_Peripheral_Layer_" << lvl << " ();"						<< std::endl;
	}
	files_out	<< ""																			<< std::endl << std::endl;
	files_out << "void Platform_Execute_BseLine (unsigned int bl);"								<< std::endl;
	//for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	//{	for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
	//		if (!CG_PEs[lvl][bline].empty())
	//			files_out	<< "void Platform_Execute_Layer_" 
	//						<< lvl << "_bl_" << bline << " ();"									<< std::endl;
	//}
	files_out	<< ""																			<< std::endl << std::endl;
	//files_out	<< "alignas(EventCallBacker_I)"													<< std::endl;
	//files_out	<< "static unsigned char ECB_storage[sizeof(EventCallBacker_I)];"				<< std::endl;
	//files_out	<< "static EventCallBacker_I* ECB;"												<< std::endl << std::endl << std::endl;
	//files_out	<< "int DMA_Read_SA [" << Ava_Vaults << "][" << MPpV << "];"					<< std::endl;
	files_out	<< "unsigned int DMA_ZDBT_info[" << Ava_Vaults << "];"							<< std::endl;
	files_out	<< "unsigned int DMA_handler_cntr[" << Ava_Vaults << "];"						<< std::endl;
	files_out	<< "unsigned int DMA_handler_Max [" << Ava_Vaults << "];"						<< std::endl;
	files_out	<< "unsigned int EXE_Bline_cntr(0);"											<< std::endl;
	files_out	<< "unsigned int PC_INT_Cntr;"													<< std::endl;
	files_out	<< "unsigned int DMA_Done_Cntr;"												<< std::endl;
	files_out	<< "unsigned int EXE_Done_Cntr(0);"												<< std::endl;
	files_out	<< "unsigned int PRI_Done_Cntr(0);"												<< std::endl;
	//files_out	<< "unsigned int PE_BaseLine_Done_Cntr[4][16];"									<< std::endl;
	files_out	<< "unsigned int TIMER_INT_Cntr(0);"											<< std::endl;
	files_out	<< "unsigned int TIMER_INT_Cntr_Sec(0);"										<< std::endl << std::endl << std::endl;
	files_out	<< "int main()"																	<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Initiation"																<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\tEXE_Bline_cntr = 0;"														<< std::endl;
	//files_out	<< "\tfor (int pln = 0; pln < " << Ava_Planes << "; pln ++)"					<< std::endl;
	//files_out	<< "\t\tfor (int vlt = 0; vlt < " << Ava_Vaults << "; vlt ++)"					<< std::endl;
	//files_out	<< "\t\t\tPE_BaseLine_Done_Cntr[pln][vlt] = 0;"									<< std::endl << std::endl;
	files_out	<< "\tfor (int vlt = 0; vlt < " << Ava_Vaults << "; vlt++)"						<< std::endl;
	files_out	<< "\t\tDMA_handler_cntr[vlt] = 0;"												<< std::endl << std::endl;
	//files_out	<< "\tECB = new (ECB_storage) EventCallBacker_I();"								<< std::endl << std::endl;
	files_out	<< "\tfuncPtrArray[0]  = &intr_PC;"												<< std::endl;
	files_out	<< "\tfuncPtrArray[1]  = &intr_TBE;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[2]  = &intr_RBF;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[3]  = &intr_TXD;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[4]  = &intr_RXD;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[5]  = &intr_Timer;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[6]  = &intr_MPDR;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[7]  = &intr_DMA;"											<< std::endl;
	files_out	<< "\tfuncPtrArray[8]  = &intr_PLANE_0;"										<< std::endl;
	files_out	<< "\tfuncPtrArray[9]  = &intr_PLANE_1;"										<< std::endl;
	files_out	<< "\tfuncPtrArray[10] = &intr_PLANE_2;"										<< std::endl;
	files_out	<< "\tfuncPtrArray[11] = &intr_PLANE_3;"										<< std::endl << std::endl << std::endl;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	System Control					"										<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\tCONT_REG_DRAM_connect();"													<< std::endl;
	files_out	<< "\tfor (unsigned int pln = 0; pln < " << Ava_Planes << "; pln++)"			<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tCONT_REG_ACC_Plane_reset (pln);"										<< std::endl;
	files_out	<< "\t\tCONT_REG_ACC_Plane_normal(pln);"										<< std::endl;
	files_out	<< "\t}"																		<< std::endl << std::endl << std::endl;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Interrupt Handlre Configuration	"										<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\//\\/\\/\\/\\*/ "							<< std::endl;
	files_out	<< "\tINTH_enable_intr_pc();"													<< std::endl;
	files_out	<< "\tINTH_enable_intr_timer_group	(0x01);"									<< std::endl;
	files_out	<< "\tINTH_enable_intr_DMA_done_group	(0X0000);"								<< std::endl;
	files_out	<< "\tINTH_enable_PSU_done_group		(PLANE_0,	0xFFFF);"					<< std::endl;
	if (Ava_Planes > 1)
		files_out << "\tINTH_enable_PSU_done_group		(PLANE_1,	0xFFFF);"					<< std::endl;
	if (Ava_Planes > 2)
		files_out << "\tINTH_enable_PSU_done_group		(PLANE_2,	0xFFFF);"					<< std::endl;
	if (Ava_Planes > 3)
		files_out << "\tINTH_enable_PSU_done_group		(PLANE_3,	0xFFFF);"					<< std::endl;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Timer"																	<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\t//timer is set to generate interrupts every 1 ms"							<< std::endl;
	files_out	<< "\tTIMER_config(0, " << (Clock_Period/10000)-1 << ", 10, 1);"				<< std::endl;
	files_out	<< "\tTIMER_start	(0);"														<< std::endl << std::endl << std::endl;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Execution"																<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
	{
		if (!CG_PEs[lvl].empty())
			files_out << "\tPlatform_Execute_Layer_" << lvl << " ();"							<< std::endl;
		if (!CG_MPDRs[lvl].empty())
			files_out << "\tPlatform_Peripheral_Layer_" << lvl << " ();"						<< std::endl;
	}
	files_out	<< "\t"																			<< std::endl << std::endl;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	This Is The End"														<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\twhile(true){}"															<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
}


// Emit Platform_Execute_Layer_<lvl> wrapper that triggers each baseline.
// - Resets baseline done counters and calls each baseline function.
void Code_Generator::Generate_Platform_Execute_Layer_lvl	(Data_Logger* DataL, std::ofstream& files_out, size_t lvl)
{
	files_out	<< "void Platform_Execute_Layer_" << lvl << "()"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t//"																		<< std::endl;
	files_out	<< "\t//	Platform Execute Layer " << lvl										<< std::endl;
	files_out	<< "\t//"																		<< std::endl;
	files_out	<< "\tfor (unsigned int bls=0; bls < Capacity_lvl_" << lvl << "; bls++)"		<< std::endl;
	files_out	<< "\t\tPlatform_Execute_BseLine(EXE_Bline_cntr++);"							<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
}


// Emit Platform_Execute_Layer_<lvl>_bl_<bline> for one baseline.
// - Gathers PZero inputs and emits DMA setup/transfers for zero blocks.
// - Initializes STA/UPA addressing for outputs/accumulators/weights/inputs.
// - Emits PE configs, event counters, and PE control sequences.
// - Waits for DMA and execution completion.
void Code_Generator::Generate_Platform_Execute_Layer_bl		(Data_Logger* DataL, std::ofstream& files_out, size_t lvl, size_t bline)
{
	size_t bl = bline;

	files_out	<< "void Platform_Execute_Layer_" << lvl << "_bl_" << bl << "()"				<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tEXE_Done_Cntr = 0;"														<< std::endl << std::endl;
	//files_out	<< "\tfor (int pln = 0; pln < 4; pln ++)"										<< std::endl;
	//files_out	<< "\t\tfor (int vlt = 0; vlt < 16; vlt ++)"									<< std::endl;
	//files_out	<< "\t\t\tPE_BaseLine_Done_Cntr[pln][vlt] = 0;"									<< std::endl << std::endl;



	// if there were some zero blocks
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"							<< std::endl;
	files_out	<< "\t	Transfering Zero Blocks"	 											<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "							<< std::endl;
	files_out	<< "\tDMA_Done_Cntr = 0;"														<< std::endl;
	files_out	<< "\tif (DMA_max_thread_lvl_" << lvl << "_bl_" << bl << " > 0)"				<< std::endl;
	files_out	<< "\t\tBline_DMA_ZDB_Transfer\t("												<< std::endl;
	files_out	<< "\t\t\t\t\t  DMA_ZDB_Control_lvl_" << lvl << "_bl_" << bl << ");"			<< std::endl << std::endl << std::endl;
	


	// initialization of STA and UPA
	Generage_Data_Blocks_Exe_lvl_bline(DataL, lvl, bline);
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Initialization of STA and UPA"	 										<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< std::dec;
	files_out	<< "\tBline_Initiate_STA_UPA(\t Capacity_lvl_" << lvl << "_bl_" << bl << ","	<< std::endl;
	files_out	<< "\t					 Control_word_lvl_" << lvl << "_bl_" << bl << ","		<< std::endl;
	files_out	<< "\t						   Counts_lvl_" << lvl << "_bl_" << bl << ","		<< std::endl;
	files_out	<< "\t							Ivals_lvl_" << lvl << "_bl_" << bl << ","		<< std::endl;
	files_out	<< "\t			UPA_Inp_base_addr_ptr_lvl_" << lvl << "_bl_" << bl << ","		<< std::endl;
	files_out	<< "\t			UPA_Wgt_base_addr_ptr_lvl_" << lvl << "_bl_" << bl << ","		<< std::endl;
	files_out	<< "\t			UPA_Out_base_addr_ptr_lvl_" << lvl << "_bl_" << bl << ","		<< std::endl;
	files_out	<< "\t			UPA_Acc_base_addr_ptr_lvl_" << lvl << "_bl_" << bl << ");"		<< std::endl;
	files_out	<< "\t"																			<< std::endl << std::endl;


	// Config Holder 
	// TODO: optimize this 
	files_out	<< std::dec;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Config Holder "	 														<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\tfor (size_t pln = 0; pln < " << Ava_Planes << "; pln++)"					<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tfor (size_t vlt = 0; vlt < " << Ava_Vaults << "; vlt++)"				<< std::endl;
	files_out	<< "\t\t\tCONFH_set_conf(pln, vlt, Conf_PE_lvl_" << lvl << "_bl_" << bl << ");"	<< std::endl;
	files_out	<< "\t\tCONFH_refresh (pln); "													<< std::endl;
	files_out	<< "\t}"																		<< std::endl << std::endl << std::endl;


	// Plannar Event Counter Configuration
	files_out	<< std::dec;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Plannar EC Configuration"	 											<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	//files_out	<< "\t//	E_PSU_EC_target:"													<< std::endl;
	//files_out	<< "\t//			0	->	PEs"												<< std::endl;
	//files_out	<< "\t//			1	->	STA"												<< std::endl;
	//files_out	<< "\t//			2	->	UPA"												<< std::endl;
	files_out	<< "\tPSU_EC_reset_all_STAs	(PLANE_0);"											<< std::endl;
	files_out	<< "\tPSU_EC_reset_all_STAs	(PLANE_1);"											<< std::endl;
	files_out	<< "\tPSU_EC_reset_all_STAs	(PLANE_2);"											<< std::endl;
	files_out	<< "\tPSU_EC_reset_all_STAs	(PLANE_3);"											<< std::endl;
	files_out	<< "\tBline_Initiate_STA_ECs(\t Capacity_lvl_" << lvl << "_bl_" << bl << ","	<< std::endl;
	files_out	<< "\t					 Control_word_lvl_" << lvl << "_bl_" << bl << ");"		<< std::endl << std::endl << std::endl;
	

	// Waiting for Completion of DMA transfers
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Waiting for Completion of DMA transfers" << std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\twhile(DMA_Done_Cntr < DMA_max_thread_lvl_" << lvl << "_bl_" << bl << ")"	<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tECB->Chk_CallBack();"													<< std::endl;
	files_out	<< "\t}"																		<< std::endl << std::endl;
	

	// Plannar Processing Element Control
	files_out	<< std::dec;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Plannar Processing Element Control"	 									<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\tBline_Initiate_PE_Start( Capacity_lvl_"	<< lvl << "_bl_" << bl << ","	<< std::endl;
	files_out	<< "\t	 Control_word_lvl_"						<< lvl << "_bl_" << bl << ", "	<< std::endl;
	files_out	<< "\t		 STA_info_lvl_"						<< lvl << "_bl_" << bl << ");"	<< std::endl << std::endl << std::endl;


	// Wait for Completion of this baseline 
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Wait for Completion"	 												<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\twhile(EXE_Done_Cntr < " << CG_PEs[lvl][bline].size() << ") {}"			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
}


// Emit Platform_Execute_BseLine for all baselines.
void Code_Generator::Generate_Platform_Execute_BseLine		(std::ofstream& files_out)
{
	files_out	<< "void Platform_Execute_BseLine (unsigned int bline)"							<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tEXE_Done_Cntr = 0;"														<< std::endl << std::endl;



	// if there were some zero blocks
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"							<< std::endl;
	files_out	<< "\t	Transfering Zero Blocks"	 											<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "							<< std::endl;
	files_out	<< "\tDMA_Done_Cntr = 0;"														<< std::endl;
	files_out	<< "\tif (DMA_max_thread[bline] > 0)"											<< std::endl;
	files_out	<< "\t\tBline_DMA_ZDB_Transfer(\t  DMA_ZDB_Control[bline]);"					<< std::endl << std::endl << std::endl;
	


	// initialization of STA and UPA
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Initialization of STA and UPA"	 										<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< std::dec;
	files_out	<< "\tBline_Initiate_STA_UPA(\t\t\t\t Capacity[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t					 Control_word[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t						   Counts[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t							Ivals[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t			UPA_Inp_base_addr_ptr[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t			UPA_Wgt_base_addr_ptr[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t			UPA_Out_base_addr_ptr[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t			UPA_Acc_base_addr_ptr[bline]);"							<< std::endl << std::endl << std::endl;


	// Config Holder 
	// TODO: optimize this, adding per PE info 
	files_out	<< std::dec;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Config Holder "	 														<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\tfor (size_t pln = 0; pln < " << Ava_Planes << "; pln++)"					<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tfor (size_t vlt = 0; vlt < " << Ava_Vaults << "; vlt++)"				<< std::endl;
	files_out	<< "\t\t\tCONFH_set_conf(pln, vlt, Conf_PE[bline]);"	<< std::endl;
	files_out	<< "\t\tCONFH_refresh (pln); "													<< std::endl;
	files_out	<< "\t}"																		<< std::endl << std::endl << std::endl;


	// Plannar Event Counter Configuration
	files_out	<< std::dec;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Plannar EC Configuration"	 											<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	//files_out	<< "\t//	E_PSU_EC_target:"													<< std::endl;
	//files_out	<< "\t//			0	->	PEs"												<< std::endl;
	//files_out	<< "\t//			1	->	STA"												<< std::endl;
	//files_out	<< "\t//			2	->	UPA"												<< std::endl;
	files_out	<< "\tPSU_EC_reset_all	(PLANE_0);"												<< std::endl;
	files_out	<< "\tPSU_EC_reset_all	(PLANE_1);"												<< std::endl;
	files_out	<< "\tPSU_EC_reset_all	(PLANE_2);"												<< std::endl;
	files_out	<< "\tPSU_EC_reset_all	(PLANE_3);"												<< std::endl;
	files_out	<< "\tBline_Initiate_STA_ECs(\t\t\t\t Capacity[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t\t\t			 Control_word[bline]);"								<< std::endl << std::endl << std::endl;
	

	// Waiting for Completion of DMA transfers
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Waiting for Completion of DMA transfers"								<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\twhile(DMA_Done_Cntr < DMA_max_thread[bline])	{}"							<< std::endl << std::endl << std::endl;
	

	// Plannar Processing Element Control
	files_out	<< std::dec;
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Plannar Processing Element Control"	 									<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\tBline_Initiate_PE_Start(\t\t\t Capacity[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t					 Control_word[bline],"							<< std::endl;
	files_out	<< "\t\t\t\t						 STA_info[bline]);"							<< std::endl << std::endl << std::endl;


	// Wait for Completion of this baseline 
	files_out	<< "\t/*\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\"		<< std::endl;
	files_out	<< "\t	Wait for Completion"	 												<< std::endl;
	files_out	<< "\t/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/*/ "		<< std::endl;
	files_out	<< "\twhile(EXE_Done_Cntr < Capacity[bline]) {}"								<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
}


// Emit Platform_Peripheral_Layer_<lvl> to configure and start MPDR units.
// - Groups baselines by vault to avoid conflicts.
// - For each group, configures MPDR counters and addressing, then starts MPDR.
// - Uses NETWORK maxes to pass column/channel limits.
// - Waits for PRI_Done_Cntr; uneven-spacing path is not implemented.
void Code_Generator::Generate_Platform_Peripheral_Layer_lvl	(Dependency_Logger* DpndL, Data_Logger* DataL, NETWORK* network, std::ofstream& files_out, size_t lvl)
{
	files_out	<< "void Platform_Peripheral_Layer_" << lvl << "()"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t//"																		<< std::endl;
	files_out	<< "\t//	Platform Peripheral Layer " << lvl									<< std::endl;
	files_out	<< "\t//"																		<< std::endl;

	std::vector<std::vector<size_t>> All_BLs;
	size_t Bl_cntr(0);
	while (Bl_cntr != CG_MPDRs[lvl].size())
	{
		std::vector<size_t> BLs;
		bool working[16] = {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

		for (size_t Bline = 0; Bline < CG_MPDRs[lvl].size(); Bline++)
		{
			if (CG_MPDRs[lvl][Bline][0].gen)
				continue;
			size_t vlt = CG_MPDRs[lvl][Bline][0].Vault;
			if (working[vlt])
				continue;
			CG_MPDRs[lvl][Bline][0].gen = true;
			BLs.push_back(Bline);
			working[vlt] = true;
			Bl_cntr++;
		}

		if (BLs.size() > 0)
			All_BLs.push_back(BLs);
	}
	Generage_Data_Blocks_Peri_lvl(DpndL, DataL, network, lvl, All_BLs);
	
	
	files_out	<< "\tfor (unsigned int bl = 0; bl < MPDR_Baseline_Count_lvl_" << lvl << "; bl++)"	<< std::endl;
	files_out	<< std::dec;
	files_out	<< "\t{"																			<< std::endl;
	files_out	<< "\t\tPRI_Done_Cntr = 0;"															<< std::endl;
	files_out	<< "\t\tunsigned int Max_count = MPDR_Thread_Counts_lvl_"	<< lvl << "[bl];"		<< std::endl << std::endl;
	files_out	<< "\t\tBline_Initiate_MPDR(\t\t   Max_count,"										<< std::endl;
	files_out	<< "\t\t		  MPDR_Control_word_H_lvl_"					<< lvl << "[bl],"		<< std::endl;
	files_out	<< "\t\t		  MPDR_Control_word_L_lvl_"					<< lvl << "[bl],"		<< std::endl;
	files_out	<< "\t\t		   MPDR_base_addr_ptr_lvl_"					<< lvl << "[bl]);"		<< std::endl;
	files_out	<< "\t\t"																			<< std::endl;
	files_out	<<	"\t\twhile ( PRI_Done_Cntr != Max_count ) {}"									<< std::endl;
	files_out	<< "\t}"																			<< std::endl;
	files_out	<< "}"																				<< std::endl << std::endl << std::endl;
}


// Emit the tail of generated main.cpp.
// - intr_* handlers update counters and trigger PE restarts.
// - DMA_CallBack schedules the next transfer and re-registers itself.
// - EXT_INT handlers dispatch via the interrupt vector table.
void Code_Generator::Generate_Main_P2						(std::ofstream& files_out)
{
	files_out	<< "void intr_PC		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t// Host Requested an interrupt"											<< std::endl;
	files_out	<< "\t// ok, what does it want to do?"											<< std::endl;
	files_out	<< "\t// i guasee it is open to furture developments"							<< std::endl;
	files_out	<< "\tPC_INT_Cntr++;"															<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_TBE		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_RBF		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tRecieve_Buffer_Full_interrupt_handler();"									<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_TXD		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_RXD		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_Timer		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t// Once more, I DO NOT know what to do."									<< std::endl;
	files_out	<< "\t// just generate it because i can generate it"							<< std::endl;
	files_out	<< "\t// ok, let increment a counter :D "										<< std::endl;
	files_out	<< "\tif (++TIMER_INT_Cntr == 1000)"											<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tTIMER_INT_Cntr = 0;"													<< std::endl;
	files_out	<< "\t\tTIMER_INT_Cntr_Sec++;"													<< std::endl;
	files_out	<< "\t}"																		<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_MPDR		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tPRI_Done_Cntr++;"															<< std::endl;
	files_out	<< "\tMPDR_Stop(intr_addr);"													<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_DMA		(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\t// Nothing yet"															<< std::endl;
	files_out	<< "\tint which = intr_addr - 16;"												<< std::endl;
	files_out	<< "\tDMA_CallBack(DMA_ZDBT_info[which]);"										<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_PLANE_0	(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tint which = intr_addr - 32;"												<< std::endl;
	files_out	<< "\tint unit  = which / 16;	"												<< std::endl;
	files_out	<< "\tint pe_add= which % 16;	"												<< std::endl;
	files_out	<< "\tif(unit == STA)"															<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tEXE_Done_Cntr++;"														<< std::endl;
	files_out	<< "\t\tPE_CONT_PE_start(PLANE_0, pe_add);"										<< std::endl;
	files_out	<< "\t}"																		<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_PLANE_1	(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tint which = intr_addr - 32;"												<< std::endl;
	files_out	<< "\tint unit  = which / 16;	"												<< std::endl;
	files_out	<< "\tint pe_add= which % 16;	"												<< std::endl;
	files_out	<< "\tif(unit == STA)"															<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tEXE_Done_Cntr++;"														<< std::endl;
	files_out	<< "\t\tPE_CONT_PE_start(PLANE_1, pe_add);"										<< std::endl;
	files_out	<< "\t}"																		<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_PLANE_2	(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tint which = intr_addr - 32;"												<< std::endl;
	files_out	<< "\tint unit  = which / 16;	"												<< std::endl;
	files_out	<< "\tint pe_add= which % 16;	"												<< std::endl;
	files_out	<< "\tif(unit == STA)"															<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tEXE_Done_Cntr++;"														<< std::endl;
	files_out	<< "\t\tPE_CONT_PE_start(PLANE_2, pe_add);"										<< std::endl;
	files_out	<< "\t}"																		<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void intr_PLANE_3	(unsigned int intr_addr)"								<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tint which = intr_addr - 32;"												<< std::endl;
	files_out	<< "\tint unit  = which / 16;	"												<< std::endl;
	files_out	<< "\tint pe_add= which % 16;	"												<< std::endl;
	files_out	<< "\tif(unit == STA)"															<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tEXE_Done_Cntr++;"														<< std::endl;
	files_out	<< "\t\tPE_CONT_PE_start(PLANE_3, pe_add);"										<< std::endl;
	files_out	<< "\t}"																		<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void DMA_CallBack	(unsigned int cntrol_word_ptr)"							<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tunsigned int* control = (unsigned int*) cntrol_word_ptr;"					<< std::endl;
	files_out	<< "\tunsigned int* Array   = (unsigned int*)(cntrol_word_ptr+1);"				<< std::endl;
	files_out	<< "\t*control++;"																<< std::endl;
	files_out	<< "\tunsigned int cnt = (*control >> 24);"										<< std::endl;
	files_out	<< "\tunsigned int vlt = (*control >> 20)	& 0xF;"								<< std::endl;
	files_out	<< "\tunsigned int top = (*control >> 10)	& 0x3FF;"							<< std::endl;
	files_out	<< "\tunsigned int idx = (*control >> 0)	& 0x3FF;"							<< std::endl;
	files_out	<< "\tDMA_start_transfer	(vlt,	vlt * 0x1000000,	Array[idx],	cnt);"		<< std::endl;
	files_out	<< "\tif (idx == top)"															<< std::endl;
	files_out	<< "\t{"																		<< std::endl;
	files_out	<< "\t\tDMA_Done_Cntr++;"														<< std::endl;
	files_out	<< "\t\t*control = *control & 0x3FF;"											<< std::endl;
	files_out	<< "\t\tINTH_disable_intr_DMA_done(vlt);;"										<< std::endl;
	files_out	<< "\t\treturn;"																<< std::endl;
	files_out	<< "\t}"																		<< std::endl;
	files_out	<< "\t// Enabling the interrupt "												<< std::endl;
	files_out	<< "\tDMA_EC_reset(vlt);"														<< std::endl;
	files_out	<< "\tDMA_EC_CNTR_config(vlt, 1, 1, 0, 1);"										<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void EXT_INT_0_handler()"													<< std::endl;
	files_out	<< "{"																			<< std::endl;
	files_out	<< "\tunsigned int this_intr_addr;"												<< std::endl;
	files_out	<< "\tunsigned int this_intr_code;"												<< std::endl;
	files_out	<< "\tINTH_get_intr_address(this_intr_addr, this_intr_code);"					<< std::endl;
	files_out	<< "\tfuncPtrArray[this_intr_code](this_intr_addr);"							<< std::endl;
	files_out	<< "}"																			<< std::endl << std::endl << std::endl;
	files_out	<< "void EXT_INT_1_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_2_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_3_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_4_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_5_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_6_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_7_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_8_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_9_handler(){}"													<< std::endl;
	files_out	<< "void EXT_INT_10_handler(){}"												<< std::endl;
	files_out	<< "void EXT_INT_11_handler(){}"												<< std::endl;
	files_out	<< "void EXT_INT_12_handler(){}"												<< std::endl;
	files_out	<< "void EXT_INT_13_handler(){}"												<< std::endl;
	files_out	<< "void EXT_INT_14_handler(){}"												<< std::endl;
	files_out	<< "void EXT_INT_15_handler(){}"												<< std::endl << std::endl << std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
	files_out	<< "\t"																			<< std::endl;
}


// adding data to the Data.h and making its array
void Code_Generator::Generage_Data_Blocks_Exe_lvl_bline		(Data_Logger* DataL, size_t lvl, size_t bline)
{
	// initialization, node_count
	size_t node_count = CG_PEs[lvl][bline].size();


	// initialization, Zero Data Block Array
	size_t PZmax(0);
	size_t max_vlt(0);
	std::vector<DBID_t> PZeros[Ava_Vaults];
	for (size_t node = 0; node < CG_PEs[lvl][bline].size(); node++)
		for (size_t idx = 0; idx < CG_PEs[lvl][bline][node].Inputs_ID.size(); idx++)
			if (DataL->Get_Type_of_DBID(CG_PEs[lvl][bline][node].Inputs_ID[idx]) == DBT_PZero)
				PZeros[DataL->Get_Vault_of_DBID(CG_PEs[lvl][bline][node].Inputs_ID[idx])].push_back(CG_PEs[lvl][bline][node].Inputs_ID[idx]);
	for (size_t vlt = 0; vlt < Ava_Vaults; vlt++)
	{
		PZmax = std::max(PZmax, PZeros[vlt].size());
		max_vlt += (PZeros[vlt].size() > 0);
	}


	// Assertion -> not equally spaced Inp, Out, or Acc
	for (size_t node = 0; node < node_count; node++)
	{
		bool Inp_has_Pattern = CG_PEs[lvl][bline][node].ESI;
		bool Out_has_Pattern = CG_PEs[lvl][bline][node].ESO;
		bool Acc_has_Pattern = CG_PEs[lvl][bline][node].ESP;

		if (!(Out_has_Pattern && Acc_has_Pattern && Inp_has_Pattern))
		{
			std::cout << "\tE: non equally spaced Input, Output or Acc is not implemented!";
			throw("\tE: non equally spaced Input, Output or Acc is not implemented!");
			return;
		}
	}


	// Assertion -> ZDB count per vault 
	if (PZmax >= (size_t)(1 << 10))
	{
		std::cout << "\tE: Number of Zero Data Blocks per vault can not be more than 1022!";
		throw("\tE: Number of Zero Data Blocks per vault can not be more than 1022!");
		return;
	}


	// Node Count of this baseline
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Capacity_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd(lvl, bline) << "\t=\t" << node_count << "; " << std::endl;
	Data_H_file << std::endl;


	// PE Config values
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const S_PE_cofig";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Conf_PE_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd(lvl, bline) << "\t=\t"		<< std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	Data_H_file << "FSM_CNN_1,	16,	FULL,	IUNSIGN,	WUNSIGN,	3,	15,	15,	15";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// STA config values
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const S_CONF_STA_info";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "STA_info_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd(lvl, bline) << "\t=\t"		<< std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl << "\t\t\t\t\t";
	Data_H_file	<< "0,	0,	1,	0,	0,	0,	0,	1,	0,	1, ";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Control Word
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Control_word_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, bline, node_count) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	for (size_t node = 0; node < node_count; node++)
	{
		size_t cnt = CG_PEs[lvl][bline][node].Output_ID.size();
		size_t pln = CG_PEs[lvl][bline][node].Plane;
		size_t vlt = CG_PEs[lvl][bline][node].Vault;
		size_t tmp = (cnt << 16) + (pln << 4) + vlt;
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << tmp << ",\t";
		if ((((node+1) % 8) == 0) && (node != (node_count-1)))
			Data_H_file			<< std::endl	<< "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Counts
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Counts_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, bline, node_count) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	for (size_t node = 0; node < node_count; node++)
	{
		// TODO : Optimize this
		size_t Inp_count = 0xFF;
		size_t Wgt_count = 0xFF;
		size_t Out_count = 0xFF;
		size_t Acc_count = 0xFF;
		size_t tmp		 = (Inp_count << 24) + (Wgt_count << 16) + (Out_count << 8) + Acc_count;
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << tmp << ",\t";
		if ((((node + 1) % 8) == 0) && (node != (node_count - 1)))
			Data_H_file			<< std::endl	<< "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Intervals
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Ivals_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, bline, node_count) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl << "\t\t\t\t\t";
	for (size_t node = 0; node < node_count; node++)
	{
		size_t Inp_Ival = CG_PEs[lvl][bline][node].Inp_Seqnc;
		size_t Wgt_Ival = 0;
		size_t Out_Ival = CG_PEs[lvl][bline][node].Out_Seqnc;
		size_t Acc_Ival = CG_PEs[lvl][bline][node].Acc_Seqnc;
		size_t tmp		= (Inp_Ival << 24) + (Wgt_Ival << 16) + (Out_Ival << 8) + Acc_Ival;
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << tmp << ",\t";
		if ((((node + 1) % 8) == 0) && (node != (node_count - 1)))
			Data_H_file << std::endl << "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Input Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Inp_base_addr_ptr_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, bline, node_count) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl << "\t\t\t\t\t";
	for (size_t node = 0; node < node_count; node++)
	{
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << DataL->GET_EA(CG_PEs[lvl][bline][node].Inputs_ID[0]) << ",\t";
		if ((((node + 1) % 8) == 0) && (node != (node_count - 1)))
			Data_H_file << std::endl << "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Weights Base Address
	Data_H_file << std::dec << std::endl;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Wgt_base_addr_ptr_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array_2D(lvl, bline, node_count, 9) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl;
	for (size_t node = 0; node < node_count; node++)
	{
		Data_H_file	<< "\t\t\t\t{\t";
		for (size_t idx = 0; idx < 9; idx++)
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << DataL->GET_EA(CG_PEs[lvl][bline][node].Weigth_ID[idx]) << ",\t";
		Data_H_file	<< "\t}, "	<< std::endl;
	}
	Data_H_file	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Output Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Out_base_addr_ptr_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, bline, node_count) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl << "\t\t\t\t\t";
	for (size_t node = 0; node < node_count; node++)
	{
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << DataL->GET_EA(CG_PEs[lvl][bline][node].Output_ID[0]) << ",\t";
		if ((((node + 1) % 8) == 0) && (node != (node_count - 1)))
			Data_H_file << std::endl << "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;


	// Accumulate Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Acc_base_addr_ptr_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, bline, node_count) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl << "\t\t\t\t\t";
	for (size_t node = 0; node < node_count; node++)
	{
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << DataL->GET_EA(CG_PEs[lvl][bline][node].Acum_DBID[0]) << ",\t";
		if ((((node + 1) % 8) == 0) && (node != (node_count - 1)))
			Data_H_file << std::endl << "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;

	
	// building the trasfer array, if there were some zero blocks
	Data_H_file	<< std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "DMA_max_thread_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd(lvl, bline) << "\t=\t" << max_vlt << "; " << std::endl;
	Data_H_file << std::endl;
	if (PZmax > 0)
	{
		for (size_t vlt = 0; vlt < Ava_Vaults; vlt++)
		{
			if (PZeros[vlt].size() > 0)
			{
				Data_H_file << std::dec;
				Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
				Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Zero_Block_Write_addr_lvl_"; 
				Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_dma_array(lvl, bline, vlt, PZeros[vlt].size()) << "\t=\t" << std::endl;
				Data_H_file << "\t\t\t{"	<< std::endl << "\t\t\t\t\t";
				for (size_t idx = 0; idx < PZeros[vlt].size(); idx++)
				{
					Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << DataL->GET_EA(PZeros[vlt][idx]) << ",\t";
					if ((((idx + 1) % 8) == 0) && (idx != (PZeros[vlt].size() - 1)))
						Data_H_file << std::endl << "\t\t\t\t\t";
				}
				Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;
			}
		}
		Data_H_file << std::dec;
		Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
		Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "DMA_ZDB_Control_lvl_";
		Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array_2D(lvl, bline, Ava_Vaults, 2) << "\t=\t" << std::endl;
		Data_H_file << "\t\t\t{" << std::endl << "\t\t\t\t";
		for (size_t vlt = 0; vlt < Ava_Vaults; vlt++)
		{
			// TODO: Optimize data transfer count
			size_t Cnt = Block_size - 1;
			size_t Max = PZeros[vlt].size();
			size_t tmp = (Cnt << 24) + (vlt << 20) + (Max << 10);
			Data_H_file << "{\t" << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << tmp << ",\t";
			if (PZeros[vlt].size() > 0)
				Data_H_file << "(unsigned int)" << std::setw(44) << std::setfill(' ') << ("Zero_Block_Write_addr_lvl_" + std::to_string(lvl) + "_bl_" + std::to_string(bline) + "_DMA_" + std::to_string(vlt));
			else
				Data_H_file << "(unsigned int)" << std::setw(44) << std::setfill(' ') <<  "nullptr";
			Data_H_file << "},\t";
			if ((((vlt + 1) % 2) == 0) && (vlt != (Ava_Vaults - 1)))
				Data_H_file << std::endl << "\t\t\t\t";
		}
		Data_H_file << std::endl << "\t\t\t};" << std::endl << std::endl;
	}
	else
	{
		Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
		Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "DMA_ZDB_Control_lvl_";
		Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array_2D(lvl, bline, 1, 2) << "\t=\t{};" << std::endl << std::endl;
	}
}


// adding necessary data for peripheral part to the Data.h and making its array
void Code_Generator::Generage_Data_Blocks_Peri_lvl			(Dependency_Logger* DpndL, Data_Logger* DataL, NETWORK* network, size_t lvl, std::vector<std::vector<size_t>> All_BLs)
{
	// number of Baselines within this level
	Data_H_file	<< std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "MPDR_Baseline_Count_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << lvl << "\t=\t" << All_BLs.size() << "; " << std::endl;
	Data_H_file << std::endl;




	// number of threads per baselines 
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "MPDR_Thread_Counts_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, All_BLs.size()) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << All_BLs[bl].size() << ",\t";
		if ((((bl + 1) % 8) == 0) && (bl != (All_BLs.size() - 1)))
			Data_H_file			<< std::endl	<< "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;




	// Control word High
	//			field		:	vlt			Col			chan,		rounds		Out_Ival
	//			Hex pos		:	00,			0,			0,			00,			00
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		// If the CG_MPDRs' input does not evenly spaced in the allocation phase, it's main reason is that the last layer is MPDR
		if (!CG_MPDRs[lvl][All_BLs[bl][0]][0].ESI)
		{
			// TODO: implement this
			std::cout << "\tError: Uneven spatial spacing on input blocks is not supported yet," << std::endl;
			throw("Error: Uneven spatial spacing on input blocks is not supported yet");
		}

		Data_H_file << std::dec;
		Data_H_file << std::setfill(' ')	<< std::left	<< std::setw(24)	<< "const unsigned int";
		Data_H_file << std::setfill(' ')	<< std::right	<< std::setw(32)	<< "MPDR_Control_word_H_lvl_";
		Data_H_file << std::setfill(' ')	<< std::left	<< std::setw(28)	<< embedd_array(lvl, bl, All_BLs[bl].size()) << "\t=\t";
		Data_H_file << std::endl			<< "\t\t\t{"	<< std::endl		<< "\t\t\t\t\t";
		for (size_t idx = 0; idx < All_BLs[bl].size(); idx++)
		{
			size_t Bline	= All_BLs[bl][idx];
			size_t layer	= DpndL->Get_Layer_of(CG_MPDRs[lvl][Bline][0].Node);
			auto Maxes_info = network->Get_Maxes_info(layer);
			size_t vlt		= CG_MPDRs[lvl][Bline][0].Vault;
			size_t Max_Colm = Maxes_info.Width_size;
			size_t Max_Chan = Maxes_info.Channel_size;
			size_t runs		= CG_MPDRs[lvl][Bline].size();
			size_t Out_Diff	= (CG_MPDRs[lvl][Bline].size() > 1)	?	DataL->Calculate_Space_Between(CG_MPDRs[lvl][Bline][0].Output,		CG_MPDRs[lvl][Bline][1].Output)		:	0;
			
			// Assertion
			if (runs >= (size_t)(1 << 8))
			{
				std::cout << "\tE: Number of MPDR Runs per baseline can not be more than 255!";
				throw("\tE: Number of MPDR Runs per baseline can not be more than 255!");
				return;
			}
			
			//			field		:	vlt		Col		chan,	rounds	Out_Ival
			//			Hex pos		:	00,		0,		0,		00,		00
			size_t tmp	= (vlt << 24) + ((Max_Colm-1) << 20) + ((Max_Chan-1) << 16) + (runs << 8) + Out_Diff;
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << tmp << ",\t";
			if ((((idx + 1) % 8) == 0) && (idx != (All_BLs[bl].size() - 1)))
				Data_H_file << std::endl << "\t\t\t\t\t";
		}
		Data_H_file << std::endl			<< "\t\t\t};"	<< std::endl		<< std::endl;
	}
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "MPDR_Control_word_H_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, All_BLs.size()) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		Data_H_file << std::setfill(' ') << std::right << std::setw(40) << ("MPDR_Control_word_H_lvl_" + embedd(lvl, bl)) << ",\t";
		if ((((bl + 1) % 4) == 0) && (bl != (All_BLs.size() - 1)))
			Data_H_file			<< std::endl	<< "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;




	// Control word Low
	//			field		:	R1C1_Ival	R1C2_Ival	R2C1_Ival	R2C2_Ival
	//			Hex pos		:	00,			00,			00,			00
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		Data_H_file	<< std::dec;
		Data_H_file	<< std::setfill(' ')	<< std::left	<< std::setw(24)	<< "const unsigned int";
		Data_H_file	<< std::setfill(' ')	<< std::right	<< std::setw(32)	<< "MPDR_Control_word_L_lvl_";
		Data_H_file	<< std::setfill(' ')	<< std::left	<< std::setw(28)	<< embedd_array(lvl, bl, All_BLs[bl].size()) << "\t=\t";
		Data_H_file << std::endl			<< "\t\t\t{"	<< std::endl		<< "\t\t\t\t\t";
		for (size_t idx = 0; idx < All_BLs[bl].size(); idx++)
		{
			size_t Bline	= All_BLs[bl][idx];
			size_t In0_Diff	= (CG_MPDRs[lvl][Bline].size() > 1)	?	DataL->Calculate_Space_Between(CG_MPDRs[lvl][Bline][0].Inputs[0],	CG_MPDRs[lvl][Bline][1].Inputs[0])	:	0;
			size_t In1_Diff	= (CG_MPDRs[lvl][Bline].size() > 1)	?	DataL->Calculate_Space_Between(CG_MPDRs[lvl][Bline][0].Inputs[1],	CG_MPDRs[lvl][Bline][1].Inputs[1])	:	0;
			size_t In2_Diff	= (CG_MPDRs[lvl][Bline].size() > 1)	?	DataL->Calculate_Space_Between(CG_MPDRs[lvl][Bline][0].Inputs[2],	CG_MPDRs[lvl][Bline][1].Inputs[2])	:	0;
			size_t In3_Diff	= (CG_MPDRs[lvl][Bline].size() > 1)	?	DataL->Calculate_Space_Between(CG_MPDRs[lvl][Bline][0].Inputs[3],	CG_MPDRs[lvl][Bline][1].Inputs[3])	:	0;
			//			field		:	R1C1_Ival	R1C2_Ival	R2C1_Ival	R2C2_Ival
			//			Hex pos		:	00,			00,			00,			00
			size_t tmp	= (In0_Diff << 24) + (In1_Diff << 16) + (In2_Diff << 8) + In3_Diff;
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << tmp << ",\t";
			if ((((idx + 1) % 8) == 0) && (idx != (All_BLs[bl].size() - 1)))
				Data_H_file << std::endl << "\t\t\t\t\t";
		}
		Data_H_file << std::endl			<< "\t\t\t};"	<< std::endl		<< std::endl;
	}
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "MPDR_Control_word_L_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, All_BLs.size()) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		Data_H_file << std::setfill(' ') << std::right << std::setw(40) << ("MPDR_Control_word_L_lvl_" + embedd(lvl, bl)) << ",\t";
		if ((((bl + 1) % 4) == 0) && (bl != (All_BLs.size() - 1)))
			Data_H_file			<< std::endl	<< "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;




	// Effective Address of DataBlocks
	//			Addr Low	:	Out_EA
	//					+1	:	In0_EA
	//					+2	:	In1_EA
	//					+3	:	In2_EA
	//					+4	:	In3_EA
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		Data_H_file << std::dec;
		Data_H_file << std::setfill(' ')	<< std::left	<< std::setw(24) << "const unsigned int";
		Data_H_file << std::setfill(' ')	<< std::right	<< std::setw(32) << "MPDR_base_addr_ptr_lvl_";
		Data_H_file << std::setfill(' ')	<< std::left	<< std::setw(28) << embedd_array_2D(lvl, bl, All_BLs[bl].size(), 5) << "\t=\t";
		Data_H_file << std::endl			<< "\t\t\t{"	<< std::endl;
		for (size_t idx = 0; idx < All_BLs[bl].size(); idx++)
		{
			size_t Bline	= All_BLs[bl][idx];
			size_t Out_EA	= DataL->GET_EA(CG_MPDRs[lvl][Bline][0].Output);
			size_t In0_EA	= DataL->GET_EA(CG_MPDRs[lvl][Bline][0].Inputs[0]);
			size_t In1_EA	= DataL->GET_EA(CG_MPDRs[lvl][Bline][0].Inputs[1]);
			size_t In2_EA	= DataL->GET_EA(CG_MPDRs[lvl][Bline][0].Inputs[2]);
			size_t In3_EA	= DataL->GET_EA(CG_MPDRs[lvl][Bline][0].Inputs[3]);
			Data_H_file << "\t\t\t\t{\t";
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << Out_EA << ",\t";
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << In0_EA << ",\t";
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << In1_EA << ",\t";
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << In2_EA << ",\t";
			Data_H_file << std::hex << "0x" << std::right << std::setw(8) << std::setfill('0') << In3_EA << "\t}," << std::endl;
		}
		Data_H_file		<< "\t\t\t};"	<< std::endl << std::endl;
	}
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const void*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "MPDR_base_addr_ptr_lvl_";
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(28) << embedd_array(lvl, All_BLs.size()) << "\t=\t" << std::endl;
	Data_H_file << "\t\t\t{"	<< std::endl	<< "\t\t\t\t\t";
	for (size_t bl = 0; bl < All_BLs.size(); bl++)
	{
		Data_H_file << std::setfill(' ') << std::right << std::setw(44) << ("(const void *)MPDR_base_addr_ptr_lvl_" + embedd(lvl, bl)) << ",\t";
		if ((((bl + 1) % 4) == 0) && (bl != (All_BLs.size() - 1)))
			Data_H_file			<< std::endl	<< "\t\t\t\t\t";
	}
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;
}


// adding datablocks
void Code_Generator::Generage_Data_Blocks_Exe_Baseline		(Data_Logger* DataL)
{
	size_t size(0);
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
				size++;



	// Node Count of this baseline
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		if (!CG_PEs[lvl].empty())
		{
			Data_H_file << std::dec;
			Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
			Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Capacity_lvl_" << lvl << "\t=\t" << CG_PEs[lvl].size() << ";" << std::endl << std::endl;
		}

	

	// Node Count of this baseline
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Capacity[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "Capacity_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;
	
	

	// PE Config values
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const S_PE_cofig";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Conf_PE[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "Conf_PE_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;

	

	// STA config values
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const S_CONF_STA_info";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "STA_info[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "STA_info_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Control Word
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Control_word[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "Control_word_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Counts
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Counts[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "Counts_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Intervals
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "Ivals[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "Ivals_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Input Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Inp_base_addr_ptr[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "UPA_Inp_base_addr_ptr_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Weights Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "(*UPA_Wgt_base_addr_ptr[" << std::to_string(size) << "])[9" << "]\t = \t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "UPA_Wgt_base_addr_ptr_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Output Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Out_base_addr_ptr[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "UPA_Out_base_addr_ptr_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// Accumulate Base Address
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int*";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "UPA_Acc_base_addr_ptr[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "UPA_Acc_base_addr_ptr_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// DMA Transfer size
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "DMA_max_thread[" << size << "]\t=\t";
	Data_H_file << std::endl	<< "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
					Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "DMA_max_thread_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl	<< "\t\t\t};"	<< std::endl	<< std::endl;



	// DMA Transfer info
	Data_H_file << std::dec;
	Data_H_file << std::setfill(' ') << std::left	<< std::setw(24) << "const unsigned int";
	Data_H_file << std::setfill(' ') << std::right	<< std::setw(32) << "(*DMA_ZDB_Control[" << size << "])[2" << "]\t = \t";
	Data_H_file << std::endl << "\t\t\t{";
	for (size_t lvl = 0; lvl < CG_PEs.size(); lvl++)
		for (size_t bline = 0; bline < CG_PEs[lvl].size(); bline++)
			if (!CG_PEs[lvl][bline].empty())
				Data_H_file << std::endl << "\t\t\t\t\t" << std::setw(40) << "DMA_ZDB_Control_lvl_" << embedd(lvl, bline) << ",\t";
	Data_H_file << std::endl << "\t\t\t};" << std::endl << std::endl;
}





