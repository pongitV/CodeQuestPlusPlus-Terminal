#include "Domain/Characters/Classes/Bard.h"

#include <array>
#include <functional>
#include <iostream>
#include <memory>

#include "Systems/Combat/Combat.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Constants.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Combat/CombatScreen.h"

// --- INFORMACOES DA CLASSE ---
std::string Bard::getClassName() const 
{
     return "Bardo"; 
}

const std::vector<std::string>& Bard::getAppearanceClassMenu() const 
{
    static const std::vector<std::string> appearance = 
    {
        "                       ..:: :                    ",
        "                      =-.-:-:=:                  ",
        "                      =:--+-:--                  ",
        "                      ::-::::*=                  ",
        "                      .. ::-+-:                  ",
        "                    :-*-:-**=:.                  ",
        "                -**+******=+=**:                 ",
        "              :*+++*++++***#*#*#-                ",
        "             +*+==++===**=*+**+*-:               ",
        "             .+==+=*+====++.-==*==               ",
        "             :=*++***+=+*+*:====++               ",
        "            +:*****#*+++===++**+**               ",
        "            :=**+*##***+=+=+=#****               ",
        "           -*****#%****++++*##****.              ",
        "           ##*#***%#*****+*%#****#:              ",
        "           ##%+**#%******%#***#***+              ",
        "          *#%###***%#+*%%#**#+*###*:             ",
        "          ##@##*:-*#*%%%%%%%%+###***:            ",
        "         =##%%**#**%%%**%%%%*%******+            ",
        "         *%#@@#@@%##%#******%********#*.         ",
        "        -#%#%@@@@%%%%%####*===****** =#=#+       ",
        "        *%%%%%@@@@%@%==---==++#*****  %**        ",
        "       *%%%%%%@@@@%*=====++*+=#****   **=        ",
        "      +*%%%#%%%@@@%*++===+*#+=####*  -**         ",
        "     **%%%%%%%%###%*+===*+%#*====-   #**         ",
        "      *%#%%%%#%###%*+==+=%%%*++==    ***         ",
        "       #%%%%%#####%*+==+#-@%**++*   :**=         ",
        "       #@%%%######%*+++*% @%**+*.  :***:         ",
        "      +%%%%%#####%#*++*#* @%*++* -#****=         ",
        "      #%%%%#####*%*****%*:%#****#*******+        ",
        "    .#*%%%##%**##%*****@*@%****%*********+       ",
        "    %=%%%%###**#@%#***#**%%####@******#**+       ",
        "     %*%%%%     @#%%#%%@%@@%%#%@***#******=      ",
        "    #%%%@@%:   :%@%%%@@+@@@@@%%@***********      ",
        "      %%@%%%   @@@%@@@@. @@%%%%#***********      ",
        "          ** %@@@@%@: -@@@@   ***#********:      ",
        "                %@@@@@   :@@@# ***********       ",
        "                @@@@@:   -@@%%  *********        ",
        "                @%%@#::= #@@@@%%##               ",
        "                #%%@+      **%@%%@@              ",
        "               #%#%@=                            ",
        "              =@#%@@-                            ",
        "              %*#@@:                             ",
        "               :=:                               "
    };
    return appearance;
}

Attributes Bard::getAttributesClass() const
{
    return { 0, 10, 10, 3, 10, 10, 10};
}

std::vector<std::unique_ptr<Item>> Bard::getEquipmentClass() const 
{
    auto equipment = ItemFactory::createPotionKit();
    
    equipment.push_back(ItemFactory::createItem(ItemID::ViolaEnchanted));
    equipment.push_back(ItemFactory::createItem(ItemID::CoverMagic));
    equipment.push_back(ItemFactory::createItem(ItemID::CostumeNoble));
    return equipment;
}

// --- PASSIVA DA CLASSE ---
std::string Bard::getNamePassiveClass() const 
{ 
    return "Touch the sky"; 
}

std::string Bard::getDescriptionPassiveClass() const 
{ 
    return "Curas e buffs recebidos sao 40% mais fortes."; 
}

int Bard::processCurePassiveBard(int cureBase) const 
{
    return static_cast<int>(cureBase * Constants::BARD_HEALING_MULTIPLIER);
}

double Bard::processMultiplierBuffPassiveBard(double multBase) const 
{
    if (multBase > 1.0) return 1.0 + (multBase - 1.0) * 1.4;
    return multBase;
}

// --- HABILIDADE DA CLASSE ---
std::string Bard::getRechargeSkillClass() const 
{ 
    return "Recarga: 3 turnos (Individuais)."; 
}

std::string Bard::getNameSkillClass() const 
{ 
    return "Sinfonia do Bardo"; 
}

std::string Bard::getDescriptionSkillClass() const 
{ 
    return "Possui 3 habilidades: Flashing lights, On sight e Through the wire."; 
}

void Bard::useSkillClass(Combat* /*combate*/, Character* characterUser, std::vector<Character*>& /*listaDeInimigos*/)
{
    struct SubSkill {
        SkillID id;
        std::string name;
        std::string description;
        std::function<void(Character*)> action;
    };

    const std::array<SubSkill, 3> skills = {{
        { SkillID::FlashingLights, "Flashing lights", "Cura e pula o turno", [this](Character* characterSkill) {
            characterSkill->setJumpShiftEnemy(true);
            int healing = static_cast<int>((characterSkill->getWisdom() * 2) + (characterSkill->getMaxHealth() * 0.15));
            characterSkill->modifyHealth(healing);
            characterSkill->setCooldown(SkillID::FlashingLights, 3);
            std::string msg = DialogueFunctions::formatSkillMsg("!Flashing lights! Voce recuperou " + std::to_string(healing) + " HP e encantou os inimigos!", Color::GREEN);
            this->notifyMessageCombat(msg, msg);
        }},
        { SkillID::OnSight, "On sight", "1.5x Dano no proximo ataque", [this](Character* characterSkill) {
            characterSkill->setMultiplier(1.5);
            characterSkill->setCooldown(SkillID::OnSight, 3);
            std::string msg = DialogueFunctions::formatSkillMsg(characterSkill->getName() + " tocou 'On sight'! Proximo ataque com 1.5x dano!");
            this->notifyMessageCombat(msg, msg);
        }},
        { SkillID::ThroughTheWire, "Through the wire", "Metade do dano recebido", [this](Character* characterSkill) {
            characterSkill->addEffect(std::make_unique<HalfDamageEffect>(1));
            characterSkill->setCooldown(SkillID::ThroughTheWire, 3);
            std::string msg = DialogueFunctions::formatSkillMsg("!Through the wire! Voce esta protegido contra metade do dano recebido!", Color::CYAN);
            this->notifyMessageCombat(msg, msg);
        }}
    }};

    std::cout << "\n" << CombatScreen::combatMargin() << "--- SINFONIA DO BARDO ---\n\n";
    std::vector<std::string> optionsSkills;
    for (size_t i = 0; i < skills.size(); ++i) {
        int cd = characterUser->getCooldown(skills[i].id);
        optionsSkills.push_back(skills[i].name + " (" + skills[i].description + " | Recarga: " + std::to_string(cd) + ")");
    }
    optionsSkills.push_back("CANCELAR");

    int choice = InputControl::readSelectionMenuWithArrows(optionsSkills, false, CombatScreen::combatMargin());

    if (choice == static_cast<int>(skills.size())) {
        characterUser->setSkillCanceled(true);
        return;
    }
    
    const auto& hab = skills[choice];
    int cd = characterUser->getCooldown(hab.id);
    if (checkEReportRecharge(characterUser, cd, hab.name)) return;

    hab.action(characterUser);
}
