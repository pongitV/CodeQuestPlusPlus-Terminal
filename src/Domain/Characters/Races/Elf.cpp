#include "Domain/Characters/Races/Elf.h"

#include <iostream>

#include "Core/Utils/RandomGenerator.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

// [PT-BR] --- INFORMACOES DA RACA ---
// [EN-US] --- RACE INFORMATION ---
std::string Elf::getRaceName() const 
{
    return "Elfo";
}

Attributes Elf::getAttributesRace() const
{
    return { 90, 5, 15, 0, 10, 15, 5 };
}

// [PT-BR] --- APARENCIA ---
// [EN-US] --- APPEARANCE ---
const std::vector<std::string>& Elf::getAppearanceRace() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                        ::::                          ",
        "                      =-:-..::                        ",
        "                     =-:::-.:-*  ___-                 ",
        "                     =*%:*:*.=:@###.                  ",
        "                     .**=::#:@%@#:.                   ",
        "                   *+++==#%==::+-:                    ",
        "                   .:=%##:=#*:::=:.                   ",
        "                 .  @-=*+-:=*:=:.+-                   ",
        "                   %%%%#%#-*+.:: -+=                  ",
        "                  #%%%#%#*%+*=++=:=*                  ",
        "                 +@*%%+*=#:.****.-==-                 ",
        "                -%%%%%*%=+#-=%:=%:++=.                ",
        "                *@*%@%*--=***+%#+%+===                ",
        "                .%%#@%%%#*##%**-+%*+*=                ",
        "                 .:@@@@@%%@@%@@*%%%#**                ",
        "                   *#*%%%*%%%%@@%%%#*+                ",
        "                  :%%%#@%%%%#+#%%%%*+                 ",
        "                  %%%%#**#%%#-+*@@%**                 ",
        "                  =%%*-#-*%%%%#@%#%+=                 ",
        "                  %*=*%=*%*#:=*#%%%*+.                ",
        "                  *+=:-*%#%*:.=%%#%##+                ",
        "                .%===*=@%%*-*+-:%%%%**                ",
        "                +%#+=:%%#@=--:#:=%%.%%                ",
        "                #*+*+***@@#=-::#:*#* .                ",
        "               =+*=#%%%@@@@*--=*=-#-                  ",
        "               %*+++%@%@%@@+=--=*--#=                 ",
        "              :%#**#@%@@%@@*=-==*--:*=                ",
        "              :%%*##@%@%%%@@*===*==:-=                ",
        "              .%%##%%@%%%%@@*+=++#+--..               ",
        "               %*%#%*%%##%@@*+++*%=---+               ",
        "               %*###%%%##%@@*+****+===*               ",
        "              .%%###%%###%@@******#===:               ",
        "              .%%##%@@*%%%%@******#+++=               ",
        "              :%%###@@@@@@@@*****%%=+++               ",
        "              +%####@%@@@@@@***##@@#++*:              ",
        "             .%%#@@#@@@@@@@@##*@%@@%++#*              ",
        "             :%%#@@@@@@#.:%@%*=@%@@@**#               ",
        "               +%@%%@:         @@@##.                 ",
        "             .%@%*%@@=         @%%@+                  ",
        "            @@@@%.            :%%**                   ",
        "                               @@%@                   "
    };
    return appearance;
}

// [PT-BR] --- HABILIDADE DA RACA ---
// [EN-US] --- RACE SKILL ---
std::string Elf::getNameSkillRace() const 
{ 
    return "Agil e preciso"; 
}

std::string Elf::getDescriptionSkillRace() const 
{ 
    return "Possui 33% chance de causar 1.5x de dano em cada ataque"; 
}

// [PT-BR] --- PROCESSAMENTO DE DANO ---
// [EN-US] --- DAMAGE PROCESSING ---
int Elf::processDamageOffensive(int damageBase, Character* attacker) 
{
    if (RandomGenerator::rollChance(33)) 
    {
        std::string msg = CombatScreen::combatMargin() + "[PASSIVA]: Agil e preciso! Golpe critico.\n";
        CombatScreen::addFixedMessage(msg);
        return static_cast<int>(damageBase * 1.5);
    }
    return damageBase;
}






