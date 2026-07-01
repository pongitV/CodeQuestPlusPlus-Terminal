#include "../../../Core/Utilidades/ConversorString.h"
#pragma once

#include "../../../Core/Utilidades/Aparencia.h"

namespace ArtesAtributos {

    inline const std::vector<std::string> logoFicha = ConversorString::converterStringBrutaParaVetor(R"(
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
