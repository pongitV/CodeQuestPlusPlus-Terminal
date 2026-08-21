#include "Domain/Characters/Classes/Archer.h"

#include <iostream>
#include <memory>

#include "Systems/Combat/Combat.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"

// [PT-BR] --- INFORMACOES DA CLASSE ---
// [EN-US] --- CLASS INFORMATION ---
std::string Archer::getClassName() const 
{
     return "Arqueiro"; 
}

const std::vector<std::string>& Archer::getAppearanceClassMenu() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                  = --%=-                        ",
        "                    #@* @=                       ",
        "                       %@- %*                    ",
        "                         *% *                    ",
        "                         ** *                    ",
        "                          * **                   ",
        "                         +*%*#:*                 ",
        "                  ..      .%:#@%#:               ",
        "               : =-. .       ---=**.             ",
        "            :%#.:*+::::        #  %*             ",
        "           *@#+-:::*.- - #+    %*++**            ",
        "            =:+=*#+=::+#**%*:  #+*..+.           ",
        "            +#*+*%*%*%*+%*=%@#*+*#++ .           ",
        "        #@*::+*-**++**++*=#**==+*++--:           ",
        "       %@###%*#%%:#*%%#@#%@%%#%*:#*+=-+          ",
        "        =%@%@%:%#**####@@%% %@%%   #%.-#         ",
        "               #*%*##%#@%%-##        %*:-#       ",
        "                #%*#%%@%###$          **@%**     ",
        "                *#%%@%@%#*#            -=-=      ",
        "                 #+%%%*%%%#            -@-#      ",
        "                 -%:##*%#%#=%%* * %+    --=      ",
        "      +-=-===-+%==#*@=*%*@#**::##=**.#@@  ==     ",
        "    ++*=******##*%%**%*+#***+##*%# %##@@#        ",
        "      **+*****#*#%#*%#@##*:+#%---==.%%@*         ",
        "              *@#*%#*#+**:*%**                   ",
        "             %%%%##%%#+=%@%%%=                   ",
        "           *@@%%@%%:    %%%**=* *# .%            ",
        "          +*%@@%@        %%%*+* @# @+            ",
        "          #*=@@%%@@@      @%#*#* @* #            ",
        "           %#@@**@@@@#%    %## +=    @           ",
        "                  @.@*@     ##-:+=*              ",
        "                    @@#     %##%#**              ",
        "                    #*      *#@@@%%.             ",
        "                              +@@%%@             ",
        "                                @@%%             ",
        "                                 @%%             ",
        "                                  @%%            ",
        "                                  -%#%           ",
        "                                   %%%%          ",
        "                                    #:%          ",
        "                                    ##%%         ",
        "                                    *=:*         ",
        "                                     *:*         "
    };
    return appearance;
}

Attributes Archer::getAttributesClass() const
{
    return { 0, 10, 20, 3, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> Archer::getEquipmentClass() const 
{
    auto equipment = ItemFactory::createPotionKit();

    equipment.push_back(ItemFactory::createItem(ItemID::BowWood));
    equipment.push_back(ItemFactory::createItem(ItemID::ArmbandsSilver));
    equipment.push_back(ItemFactory::createItem(ItemID::ArmorLeather));
    return equipment;
}

// [PT-BR] --- PASSIVA DA CLASSE ---
// [EN-US] --- CLASS PASSIVE ---
std::string Archer::getNamePassiveClass() const 
{ 
    return "Passos leves"; 
}

std::string Archer::getDescriptionPassiveClass() const 
{ 
    return "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade."; 
}

int Archer::processPenaltyArmorPassiveArcher(int penaltyBase) const 
{
    return penaltyBase / 2;
}

int Archer::applyArcherPassiveSlownessPenalty(int dexterityCurrent) const 
{
    return (dexterityCurrent * 3) / 4;
}

int Archer::revertArcherPassiveSlownessPenalty(int dexterityCurrent) const 
{
    return (dexterityCurrent * 4) / 3;
}

// [PT-BR] --- HABILIDADE DA CLASSE ---
// [EN-US] --- CLASS SKILL ---
std::string Archer::getRechargeSkillClass() const 
{ 
    return "Recarga: 1 turno."; 
}

std::string Archer::getNameSkillClass() const 
{ 
    return "Retirada com pontaria"; 
}

std::string Archer::getDescriptionSkillClass() const 
{ 
    return "Se afasta durante um turno, no proximo turno causa 2x dano"; 
}

void Archer::useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& /*enemyList*/) 
{
    int remainingTurns = characterUser->getCooldown(SkillID::WithdrawalWithAim);
    if (checkEReportRecharge(characterUser, remainingTurns, getNameSkillClass())) return;

    characterUser->addEffect(std::make_unique<InvincibleEffect>(1));
    characterUser->setCooldown(SkillID::WithdrawalWithAim, 2);
    
    std::string msg = DialogueFunctions::formatSkillMsg("Retirada com pontaria! (Afasta-se)");
    notifyMessageCombat(msg, msg);
}
