#include "UI/Renderers/3D/RaycasterScreens/Attributes/RaycasterAttributesScreen.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Attributes/AttributesScreenLayout.h"
#include "UI/Screens/BaseScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Items/Item.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <algorithm>

struct EffectInfo {
    EffectID effectId;
    Color colorId;
    const char* displayName;
    bool showShifts;
};

static void displayTitleFloating(int startY) {
    int widthConsole = Appearance::getTerminalWidth();
    int soonHeight = (int)ArtsAttributes::sheetLogo.size();
    
    int compVisualSoon = 0;
    for (const auto& line : ArtsAttributes::sheetLogo) {
        int comp = Appearance::getVisualLength(line);
        if (comp > compVisualSoon) compVisualSoon = comp;
    }
    
    int soonY = startY > 0 ? (startY - 1 - soonHeight) : 1;
    if (soonY < 0) soonY = 0;
    int soonX = (widthConsole - compVisualSoon) / 2;
    if (soonX < 0) soonX = 0;
    
    if (widthConsole >= compVisualSoon && (startY >= soonHeight + 1 || soonY == 0)) {
        std::string bgDark = "\033[48;2;20;20;20m";
        std::string colorTitle = Appearance::color(Color::MAGENTA);
        std::string reset = "\033[0m";
        
        for (int i = 0; i < soonHeight; ++i) {
            Appearance::moveCursor(soonX, soonY + i);
            const std::string& line = ArtsAttributes::sheetLogo[i];
            
            std::string buffer = bgDark + colorTitle;
            for (size_t j = 0; j < line.length(); ) {
                unsigned char uc = line[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                buffer += line.substr(j, charLen);
                j += charLen;
            }
            buffer += reset;
            std::cout << buffer;
        }
        std::cout << std::flush;
    } else if (startY >= 2) {
        std::string titleCompact = "[ === FICHA DO PERSONAGEM === ]";
        int compCompact = Appearance::getVisualLength(titleCompact);
        int cx = std::max(0, (widthConsole - compCompact) / 2);
        int cy = std::max(0, startY - 1);
        Appearance::moveCursor(cx, cy);
        std::cout << Appearance::color(Color::MAGENTA) << "\033[48;2;25;25;25m" << titleCompact << "\033[0m" << std::flush;
    }
}

void RaycasterAttributesScreen::display(Character* currentPlayer) {}

enum StateAttributes { MAIN, SKILLS, DETAILS, RISE_LEVEL, ERROR_LEVEL };

void RaycasterAttributesScreen::managePlayerCharacterSheet(Character* currentPlayer) {
    if (!currentPlayer) return;
    
    int selectionCurrent = 0;
    int selectionRise = 0;
    std::vector<std::string> options = {"Subir de Nivel", "Habilidades e Equipamento", "Detalhes de Atributos", "Voltar"};
    StateAttributes state = MAIN;
    
    bool running = true;
    
    int lastStartX = -1;
    int lastStartY = -1;
    int lastW = -1;
    int lastH = -1;

    while (running) {
        std::cout << "\033[?25l";
        
        int widthConsole = Appearance::getTerminalWidth();
        double currentAttributeMultiplier = currentPlayer->getMultiplier();
        DebuffInfo debuff = AttributesScreen::calculateDebuff(currentPlayer);
        bool hasBuff = debuff.hasBuff;
        
        std::vector<std::string> linesMain;
        std::vector<std::string> linesSkills;
        std::vector<std::string> linesDetails;
        std::vector<std::string> linesRiseLevel;
        std::vector<std::string> linesErrorLevel;

        // --- 1. PREPARAR PRINCIPAL ---
        linesMain.push_back("NOME: " + currentPlayer->getName() + "   RACA: " + currentPlayer->getRace()->getRaceName() + "   CLASSE: " + currentPlayer->getClassName());
        double percentageXp = static_cast<double>(currentPlayer->getCurrentXp()) / std::max(1, currentPlayer->getXpForRise());
        std::string barXp = BaseScreen::generateBarGradient(percentageXp, 10, Color::CYAN);
        double pctLife = static_cast<double>(currentPlayer->getHealth()) / std::max(1, currentPlayer->getMaxHealth());
        Color colorLifeEnum = (pctLife > 0.7) ? Color::GREEN : (pctLife > 0.3) ? Color::YELLOW : Color::RED;
        std::string barLife = BaseScreen::generateBarGradient(pctLife, 10, colorLifeEnum);

        linesMain.push_back("NIVEL: " + std::to_string(currentPlayer->getLevel()) + " [" + barXp + Appearance::color(Color::RESET) + "]   HP: [" + barLife + Appearance::color(Color::RESET) + "] " + std::to_string(currentPlayer->getHealth()) + "/" + std::to_string(currentPlayer->getMaxHealth()) + "   OURO: " + Appearance::color(Color::YELLOW) + std::to_string(currentPlayer->getInventory()->getGold()) + "G" + Appearance::color(Color::RESET));

        std::string difStr = "DIFICULDADE: ";
        switch (currentPlayer->getDifficulty()) {
            case GameDifficulty::Easy: difStr += Appearance::color(Color::GREEN) + "Facil"; break;
            case GameDifficulty::Normal: difStr += Appearance::color(Color::YELLOW) + "Normal"; break;
            case GameDifficulty::Difficult: difStr += Appearance::color(Color::RED) + "Dificil"; break;
        }
        difStr += Appearance::color(Color::RESET) + "   [PARRY]: ";
        difStr += currentPlayer->getParryActivated() ? (Appearance::color(Color::GREEN) + "Ligado" + Appearance::color(Color::RESET)) : (Appearance::color(Color::GRAY) + "Desligado" + Appearance::color(Color::RESET));
        linesMain.push_back(difStr);
        
        static const EffectInfo effectsForDisplay[] = {
            {EffectID::AttributesBuff, Color::LIGHT_GREEN, "Buff Atributos", true},
            {EffectID::Slowness, Color::MAGENTA, "Lentidao", true},
            {EffectID::Bleeding, Color::RED, "Sangramento", true},
            {EffectID::Weakness, Color::RED, "Fraqueza", true},
            {EffectID::ResistanceBreak, Color::CYAN, "Quebra de Resistencia", false},
            {EffectID::AdaptationWheel, Color::YELLOW, "Adaptacao Divina", false},
            {EffectID::Necrosis, Color::MAGENTA, "Necrose", true},
        };
        std::string statusCurrent = "STATUS: ";
        bool hasStatus = false;
        for (const auto& info : effectsForDisplay) {
            if (currentPlayer->ownsEffect(info.effectId)) {
                if (hasStatus) statusCurrent += ", ";
                statusCurrent += Appearance::color(info.colorId) + info.displayName;
                if (info.showShifts) statusCurrent += " (" + std::to_string(currentPlayer->getShiftsEffect(info.effectId)) + ")";
                statusCurrent += Appearance::color(Color::RESET);
                hasStatus = true;
            }
        }
        if (!hasStatus) statusCurrent += "Nenhum";
        linesMain.push_back(statusCurrent);
        linesMain.push_back("");

        auto formatAttribute = [hasBuff, currentAttributeMultiplier](std::string attributeName, int baseAttributeValue, int valueLostByDebuff, Color colorBase) -> std::string {
            int bonusBuff = hasBuff ? static_cast<int>(baseAttributeValue * currentAttributeMultiplier) - baseAttributeValue : 0;
            double pct = std::min(1.0, baseAttributeValue / 50.0);
            std::string bar = BaseScreen::generateBarGradient(pct, 10, colorBase);
            std::ostringstream ss;
            ss << std::left << std::setw(13) << attributeName << ": " << std::setw(3) << baseAttributeValue << " [" << bar << Appearance::color(Color::RESET) << "]";
            if (hasBuff && bonusBuff > 0) ss << " " << Appearance::color(Color::LIGHT_GREEN) << "(+" << bonusBuff << ")" << Appearance::color(Color::RESET);
            else if (valueLostByDebuff > 0) ss << " " << Appearance::color(Color::RED) << "(-" << valueLostByDebuff << ")" << Appearance::color(Color::RESET);
            return ss.str();
        };

        linesMain.push_back(formatAttribute("Forca", currentPlayer->getStrength(), debuff.lostStrength, Color::RED) + "   " + formatAttribute("Inteligencia", currentPlayer->getIntelligence(), 0, Color::PURPLE));
        linesMain.push_back(formatAttribute("Destreza", currentPlayer->getDexterity(), debuff.dexterityLost, Color::ORANGE) + "   " + formatAttribute("Sabedoria", currentPlayer->getWisdom(), 0, Color::LILAC));
        linesMain.push_back(formatAttribute("Resistencia", currentPlayer->getResistance(), debuff.resLost, Color::BLUE) + "   " + formatAttribute("Constituicao", currentPlayer->getConstitution(), debuff.constLost, Color::CYAN));
        linesMain.push_back("");

        PowerCombat power = AttributesScreen::calculatePowerCombat(currentPlayer, currentAttributeMultiplier);
        std::string sPhys = Appearance::color(Color::RED) + std::to_string(power.damagePhysIs) + Appearance::color(Color::RESET);
        std::string sMagic = Appearance::color(Color::RED) + std::to_string(power.damageMagicIs) + Appearance::color(Color::RESET);
        std::string sDef = Appearance::color(Color::BLUE) + std::to_string(power.defFixed) + Appearance::color(Color::RESET);
        std::ostringstream ssMit; ssMit << std::fixed << std::setprecision(1) << power.mitigation;
        std::string sMit = Appearance::color(Color::CYAN) + ssMit.str() + "%" + Appearance::color(Color::RESET);
        
        linesMain.push_back("PODER DE COMBATE: Dano Fisico: " + sPhys + " | Dano Magico: " + sMagic + " | Defesa Fixa: " + sDef + " | Mitigacao: " + sMit);
        linesMain.push_back("");
        
        for (int i = 0; i < (int)options.size(); ++i) {
            std::string cursor = (i == selectionCurrent && state == MAIN) ? (Appearance::color(Color::GREEN) + " > ") : "   ";
            std::string colorOption = (i == selectionCurrent && state == MAIN) ? Appearance::color(Color::GREEN) : Appearance::color(Color::WHITE);
            linesMain.push_back(cursor + colorOption + options[i] + Appearance::color(Color::RESET));
        }

        // --- 2. PREPARAR HABILIDADES ---
        auto addDescriptionSplit = [&](const std::string& text) {
            std::istringstream stream(text);
            std::string lineDesc;
            while (std::getline(stream, lineDesc)) linesSkills.push_back(" - " + Appearance::color(Color::GRAY) + lineDesc + Appearance::color(Color::RESET));
        };
        linesSkills.push_back("[HAB. Passiva de Raca]  : " + currentPlayer->getRace()->getNameSkillRace());
        addDescriptionSplit(currentPlayer->getRace()->getDescriptionSkillRace());
        linesSkills.push_back("[HAB. Passiva de Classe]: " + currentPlayer->getClass()->getNamePassiveClass());
        addDescriptionSplit(currentPlayer->getClass()->getDescriptionPassiveClass());
        linesSkills.push_back("[HAB. Ativa de Classe]  : " + currentPlayer->getClass()->getNameSkillClass());
        addDescriptionSplit(currentPlayer->getClass()->getDescriptionSkillClass());
        linesSkills.push_back("");
        linesSkills.push_back("EQUIPAMENTOS:");
        std::string weapon = currentPlayer->getWeapons() ? currentPlayer->getWeapons()->getItemName() + currentPlayer->getWeapons()->getInfoStatus() : "Punhos";
        std::string shield = currentPlayer->getShield() ? currentPlayer->getShield()->getItemName() + currentPlayer->getShield()->getInfoStatus() : "Nenhum";
        std::string armor = currentPlayer->getArmor() ? currentPlayer->getArmor()->getItemName() + currentPlayer->getArmor()->getInfoStatus() : "Trapos";
        linesSkills.push_back(" > Arma    : " + weapon);
        linesSkills.push_back(" > Escudo  : " + shield);
        linesSkills.push_back(" > Armadura: " + armor);
        linesSkills.push_back("");
        linesSkills.push_back(Appearance::color(Color::WHITE) + " [ Voltar ]" + Appearance::color(Color::RESET));

        // --- 3. PREPARAR DETALHES ---
        std::vector<std::string> artRace = currentPlayer->getRace()->getAppearanceRace();
        std::vector<std::string> artClass = currentPlayer->getClass()->getAppearanceClassMenu();
        
        int maxArt = std::max(artRace.size(), artClass.size());
        int wideArtBase = 45; 
        linesDetails.push_back("RACA: " + currentPlayer->getRace()->getRaceName() + std::string(wideArtBase - 6 - currentPlayer->getRace()->getRaceName().length(), ' ') + "CLASSE: " + currentPlayer->getClassName());
        linesDetails.push_back("");
        
        for (int i = 0; i < maxArt; ++i) {
            std::string rL = (i < (int)artRace.size()) ? artRace[i] : "";
            std::string cL = (i < (int)artClass.size()) ? artClass[i] : "";
            
            int lenR = Appearance::getVisualLength(rL);
            if (lenR < wideArtBase) rL += std::string(wideArtBase - lenR, ' ');
            linesDetails.push_back(" " + rL + " " + cL);
        }
        linesDetails.push_back("");

        std::string cB = Appearance::color(Color::WHITE);
        linesDetails.push_back(" " + Appearance::color(Color::GREEN)          + "Vida         " + cB + ": Pontos de vida (HP) maximos do personagem.");
        linesDetails.push_back(" " + Appearance::color(Color::RED)       + "Forca        " + cB + ": Aumenta o dano base de ataques fisicos.");
        linesDetails.push_back(" " + Appearance::color(Color::ORANGE)        + "Destreza     " + cB + ": Aumenta o dano fisico % e define a ordem de turno.");
        linesDetails.push_back(" " + Appearance::color(Color::BLUE)           + "Resistencia  " + cB + ": Reduz o dano recebido de forma fixa.");
        linesDetails.push_back(" " + Appearance::color(Color::CYAN)          + "Constituicao " + cB + ": Reduz o dano recebido em porcentagem.");
        linesDetails.push_back(" " + Appearance::color(Color::PURPLE)           + "Inteligencia " + cB + ": Aumenta o dano base de ataques magicos.");
        linesDetails.push_back(" " + Appearance::color(Color::LILAC)          + "Sabedoria    " + cB + ": Aumenta o dano magico % e a potencia de curas.");
        linesDetails.push_back("");
        linesDetails.push_back("═══ ATRIBUTOS RECOMENDADOS PARA A CLASSE " + currentPlayer->getClassName() + " ═══");
        
        TypeClass type = currentPlayer->getTypeClass();
        if (type == TypeClass::Warrior) {
            linesDetails.push_back(" 1. " + Appearance::color(Color::RED)    + "Forca    " + cB + ": Aumenta o dano base, essencial para armas pesadas.");
            linesDetails.push_back(" 2. " + Appearance::color(Color::ORANGE)     + "Destreza " + cB + ": Multiplica o dano final, util para qualquer build fisica.");
        } else if (type == TypeClass::Archer) {
            linesDetails.push_back(" 1. " + Appearance::color(Color::ORANGE)     + "Destreza " + cB + ": Atributo principal, aumenta o dano e define a ordem de turno.");
            linesDetails.push_back(" 2. " + Appearance::color(Color::RED)    + "Forca    " + cB + ": Aumenta o dano base, fortalecendo o dano fisico geral.");
        } else if (type == TypeClass::Wizard) {
            linesDetails.push_back(" 1. " + Appearance::color(Color::PURPLE)        + "Inteligencia " + cB + ": Essencial, aumenta drasticamente o dano base de magias.");
            linesDetails.push_back(" 2. " + Appearance::color(Color::LILAC)       + "Sabedoria    " + cB + ": Multiplica o dano magico final e fortalece habilidades.");
        } else if (type == TypeClass::Bard) {
            linesDetails.push_back(" 1. " + Appearance::color(Color::LILAC)       + "Sabedoria    " + cB + ": Fortalece intensamente os efeitos das curas e utilidade do Bardo.");
            linesDetails.push_back(" 2. " + Appearance::color(Color::PURPLE)        + "Inteligencia " + cB + ": Melhora o dano magico, permitindo que o Bardo lute efetivamente.");
        } else if (type == TypeClass::NECROMANCER) {
            linesDetails.push_back(" 1. " + Appearance::color(Color::LILAC)       + "Sabedoria    " + cB + ": Melhora magias e a durabilidade dos seus lacaios invocados.");
            linesDetails.push_back(" 2. " + Appearance::color(Color::CYAN)       + "Constituicao " + cB + ": Importante para sobrevivencia enquanto suas invocacoes lutam.");
        } else {
            linesDetails.push_back(" Nenhum atributo de dano especifico definido para esta classe.");
        }
        linesDetails.push_back("");
        linesDetails.push_back(Appearance::color(Color::WHITE) + " [ Voltar ]" + Appearance::color(Color::RESET));

        // --- 4. PREPARAR SUBIR NIVEL ---
        std::vector<std::string> namesAttr = {"Vida", "Forca", "Destreza", "Resistencia", "Constituicao", "Inteligencia", "Sabedoria"};
        std::vector<std::string> optionsAttr;
        for (int i = 1; i <= 7; ++i) {
            auto clonePreview = currentPlayer->clone();
            clonePreview->levelUp(static_cast<AttributeType>(i));

            int valCurrent = 0, valNew = 0;
            switch (i) {
                case 1: valCurrent = currentPlayer->getMaxHealth(); valNew = clonePreview->getMaxHealth(); break;
                case 2: valCurrent = currentPlayer->getStrength(); valNew = clonePreview->getStrength(); break;
                case 3: valCurrent = currentPlayer->getDexterity(); valNew = clonePreview->getDexterity(); break;
                case 4: valCurrent = currentPlayer->getResistance(); valNew = clonePreview->getResistance(); break;
                case 5: valCurrent = currentPlayer->getConstitution(); valNew = clonePreview->getConstitution(); break;
                case 6: valCurrent = currentPlayer->getIntelligence(); valNew = clonePreview->getIntelligence(); break;
                case 7: valCurrent = currentPlayer->getWisdom(); valNew = clonePreview->getWisdom(); break;
            }
            int gain = valNew - valCurrent;
            optionsAttr.push_back(namesAttr[i - 1] + " " + Appearance::color(Color::GRAY) + "(" + std::to_string(valCurrent) + " -> " + std::to_string(valNew) + " [" + Appearance::color(Color::GREEN) + "+" + std::to_string(gain) + Appearance::color(Color::GRAY) + "])" + Appearance::color(Color::RESET));
        }
        optionsAttr.push_back("Cancelar");

        linesRiseLevel.push_back("Escolha o atributo para melhorar:");
        linesRiseLevel.push_back("");
        for (int i = 0; i < (int)optionsAttr.size(); ++i) {
            std::string cursor = (i == selectionRise && state == RISE_LEVEL) ? (Appearance::color(Color::GREEN) + " > ") : "   ";
            std::string colorOption = (i == selectionRise && state == RISE_LEVEL) ? Appearance::color(Color::GREEN) : Appearance::color(Color::WHITE);
            linesRiseLevel.push_back(cursor + colorOption + optionsAttr[i] + Appearance::color(Color::RESET));
        }

        // --- 5. PREPARAR ERRO NIVEL ---
        linesErrorLevel.push_back("");
        linesErrorLevel.push_back("");
        linesErrorLevel.push_back(Appearance::color(Color::RED) + " Voce nao tem XP suficiente para subir de nivel!" + Appearance::color(Color::RESET));
        linesErrorLevel.push_back("");
        linesErrorLevel.push_back("");
        linesErrorLevel.push_back(Appearance::color(Color::WHITE) + " [ Voltar ]" + Appearance::color(Color::RESET));

        // --- PADDING & RENDERIZACAO ---
        std::vector<std::string>* linesTarget = nullptr;
        std::string titleBox = "";
        
        if (state == MAIN) { linesTarget = &linesMain; }
        else if (state == SKILLS) { linesTarget = &linesSkills; titleBox = "Habilidades & Equipamentos"; }
        else if (state == DETAILS) { linesTarget = &linesDetails; titleBox = "Detalhes de Atributos"; }
        else if (state == RISE_LEVEL) { linesTarget = &linesRiseLevel; titleBox = "Subir de Nivel"; }
        else if (state == ERROR_LEVEL) { linesTarget = &linesErrorLevel; titleBox = "Aviso"; }

        int termH = Appearance::getTerminalHeight();
        int boxW = std::min(80, widthConsole - 4);
        for (const auto& l : *linesTarget) {
            int len = Appearance::getVisualLength(l);
            if (len > boxW) boxW = std::min(len, widthConsole - 4);
        }

        std::vector<std::string> boxEnd = BaseScreen::createBox(*linesTarget, titleBox, boxW, Color::MAGENTA, "\033[48;2;25;25;25m");
        
        int outW = Appearance::getVisualLength(boxEnd[0]);
        int outH = (int)boxEnd.size();
        int startX = std::max(0, (widthConsole - outW) / 2);
        
        int soonHeight = (int)ArtsAttributes::sheetLogo.size();
        int totalH = outH + soonHeight + 1;
        int startY = 0;
        if (termH > totalH) {
            startY = (termH - totalH) / 2 + soonHeight + 1;
        } else {
            startY = std::max(0, (termH - outH) / 2);
        }
        if (startY + outH > termH) startY = std::max(0, termH - outH);
        if (startX + outW > widthConsole) startX = std::max(0, widthConsole - outW);

        // Limpa o fantasma da caixa anterior desenhando o fundo 3D salvo novamente
        if (lastH > 0 && lastW > 0) {
            Raycaster::restoreLastFrame();
        }
        
        lastStartX = startX;
        lastStartY = startY;
        lastW = outW;
        lastH = outH;

        displayTitleFloating(startY);
        
        for (size_t i = 0; i < boxEnd.size(); ++i) {
            if (startY + (int)i < termH) {
                Appearance::moveCursor(startX, startY + i);
                std::cout << boxEnd[i];
            }
        }
        std::cout << std::flush;
        
        // --- INPUT ---
        char c = InputControl::readKey();
        if (state == MAIN) {
            if (c == 'w' || c == 'W' || c == 72) {
                selectionCurrent--; if (selectionCurrent < 0) selectionCurrent = options.size() - 1;
            } else if (c == 's' || c == 'S' || c == 80) {
                selectionCurrent++; if (selectionCurrent >= (int)options.size()) selectionCurrent = 0;
            } else if (c == '\r' || c == '\n') {
                if (selectionCurrent == 0) {
                    if (!currentPlayer->canLevelUp()) state = ERROR_LEVEL;
                    else { state = RISE_LEVEL; selectionRise = 0; }
                }
                else if (selectionCurrent == 1) state = SKILLS;
                else if (selectionCurrent == 2) state = DETAILS;
                else if (selectionCurrent == 3) running = false;
            } else if (c == 27) {
                running = false;
            }
        } else if (state == RISE_LEVEL) {
            if (c == 'w' || c == 'W' || c == 72) {
                selectionRise--; if (selectionRise < 0) selectionRise = 7;
            } else if (c == 's' || c == 'S' || c == 80) {
                selectionRise++; if (selectionRise > 7) selectionRise = 0;
            } else if (c == '\r' || c == '\n') {
                if (selectionRise == 7) { 
                    state = MAIN;
                } else {
                    currentPlayer->levelUp(static_cast<AttributeType>(selectionRise + 1));
                    state = MAIN;
                }
            } else if (c == 27) {
                state = MAIN;
            }
        } else {
            if (c == '\r' || c == '\n' || c == 27) {
                state = MAIN;
            }
        }
    }
}

void RaycasterAttributesScreen::displayDetailsAttributes(Character*) {}
