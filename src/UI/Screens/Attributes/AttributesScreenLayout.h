#include "Core/Utils/StringConverter.h"
#pragma once

#include "Core/Utils/Appearance.h"

namespace ArtsAttributes {

    inline const std::vector<std::string> sheetLogo = StringConvert::convertRawStringToVector(R"(
███████████  █████   █████████  █████   █████   █████████   
░░███░░░░░█ ░░███   ███░░░░░███░░███   ░░███   ███░░░░░███  
 ░███   █ ░  ░███  ███     ░░░  ░███    ░███  ░███    ░███  
 ░███████    ░███ ░███          ░███████████  ░███████████  
 ░███░░░█    ░███ ░███          ░███░░░░░███  ░███░░░░░███  
 ░███  ░     ░███ ░░███     ███ ░███    ░███  ░███    ░███  
 █████       █████ ░░█████████  █████   █████ █████   █████ 
 ░░░░░       ░░░░░   ░░░░░░░░░  ░░░░░   ░░░░░ ░░░░░   ░░░░░ 
)");

}
