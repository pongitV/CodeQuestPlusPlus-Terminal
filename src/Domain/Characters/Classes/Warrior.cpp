#include "Domain/Characters/Classes/Warrior.h"

#include <iostream>
#include <memory>

#include "Systems/Combat/Combat.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "UI/Screens/Combat/CombatScreen.h"

// --- INFORMACOES DA CLASSE ---
std::string Warrior::getClassName() const 
{ 
    return "Guerreiro"; 
}

const std::vector<std::string>& Warrior::getAppearanceClassMenu() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                                                          ",
        "                              *###:                       ",
        "                             #+=%##%                      ",
        "                             -::+###-                     ",
        "                             +*%%+++*..                   ",
        "                             +: ==-%%.                    ",
        "                            .-%#%@@%@@%                   ",
        "                             -##@@@=:.###                 ",
        "                          =:#*##%@#:+%@@%@@=              ",
        "                        #+#*@:.#%*%@-:==-=#@              ",
        "                        *%@=:#*-: :=@*+#@#=@              ",
        "                        =++#==+=#+##@::#*=#               ",
        "                        =#*@##@%+@%@@=:+=*#               ",
        "                         =%%#%@#@%@@@===*%#               ",
        "                        =#==:==%#@@@@@+==%#               ",
        "                        =#===::+-:-####-=#*#              ",
        "                       -+#.==+=:=%###==-##-@              ",
        "                      =+#.*#@####+%#%+:=###@              ",
        "                     =:-#+@#@#%@#*@#%=+#*=                ",
        "                    :#:==-##@#%====%@#=@#*                ",
        "                    =#*+#=+#@@@@@@@@*+@@-                 ",
        "                    %#%%#+#+%##%=@@#:=@@#+                ",
        "                   ++@##=@#==%@*#*=.:=#:+#                ",
        "                   %=@@####%*=@**@:%##*@%@                ",
        "                  .==%#*=%%#@+@*#%#@@@@@@@#               ",
        "                 -==###@##=#*@%%+##@@@@@@@@               ",
        "                 @%###@%#%++@%@@%#@%@@@@%@#               ",
        "                 %%  ####=@%#@#@@@*@##@@@@@@              ",
        "                ##  ##%#+%##@@#@%@##@#@@@@%@#             ",
        "               #%-   **@@#++@%#@@%*%%%@@@@=%%             ",
        "              =#* ++#*@+=@#+@%@*@#+@@@@%%*#               ",
        "              %#      @@@@%@##*@@%+**=@@@@#=@#            ",
        "             ##       #=@@%@@@@@@+=#*@@@@%#@#%#           ",
        "            ##         =*@@@@@@@@-=*=@@@@@#@*%#*          ",
        "           ###         ::@@@@@ @@=+##@@@@@#@##%@          ",
        "          %##           %#%####  #++#%@@@@#@%*#           ",
        "         #%@            %@@#@@:  **+#+@%@@#@%@%           ",
        "         #%:            =@#%%@=  = #% %@@@@*==@#          ",
        "        ##              =##%@%         @@@@#=##           ",
        "       *#               %%#@@#         %@@@@.             ",
        "       #             *%%%%#@%@*        @@@@@+             ",
        "                  +####%@%%###       @#%%@#@@             ",
        "                                    ##%*#@@%%             ",
        "                                                          "
    };
    return appearance;
}

Attributes Warrior::getAttributesClass() const
{
    return { 0, 20, 10, 5, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Warrior::getEquipmentClass() const 
{
    auto equipment = ItemFactory::createPotionKit();

    equipment.push_back(ItemFactory::createItem(ItemID::SwordIron));
    equipment.push_back(ItemFactory::createItem(ItemID::ShieldMetal));
    equipment.push_back(ItemFactory::createItem(ItemID::ArmorMesh));
    return equipment;
}

// --- PASSIVA DA CLASSE ---
std::string Warrior::getNamePassiveClass() const 
{ 
    return "Golpe decisivo"; 
}

std::string Warrior::getDescriptionPassiveClass() const 
{ 
    return "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP."; 
}

// --- HABILIDADE DA CLASSE ---
std::string Warrior::getRechargeSkillClass() const 
{ 
    return "Recarga: 3 turnos."; 
}

std::string Warrior::getNameSkillClass() const 
{ 
    return "Grito de guerra"; 
}

std::string Warrior::getDescriptionSkillClass() const 
{ 
    return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; 
}

void Warrior::useSkillClass(Combat* /*combate*/, Character* characterUser, std::vector<Character*>& /*enemyList*/) 
{
    int remainingTurns = characterUser->getCooldown(SkillID::Determination);
    if (checkEReportRecharge(characterUser, remainingTurns, getNameSkillClass())) return;

    if (characterUser->ownsEffect(EffectID::WarCry)) {
        std::string msg = DialogueFunctions::formatSystemMsg("A habilidade " + getNameSkillClass() + " ja esta ativa!", Color::YELLOW);
        std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
        Appearance::registerBattleLog(msg);
        InputControl::waitForEnter();
        characterUser->setSkillCanceled(true);
        return;
    }

    int strengthBonus = characterUser->getStrength() / 2;
    int dexterityBonus = characterUser->getDexterity() / 2;
    
    characterUser->addEffect(std::make_unique<WarCryEffect>(2, strengthBonus, dexterityBonus));
    characterUser->setCooldown(SkillID::Determination, 4);
    
    std::string msg = DialogueFunctions::formatSkillMsg("Grito de guerra! Forca +" + std::to_string(strengthBonus) + " e Destreza +" + std::to_string(dexterityBonus) + "!");
    notifyMessageCombat(msg, msg);
}

// --- PROCESSAMENTO DE DANO  ---
int Warrior::processDamagePreAttack(Character* /*atacante*/, Character* defender, int damageBase, bool /*isAtacanteJogador*/, size_t /*qtdInimigos*/) {
    int finalDamage = damageBase;
    
    if (!defender) return finalDamage;

    double percLife = (double)defender->getHealth() / defender->getMaxHealth();
    int bonus = 0;
    std::string state = "";
    
    if (percLife < 0.10) { bonus = 30; state = "nas ultimas"; }
    else if (percLife < 0.20) { bonus = 20; state = "gravemente ferido"; }
    else if (percLife < 0.30) { bonus = 10; state = "ferido"; }
    
    if (bonus > 0) {
        finalDamage = static_cast<int>(finalDamage * (1.0 + bonus / 100.0));
        std::string textLog = DialogueFunctions::formatSkillMsg("Golpe Decisivo: O inimigo esta " + state + "! Dano aumentado em " + std::to_string(bonus) + "%!", Color::RED);
        notifyMessageCombat(textLog, textLog);
    }
    
    return finalDamage;
}
