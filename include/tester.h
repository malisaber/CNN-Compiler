#pragma once
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "Types.h"
#include "Utils.h"



class CNN_Node_Mapper
{
public:
    static std::vector<Mapping_Result>
        MapNodes(const std::vector<Scheduling_Node>& Nodes);

private:
    struct WeightKey
    {
        size_t K, C, FH, FW;

        bool operator==(const WeightKey& other) const
        {
            return K == other.K && C == other.C &&
                FH == other.FH && FW == other.FW;
        }
    };

    struct ExecKey
    {
        size_t B, K, C, W, FH, FW;

        bool operator==(const ExecKey& o) const
        {
            return B == o.B && K == o.K && C == o.C &&
                W == o.W && FH == o.FH && FW == o.FW;
        }
    };

    struct WeightHasher
    {
        size_t operator()(const WeightKey& k) const noexcept
        {
            return std::hash<size_t>()(k.K) ^
                (std::hash<size_t>()(k.C) << 1) ^
                (std::hash<size_t>()(k.FH) << 2) ^
                (std::hash<size_t>()(k.FW) << 3);
        }
    };

    struct ExecHasher
    {
        size_t operator()(const ExecKey& k) const noexcept
        {
            return std::hash<size_t>()(k.B) ^
                (std::hash<size_t>()(k.K) << 1) ^
                (std::hash<size_t>()(k.C) << 2) ^
                (std::hash<size_t>()(k.W) << 3) ^
                (std::hash<size_t>()(k.FH) << 4) ^
                (std::hash<size_t>()(k.FW) << 5);
        }
    };
};