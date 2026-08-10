#include "Domain/Characters/Races/Human.h"

#include <iostream>
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

// --- INFORMACOES DA RACA ---
std::string Human::getRaceName() const 
{
    return "Humano";
}

Attributes Human::getAttributesRace() const
{
    return { 100, 10, 10, 0, 10, 10, 10 };
}

// --- APARENCIA ---
const std::vector<std::string>& Human::getAppearanceRace() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                        @%%#                     ",
        "                       ##@%@#=                   ",
        "                      :=:::-=#                   ",
        "                      ::=+:+*=                   ",
        "                       ::::*=                    ",
        "                       :::-++                    ",
        "                       :::=+=                    ",
        "                   .##.=::=:--==                 ",
        "                 #####. ==...####=               ",
        "               #**#### .*...=#######             ",
        "              . +++**#-. ...####*##::            ",
        "              .. +++***....*######.-:            ",
        "              .:.+++**+:..:**#*###.:::           ",
        "              ...++****+=:#**#####.::.           ",
        "              . *++++**#-:##**%*##.::.           ",
        "             .. ++=+++**:.%*%#*%%#.::::          ",
        "             .:.:++++++*++%#+###%#.::::          ",
        "             .:::=+=+++*+.@######:.::::          ",
        "            ..::==.*#**=+*##%#*%% .::::          ",
        "             .:+=***+++#+.##%*+%#@..:::          ",
        "           ..=.:.+*+*+*++:*#**+##%:..-:.         ",
        "              .= +***++**:*@#*+##%:::.-:         ",
        "            -: =.***+**++:*%@###%%--==-          ",
        "             :==+*+*+++*#*#%%*##%%-:*+           ",
        "                :::=+++*+*+=%*##*=.              ",
        "                :::--==##=--#*+*+=               ",
        "                :::-=++##---#=====               ",
        "               ::---==*# =-=#=====               ",
        "               ::--==++#  -=#=====               ",
        "               :-:===++   =-=====.               ",
        "               ---=+++    =-=====                ",
        "              +##@*#:      %##%%%                ",
        "             -##=@#%@     =***%#%                ",
        "              #%#%%#       %%@@%                 ",
        "              ###%%:       #*%##                 ",
        "              ###%#        %#%%                  ",
        "              ##%#         ###*                  ",
        "              #%%%         ###:                  ",
        "             :#%%-        :###                   ",
        "            :@#%%         +#%#                   ",
        "           @@%###-        =##*+                  ",
        "         %#@@@:.           ##@@                  "
    };
    return appearance;
}

// --- HABILIDADE DA RACA ---
std::string Human::getNameSkillRace() const 
{ 
    return "Espirito indomavel"; 
}

std::string Human::getDescriptionSkillRace() const 
{ 
    return "Revive com metade da vida maxima uma vez"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Human::processDamageDefensive(int finalDamage, Character* defender) 
{
    // Verifica se o golpe seria fatal
    if ((defender->getHealth() - finalDamage) <= 0 && defender->canUseResurrection()) 
    {
        defender->consumeResurrection();
        int cureRevive = defender->getMaxHealth() / 2;
        defender->modifyHealth(cureRevive);
        std::string msg = CombatScreen::combatMargin() + Appearance::color(Color::MAGENTA) + "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!" + Appearance::color(Color::RESET) + "\n";
        CombatScreen::addFixedMessage(msg);
        Appearance::registerBattleLog(Appearance::color(Color::MAGENTA) + "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!" + Appearance::color(Color::RESET));
        return 0; // O dano atual e anulado pois a vida foi resetada
    }
    return finalDamage;
}






