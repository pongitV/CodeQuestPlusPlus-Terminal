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
        "                  *%@#+#@@***%* ",
        "                  *%%%%*#**%%%%                ",
        "                  *%**%%*+*%##%:               ",
        "              -##@*%#%%#*+*%#%%%* ",
        "          ***###*%%%@%%%%#%%@@@@%#*%* ",
        "        @%%%*+*#%%%%@@%%%%%%%%%%@%#%##*.       ",
        "      .####%%%%%%%#@@%#+#%##+#%%%%*%%%%%.      ",
        "      .+%**%*%#%%%%%@*%=-+**+=*%%#%%%%%#%.     ",
        "       :%%%%%%%%%*%##%*+*===***%%###*#%%.      ",
        "       %%%%%%%@%#+*+###*=:**=*#*#%%%@%%%.      ",
        "       %%%%%%@%+*+=**+*+****===***#%%%%%.      ",
        "       *%%#%%:.%%*#**+**++*+++=*+*+%***@       ",
        "       %*%*+** **++**%#**%*++*++*%*+++** ",
        "       %%%%%@#  +*+*==**#*###**=**#***%%%      ",
        "       #%%%%%* %=++****#***=+****@%%%%%%      ",
        "        %%%%%   %%@*+*###*#=*****%:@%%%%%:     ",
        "        %%%%@  %#%%@%%%#**%#***%#%%=**#%%* ",
        "        %%%%: %#%*%*#%%@*###**%##*%: =%%#%     ",
        "        %##%#=%%#%*+%#%@%*++*%%+#****#%#%:     ",
        "        %%% -*%%%***#**@%%#**##*%**###%%=.     ",
        "         %%**#%%**%%*+*#+==*%%@%=%** ",
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
        "           -*+-==**         *@##%+*===          ",
        "          .*#**#*%%*-       @@%#+=*=-+=        ",
        "          =#:***** .:        %*##*+*##** ",
        "                              =******%.=     "
    };
}

Atributos RacaDwarf::obterAtributosRaca() const
{
    return { 20, 12, 6, 15, 12, 5, 8 };
}

std::string RacaDwarf::obterNomeHabilidadeRaca() const { return "Forjado com determinacao"; }
std::string RacaDwarf::obterDescricaoHabilidadeRaca() const { return "Escudos tem o dobro de durabilidade"; }

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