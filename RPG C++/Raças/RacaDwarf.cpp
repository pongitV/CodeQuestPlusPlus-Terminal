#include <iostream>

#include "RacaDwarf.h"

std::string RacaDwarf::obterNomeRaca() const
{
    return "Dwarf";
}

std::vector<std::string> RacaDwarf::obterAparenciaRaca() const 
{
    return 
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
}

Atributos RacaDwarf::obterAtributosRaca() const
{
    return { 0, 15, 5, 0, 10, 5, 15 };
}

std::string RacaDwarf::obterNomeHabilidadeRaca() const { return "Forjado com determinacao"; }
std::string RacaDwarf::obterDescricaoHabilidadeRaca() const { return "Escudos possuem o dobro de durabilidade"; }
int RacaDwarf::processarDanoDefensivo(int danoFinal, Personagem* defensor) 
{
    if (defensor->obterDefendendo() && defensor->obterEscudo() != nullptr) 
    {
        if (rand() % 100 < 50) 
        {
            defensor->obterEscudo()->aumentarDurabilidade(1);
            std::cout << "[PASSIVA]: Forjado com determinacao poupou a durabilidade do escudo!\n";
        }
    }
    return danoFinal;
}