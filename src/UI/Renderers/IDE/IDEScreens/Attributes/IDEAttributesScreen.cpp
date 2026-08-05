#include "UI/Renderers/IDE/IDEScreens/Attributes/IDEAttributesScreen.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <chrono>
#include "UI/Renderers/IDE/IDETheme.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Attributes/AttributesScreenLayout.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Items/Item.h"

struct EffectInfo {
    EffectID effectId;
    Color colorId;
    const char* displayName;
    bool showShifts;
};

void ScreenAttributesGO::displayDetailsAttributes(Character* currentPlayer) {
    Appearance::clearScreen();
    Appearance::displayTextPanel("DETALHES DOS ATRIBUTOS", Color::MAGENTA);

    std::vector<std::string> panelRace = {"[ RACA: " + currentPlayer->getRace()->getRaceName() + " ]", ""};
    for (const auto& line : currentPlayer->getRace()->getAppearanceRace()) panelRace.push_back(line);

    std::vector<std::string> panelClass = {"[ CLASSE: " + currentPlayer->getClassName() + " ]", ""};
    for (const auto& line : currentPlayer->getClass()->getAppearanceClassMenu()) panelClass.push_back(line);

    Appearance::printSideASide(panelRace, panelClass, 0, 10, Color::WHITE, Color::WHITE, 10);
    std::cout << "\n";
    BaseScreen::printLineDivider();

    std::vector<std::string> lines;
    std::string cB = Appearance::color(Color::WHITE);
    lines.push_back("");
    lines.push_back("═══ EFEITOS DE CADA ATRIBUTO ═══");
    lines.push_back(" " + Appearance::color(Color::GREEN)          + "Vida         " + cB + ": Pontos de vida (HP) maximos do personagem.");
    lines.push_back(" " + Appearance::color(Color::RED)       + "Forca        " + cB + ": Aumenta o dano base de ataques fisicos.");
    lines.push_back(" " + Appearance::color(Color::ORANGE)        + "Destreza     " + cB + ": Aumenta o dano fisico final em porcentagem e define a ordem de turno.");
    lines.push_back(" " + Appearance::color(Color::BLUE)           + "Resistencia  " + cB + ": Reduz o dano recebido de forma fixa.");
    lines.push_back(" " + Appearance::color(Color::CYAN)          + "Constituicao " + cB + ": Reduz o dano recebido em porcentagem.");
    lines.push_back(" " + Appearance::color(Color::PURPLE)           + "Inteligencia " + cB + ": Aumenta o dano base de ataques magicos.");
    lines.push_back(" " + Appearance::color(Color::LILAC)          + "Sabedoria    " + cB + ": Aumenta o dano magico final em porcentagem e aumenta a potencia de curas.");
    lines.push_back("");

    lines.push_back("═══ ATRIBUTOS DE DANO RECOMENDADOS PARA A CLASSE " + currentPlayer->getClassName() + " ═══");

    TypeClass type = currentPlayer->getTypeClass();
    if (type == TypeClass::Warrior) {
        lines.push_back(" 1. " + Appearance::color(Color::RED)    + "Forca    " + cB + ": Aumenta o dano base, essencial para armas pesadas.");
        lines.push_back(" 2. " + Appearance::color(Color::ORANGE)     + "Destreza " + cB + ": Multiplica o dano final, util para qualquer build fisica.");
    } else if (type == TypeClass::Archer) {
        lines.push_back(" 1. " + Appearance::color(Color::ORANGE)     + "Destreza " + cB + ": Atributo principal, aumenta o dano e define a ordem de turno.");
        lines.push_back(" 2. " + Appearance::color(Color::RED)    + "Forca    " + cB + ": Aumenta o dano base, fortalecendo o dano fisico geral.");
    } else if (type == TypeClass::Wizard) {
        lines.push_back(" 1. " + Appearance::color(Color::PURPLE)        + "Inteligencia " + cB + ": Essencial, aumenta drasticamente o dano base de magias.");
        lines.push_back(" 2. " + Appearance::color(Color::LILAC)       + "Sabedoria    " + cB + ": Multiplica o dano magico final e fortalece habilidades.");
    } else if (type == TypeClass::Bard) {
        lines.push_back(" 1. " + Appearance::color(Color::LILAC)       + "Sabedoria    " + cB + ": Fortalece intensamente os efeitos das curas e utilidade do Bardo.");
        lines.push_back(" 2. " + Appearance::color(Color::PURPLE)        + "Inteligencia " + cB + ": Melhora o dano magico, permitindo que o Bardo lute efetivamente.");
    } else if (type == TypeClass::NECROMANCER) {
        lines.push_back(" 1. " + Appearance::color(Color::LILAC)       + "Sabedoria    " + cB + ": Melhora magias e a durabilidade dos seus lacaios invocados.");
        lines.push_back(" 2. " + Appearance::color(Color::CYAN)       + "Constituicao " + cB + ": Importante para sobrevivencia enquanto suas invocacoes lutam.");
    } else {
        lines.push_back(" Nenhum atributo de dano especifico definido para esta classe.");
    }

    Appearance::printBlockCentralized(lines, Appearance::color(Color::WHITE), 10);
    InputControl::waitForEnter();
}

void ScreenAttributesGO::display(Character* currentPlayer) {
    if (currentPlayer == nullptr) return;
    Appearance::clearScreen();

    static auto lastAccessSheet = std::chrono::steady_clock::now() - std::chrono::hours(1);
    bool animate = BaseScreen::mustAnimateScreenEntry(lastAccessSheet, 300);

    Appearance::displayArtPanel(ArtsAttributes::soonSheet, 59, Color::MAGENTA, "", animate);

    double multiplierDeAttributesCurrent = currentPlayer->getMultiplier();
    DebuffInfo debuff = AttributesScreen::calculateDebuff(currentPlayer);
    bool hasBuff = debuff.hasBuff;

    std::vector<std::string> infoGeneral;
    infoGeneral.push_back("NOME: " + currentPlayer->getName() + "   RACA: " + currentPlayer->getRace()->getRaceName() + "   CLASSE: " + currentPlayer->getClassName());
    double percentageXp = static_cast<double>(currentPlayer->getCurrentXp()) / std::max(1, currentPlayer->getXpForRise());
    std::string barXp = BaseScreen::generateBarGradient(percentageXp, 10, Color::CYAN);
    double pctLife = static_cast<double>(currentPlayer->getHealth()) / std::max(1, currentPlayer->getMaxHealth());
    Color colorLifeEnum = (pctLife > 0.7) ? Color::GREEN : (pctLife > 0.3) ? Color::YELLOW : Color::RED;
    std::string barLife = BaseScreen::generateBarGradient(pctLife, 10, colorLifeEnum);

    infoGeneral.push_back("NIVEL: " + std::to_string(currentPlayer->getLevel()) + " [" + barXp + Appearance::color(Color::RESET) + "]   HP: [" + barLife + Appearance::color(Color::RESET) + "] " + std::to_string(currentPlayer->getHealth()) + "/" + std::to_string(currentPlayer->getMaxHealth()) + "   OURO: " + Appearance::color(Color::YELLOW) + std::to_string(currentPlayer->getInventory()->getGold()) + "G" + Appearance::color(Color::RESET));

    std::string difStr = "DIFICULDADE: ";
    switch (currentPlayer->getDifficulty()) {
        case GameDifficulty::Easy: difStr += Appearance::color(Color::GREEN) + "Facil"; break;
        case GameDifficulty::Normal: difStr += Appearance::color(Color::YELLOW) + "Normal"; break;
        case GameDifficulty::Difficult: difStr += Appearance::color(Color::RED) + "Dificil"; break;
    }
    difStr += Appearance::color(Color::RESET) + "   [PARRY]: ";
    difStr += currentPlayer->getParryActivated() ? (Appearance::color(Color::GREEN) + "Ligado" + Appearance::color(Color::RESET)) : (Appearance::color(Color::GRAY) + "Desligado" + Appearance::color(Color::RESET));
    infoGeneral.push_back(difStr);

    std::vector<std::string> boxGeneral = BaseScreen::createBox(infoGeneral, "", 72, Color::MAGENTA);

    auto formatAtr = [hasBuff, multiplierDeAttributesCurrent](std::string nameDoAttribute, int valueBaseDoAttribute, int valueLostByDebuff, Color colorBase, int atrRace, int atrClass) -> std::string
    {
        int bonusBuff = hasBuff ? static_cast<int>(valueBaseDoAttribute * multiplierDeAttributesCurrent) - valueBaseDoAttribute : 0;
        double pct = std::min(1.0, valueBaseDoAttribute / 50.0);
        std::string bar = BaseScreen::generateBarGradient(pct, 10, colorBase);

        std::ostringstream ss;
        ss << std::left << std::setw(13) << nameDoAttribute << ": " << std::setw(3) << valueBaseDoAttribute << " [" << bar << Appearance::color(Color::RESET) << "]";

        if (hasBuff && bonusBuff > 0) {
            ss << " " << Appearance::color(Color::LIGHT_GREEN) << "(+" << bonusBuff << ")" << Appearance::color(Color::RESET);
        } else if (valueLostByDebuff > 0) {
            ss << " " << Appearance::color(Color::RED) << "(-" << valueLostByDebuff << ")" << Appearance::color(Color::RESET);
        }

        ss << "  " << Appearance::color(Color::GRAY) << "[R: " << atrRace << " | C: " << atrClass << "]" << Appearance::color(Color::RESET);

        return ss.str();
    };

    Attributes atrRace = currentPlayer->getRace()->getAttributesRace();
    Attributes atrClass = currentPlayer->getClass()->getAttributesClass();

    std::vector<std::string> atrLines;
    atrLines.push_back(formatAtr("Forca", currentPlayer->getStrength(), debuff.lostStrength, Color::RED, atrRace.strength, atrClass.strength));
    atrLines.push_back(formatAtr("Destreza", currentPlayer->getDexterity(), debuff.dexterityLost, Color::ORANGE, atrRace.dexterity, atrClass.dexterity));
    atrLines.push_back(formatAtr("Resistencia", currentPlayer->getResistance(), debuff.resLost, Color::BLUE, atrRace.resistance, atrClass.resistance));
    atrLines.push_back(formatAtr("Constituicao", currentPlayer->getConstitution(), debuff.constLost, Color::CYAN, atrRace.constitution, atrClass.constitution));
    atrLines.push_back(formatAtr("Inteligencia", currentPlayer->getIntelligence(), 0, Color::PURPLE, atrRace.intelligence, atrClass.intelligence));
    atrLines.push_back(formatAtr("Sabedoria", currentPlayer->getWisdom(), 0, Color::LILAC, atrRace.wisdom, atrClass.wisdom));

    std::vector<std::string> boxAttributes = BaseScreen::createBox(atrLines, "ATRIBUTOS", 35, Color::MAGENTA);

    std::vector<std::string> habLines;
    auto addDescriptionSplit = [&](const std::string& text) {
        std::istringstream stream(text);
        std::string lineDesc;
        while (std::getline(stream, lineDesc)) {
            habLines.push_back(" - " + Appearance::color(Color::GRAY) + lineDesc + Appearance::color(Color::RESET));
        }
    };

    habLines.push_back("[HAB. Passiva de Raca]  : " + currentPlayer->getRace()->getNameSkillRace());
    addDescriptionSplit(currentPlayer->getRace()->getDescriptionSkillRace());
    habLines.push_back("[HAB. Passiva de Classe]: " + currentPlayer->getClass()->getNamePassiveClass());
    addDescriptionSplit(currentPlayer->getClass()->getDescriptionPassiveClass());
    habLines.push_back("[HAB. Ativa de Classe]  : " + currentPlayer->getClass()->getNameSkillClass());
    addDescriptionSplit(currentPlayer->getClass()->getDescriptionSkillClass());
    habLines.push_back("");
    habLines.push_back("EQUIPAMENTOS:");
    std::string weapon = currentPlayer->getWeapons() ? currentPlayer->getWeapons()->getItemName() + currentPlayer->getWeapons()->getInfoStatus() : "Punhos";
    std::string shield = currentPlayer->getShield() ? currentPlayer->getShield()->getItemName() + currentPlayer->getShield()->getInfoStatus() : "Nenhum";
    std::string armor = currentPlayer->getArmor() ? currentPlayer->getArmor()->getItemName() + currentPlayer->getArmor()->getInfoStatus() : "Trapos";
    habLines.push_back(" > Arma    : " + weapon);
    habLines.push_back(" > Escudo  : " + shield);
    habLines.push_back(" > Armadura: " + armor);

    std::vector<std::string> boxSkills = BaseScreen::createBox(habLines, "HABILIDADES & EQUIPAMENTOS", 35, Color::MAGENTA);

    PowerCombat power = AttributesScreen::calculatePowerCombat(currentPlayer, multiplierDeAttributesCurrent);

    std::string sFis = Appearance::color(Color::RED) + std::to_string(power.damageFisIs) + Appearance::color(Color::RESET);
    std::string sMag = Appearance::color(Color::RED) + std::to_string(power.damageMagIs) + Appearance::color(Color::RESET);
    std::string sDef = Appearance::color(Color::BLUE) + std::to_string(power.defFixed) + Appearance::color(Color::RESET);

    std::ostringstream ssMit;
    ssMit << std::fixed << std::setprecision(1) << power.mitigation;
    std::string sMit = Appearance::color(Color::CYAN) + ssMit.str() + "%" + Appearance::color(Color::RESET);

    std::vector<std::string> powerLines = {
        " Dano Fisico  : " + sFis, " Dano Magico  : " + sMag, " Defesa Fixa  : " + sDef, " Mitigacao (%): " + sMit
    };
    std::vector<std::string> boxPower = BaseScreen::createBox(powerLines, "PODER DE COMBATE", 35, Color::MAGENTA);

    static const EffectInfo effectsForDisplay[] = {
        {EffectID::AttributesBuff,     Color::LIGHT_GREEN,   "Buff Atributos",   true},
        {EffectID::Slowness,           Color::MAGENTA, "Lentidao",         true},
        {EffectID::Bleeding,        Color::RED,"Sangramento",      true},
        {EffectID::Weakness,           Color::RED,"Fraqueza",         true},
        {EffectID::ResistanceBreak, Color::CYAN,   "Quebra de Resistencia", false},
        {EffectID::AdaptationWheel,     Color::YELLOW, "Adaptacao Divina",  false},
        {EffectID::Necrosis,           Color::MAGENTA, "Necrose",           true},
    };

    std::vector<std::string> statusLines;
    bool hasStatus = false;
    for (const auto& info : effectsForDisplay) {
        if (currentPlayer->ownsEffect(info.effectId)) {
            std::string stStr = "Efeito: " + Appearance::color(info.colorId) + info.displayName;
            if (info.showShifts) {
                stStr += " (" + std::to_string(currentPlayer->getShiftsEffect(info.effectId)) + " turnos)";
            }
            stStr += Appearance::color(Color::RESET);
            statusLines.push_back(stStr);
            hasStatus = true;
        }
    }
    if (!hasStatus) statusLines.push_back("Nenhum status ativo.");
    std::vector<std::string> boxStatus = BaseScreen::createBox(statusLines, "STATUS ATUAIS", 35, Color::MAGENTA);

    Appearance::printBlockCentralized(boxGeneral, "");
    Appearance::printSideASide(boxAttributes, boxSkills, 40, 2, Color::RESET, Color::RESET, 0);
    Appearance::printSideASide(boxPower, boxStatus, 40, 2, Color::RESET, Color::RESET, 0);

    std::cout << "\n";
    BaseScreen::printLineDivider();
}
