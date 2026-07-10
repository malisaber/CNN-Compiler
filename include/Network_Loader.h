#pragma once

#include <unordered_map>
#include <filesystem>
#include <stdexcept>
#include <fstream>
#include "json.hpp"
#include "Compiler.h"

void BuildNetworkFromFile(const std::filesystem::path& path, Compiler& compiler);