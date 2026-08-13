#include "Domain/Characters/Classes/Mage.h"

#include <iostream>
#include <memory>

#include "Systems/Combat/Combat.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "UI/Screens/Combat/CombatScreen.h"

// --- INFORMACOES DA CLASSE ---
std::string Wizard::getClassName() const 
{
     return "Mago"; 
}

const std::vector<std::string>& Wizard::getAppearanceClassMenu() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                                        =@@@%%   ",
        "                                        *%%***.  ",
        "                                        **#- .#  ",
        "                                       #%%%##+#: ",
        "                   -#++                .%%@%-    ",
        "                  ##*+:-=              *@@@@.    ",
        "                 #%#*+:-*.              %@@@*    ",
        "                *%*%##%++*             .@@+      ",
        "               :#@%**.+*+#*            *@@%      ",
        "   : .         *##*@#*-+#:##           :@@.      ",
        "   ::* +    .#%%%%:#@=*+-#*#+*+        =@@-      ",
        "    -=+-   @%#%@@@+*@:*==%-++-::*      *%*       ",
        "    -*-* :@%#%****+*%+**++** --:-*     %@        ",
        "    +-* @#%*#*-=+*-*%+*#**:+::*-#@     *@.       ",
        "    @#*+ **@%==:*:=:%-*+#***:=@+::=    %@        ",
        "    %#*.*#*%@%@#+=-*=--##**%*#@%*::    *@        ",
        "    @@*:=#%%@+@%:**:-**%*=+-#@@@%*::-  #@        ",
        "    @@*=**@@@**+***+:#%*--+**.%@@@*-::-@*        ",
        "    @@*+##@@@: %##+*%#***=***: @@@+*-* ::        ",
        "    @@**%@@@:  @%*%@%*+%%+*#-  @@@@#:%%**+       ",
        "    @@#%@@@@  +@@*%@#++%%=#%* .@@@@*@###         ",
        "    @@%@@@@   @@%%@***#@%+#%#   #@@@@@@@*        ",
        "   :@@#@@.    @@@@@*#*%%#*%%#    @@@@@@@=        ",
        "   =@%%:    -%@@%@%*#=*+*#%%%:   =@@@@@@         ",
        "   =%%  %@%##@@@@%@*#*#*%+#%@#    %@@@@@         ",
        "   *:  =@@%@@#*#%%###%%%@@#%@@.    %@@@@         ",
        "      -@@@@%@@%###*%%@@*%@@*%%@     @@@@         ",
        "        *@@@@%##%%%%*%@%@#%%%%%@     @@          ",
        "          #@#%##@%%%%@@@@#@%%@%@*    @+          ",
        "          @@%@@#@%%%%%#@@@%%@%%@@@:              ",
        "         %@@%@@%@@@@%@@@@@%@@%@@%*@              ",
        "        @@@@%@@%@@%@%%@@@@@@@%@%@@*@             ",
        "        @@@@@@@@@@@@@@@@@@@@@%@@@@@@             ",
        "        @@@@@@@@@@@@@@@@@@@@@%%@%@@%*            ",
        "        @@@@@@@@@@@@@@@@@@@  .%@%@%%#            ",
        "        @%%@@@@@ +@@@@@@@@.   *@%@%%#            ",
        "         @@@@%@@: @@@@@@@.    :%@%%%             ",
        "         #@@@%@@* @@@@%@@      %@%%              ",
        "            #@@@%  %@@%@%@     %@%#              ",
        "             *@@%  @%%%%@@     %@%               ",
        "               +% .@%@%@@%     *#                ",
        "                   %%%#                          ",
        "                  @%%%%                          "
    };
    return appearance;
}

Attributes Wizard::getAttributesClass() const
{
    return { 0, 5, 5, 3, 10, 15, 15 };
}

std::vector<std::unique_ptr<Item>> Wizard::getEquipmentClass() const 
{
    auto equipment = ItemFactory::createPotionKit();

    equipment.push_back(ItemFactory::createItem(ItemID::StaffCrystal));
    equipment.push_back(ItemFactory::createItem(ItemID::BarrierMagic));
    equipment.push_back(ItemFactory::createItem(ItemID::Tunic));
    return equipment;
}

// --- PASSIVA DA CLASSE ---
std::string Wizard::getNamePassiveClass() const 
{ 
    return "Foco arcano"; 
}

std::string Wizard::getDescriptionPassiveClass() const 
{ 
    return "Ataques ressoam (25% em area) ou causam +25% de dano em alvo unico."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Wizard::getRechargeSkillClass() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Wizard::getNameSkillClass() const 
{ 
    return "Canalizacao arcana"; 
}

std::string Wizard::getDescriptionSkillClass() const 
{ 
    return "Pula seu turno para se defender e dobra o dano no proximo turno. Recarga: 3 turnos."; 
}

void Wizard::useSkillClass(Combat* /*combate*/, Character* characterUser, std::vector<Character*>& /*enemyList*/) 
{
    int remainingTurns = characterUser->getCooldown(SkillID::PlumbingArcane);
    if (checkEReportRecharge(characterUser, remainingTurns, getNameSkillClass())) return;
    
    characterUser->setMultiplier(2.0);
    characterUser->addEffect(std::make_unique<AttributesBuffEffect>(2)); 
    characterUser->setCooldown(SkillID::PlumbingArcane, 4);
    
    Item* shield = characterUser->getShield();
    if (shield) {
        characterUser->setDefending(true);
        std::string msg = DialogueFunctions::formatSkillMsg("Canalizacao arcana! Defendendo com " + shield->getItemName() + "! 2x Dano no prox. ataque!");
        notifyMessageCombat(msg, msg);
    } else {
        std::string msg = DialogueFunctions::formatSkillMsg("Canalizacao arcana! Foco magico para 2x Dano no proximo ataque!");
        notifyMessageCombat(msg, msg);
    }
}

// --- PROCESSAMENTO DE DANO  ---
int Wizard::processDamagePreAttack(Character* /*atacante*/, Character* defender, int damageBase, bool isAttackerPlayer, size_t enemyCount) {
    if (defender == nullptr) return damageBase;
    if (!isAttackerPlayer || enemyCount <= 1) {
        int damageIncreased = static_cast<int>(damageBase * 1.25);
        std::string logMsg = DialogueFunctions::formatSkillMsg("Foco Arcano: Dano concentrado aumentado em 25%!", Color::MAGENTA);
        notifyMessageCombat(logMsg, logMsg);
        return damageIncreased;
    }
    return damageBase;
}

void Wizard::processDamagePostAttack(Character* attacker, Character* targetCurrent, Character* defenderMain, int damageBase, int damagePiercing, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool isAttackerPlayer, bool isArea, bool& activatedPassive) {
    if (isAttackerPlayer && !isArea && targetCurrent != defenderMain && targetCurrent->getHealth() > 0) {
        if (!activatedPassive) {
            int damageAreaMsg = static_cast<int>(damageBase * 0.25);
            std::string logMsg = DialogueFunctions::formatSkillMsg("Foco Arcano: A magia ressoa, causando " + std::to_string(damageAreaMsg) + " de dano aos inimigos proximos!", Color::MAGENTA);
            notifyMessageCombat(logMsg, logMsg);
            activatedPassive = true;
        }
        int damageArea = static_cast<int>(damageBase * 0.25);
        int piercingArea = static_cast<int>(damagePiercing * 0.25);
        applyDamage(attacker, targetCurrent, damageArea, piercingArea);
    }
}
