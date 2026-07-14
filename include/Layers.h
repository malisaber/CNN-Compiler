#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include "Utils.h"
#include "Data_Logger.h"
#include "Dependency_Logger.h"


class Layer_2D
{
protected:
	bool Out_Sch_Info_rdy		= false;				// This layer output scheduling info is ready	(Out_*_Map valid).
	bool Prev_Out_Sch_Info_rdy	= false;				// Previous layer output scheduling info is ready	(Out_*_Map_PL valid).
	bool getting_from_input		= false;				// True if previous data originates from input	(affects padding/zero blocks type).

	Conv_Layer_Info Layer_info	= Conv_Layer_Info_NULL;	// Declared logical dimensions for this layer	(original B/H/W/C/K/FH/FW).
	Conv_Layer_Info Maxes_info	= Conv_Layer_Info_NULL;	// Max block sizes used for mapping/tiling this layer	(ceil division basis).
	MaxP_Layer_Info Windo_info	= MaxP_Layer_Info_NULL;	// Pooling/reshape window parameters	(height/width of MP/DR window).
	Conv_Layer_Info map_entity	= Conv_Layer_Info_NULL;	// Computed mapping entity sizes after tiling	(per-map block dimensions).
	
	size_t ID = 0;										// Layer index inside the network	(assigned by NETWORK::base_Add_Layer).

	DBID_t**** Out_Data_Map		= nullptr;				// Output data block IDs by [B][H][W][K] for this layer's produced blocks.
	SNID_t**** Out_Node_Map		= nullptr;				// Output scheduling node IDs by [B][H][W][K] for this layer's produced blocks.
	DBID_t**** Out_Data_Map_PL	= nullptr;				// Previous layer output data block IDs by [B][H][W][K]	(inputs to this layer).
	SNID_t**** Out_Node_Map_PL	= nullptr;				// Previous layer output scheduling node IDs by [B][H][W][K]	(inputs to this layer).
	
	

public:
	bool mapped					= false;				// Mapping already performed	(prevents duplicate mapping).
	
	virtual ~Layer_2D() = default;

	// is input layer
	virtual bool IsInput							()  const;

	// is output layer
	virtual bool IsOutput							() const;

	// is Max pooling, Data Reshape layer
	virtual bool IsMPDR								() const;

	// is Convolution layer
	virtual bool IsConv								() const;

	//	Returning This Layer ID	(index within NETWORK).
	size_t Get_Layer_ID								() const;

	// returning the layer info
	Conv_Layer_Info Get_Layer_info					();

	//	Returning Window Size of this layer	(used by MP/DR or conv wind).
	MaxP_Layer_Info Get_Window_info					() const;

	//	Returning Max block sizes used for mapping this layer.
	Conv_Layer_Info Get_Maxes_info					() const;

	// REQUIRED interface: map data blocks and create scheduling dependencies.
	virtual bool Map_Data_Blocks_and_Build_Dependencies(
														Data_Logger* DataL,
														Dependency_Logger* DpndL) = 0;

	//	No Need To Call This Member
	//	This function returns the scheduling info of the output of this layer:
	//	- PLONM/PLODM: output node/data maps
	//	- map_entity_info: mapped dimensions
	//	- GFI: true if previous data originates from input
	virtual bool Get_output_scheduling_info			(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI) = 0;

	// THIS replaces your template mess
	//	This function Should be called from the next layer
	//	this function will collect the scheduling information of the previous layer 
	//	therefore, it should be called here 
	//	and also it shoulc store it here;
	//	Validates compatibility between previous map entity and this layer's map entity.
	virtual bool Get_previous_output_scheduling_info(
														Layer_2D* prev_layer);
};


class Input_Layer_2D : public Layer_2D
{
public:
	// Build input layer with declared dimensions and max block sizes.
	// Sets map_entity via ceil division and allocates Out_*_Map.
	Input_Layer_2D									(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														size_t layer_id);
	~Input_Layer_2D									();

	// is input layer
	bool IsInput									()  const override;

	//	To map input data blocks and then make the dependency graph.
	bool Map_Data_Blocks_and_Build_Dependencies		(
														Data_Logger* DataL,
														Dependency_Logger* DpndL) override;

	//	This function returns the scheduling info of the output of this layer.
	bool Get_output_scheduling_info					(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI) override;

private:

};


class Conv_MKMC_2D : public Layer_2D
{
public:
	// Build convolution layer with declared dimensions and max block sizes.
	// Computes map_entity, allocates output maps, sets default 3x3 window.
	Conv_MKMC_2D									(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														size_t layer_id);
	~Conv_MKMC_2D									();

	// is Convolution layer
	bool IsConv										() const override;

	//	Map inputs/weights/partials/output and build scheduling dependencies.
	bool Map_Data_Blocks_and_Build_Dependencies		(
														Data_Logger* DataL,
														Dependency_Logger* DpndL) override;
	
	//	This function returns the scheduling info of the output of this layer.
	bool Get_output_scheduling_info					(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI) override;
	
private:

};


class MPDR_MC_2D : public Layer_2D
{
public:
	// Build max-pooling/data-reshape layer with window parameters.
	// Output maps are sized by	(H/WinH, W/WinW).
	MPDR_MC_2D										(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														MaxP_Layer_Info Window,
														size_t layer_id);
	~MPDR_MC_2D										();

	// is Max pooling, Data Reshape layer
	bool IsMPDR										() const override;

	//	Map MP/DR outputs and build scheduling dependencies for windowed inputs.
	bool Map_Data_Blocks_and_Build_Dependencies		(
														Data_Logger* DataL,
														Dependency_Logger* DpndL) override;

	//	This function returns the scheduling info of the output of this layer.
	bool Get_output_scheduling_info					(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI) override;

private:

};


class Output_Layer_2D : public Layer_2D
{
public:
	// Build output layer with declared dimensions and max block sizes.
	// Reuses previous layer's nodes and re-tags data blocks as output.
	Output_Layer_2D									(
														Conv_Layer_Info Linfo,
														Conv_Layer_Info Maxes,
														size_t layer_id);
	~Output_Layer_2D								();

	// is output layer
	bool IsOutput									() const override;

	//	Finalize output data blocks and attach dependencies to the end node.
	bool Map_Data_Blocks_and_Build_Dependencies		(
														Data_Logger* DataL,
														Dependency_Logger* DpndL) override;
	
	//	This function returns the scheduling info of the output of this layer.
	bool Get_output_scheduling_info					(
														SNID_t****& PLONM,
														DBID_t****& PLODM,
														Conv_Layer_Info& map_entity_info,
														bool& GFI) override;
	
private:

};


class NETWORK
{
private:
	std::vector<std::unique_ptr<Layer_2D>> NET;		// Ordered list of layers in the network	(ownership via unique_ptr).
	

public:
	// Construct an empty network with pre-reserved capacity.
	NETWORK();
	~NETWORK										();

	// Add a layer that does not require a custom window	(non-MPDR).
	size_t	Add_Layer								(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes);
	
	// Add a layer with a specific max-pooling window size.
	size_t	Add_Layer								(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes,
														MaxP_Layer_Info MP_window);
	
	// Build data blocks and dependencies across all layers.
	// Enforces: first layer is input, last layer is output, and only one output layer.
	bool	Build_Network							(
														Data_Logger* DataL,
														Dependency_Logger* DpndL);

	// Return maxes info for a given layer.
	Conv_Layer_Info Get_Maxes_info					(
														size_t id);

	// return the input sizes 
	void	Get_input_sizes							(
														std::vector<Conv_Layer_Info>& info,
														std::vector<Conv_Layer_Info>& minfo,
														std::vector<size_t>& lid);

	// return the weight sizes
	void	Get_weight_sizes						(
														std::vector<Conv_Layer_Info>& info,
														std::vector<Conv_Layer_Info>& minfo,
														std::vector<size_t>& lid);

private:
	// Internal layer factory that instantiates the requested derived type.
	size_t base_Add_Layer							(
														Layer_2D_Types ltype,
														Conv_Layer_Info linfo,
														Conv_Layer_Info lmaxes,
														MaxP_Layer_Info MP_window);
};

