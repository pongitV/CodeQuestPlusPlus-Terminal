#include "Dwarf.h"

#include <iostream>

#include "../Utilidades/GeradorAleatorio.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Dwarf::obterNomeRaca() const
{
    return "Dwarf";
}

const std::vector<std::string>& Dwarf::obterAparenciaRaca() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                     =%%#*#%                   ",
        "                   .%%%#**#%%=                 ",
        "                   %%+==-===*%:                ",
        "                  =%%%@@%%@%%@@                ",
        "                  +%*+==++===%#                ",
        "                  *%@#+#@@***%*                ",
        "                  *%%%%*#**%%%%                ",
        "                  *%**%%*+*%##%:               ",
        "              -##@*%#%%#*+*%#%%%*              ",
        "          ***###*%%%@%%%%#%%@@@@%#*%*          ",
        "        @%%%*+*#%%%%@@%%%%%%%%%%@%#%##*.       ",
        "      .####%%%%%%%#@@%#+#%##+#%%%%*%%%%%.      ",
        "      .+%**%*%#%%%%%@*%=-+**+=*%%#%%%%%#%.     ",
        "       :%%%%%%%%%*%##%*+*===***%%###*#%%.      ",
        "       %%%%%%%@%#+*+###*=:**=*#*#%%%@%%%.      ",
        "       %%%%%%@%+*+=**+*+****===***#%%%%%.      ",
        "       *%%#%%:.%%*#**+**++*+++=*+*+%***@       ",
        "       %*%*+** **++**%#**%*++*++*%*+++**       ",
        "       %%%%%@#  +*+*==**#*###**=**#***%%%      ",
        "       #%%%%%* %=++****#***=+****@%%%%%%       ",
        "        %%%%%   %%@*+*###*#=*****%:@%%%%%:     ",
        "        %%%%@  %#%%@%%%#**%#***%#%%=**#%%*     ",
        "        %%%%: %#%*%*#%%@*###**%##*%: =%%#%     ",
        "        %##%#=%%#%*+%#%@%*++*%%+#****#%#%:     ",
        "        %%% -*%%%***#**@%%#**##*%**###%%=.     ",
        "         %%**#%%**%%*+*#+==*%%@%=%**           ",
        "             %@##%%+*%%*#%%@@%%%%*%%           ",
        "             %%%@%*#%%@@ .%%%#*=+*##           ",
        "             %%##%%%%%%.  @%%*#***#%           ",
        "            =%%%%#*+*%%   @%%%#***#%           ",
        "           .%#######%%=  .@%%%*****#-          ",
        "            -@%%%%%%%%    :@@@@@@@@@:          ",
        "           :#***#***%      ****=+**#           ",
        "             %%%%%##      ..#%###%%:.          ",
        "            .%@@@@+         @@@@@@+            ",
        "            %%%%*%=         @%#%*%             ",
        "            %*#*#%          %%*%**%            ",
        "           .%**=-==        *%#%%%%%-           ",
        "           -*+-==**         *@##%+*===         ",
        "          .*#**#*%%*-       @@%#+=*=-+=        ",
        "          =#:***** .:        %*##*+*##**       ",
        "                              =******%.=       "
    };
    return aparencia;
}

Atributos Dwarf::obterAtributosRaca() const
{
    return { 110, 15, 5, 0, 10, 5, 15 };
}

std::string Dwarf::obterNomeHabilidadeRaca() const { return "Forjado com determinacao"; }
std::string Dwarf::obterDescricaoHabilidadeRaca() const { return "Escudos possuem o dobro de durabilidade"; }
int Dwarf::processarDanoDefensivo(int danoFinal, SistemaPersonagem* defensor) 
{
    if (defensor->obterDefendendo() && defensor->obterEscudo() != nullptr) 
    {
        if (GeradorAleatorio::rolarChance(50)) 
        {
            defensor->obterEscudo()->aumentarDurabilidade(1);
            std::cout << SimplificacoesAparencia::margemCombate() << "[PASSIVA]: Forjado com determinacao poupou a durabilidade do escudo!\n";
        }
    }
    return danoFinal;
}