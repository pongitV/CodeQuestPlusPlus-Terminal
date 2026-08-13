#include "UI/Renderers/3D/RaycasterScreens/Combat/RaycasterCombatScreen.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Systems/Combat/Combat.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Items/Item.h"
#include "Systems/Inventory/Inventory.h"
#include "Core/Utils/Appearance.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <unordered_map>

struct PlayerHUDStateRaycaster {
    double hpGhost = -1.0;
    double hpPrevious = -1.0;
};
static std::unordered_map<Character*, PlayerHUDStateRaycaster> hudStatesRaycaster;


std::vector<std::string> RaycasterCombatScreen::getPlayerStatusBarLines(Character* currentPlayer, Color colorHighlight, int damageAnimation, int frameAnimation, bool isHealing) {
    if (!currentPlayer) return {};

    auto now = std::chrono::steady_clock::now();
    (void)now; // caso precise no futuro

    std::string weaponName = (currentPlayer->getWeapons()) ? currentPlayer->getWeapons()->getItemName() + currentPlayer->getWeapons()->getInfoStatus() : "Punhos";
    std::string shieldName = (currentPlayer->getShield()) ? currentPlayer->getShield()->getItemName() + currentPlayer->getShield()->getInfoStatus() : "Nenhum";
    std::string armorName = (currentPlayer->getArmor()) ? currentPlayer->getArmor()->getItemName() + currentPlayer->getArmor()->getInfoStatus() : "Trapos";
    
    Item* consumable = currentPlayer->getConsumableQuickly();
    std::string nameConsumable = consumable ? consumable->getItemName() + " (" + std::to_string(currentPlayer->getInventory()->countItem(consumable->getItemName())) + "x)" : "Vazio";

    PlayerHUDStateRaycaster& stateHUD = hudStatesRaycaster[currentPlayer];
    double lifeCurrent = currentPlayer->getHealth();
    if (stateHUD.hpPrevious == -1.0) stateHUD.hpGhost = lifeCurrent;
    if (lifeCurrent < stateHUD.hpGhost) {
        stateHUD.hpGhost -= std::max(0.5, (stateHUD.hpGhost - lifeCurrent) * 0.10);
        if (stateHUD.hpGhost < lifeCurrent) stateHUD.hpGhost = lifeCurrent;
    } else {
        stateHUD.hpGhost = lifeCurrent;
    }
    stateHUD.hpPrevious = lifeCurrent;

    double pctLife = lifeCurrent / std::max(1, currentPlayer->getMaxHealth());
    double pctGhost = stateHUD.hpGhost / std::max(1, currentPlayer->getMaxHealth());
    
    std::string colorLifeBase = (pctLife > 0.70) ? "\033[38;2;100;255;100m" : (pctLife > 0.30) ? "\033[38;2;255;255;100m" : "\033[38;2;255;100;100m";
    std::string colorHPBackground = "\033[38;2;60;60;60m";
    std::string colorGhost = "\033[38;2;200;100;100m";

    if (damageAnimation > 0 && frameAnimation > 0) {
        if (frameAnimation % 2 == 1) {
            colorLifeBase = isHealing ? "\033[1;38;2;150;255;150m" : "\033[1;38;2;255;150;150m";
        }
    }

    std::string barHP = "[";
    int sizeBar = 5;
    int filled = static_cast<int>(pctLife * sizeBar);
    int filledGhost = static_cast<int>(pctGhost * sizeBar);
    for (int i = 0; i < sizeBar; ++i) {
        if (i < filled) barHP += colorLifeBase + "█";
        else if (i < filledGhost) barHP += colorGhost + "█";
        else barHP += colorHPBackground + "░";
    }
    barHP += "\033[0m]" + colorLifeBase + std::to_string(currentPlayer->getHealth()) + "/" + std::to_string(currentPlayer->getMaxHealth()) + "\033[0m";

    std::string colorGold = "\033[38;2;255;255;255m"; // Branco
    std::string colorXP = "\033[38;2;0;200;255m"; // Ciano
    std::string colorText = "\033[38;2;200;200;200m"; // Cinza claro
    std::string colorHighlightGeneral = "\033[1;38;2;255;255;255m";
    std::string colorLabel = "\033[38;2;150;150;150m";

    // Formatacao da XP
    int xpCurrent = currentPlayer->getCurrentXp();
    int xpBasePrevious = 0; // Aproximacao simples, o ideal seria ter xpBaseDoNivel
    int deltaTotalXP = currentPlayer->getXpForRise() - xpBasePrevious;
    double pctXp = static_cast<double>(xpCurrent - xpBasePrevious) / std::max(1, deltaTotalXP);
    if (pctXp > 1.0) pctXp = 1.0;
    
    std::string barXP = "[";
    int sizeBarXP = 5;
    int xpFilled = static_cast<int>(pctXp * sizeBarXP);
    for (int i = 0; i < sizeBarXP; ++i) {
        if (i < xpFilled) barXP += colorXP + "█";
        else barXP += colorHPBackground + "░";
    }
    barXP += "\033[0m]";

    std::string prefixHP = colorLabel + " | HP: ";
    if (damageAnimation > 0 && frameAnimation > 0) {
        if (frameAnimation % 2 == 1) {
            prefixHP = isHealing ? "\033[1;38;2;150;255;150m | HP: " : "\033[1;38;2;255;150;150m | HP: ";
        }
    }

    std::vector<std::string> lines;
    std::string infoRace = currentPlayer->getRace() ? currentPlayer->getRace()->getRaceName() : "Desconhecido";
    std::string infoClass = currentPlayer->getClassName();

    std::string line1 = colorHighlightGeneral + " " + currentPlayer->getName() + " (Nv " + std::to_string(currentPlayer->getLevel()) + ") - " 
                       + infoRace + " " + infoClass + "\033[0m"
                       + prefixHP + barHP
                       + colorLabel + " | XP: " + barXP
                       + colorLabel + " | " + colorGold + std::to_string(currentPlayer->getInventory()->getGold()) + "g\033[0m"
                       + colorLabel + " | Pocao: " + colorText + nameConsumable + "\033[0m";
    
    std::string line2 = colorLabel + " Arma: " + colorText + weaponName + "\033[0m"
                       + colorLabel + " | Escudo: " + colorText + shieldName + "\033[0m";

    std::string line3 = colorLabel + " Traje: " + colorText + armorName + "\033[0m";

    std::vector<EffectID> effects;
    currentPlayer->getIDsEffectsAssets(effects);
    std::string strStatusHUD = "";
    for (auto id : effects) {
        const StatusEffect* ef = currentPlayer->findEffect(id);
        if (ef) {
            strStatusHUD += "\033[38;2;255;150;50m[" + ef->getName() + "]\033[0m ";
        }
    }
    if (!strStatusHUD.empty()) {
        line3 += colorLabel + " | Status: " + strStatusHUD;
    }

    // Retrato / Mugshot
    std::vector<std::string> mugshot = { " /_\\ ", "(o_o)", " \\_/ " }; // Base humana
    if (currentPlayer->getRace()) {
        TypeRace t = currentPlayer->getRace()->getTypeRace();
        if (t == TypeRace::Elf) mugshot = { " /\\/\\ ", "(o_o)", " \\__/ " };
        else if (t == TypeRace::Ork || t == TypeRace::ExiledOrc) mugshot = { " [__] ", "(>_<)", " \\##/ " };
        else if (t == TypeRace::Dwarf) mugshot = { " _██_ ", "(o_o)", " {##} " };
    }
    
    if (pctLife <= 0.30) { // Hurt mugshot
        mugshot[1] = "(x_x)";
        if (damageAnimation > 0 && !isHealing) mugshot[1] = "(>O<)"; // Tomando porrada
    } else if (pctLife > 0.70 && damageAnimation > 0 && isHealing) { // Curando e feliz
        mugshot[1] = "(^_^)";
    }

    std::string colorMugshot = "\033[38;2;255;220;180m";
    if (pctLife <= 0.30) colorMugshot = "\033[38;2;255;100;100m"; // Sangrando

    line1 = colorMugshot + mugshot[0] + "\033[0m|" + line1;
    line2 = colorMugshot + mugshot[1] + "\033[0m|" + line2;
    line3 = colorMugshot + mugshot[2] + "\033[0m|" + line3;

    lines.push_back(line1);
    lines.push_back(line2);
    lines.push_back(line3);
    
    return lines;
}

#include "Systems/Combat/Combat.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterCombatRenderer.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterHUD.h"
#include "UI/Renderers/3D/RaycasterScreens/Menu/RaycasterParryScreen.h"
#include "Core/Utils/InputControl.h"
#include "Systems/Combat/Parry.h"
#include "UI/Screens/Menu/BaseMenuScreen.h"
#include <thread>
#include <iostream>

// Estado Estatico do Contexto
static bool s_isContext3D = false;
static std::vector<std::string> s_contextMap;
static float s_contextPostX = 0.0f;
static float s_contextPostY = 0.0f;
static float s_contextAngle = 0.0f;
static std::string s_contextTitleMap = "";
static std::string s_titleShiftHUD = "";

struct MsgLogRaycaster {
    std::string text;
    std::chrono::time_point<std::chrono::steady_clock> timestamp;
};
static std::vector<MsgLogRaycaster> logBattle;

// ---- Inicio das Funcoes Implementadas ----

void RaycasterCombatScreen::configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    s_isContext3D = mode3D;
    s_contextMap = matrix;
    s_contextPostX = postX;
    s_contextPostY = postY;
    s_contextAngle = angle;
    s_contextTitleMap = title;
}

void RaycasterCombatScreen::setShiftVisible(int shift, const std::string& name) {
    (void)shift;
    std::string colorShift = (name == "INIMIGOS") ? "\033[1;38;2;255;100;100m" : "\033[1;38;2;100;255;100m";
    s_titleShiftHUD = colorShift + "[ TURNO DE " + name + " ]";
}

void RaycasterCombatScreen::addFixedMessage(const std::string& msg) {
    MsgLogRaycaster log;
    log.text = msg;
    log.timestamp = std::chrono::steady_clock::now();
    logBattle.push_back(log);
    if (logBattle.size() > 5) {
        logBattle.erase(logBattle.begin());
    }
}

void RaycasterCombatScreen::cleanMessagesFixed() {
    logBattle.clear();
}

void RaycasterCombatScreen::updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance, std::function<void(std::vector<std::string>&)> callbackOverlay) {
    int terminalWidth = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();
    (void)animateEntrance;
    (void)allies;
    (void)combatTitle;
    
    auto now = std::chrono::steady_clock::now();
    int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());

    std::vector<std::string> screen3D = RaycasterRendererCombat::renderFrame(
        s_contextTitleMap, currentPlayer, enemies, 
        nullptr, 0, 0, 0, false, timeMs, false, {}, 1.0f
    );
    
    // Desenho dos logs de batalha
    std::vector<MsgLogRaycaster> messagesActive;
    for (auto it = logBattle.begin(); it != logBattle.end(); ) {
        int elapsedMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - it->timestamp).count());
        if (elapsedMs > 5000) {
            it = logBattle.erase(it);
        } else {
            messagesActive.push_back(*it);
            ++it;
        }
    }

    if (!messagesActive.empty()) {
        int maxCompVis = 0;
        for (const auto& msg : messagesActive) {
            int compVis = Appearance::getVisualLength(msg.text);
            if (compVis > maxCompVis) maxCompVis = compVis;
        }

        if (maxCompVis > terminalWidth - 6) maxCompVis = terminalWidth - 6;

        int boxW = maxCompVis + 4; // 1 space padding each side + borders
        int boxX = terminalWidth - boxW - 2;
        if (boxX < 0) boxX = 0;
        int boxY = 2; // Canto superior direito

        std::string bg = "\033[48;2;25;25;25m";
        std::string br = "\033[38;2;255;255;255m";
        std::string traces = "";
        for (int i = 0; i < boxW - 2; ++i) traces += "═";

        if (boxY >= 0 && boxY < (int)screen3D.size()) {
            screen3D[boxY] = Appearance::superimposePanelOnAnsiLine(screen3D[boxY], bg + br + "╔" + traces + "╗\033[0m", boxX);
        }

        for (size_t i = 0; i < messagesActive.size(); ++i) {
            int yCurrent = boxY + 1 + i;
            if (yCurrent >= 0 && yCurrent < (int)screen3D.size()) {
                std::string text = messagesActive[i].text;
                int compText = Appearance::getVisualLength(text);
                int pad = maxCompVis - compText;
                if (pad < 0) pad = 0;
                std::string lineStr = bg + br + "║ " + bg + text + "\033[0m" + bg + std::string(pad, ' ') + br + " ║\033[0m";
                screen3D[yCurrent] = Appearance::superimposePanelOnAnsiLine(screen3D[yCurrent], lineStr, boxX);
            }
        }

        int yEnd = boxY + 1 + messagesActive.size();
        if (yEnd >= 0 && yEnd < (int)screen3D.size()) {
            screen3D[yEnd] = Appearance::superimposePanelOnAnsiLine(screen3D[yEnd], bg + br + "╚" + traces + "╝\033[0m", boxX);
        }
    }

    // Minigame de Parry Popup Overwrite
    if (!Parry::minigameBar.empty() || !Parry::minigameMessage.empty()) {
        int compBar = Appearance::getVisualLength(Parry::minigameBar);
        int compMsg = Appearance::getVisualLength(Parry::minigameMessage);
        int boxW = std::max(compBar, compMsg) + 10;
        if (boxW > terminalWidth - 2) boxW = terminalWidth - 2;
        int boxH = 7;
        int boxX = (terminalWidth - boxW) / 2;
        if (boxX < 0) boxX = 0;
        int boxY = (screen3D.size() - boxH) / 2;
        
        std::string bg = "\033[48;2;25;25;25m";
        std::string br = "\033[38;2;255;255;255m";
        std::string traces = "";
        for (int i = 0; i < boxW - 2; ++i) traces += "═";
        
        if (boxY >= 0 && boxY < (int)screen3D.size()) {
            screen3D[boxY] = Appearance::superimposePanelOnAnsiLine(screen3D[boxY], bg + br + "╔" + traces + "╗\033[0m", boxX);
        }
        
        for (int row = 1; row < boxH - 1; ++row) {
            int ty = boxY + row;
            if (ty >= 0 && ty < (int)screen3D.size()) {
                std::string emptyLine = std::string(boxW - 2, ' ');
                screen3D[ty] = Appearance::superimposePanelOnAnsiLine(screen3D[ty], bg + br + "║" + bg + emptyLine + br + "║\033[0m", boxX);
            }
        }
        
        if (boxY + boxH - 1 >= 0 && boxY + boxH - 1 < (int)screen3D.size()) {
            screen3D[boxY + boxH - 1] = Appearance::superimposePanelOnAnsiLine(screen3D[boxY + boxH - 1], bg + br + "╚" + traces + "╝\033[0m", boxX);
        }
        
        if (!Parry::minigameMessage.empty()) {
            RaycasterRendererCombat::paintTextNoBuffer(screen3D, terminalWidth, screen3D.size(), boxX + (boxW - compMsg)/2, boxY + 2, Parry::minigameMessage, "\033[38;2;255;255;255m", "\033[48;2;25;25;25m");
        }
        if (!Parry::minigameBar.empty()) {
            RaycasterRendererCombat::paintTextNoBuffer(screen3D, terminalWidth, screen3D.size(), boxX + (boxW - compBar)/2, boxY + 4, Parry::minigameBar, "\033[38;2;255;255;255m", "\033[48;2;25;25;25m");
        }
    }

    // Desenha o HUD
    if (currentPlayer) {
        RaycasterHUD::drawBarStatus(screen3D, terminalWidth, terminalHeight, currentPlayer, s_contextAngle, s_titleShiftHUD);
    }

    if (callbackOverlay) {
        callbackOverlay(screen3D);
    }

    // Joga na tela
    std::string out = "\033[?25l\033[H";
    for (size_t i = 0; i < screen3D.size(); ++i) {
        out += screen3D[i];
        if (i < screen3D.size() - 1) out += "\n";
    }
    Raycaster::s_lastFrameRendered = out;
    std::cout << out << std::flush;
}

void RaycasterCombatScreen::displayEnemyHordeSideBySide(const std::vector<Character*>& enemies, Character* targetAnimation, int frameAnimation, bool isHealing, bool animateEmergence, bool isDeath, Item* weaponAttacker, int damageAnimation, const std::vector<std::string>& dropsAnimation) {
    int terminalWidth = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();
    (void)animateEmergence;
    (void)weaponAttacker;
    
    auto now = std::chrono::steady_clock::now();
    int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());

    std::vector<std::string> screen3D = RaycasterRendererCombat::renderFrame(
        s_contextTitleMap, nullptr, enemies, 
        targetAnimation, frameAnimation, 0, damageAnimation, isHealing, timeMs, isDeath, dropsAnimation, 1.0f
    );

    // Joga na tela
    std::string out = "\033[?25l\033[H";
    for (size_t i = 0; i < screen3D.size(); ++i) {
        out += screen3D[i];
        if (i < screen3D.size() - 1) out += "\n";
    }
    Raycaster::s_lastFrameRendered = out;
    std::cout << out << std::flush;
}

static void rotateLoopAnimation(int framesTotals, int intervalMs, int step, const std::function<void(int)>& renderFrame) {
    for (int frame = 1; frame <= framesTotals; frame += step) {
        renderFrame(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}

void RaycasterCombatScreen::animateCombatIntro(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer) {
    (void)combatTitle;
    (void)enemies;
    (void)currentPlayer;
    // O usuario solicitou a remocao da animacao de introducao do combate
}

void RaycasterCombatScreen::displayLogoForCombatScreen(const std::string& screenTitle, bool animate) {
    (void)screenTitle;
    (void)animate;
}

void RaycasterCombatScreen::animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) {
    (void)combatTitle;
    (void)allies;
    rotateLoopAnimation(10, 50, 1, [&](int frame) {
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::vector<std::string> screen = RaycasterRendererCombat::renderFrame(s_contextTitleMap, currentPlayer, enemies, targetAnimation, frame, 0, damageAnimation, false, timeMs, false, {}, 1.0f);
        if (currentPlayer) RaycasterHUD::drawBarStatus(screen, Appearance::getTerminalWidth(), Appearance::getTerminalHeight(), currentPlayer, s_contextAngle, s_titleShiftHUD);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < screen.size(); ++i) {
            out += screen[i];
            if (i < screen.size() - 1) out += "\n";
        }
        Raycaster::s_lastFrameRendered = out;
        std::cout << out << std::flush;
    });
}

void RaycasterCombatScreen::animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) {
    (void)combatTitle;
    (void)allies;
    if (targetAnimation && healingAnimation > 0) {
        addFixedMessage(Appearance::color(Color::GREEN) + targetAnimation->getName() + " curou " + std::to_string(healingAnimation) + " HP!" + Appearance::color(Color::RESET));
    }
    rotateLoopAnimation(10, 50, 1, [&](int frame) {
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::vector<std::string> screen = RaycasterRendererCombat::renderFrame(s_contextTitleMap, currentPlayer, enemies, targetAnimation, frame, 0, healingAnimation, true, timeMs, false, {}, 1.0f);
        if (currentPlayer) RaycasterHUD::drawBarStatus(screen, Appearance::getTerminalWidth(), Appearance::getTerminalHeight(), currentPlayer, s_contextAngle, s_titleShiftHUD);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < screen.size(); ++i) {
            out += screen[i];
            if (i < screen.size() - 1) out += "\n";
        }
        Raycaster::s_lastFrameRendered = out;
        std::cout << out << std::flush;
    });
}

extern Character* g_enemyAttackerParry;

void RaycasterCombatScreen::animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) {
    (void)combatTitle;
    (void)allies;
    if (targetAnimation && damageAnimation >= 0 && !isParry) {
        std::string attacker = g_enemyAttackerParry ? g_enemyAttackerParry->getName() : "Inimigo";
        addFixedMessage(Appearance::color(Color::ORANGE) + attacker + " causou " + std::to_string(damageAnimation) + " dano no " + targetAnimation->getName() + "!" + Appearance::color(Color::RESET));
    }
    rotateLoopAnimation(6, 60, 1, [&](int frame) {
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::vector<std::string> screen3D = RaycasterRendererCombat::renderFrame(s_contextTitleMap, currentPlayer, enemies, nullptr, 0, frame, 0, false, timeMs, false, {}, 1.0f);
        
        RaycasterHUD::drawBarStatus(screen3D, width, height, currentPlayer, s_contextAngle, s_titleShiftHUD, frame, damageAnimation);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < screen3D.size(); ++i) {
            out += screen3D[i];
            if (i < screen3D.size() - 1) out += "\n";
        }
        Raycaster::s_lastFrameRendered = out;
        std::cout << out << std::flush;
    });
}

void RaycasterCombatScreen::animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) {
    (void)combatTitle;
    (void)allies;
    if (targetAnimation && healingAnimation > 0) {
        addFixedMessage(Appearance::color(Color::GREEN) + targetAnimation->getName() + " curou " + std::to_string(healingAnimation) + " HP!" + Appearance::color(Color::RESET));
    }
    rotateLoopAnimation(6, 60, 1, [&](int frame) {
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::vector<std::string> screen3D = RaycasterRendererCombat::renderFrame(s_contextTitleMap, currentPlayer, enemies, nullptr, 0, 0, 0, false, timeMs, false, {}, 1.0f);
        
        RaycasterHUD::drawBarStatus(screen3D, width, height, currentPlayer, s_contextAngle, s_titleShiftHUD, frame, healingAnimation, true);
        
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < screen3D.size(); ++i) {
            out += screen3D[i];
            if (i < screen3D.size() - 1) out += "\n";
        }
        Raycaster::s_lastFrameRendered = out;
        std::cout << out << std::flush;
    });
}
void RaycasterCombatScreen::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) {
    (void)combatTitle;
    (void)allies;
    rotateLoopAnimation(10, 60, 1, [&](int frame) {
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::vector<std::string> screen3D = RaycasterRendererCombat::renderFrame(s_contextTitleMap, currentPlayer, enemies, enemyDead, frame, 0, 0, false, timeMs, true, drops, 1.0f);
        RaycasterHUD::drawBarStatus(screen3D, width, height, currentPlayer, s_contextAngle, s_titleShiftHUD);
        std::string out = "\033[?25l\033[H";
        for (size_t i = 0; i < screen3D.size(); ++i) {
            out += screen3D[i];
            if (i < screen3D.size() - 1) out += "\n";
        }
        Raycaster::s_lastFrameRendered = out;
        std::cout << out << std::flush;
    });

    // Drops removidos daqui conforme solicitado; eles ja aparecem no log.
}

// Menus de Input
int RaycasterCombatScreen::getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    (void)currentTurn;
    (void)characterActing;
    InputControl::clearBuffer();
    CombatScreen::context.selectionTargetCurrent = -1; // Garante que nenhum alvo esta selecionado ao escolher acao
    std::vector<std::string> actions = {"Atacar", "Habilidade", "Defender", "Itens", "Diario"};
    int selected = 0;
    while(true) {
        int width = Appearance::getTerminalWidth();
        int height = Appearance::getTerminalHeight();
        
        int boxY = height - 10; 
        
        std::string bg = "\033[48;2;25;25;25m";
        std::string lineActions = "";
        for(size_t i=0; i<actions.size(); i++) {
            if (i == (size_t)selected) {
                lineActions += bg + "\033[38;2;255;255;0m> " + actions[i] + " <\033[0m";
            } else {
                lineActions += bg + "\033[38;2;150;150;150m  " + actions[i] + "  \033[0m";
            }
            if (i < actions.size() - 1) lineActions += bg + "   \033[0m";
        }
        
        int compVisual = 0;
        for (auto& a : actions) compVisual += a.length() + 4;
        compVisual += (actions.size() - 1) * 3;
        
        int boxW = compVisual + 4;
        int boxX = (width - boxW) / 2;
        
        auto overlayMenu = [&](std::vector<std::string>& screen) {
            std::string br = "\033[38;2;255;255;255m";
            std::string traces = "";
            for (int i = 0; i < boxW - 2; ++i) traces += "═";
            
            if (boxY >= 0 && boxY < (int)screen.size()) {
                screen[boxY] = Appearance::superimposePanelOnAnsiLine(screen[boxY], bg + br + "╔" + traces + "╗\033[0m", boxX);
            }
            if (boxY+1 >= 0 && boxY+1 < (int)screen.size()) {
                screen[boxY+1] = Appearance::superimposePanelOnAnsiLine(screen[boxY+1], bg + br + "║ " + lineActions + bg + br + " ║\033[0m", boxX);
            }
            if (boxY+2 >= 0 && boxY+2 < (int)screen.size()) {
                screen[boxY+2] = Appearance::superimposePanelOnAnsiLine(screen[boxY+2], bg + br + "╚" + traces + "╝\033[0m", boxX);
            }
        };

        updateScreenStatic("", enemies, currentPlayer, allies, false, overlayMenu);
        
        while(!InputControl::pressedKey()) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); 
            updateScreenStatic("", enemies, currentPlayer, allies, false, overlayMenu);
        }
        char c = InputControl::readKey();
        if(c == 'a' || c == 'A' || c == 75) selected = (selected - 1 + actions.size()) % actions.size();
        if(c == 'd' || c == 'D' || c == 77) selected = (selected + 1) % actions.size();
        if(c == '\r' || c == '\n') {
            if (selected == 0) return 1; // Atacar
            if (selected == 1) return 3; // Habilidade
            if (selected == 2) return 2; // Defender
            if (selected == 3) return 4; // Inventario
            if (selected == 4) return 6; // Diario
        }
    }
}

int RaycasterCombatScreen::getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    InputControl::clearBuffer();
    int selected = 0;
    while(true) {
        CombatScreen::context.selectionTargetCurrent = selected;
        updateScreenStatic(combatTitle, enemies, currentPlayer, allies, false);
        
        while(!InputControl::pressedKey()) { 
            std::this_thread::sleep_for(std::chrono::milliseconds(30)); 
            updateScreenStatic(combatTitle, enemies, currentPlayer, allies, false);
        }
        
        char c = InputControl::readKey();
        if(c == 'a' || c == 'A') selected = (selected - 1 + enemies.size()) % enemies.size();
        if(c == 'd' || c == 'D') selected = (selected + 1) % enemies.size();
        if(c == '\r' || c == '\n') {
            CombatScreen::context.selectionTargetCurrent = -1;
            return selected;
        }
        if(c == 27) {
            CombatScreen::context.selectionTargetCurrent = -1;
            return -1;
        }
    }
}

int RaycasterCombatScreen::getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) { return -1; }
int RaycasterCombatScreen::chooseShield(const std::string& characterName, const std::vector<Item*>& shields) {
    if (shields.empty()) return 0;
    std::vector<std::string> names;
    for (auto* shield : shields) names.push_back(shield->getItemName());
    int selection = InputControl::readMenuSelectionInPopup("ESCOLHA DE ESCUDO", {"Qual escudo deseja equipar?"}, names, Color::YELLOW);
    return selection + 1;
}
void RaycasterCombatScreen::selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies) {}
void RaycasterCombatScreen::notifyEnemiesMoreAct() { addFixedMessage("Inimigos sao mais ageis e atacam primeiro!"); }
void RaycasterCombatScreen::notifyShiftExtra(int, int) { addFixedMessage("Velocidade superior: Turno Extra!"); }
void RaycasterCombatScreen::notifyUnpreventionInventory() { addFixedMessage("Sem item rapido equipado!"); }
void RaycasterCombatScreen::notifyWithoutShields(const std::string& name) { addFixedMessage(name + " tentou defender mas nao tem escudos!"); }
void RaycasterCombatScreen::notifyImbalanceDefense(const std::string& name) { addFixedMessage(name + " teve sua defesa quebrada!"); }
void RaycasterCombatScreen::notifyPostureDefensive(const std::string& name, const std::string& shield) { 
    if (shield.empty()) addFixedMessage(name + " assumiu postura defensiva!");
    else addFixedMessage(name + " ergueu o " + shield + " para defender!");
}
void RaycasterCombatScreen::notifyActionInvalidates() { addFixedMessage("Acao Invalida!"); }
void RaycasterCombatScreen::notifyCancellationItem() { addFixedMessage("Uso de item cancelado."); }
void RaycasterCombatScreen::notifyUnmetRequirement(const std::string& m) { addFixedMessage(m); }

