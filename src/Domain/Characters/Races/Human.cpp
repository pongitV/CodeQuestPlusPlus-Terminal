#include "Domain/Characters/Races/Human.h"

#include <iostream>
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

// [PT-BR] --- INFORMACOES DA RACA ---
// [EN-US] --- RACE INFORMATION ---
std::string Human::getRaceName() const 
{
    return "Humano";
}

Attributes Human::getAttributesRace() const
{
    return { 100, 10, 10, 0, 10, 10, 10 };
}

// [PT-BR] --- APARENCIA ---
// [EN-US] --- APPEARANCE ---
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

// [PT-BR] --- HABILIDADE DA RACA ---
// [EN-US] --- RACE SKILL ---
std::string Human::getNameSkillRace() const 
{ 
    return "Espirito indomavel"; 
}

std::string Human::getDescriptionSkillRace() const 
{ 
    return "Revive com metade da vida maxima uma vez"; 
}

// [PT-BR] --- PROCESSAMENTO DE DANO ---
// [EN-US] --- DAMAGE PROCESSING ---
int Human::processDamageDefensive(int finalDamage, Character* defender) 
{
    // [PT-BR] Verifica se o golpe seria fatal para disparar a passiva de sobrevivencia
    // [EN-US] Checks if damage would be fatal to trigger survival passive
    if ((defender->getHealth() - finalDamage) <= 0 && defender->canUseResurrection()) 
    {
        defender->consumeResurrection();
        int cureRevive = defender->getMaxHealth() / 2;
        defender->modifyHealth(cureRevive);
        std::string msg = CombatScreen::combatMargin() + Appearance::color(Color::MAGENTA) + "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!" + Appearance::color(Color::RESET) + "\n";
        CombatScreen::addFixedMessage(msg);
        Appearance::registerBattleLog(Appearance::color(Color::MAGENTA) + "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!" + Appearance::color(Color::RESET));
        // [PT-BR] O dano atual e anulado pois a vida foi restaurada
        // [EN-US] Current damage is negated because health has been reset
        return 0;
    }
    return finalDamage;
}






