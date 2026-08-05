#include "Domain/Characters/Races/Orc.h"

#include <iostream>
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

// --- INFORMACOES DA RACA ---
std::string Ork::getRaceName() const 
{
    return "Ork";
}

Attributes Ork::getAttributesRace() const
{
    return { 120, 20, 10, 0, 10, 5, 5 };
}

// --- APARENCIA ---
const std::vector<std::string>& Ork::getAppearanceRace() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                      @@@ #%-::                         ",
        "                *+  + +**#@%@#*%                        ",
        "       %        #%#####%#%%+*==#                        ",
        "       %+   %%+%+***-%@@@%#%=*#*%*                      ",
        "       ##+# *+##-%#%+#+*@%@%#%@*%%*::+                  ",
        "        #%#%#+#%%###:*++#@@%@@@##%##+==+==*             ",
        "        -=#%########**#++##@@%%@@%#===-:-=+*%           ",
        "       ##%%#+%#*%%*##%-+*=**++=+#+:-:===+=%@%+          ",
        "       #%###=*+#*#*%%*=%##**==:..:+==*+***#%%%          ",
        "        @@@%#+#+#@##%*%####%#*==#++##*##*%*#%@          ",
        "          @*%@@%%%@%**#*%*%%%@*+=*+#%@%%%#@#*+*         ",
        "           #%##%%@%*+=+#=@%%%+*#+%%%%%#+=*+*++#@        ",
        "           ###%**%@%%%%*++=*##%#%%*#@@*#+=*%+#%%        ",
        "         ##%%###*#%%%#%*@%%%@%@#%%@@#@%*#*#*#**%        ",
        "        ###%%####%%#%##@%%%%%%@%#%#*%#*%%%#*%#*%%@      ",
        "      =%%*=-+###%%  #%%@#+*++*@%%%%%%#   %%#*+*%%@      ",
        "      %##%=-===*%    ##%***++%%*%%%%%   #@%##%#@@#@     ",
        "     :#**:+++*#%#  %*##%#%##%@@%%@@@%@  @%#*@*=@%%#     ",
        "     .%%=+==*%**   %%%%#%##%**%@####@@  %%%#@%%##@      ",
        "      #+*=-=+=-    @@%###%@=%**%@@#@%@@ %%%@%@%##%%     ",
        "     %#*+*#**+%   %%@%%%****#%###%@%%%@@#%#@#%##@%#     ",
        "    .%#**++%%#@ %@##%@%%%@@#%#@@##@#*%#+%%%%%%%@@=      ",
        "      %###%%%. @@@%%#@@%%=#%%%@%%%#=#*******%#@@##      ",
        "      ##%%%%%.+#*##%%@%%%###%%@%#%*=**%*##*##@%         ",
        "       +  +% =###*=#%@@%#*#%#%@%@%%#*##%#%%####         ",
        "        % .%  #%*++*#%@%@%#@@%@@@%##%*##%#@             ",
        "           %  %#*+##%@@%%#%@%%@@%%#*****+#@             ",
        "           % %%+**##@%%@@%%@#@@@%%##**+##%@             ",
        "             @%#%*#%%%@%@%%@%@@@ %%%%*+-*%@             ",
        "            %####+***##%@#%@% @    %%#**#%%+            ",
        "           .@%###***%###* %#%=@   %%%%%%%#%%%           ",
        "             %#%%#%#%###*%  %     %%**%#%*#%@           ",
        "              =+%*%%#%%%%@% #     %%#@*##**%%@          ",
        "               %#%%%#@**%%@@       @%##*@@*+%           ",
        "               #%%%#%%@%%%%         ##%*+#+@%           ",
        "            #%%%-##%%%#%%%%         %@%#%#**@%          ",
        "         +  ###+##%%#%%%%%%     .   *%%*%@#*##          ",
        "      +   *%%%%%###*#@%%%=%# -.     %%@%#++#%%@         ",
        "     ::*:.-@%%%%%@#%@+-.+#..:-...:  .@@%@%%+*#*#@       ",
        "       .-=.+-: .  ::*+   :   ....*:*%#@%%%%*#++*#%      ",
        "                              ..--::... +@@%*%##%%.     ",
        "                                     -...:%%%%%%#       "
    };
    return appearance;
}

// --- HABILIDADE DA RACA ---
std::string Ork::getNameSkillRace() const 
{ 
    return "Furia cega"; 
}

std::string Ork::getDescriptionSkillRace() const 
{ 
    return "Dano extra baseado na porcentagem de vida perdida"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Ork::processDamageOffensive(int damageBase, Character* attacker) 
{
    double percLifeLost = 1.0 - (static_cast<double>(attacker->getHealth()) / attacker->getMaxHealth());
    int damageExtra = static_cast<int>(damageBase * percLifeLost);
    if (damageExtra > 0) 
    {
        std::string msg = CombatScreen::combatMargin() + "[PASSIVA]: Furia cega aumentou o dano em " + std::to_string(damageExtra) + "!\n";
        CombatScreen::addFixedMessage(msg);
        return damageBase + damageExtra;
    }
    return damageBase;
}






