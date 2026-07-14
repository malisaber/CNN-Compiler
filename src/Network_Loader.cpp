#include "Network_Loader.h"

using json = nlohmann::json;

namespace {

Conv_Layer_Info ParseConvInfo(const json& j)
{
    return Conv_Layer_Info{
        j.at("B" ).get<unsigned int>(),  j.at("K").get<unsigned int>(),  j.at("C").get<unsigned int>(),
        j.at("W" ).get<unsigned int>(),  j.at("H").get<unsigned int>(),
        j.at("F1").get<unsigned int>(), j.at("F2").get<unsigned int>()
    };
}

const std::unordered_map<std::string, int>& LayerTypeMap()
{
    static const std::unordered_map<std::string, int> m = {
        {"input",   L2D_Input},
        {"conv",    L2D_CONV_MKMC},
        {"maxpool", L2D_MPDR_MC},
        {"output",  L2D_Output}
    };
    return m;
}

} // namespace

void BuildNetworkFromFile							(
														const std::filesystem::path& path,
														Compiler& compiler)
{
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("Cannot open network file: " + path.string());

    json j;
    try {
        in >> j;
    } catch (const json::parse_error& e) {
        throw std::runtime_error("JSON parse error in " + path.string() + ": " + e.what());
    }

    Conv_Layer_Info Maxes  = ParseConvInfo(j.at("maxes"));
    MaxP_Layer_Info Window = { j.at("window").at("w").get<unsigned int>(),
                                j.at("window").at("h").get<unsigned int>() };

    const auto& typeMap = LayerTypeMap();

    for (const auto& lj : j.at("layers")) {
        std::string type = lj.at("type").get<std::string>();
        auto it = typeMap.find(type);
        if (it == typeMap.end())
            throw std::runtime_error("Unknown layer type '" + type + "' in " + path.string());

        Conv_Layer_Info info = ParseConvInfo(lj);
        if (it->second == L2D_MPDR_MC)
            compiler.Add_Layer_to_Notwork(static_cast<Layer_2D_Types>(it->second), info, Maxes, Window);
        else
            compiler.Add_Layer_to_Notwork(static_cast<Layer_2D_Types>(it->second), info, Maxes);
    }
}