#include "Domain/Characters/Races/Dwarf.h"

#include <iostream>

#include "Core/Utils/RandomGenerator.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

// --- INFORMACOES DA RACA ---
std::string Dwarf::getRaceName() const
{
    return "Dwarf";
}

Attributes Dwarf::getAttributesRace() const
{
    return { 110, 15, 5, 0, 10, 5, 15 };
}

// --- APARENCIA ---
const std::vector<std::string>& Dwarf::getAppearanceRace() const 
{
    static const std::vector<std::string> appearance = 
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
    return appearance;
}

// --- HABILIDADE DA RACA ---
std::string Dwarf::getNameSkillRace() const 
{ 
    return "Forjado com determinacao"; 
}

std::string Dwarf::getDescriptionSkillRace() const 
{ 
    return "Escudos possuem o dobro de durabilidade"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Dwarf::processDamageDefensive(int damageEnd, Character* defender) 
{
    if (defender->getDefending() && defender->getShield() != nullptr) 
    {
        if (RandomGenerator::rollChance(50)) 
        {
            defender->getShield()->increaseDurability(1);
            std::string msg = CombatScreen::combatMargin() + "[PASSIVA]: Forjado com determinacao poupou a durabilidade do escudo!\n";
            CombatScreen::addFixedMessage(msg);
        }
    }
    return damageEnd;
}






