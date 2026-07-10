#include "tester.h"

std::vector<Mapping_Result>
CNN_Node_Mapper::MapNodes						   (
														const std::vector<Scheduling_Node>& Nodes)
{
    constexpr uint8_t PLANES = 4;
    constexpr uint8_t VAULTS = 16;

    std::vector<Mapping_Result> results;
    results.reserve(Nodes.size());

    // round-robin plane per vault
    uint8_t next_plane[VAULTS] = { 0 };

    // cache execution group -> assigned PE
    std::unordered_map<ExecKey, std::pair<uint8_t, uint8_t>, ExecHasher> exec_to_pe;

    for (const auto& node : Nodes)
    {
        if (node.type == Sch_Null)
            continue;

        const auto& cli = node.CLI;

        WeightKey wkey{ cli.Kernel_size, cli.Channel_size,
                       cli.FiltH_Size, cli.FiltW_Size };

        size_t vault = WeightHasher{}(wkey) % VAULTS;
        uint8_t pe_index = static_cast<uint8_t>(vault);

        ExecKey ekey{ cli.Batch_size, cli.Kernel_size,
                     cli.Channel_size, cli.Width_size,
                     cli.FiltH_Size, cli.FiltW_Size };

        uint8_t plane;

        auto it = exec_to_pe.find(ekey);
        if (it == exec_to_pe.end())
        {
            plane = next_plane[vault]++ % PLANES;
            exec_to_pe.emplace(ekey, std::make_pair(plane, pe_index));
        }
        else
        {
            plane = it->second.first;
        }

        results.push_back({
            node.ID,
            plane,
            pe_index,
            static_cast<uint8_t>(vault)
            });
    }

    return results;
}