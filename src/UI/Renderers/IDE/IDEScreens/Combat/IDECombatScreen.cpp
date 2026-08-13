#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <thread>
#include <chrono>
#include <fstream>
#include <functional>
#include <unordered_map>

#include "UI/Renderers/IDE/IDEScreens/Combat/IDECombatScreen.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/TerminalAppearance.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Items/Item.h"
#include "Core/Utils/InputControl.h"
#include "UI/Screens/Combat/CombatScreenLayout.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterCombatRenderer.h"
#include "Systems/Combat/Parry.h"
#include "UI/Screens/BaseScreen.h"
#include "UI/Screens/Scene3DScreen.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "UI/PerspectiveManager.h"

extern Character* g_enemyAttackerParry;
extern int g_parryStatus;

namespace {
    struct PlayerHUDVisualState {
        double hpGhost = -1.0;
        double hpPrevious = -1.0;
        int goldPrevious = -1;
        int xpPrevious = -1;
        long long timeLastRewardGold = 0;
        long long timeLastRewardXp = 0;
    };
    static std::unordered_map<Character*, PlayerHUDVisualState> hudStates;


    std::string generateBarDeXp(Character* currentPlayer, const std::string& colorXp, const std::string& colorReset) {
        int size = 8;
        double percentage = static_cast<double>(currentPlayer->getCurrentXp()) / std::max(1, currentPlayer->getXpForRise());
        std::string bar = BaseScreen::generateBarGradient(percentage, size, Color::CYAN);
        return "[" + bar + colorReset + "] " + colorXp + std::to_string(currentPlayer->getCurrentXp()) + colorReset + "/" + std::to_string(currentPlayer->getXpForRise());
    }

    struct DisplayEffect { std::string name; Color color; };
    DisplayEffect getDisplayEffect(EffectID id) {
        switch(id) {
            case EffectID::AttributesBuff: return {"Buff Atributos", Color::LIGHT_GREEN};
            case EffectID::HalfDamage: return {"Metade Dano", Color::CYAN};
            case EffectID::Invincible: return {"Inviolavel", Color::BLUE};
            case EffectID::Bleeding: return {"Sangramento", Color::RED};
            case EffectID::Slowness: return {"Lentidao", Color::MAGENTA};
            case EffectID::Weakness: return {"Fraqueza", Color::YELLOW};
            case EffectID::ResistanceBreak: return {"Quebra Def.", Color::CYAN};
            case EffectID::AdaptationWheel: return {"Adaptacao", Color::YELLOW};
            case EffectID::Necrosis: return {"Necrose", Color::MAGENTA};
            case EffectID::SureStrike: return {"Mira Certeira", Color::YELLOW};
            default: return {"", Color::RESET};
        }
    }

    std::string generateStringDeStatus(Character* currentPlayer, int timeMs) {
        std::vector<EffectID> effects;
        currentPlayer->getIDsEffectsAssets(effects);
        std::string status = "";
        bool blinksDanger = (timeMs / 300) % 2 == 0;
        for (EffectID id : effects) {
            auto disp = getDisplayEffect(id);
            if (!disp.name.empty()) {
                std::string colorUsed = Appearance::color(disp.color);
                if ((id == EffectID::Bleeding || id == EffectID::Slowness || id == EffectID::Weakness) && blinksDanger) {
                    colorUsed = "\033[38;2;255;100;100m"; // Rosa/Vermelho claro perigo
                }
                status += colorUsed + "[" + disp.name + "]" + Appearance::color(Color::RESET) + " ";
            }
        }
        return status.empty() ? "Nenhum" : status;
    }

    std::vector<std::string> messagesFixedCombat;
    std::string overlaySoonOnAnsiLine(const std::string& backgroundLine, const std::vector<std::string>& soonChars, int startX, const std::string& fgColor, int terminalWidth);

    void renderFrameBuffered(const std::function<void()>& renderFunc) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        renderFunc();
        std::cout.rdbuf(oldCout);
        
        std::string output = buffer.str();
        
        std::vector<std::string> lines;
        size_t start = 0, end = output.find('\n');
        while (end != std::string::npos) {
            lines.push_back(output.substr(start, end - start));
            start = end + 1;
            end = output.find('\n', start);
        }
        if (start < output.length()) lines.push_back(output.substr(start));

        int terminalHeight = Appearance::getTerminalHeight();
        int terminalWidth = Appearance::getTerminalWidth();
        int maxLines = (terminalHeight > 2) ? terminalHeight - 1 : 24; 
        
        if (static_cast<int>(lines.size()) > maxLines) {
            int linesForRemove = static_cast<int>(lines.size()) - maxLines;
            
            // Encontra a linha divisoria do HUD ("═════")
            int indexDivider = -1;
            for (int i = static_cast<int>(lines.size()) - 1; i >= 0; --i) {
                if (lines[i].find("═════") != std::string::npos) {
                    indexDivider = i;
                    break;
                }
            }

            if (indexDivider != -1) {
                // Calcula quantas linhas os espacamentos ocupam antes da divisoria
                int nL = 1; // Para o \n vazio que exibirHordaDeInimigosLadoALado sempre deixa no final
                
                int indexEndCut = indexDivider - nL; 
                int indexHomeCut = indexEndCut - linesForRemove;
                
                // Protege o Cabecalho (Nomes, HP, FCTs) + O Topo da arte do inimigo
                int linesProtected = 8; 
                if (indexHomeCut < linesProtected) {
                    indexHomeCut = linesProtected;
                    linesForRemove = indexEndCut - linesProtected;
                }
                
                if (linesForRemove > 0 && indexHomeCut >= 0 && indexHomeCut + linesForRemove <= static_cast<int>(lines.size())) {
                    // Remove do fundo da arte para manter o cabecalho (nome/vida) e o topo do monstro intactos
                    lines.erase(lines.begin() + indexHomeCut, lines.begin() + indexHomeCut + linesForRemove);
                } else if (linesForRemove > 0) {
                    // Fallback de seguranca
                    lines.erase(lines.begin(), lines.begin() + linesForRemove);
                }
            } else {
                lines.erase(lines.begin(), lines.begin() + linesForRemove);
            }
        }

        // Safety net de largura: trunca linhas que excedem a largura do terminal para evitar line wrap
        for (auto& line : lines) {
            int compVisual = Appearance::getVisualLength(line);
            if (compVisual > terminalWidth) {
                // Trunca respeitando sequencias ANSI (nao corta no meio de uma)
                std::string result = "";
                int accountantVisual = 0;
                for (size_t i = 0; i < line.length() && accountantVisual < terminalWidth; ) {
                    if (line[i] == '\033' && i + 1 < line.length() && line[i+1] == '[') {
                        // Copia a sequencia ANSI inteira sem contar como caractere visual
                        size_t homeAnsi = i;
                        i += 2;
                        while (i < line.length() && !(line[i] >= 0x40 && line[i] <= 0x7E)) i++;
                        if (i < line.length()) i++; // Inclui o caractere final
                        result += line.substr(homeAnsi, i - homeAnsi);
                    } else {
                        // Caractere normal (possivelmente multi-byte UTF-8)
                        unsigned char c = static_cast<unsigned char>(line[i]);
                        int len = 1;
                        if ((c & 0x80) == 0) len = 1;
                        else if ((c & 0xE0) == 0xC0) len = 2;
                        else if ((c & 0xF0) == 0xE0) len = 3;
                        else if ((c & 0xF8) == 0xF0) len = 4;
                        result += line.substr(i, len);
                        i += len;
                        accountantVisual++;
                    }
                }
                result += "\033[0m"; // Reseta cores no ponto de truncamento
                line = result;
            }
        }

        std::string endOutput = "";
        for (size_t i = 0; i < lines.size(); ++i) {
            endOutput += lines[i] + "\033[K";
            if (i < lines.size() - 1) endOutput += "\n";
        }
        
        std::cout << "\033[?25l\033[H" << endOutput << "\033[J\033[0m" << std::flush;
    }

    void renderScenePattern(const std::string& title, const std::vector<Character*>& enemies, Character* targetAnimation, int frame, bool isHealing, bool isDeath, Item* weapon, Character* currentPlayer, const std::vector<Character*>& allies, Character* targetDamagePlayer = nullptr, Color colorDamagePlayer = Color::RESET, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {}, bool animateEntrance = false) {
        // Detecta visao terminal/IDE e usa IDECombatScreen diretamente
        bool useTerminal = !PerspectiveManager::getInstance().is3DViewActive();
        if (useTerminal) {
            CombatScreen::context.isTerminalView = true;
            CombatScreen::context.isMode3D = false;
            IDECombatScreen::cleanMessagesFixed();
            // Se ha drops ativos, exibe os drops em vez do combate normal
            if (!dropsAnimation.empty()) {
                CombatScreen::context.dropsAssets = dropsAnimation;
                IDECombatScreen::displayDrops(dropsAnimation);
                CombatScreen::context.dropsAssets.clear();
                return;
            }
            IDECombatScreen::display(currentPlayer, enemies, title);
            return;
        }
        
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        renderFrameBuffered([&]() {
            (void)title;
            std::cout << "\n";

            int terminalWidth = Appearance::getTerminalWidth();

            // === Preparar dados do HUD (Comum para ambos os modos) ===
            std::vector<std::string> panelLeft;
            
            Character* highlight = currentPlayer;
            if (targetDamagePlayer != nullptr) {
                highlight = targetDamagePlayer;
            } else if (CombatScreen::context.characterHUD != nullptr) {
                highlight = CombatScreen::context.characterHUD;
            } else if (CombatScreen::context.nameShiftVisible != "" && CombatScreen::context.nameShiftVisible != "INIMIGOS") {
                if (currentPlayer->getName() == CombatScreen::context.nameShiftVisible) highlight = currentPlayer;
                for (auto* ally : allies) {
                    if (ally->getName() == CombatScreen::context.nameShiftVisible) highlight = ally;
                }
            }

            auto linesHighlight = IDECombatScreen::getPlayerStatusBarLines(highlight, (targetDamagePlayer == highlight) ? colorDamagePlayer : Color::RESET, (targetDamagePlayer == highlight) ? damageAnimation : -1, (targetDamagePlayer == highlight) ? frame : 0, (targetDamagePlayer == highlight) ? isHealing : false);
            panelLeft.insert(panelLeft.end(), linesHighlight.begin(), linesHighlight.end());

            std::vector<std::string> panelRight;
            const int WIDTH_PANEL_RIGHT = 50;

            auto padLineRight = [&](std::string& line) {
                int visualLen = Appearance::getVisualLength(line);
                if (visualLen < WIDTH_PANEL_RIGHT) {
                    line += std::string(WIDTH_PANEL_RIGHT - visualLen, ' ');
                }
            };

            if (CombatScreen::context.selectionActionCurrent != -1) {
                int blinkVal = (timeMs / 400) % 2;
                if (blinkVal < 0) blinkVal += 2;
                std::string cursorIcon = (blinkVal == 0) ? ">  " : " > ";

                std::string titleAction = CombatScreen::context.isMode3D ? "═══ ESCOLHA UMA ACAO ═══" : "public void EscolhaAcao() {";
                padLineRight(titleAction);
                panelRight.push_back(titleAction);
                for (size_t i = 0; i < 3; ++i) {
                    std::string lineSay = "";
                    for (size_t col = 0; col < 3; ++col) {
                        size_t idx = i + col * 3;
                        if (idx < CombatScreen::context.optionsMenuCurrent.size()) {
                            std::string op = "";
                            if (static_cast<int>(idx) == CombatScreen::context.selectionActionCurrent) {
                                op = Appearance::color(Color::GREEN) + cursorIcon + CombatScreen::context.optionsMenuCurrent[idx] + Appearance::color(Color::RESET);
                            } else {
                                op = "   " + CombatScreen::context.optionsMenuCurrent[idx];
                            }
                            if (col < 2) {
                                int pad = 18 - Appearance::getVisualLength(op);
                                lineSay += op + std::string(std::max(0, pad), ' ');
                            } else {
                                lineSay += op;
                            }
                        }
                    }
                    padLineRight(lineSay);
                    panelRight.push_back(lineSay);
                }
                std::string emptyLine = CombatScreen::context.isMode3D ? "" : "}";
                padLineRight(emptyLine);
                panelRight.push_back(emptyLine);
            } else if (CombatScreen::context.selectionTargetCurrent != -1) {
                std::vector<std::string> linesTarget;
                if (CombatScreen::context.isMode3D) {
                    linesTarget = {
                        "═══ ESCOLHA UM ALVO ═══",
                        "   < / > : Selecionar",
                        "   ENTER : Confirmar",
                        "   ESC   : Cancelar",
                        ""
                    };
                } else {
                    linesTarget = {
                        "public void EscolhaUmAlvo() {",
                        "    auto select = Input::EsquerdaDireita();",
                        "    auto conf   = Input::Enter();",
                        "    auto cancel = Input::Esc();",
                        "}"
                    };
                }
                for(auto& line : linesTarget) {
                    padLineRight(line);
                    panelRight.push_back(line);
                }
            } else {
                for (int i = 0; i < 5; ++i) {
                    std::string emptyLine = "";
                    padLineRight(emptyLine);
                    panelRight.push_back(emptyLine);
                }
            }

            const size_t HEIGHT_FIXED_HUD = 7;
            while (panelLeft.size() < HEIGHT_FIXED_HUD) {
                panelLeft.push_back("");
            }
            while (panelRight.size() < HEIGHT_FIXED_HUD) {
                std::string emptyLine = "";
                padLineRight(emptyLine);
                panelRight.push_back(emptyLine);
            }

            if (CombatScreen::context.isMode3D) {
                int framesDamageGame = 0;
                if (targetDamagePlayer != nullptr && damageAnimation > 0 && !isHealing) {
                    framesDamageGame = frame;
                }

                std::vector<std::string> frame3D = RaycasterRendererCombat::renderFrame(
                    CombatScreen::context.titleMapCurrent, 
                    currentPlayer, 
                    enemies,
                    targetAnimation,
                    frame,
                    framesDamageGame,
                    damageAnimation,
                    isHealing,
                    timeMs,
                    isDeath,
                    dropsAnimation
                );

                int height3D = frame3D.size();

                // Envolve os paineis do HUD com borda branca UTF-8 e fundo preto
                std::string bgBlack = "\033[48;2;0;0;0m";
                std::string colorEdgeWhite = "\033[38;2;255;255;255m";

                // 1. painelEsquerdo
                int maxLeftW = 0;
                for (const auto& l : panelLeft) {
                    int len = Appearance::getVisualLength(l);
                    if (len > maxLeftW) maxLeftW = len;
                }
                std::vector<std::string> panelLeftEmbroidery;
                
                std::string top = bgBlack + colorEdgeWhite + "╔";
                for (int k = 0; k < maxLeftW - 1; ++k) top += "═";
                top += "╗\033[0m";
                panelLeftEmbroidery.push_back(top);
                
                for (auto& l : panelLeft) {
                    std::string line = l;
                    size_t post = 0;
                    while ((post = line.find("\033[0m", post)) != std::string::npos) {
                        line.replace(post, 4, "\033[0m" + bgBlack);
                        post += 4 + bgBlack.length();
                    }
                    
                    std::string lineProcessed = bgBlack + line;
                    size_t postV = lineProcessed.find("║");
                    if (postV != std::string::npos) {
                        lineProcessed.replace(postV, 3, colorEdgeWhite + "║\033[0m" + bgBlack);
                    }
                    
                    int len = Appearance::getVisualLength(lineProcessed);
                    int pad = maxLeftW - len;
                    if (pad > 0) lineProcessed += std::string(pad, ' ');
                    lineProcessed += colorEdgeWhite + "║\033[0m";
                    panelLeftEmbroidery.push_back(lineProcessed);
                }
                
                std::string base = bgBlack + colorEdgeWhite + "╚";
                for (int k = 0; k < maxLeftW - 1; ++k) base += "═";
                base += "╝\033[0m";
                panelLeftEmbroidery.push_back(base);
                panelLeft = std::move(panelLeftEmbroidery);

                // 2. painelDireito
                int maxSayW = WIDTH_PANEL_RIGHT + 4;
                std::vector<std::string> panelRightEmbroidery;
                
                std::string topSay = bgBlack + colorEdgeWhite + "╔";
                for (int k = 0; k < maxSayW - 2; ++k) topSay += "═";
                topSay += "╗\033[0m";
                panelRightEmbroidery.push_back(topSay);
                
                for (auto& l : panelRight) {
                    std::string line = l;
                    size_t post = 0;
                    while ((post = line.find("\033[0m", post)) != std::string::npos) {
                        line.replace(post, 4, "\033[0m" + bgBlack);
                        post += 4 + bgBlack.length();
                    }
                    
                    std::string lineProcessed = bgBlack + colorEdgeWhite + "║\033[0m" + bgBlack + " " + line;
                    int len = Appearance::getVisualLength(lineProcessed);
                    int pad = maxSayW - len - 1;
                    if (pad > 0) lineProcessed += std::string(pad, ' ');
                    lineProcessed += colorEdgeWhite + "║\033[0m";
                    panelRightEmbroidery.push_back(lineProcessed);
                }
                
                std::string baseSay = bgBlack + colorEdgeWhite + "╚";
                for (int k = 0; k < maxSayW - 2; ++k) baseSay += "═";
                baseSay += "╝\033[0m";
                panelRightEmbroidery.push_back(baseSay);
                panelRight = std::move(panelRightEmbroidery);

                // Compile floating messages
                std::vector<std::string> panelMessages;

                auto cleanMsg = [](const std::string& msg) -> std::string {
                    std::string clean = msg;
                    while (!clean.empty() && (clean.back() == '\n' || clean.back() == '\r')) {
                        clean.pop_back();
                    }
                    size_t first = clean.find_first_not_of(" \t");
                    if (first != std::string::npos) {
                        clean = clean.substr(first);
                    } else {
                        return "";
                    }
                    size_t last = clean.find_last_not_of(" \t");
                    if (last != std::string::npos) {
                        clean = clean.substr(0, last + 1);
                    }
                    return clean;
                };

                if (CombatScreen::context.selectionTargetCurrent != -1 && CombatScreen::context.selectionTargetCurrent < static_cast<int>(enemies.size())) {
                    std::string feedbackTarget = "-> SELECIONADO: Inimigo (" + std::to_string(CombatScreen::context.selectionTargetCurrent + 1) + ") - " + enemies[CombatScreen::context.selectionTargetCurrent]->getName() + " <-";
                    panelMessages.push_back("\033[1;33m" + feedbackTarget + "\033[0m");
                }

                std::string shiftText = "TURNO " + std::to_string(CombatScreen::context.shiftCurrentVisible) + " │ VEZ DE " + CombatScreen::context.nameShiftVisible;
                panelMessages.push_back(shiftText);

                if (!InputControl::enterPromptText.empty()) {
                    std::string prompt = cleanMsg(InputControl::enterPromptText);
                    if (!prompt.empty()) panelMessages.push_back("\033[5m" + prompt + "\033[0m");
                }

                if (!Parry::minigameMessage.empty()) {
                    std::string msg = cleanMsg(Parry::minigameMessage);
                    if (!msg.empty()) panelMessages.push_back(msg);
                }
                if (!Parry::minigameBar.empty()) {
                    std::string msg = cleanMsg(Parry::minigameBar);
                    if (!msg.empty()) panelMessages.push_back(msg);
                }

                if (targetDamagePlayer != nullptr && damageAnimation > 0 && frame > 0 && !isHealing) {
                    std::string nameAttacker = "Desconhecido";
                    if (g_enemyAttackerParry != nullptr) {
                        for (size_t i = 0; i < enemies.size(); ++i) {
                            if (enemies[i] == g_enemyAttackerParry) {
                                nameAttacker = enemies[i]->getName() + " (" + std::to_string(i + 1) + ")";
                                break;
                            }
                        }
                        if (nameAttacker == "Desconhecido") nameAttacker = g_enemyAttackerParry->getName();
                    }
                    std::string messageDamage = nameAttacker + " causou " + std::to_string(damageAnimation) + " de dano em " + targetDamagePlayer->getName() + "!";
                    panelMessages.push_back(messageDamage);
                }

                if (!messagesFixedCombat.empty()) {
                    for (const auto& msg : messagesFixedCombat) {
                        std::string msgClean = cleanMsg(msg);
                        if (!msgClean.empty()) {
                            panelMessages.push_back(msgClean);
                        }
                    }
                }

                std::vector<std::string> panelMessagesEmbroidery;
                int maxMsgW = 0;
                if (!panelMessages.empty()) {
                    for (const auto& l : panelMessages) {
                        int len = Appearance::getVisualLength(l);
                        if (len > maxMsgW) maxMsgW = len;
                    }
                    if (maxMsgW < 40) maxMsgW = 40;
                    int boxMsgW = maxMsgW + 6; // 6 para margens internas e bordas

                    std::string topMsg = bgBlack + colorEdgeWhite + "╔";
                    for (int k = 0; k < boxMsgW - 2; ++k) topMsg += "═";
                    topMsg += "╗\033[0m";
                    panelMessagesEmbroidery.push_back(topMsg);

                    for (const auto& l : panelMessages) {
                        std::string line = l;
                        size_t post = 0;
                        while ((post = line.find("\033[0m", post)) != std::string::npos) {
                            line.replace(post, 4, "\033[0m" + bgBlack);
                            post += 4 + bgBlack.length();
                        }

                        int visualLen = Appearance::getVisualLength(line);
                        int totalPadding = (boxMsgW - 2) - visualLen;
                        int padLeft = std::max(0, totalPadding / 2);
                        int padSay = std::max(0, totalPadding - padLeft);

                        std::string lineProcessed = bgBlack + colorEdgeWhite + "║\033[0m" + bgBlack + std::string(padLeft, ' ') + line + std::string(padSay, ' ') + colorEdgeWhite + "║\033[0m";
                        panelMessagesEmbroidery.push_back(lineProcessed);
                    }

                    std::string baseMsg = bgBlack + colorEdgeWhite + "╚";
                    for (int k = 0; k < boxMsgW - 2; ++k) baseMsg += "═";
                    baseMsg += "╝\033[0m";
                    panelMessagesEmbroidery.push_back(baseMsg);
                }

                // Matematica de layout para centralizar paineis do HUD no fundo e a caixa de mensagens diretamente acima
                int totalHUDWidth = maxLeftW + 6 + maxSayW;
                int startHUDX = (terminalWidth - totalHUDWidth) / 2;
                if (startHUDX < 0) startHUDX = 0;

                int startLeftX = startHUDX;
                int startSayX = startHUDX + maxLeftW + 6;

                size_t maxLinesHUD = std::max(panelLeft.size(), panelRight.size());
                int startHUDY = height3D - maxLinesHUD - 1;
                int startMsgY = startHUDY - panelMessagesEmbroidery.size();

                for (size_t row = 0; row < maxLinesHUD; ++row) {
                    int y = startHUDY + row;
                    if (y >= 0 && y < height3D) {
                        if (row < panelLeft.size()) {
                            frame3D[y] = Appearance::superimposePanelOnAnsiLine(frame3D[y], panelLeft[row], startLeftX);
                        }
                        if (row < panelRight.size()) {
                            frame3D[y] = Appearance::superimposePanelOnAnsiLine(frame3D[y], panelRight[row], startSayX);
                        }
                    }
                }

                if (!panelMessagesEmbroidery.empty()) {
                    int startMsgX = (terminalWidth - (maxMsgW + 6)) / 2;
                    if (startMsgX < 0) startMsgX = 0;

                    for (size_t row = 0; row < panelMessagesEmbroidery.size(); ++row) {
                        int y = startMsgY + row;
                        if (y >= 0 && y < height3D) {
                            frame3D[y] = Appearance::superimposePanelOnAnsiLine(frame3D[y], panelMessagesEmbroidery[row], startMsgX);
                        }
                    }
                }

                // Compila e sobrepoe o painel de drops centralizado na area 3D restante acima do HUD e mensagens
                std::vector<std::string> dropsUsed = dropsAnimation.empty() ? CombatScreen::context.dropsAssets : dropsAnimation;
                std::vector<std::string> panelDrops;
                if (!dropsUsed.empty()) {
                    int maxDropsW = 12; // Comprimento de "DERROTADO!" e 10
                    for (const auto& d : dropsUsed) {
                        std::string cleanD = d;
                        size_t first = cleanD.find_first_not_of(" \t");
                        if (first != std::string::npos) cleanD = cleanD.substr(first);
                        size_t last = cleanD.find_last_not_of(" \t\r\n");
                        if (last != std::string::npos) cleanD = cleanD.substr(0, last + 1);
                        int len = Appearance::getVisualLength(cleanD);
                        if (len > maxDropsW) maxDropsW = len;
                    }
                    int boxW = maxDropsW + 6; // margens e bordas
                    
                    std::string topDrops = bgBlack + colorEdgeWhite + "╔";
                    for (int k = 0; k < boxW - 2; ++k) topDrops += "═";
                    topDrops += "╗\033[0m";
                    panelDrops.push_back(topDrops);
                    
                    // Linha para "DERROTADO!"
                    {
                        std::string label = "DERROTADO!";
                        std::string innerTxt = "\033[5;1;38;2;255;50;50m" + label + "\033[0m";
                        int txtLen = label.length();
                        int totalPadding = (boxW - 2) - txtLen;
                        int pLeft = totalPadding / 2;
                        int pSay = totalPadding - pLeft;
                        std::string line = bgBlack + colorEdgeWhite + "║\033[0m" + bgBlack + std::string(pLeft, ' ') + innerTxt + std::string(pSay, ' ') + colorEdgeWhite + "║\033[0m";
                        panelDrops.push_back(line);
                    }
                    
                    // Linhas para os drops
                    for (const auto& d : dropsUsed) {
                        std::string cleanD = d;
                        size_t first = cleanD.find_first_not_of(" \t");
                        if (first != std::string::npos) cleanD = cleanD.substr(first);
                        size_t last = cleanD.find_last_not_of(" \t\r\n");
                        if (last != std::string::npos) cleanD = cleanD.substr(0, last + 1);

                        std::string color = Appearance::color(Color::WHITE);
                        if (cleanD.find("XP") != std::string::npos) color = Appearance::color(Color::CYAN);
                        else if (cleanD.find("G") != std::string::npos) color = Appearance::color(Color::YELLOW);
                        
                        std::string innerTxt = color + cleanD + "\033[0m";
                        int txtLen = Appearance::getVisualLength(cleanD);
                        int totalPadding = (boxW - 2) - txtLen;
                        int pLeft = totalPadding / 2;
                        int pSay = totalPadding - pLeft;
                        std::string line = bgBlack + colorEdgeWhite + "║\033[0m" + bgBlack + std::string(pLeft, ' ') + innerTxt + std::string(pSay, ' ') + colorEdgeWhite + "║\033[0m";
                        panelDrops.push_back(line);
                    }
                    
                    std::string baseDrops = bgBlack + colorEdgeWhite + "╚";
                    for (int k = 0; k < boxW - 2; ++k) baseDrops += "═";
                    baseDrops += "╝\033[0m";
                    panelDrops.push_back(baseDrops);

                    // Encontra o indice do inimigo morto para posicionar a caixa de drops na sua coluna
                    Character* deadEnemy = (targetAnimation != nullptr && isDeath) ? targetAnimation : CombatScreen::context.enemyDeadWithDrops;
                    int deadIdx = -1;
                    if (deadEnemy != nullptr) {
                        for (size_t k = 0; k < enemies.size(); ++k) {
                            if (enemies[k] == deadEnemy) {
                                deadIdx = static_cast<int>(k);
                                break;
                            }
                        }
                    }

                    int startDropsX = (terminalWidth - boxW) / 2; // center screen fallback
                    if (deadIdx != -1 && !enemies.empty()) {
                        int indexInEnemies = static_cast<int>(enemies.size());
                        int widthColumn = terminalWidth / indexInEnemies;
                        int centerX = deadIdx * widthColumn + widthColumn / 2;
                        startDropsX = centerX - boxW / 2;
                    }
                    if (startDropsX < 0) startDropsX = 0;
                    if (startDropsX + boxW > terminalWidth) startDropsX = terminalWidth - boxW;
                    
                    int areaAvailableY = startMsgY; // area acima da caixa de mensagens e HUD
                    int startDropsY = (areaAvailableY - panelDrops.size()) / 2;
                    if (startDropsY < 0) startDropsY = 0;
                    
                    for (size_t row = 0; row < panelDrops.size(); ++row) {
                        int y = startDropsY + row;
                        if (y >= 0 && y < height3D) {
                            frame3D[y] = Appearance::superimposePanelOnAnsiLine(frame3D[y], panelDrops[row], startDropsX);
                        }
                    }
                }

                for (const auto& line : frame3D) {
                    std::cout << line << "\n";
                }
            } else {
                // Desenha a horda classicamente (que imprime no std::cout e sera interceptada pelo buffer)
                IDECombatScreen::displayEnemyHordeSideBySide(enemies, targetAnimation, frame, isHealing, animateEntrance, isDeath, weapon, damageAnimation, dropsAnimation);

                BaseScreen::printLineDivider('=');
                std::cout << Appearance::color(Color::RESET);

                Appearance::printSideASide(panelLeft, panelRight, 0, 5, Color::RESET, Color::RESET, 0);

                std::string shiftText = " TURNO " + std::to_string(CombatScreen::context.shiftCurrentVisible) + " | VEZ DE " + CombatScreen::context.nameShiftVisible + " ";
                int lengthVisual = Appearance::getVisualLength(shiftText);
                int tracesLeft = (terminalWidth - lengthVisual - 6) / 2;
                int tracesSay = terminalWidth - tracesLeft - lengthVisual - 6;
                if (tracesLeft < 0) tracesLeft = 0;
                if (tracesSay < 0) tracesSay = 0;
                
                std::string lineLeft = "/* ";
                for (int i = 0; i < tracesLeft; ++i) lineLeft += "-";
                std::string lineSay = "";
                for (int i = 0; i < tracesSay; ++i) lineSay += "-";
                lineSay += " */";
                
                Color shiftColor = (CombatScreen::context.nameShiftVisible == "INIMIGOS") ? Color::RED : Color::GREEN;
                std::string colorEdgeLeftComment = "\033[38;2;96;139;78m"; // Cor de comentario verde do VSCode
                
                std::cout << "\n" << colorEdgeLeftComment << lineLeft << Appearance::color(shiftColor) << shiftText << colorEdgeLeftComment << lineSay << Appearance::color(Color::RESET) << "\n";

                if (targetDamagePlayer != nullptr && damageAnimation > 0 && frame > 0 && !isHealing) {
                    int shakeX = (frame % 2 == 0) ? 4 : -4;
                    std::string padLeft = (shakeX > 0) ? std::string(shakeX, ' ') : "";
                    std::string padSay = (shakeX < 0) ? std::string(-shakeX, ' ') : "";

                    std::string nameAttacker = "Desconhecido";
                    if (g_enemyAttackerParry != nullptr) {
                        for (size_t i = 0; i < enemies.size(); ++i) {
                            if (enemies[i] == g_enemyAttackerParry) {
                                nameAttacker = enemies[i]->getName() + " (" + std::to_string(i + 1) + ")";
                                break;
                            }
                        }
                        if (nameAttacker == "Desconhecido") nameAttacker = g_enemyAttackerParry->getName();
                    }

                    std::string messageDamage = nameAttacker + " causou " + std::to_string(damageAnimation) + " de dano em " + targetDamagePlayer->getName() + "!";

                    std::cout << "\n";
                    if (frame % 2 == 0) std::cout << "\n";
                    
                    int spacesCentralize = std::max(0, (terminalWidth - Appearance::getVisualLength(messageDamage)) / 2);
                    std::cout << padLeft << std::string(spacesCentralize, ' ') << "\033[1;38;2;255;50;50m" << messageDamage << Appearance::color(Color::RESET) << padSay << "\n";
                }

                if (!messagesFixedCombat.empty()) {
                    std::cout << "\n";
                    for (const auto& msg : messagesFixedCombat) {
                        std::cout << msg;
                    }
                }
            }
        });
    }

    void executeAnimation(int framesTotals, int intervalMs, int step, const std::function<void(int)>& renderFrame, const std::function<void()>& renderEnd) {
        for (int frame = 1; frame <= framesTotals; frame += step) {
            renderFrame(frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        }
        renderEnd();
    }
}

void IDECombatScreen::setShiftVisible(int shift, const std::string& name) {
    CombatScreen::context.shiftCurrentVisible = shift;
    CombatScreen::context.nameShiftVisible = name;
}

void IDECombatScreen::addFixedMessage(const std::string& msg) {
    messagesFixedCombat.push_back(msg);
    if (messagesFixedCombat.size() > 6) { // Limita o numero de logs para nao empurrar o HUD para baixo demais
        messagesFixedCombat.erase(messagesFixedCombat.begin());
    }
}

void IDECombatScreen::configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    CombatScreen::context.isMode3D = mode3D;
    CombatScreen::context.currentMapMatrix = matrix;
    CombatScreen::context.playerPostX = postX;
    CombatScreen::context.playerPostY = postY;
    CombatScreen::context.playerAngle = angle;
    CombatScreen::context.titleMapCurrent = title;
}

void IDECombatScreen::cleanMessagesFixed() {
    messagesFixedCombat.clear();
}

void IDECombatScreen::displayLogoForCombatScreen(const std::string& screenTitle, bool animate) 
{
    std::cout << "\033[?25l"; // Esconde o cursor
    
    Appearance::displayArtPanel(ScreenCombatLayouts::getLogoCombat(), 95, Color::RED, screenTitle, animate);
}

void IDECombatScreen::animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    // Terminal/IDE view: intro sera exibida por renderizarCenaPadrao
    if (!PerspectiveManager::getInstance().is3DViewActive()) {
        return;
    }

    Appearance::clearScreen();
    Appearance::hideCursor();

    if (CombatScreen::context.isMode3D && currentPlayer != nullptr) {
        int terminalWidth = Appearance::getTerminalWidth();
        int terminalHeight = Appearance::getTerminalHeight();
        if (terminalWidth <= 0) terminalWidth = 120;
        if (terminalHeight <= 0) terminalHeight = 40;
        int heightHUD = 16;
        int height3D = std::max(10, terminalHeight - heightHUD);

        /*
         * ═══════════════════════════════════════════════════════════════════
         * Fase 1: Fundo 3D em Fade In (sem inimigos)
         * ═══════════════════════════════════════════════════════════════════
         */
        std::vector<std::string> frameBackground = RaycasterRendererCombat::renderFrame(
            CombatScreen::context.titleMapCurrent, 
            currentPlayer, 
            {}, // Nenhum inimigo
            nullptr, 0, 0, -1, false, 0, false, {}, 1.0f
        );

        // Renderiza fundo completo uma vez e fade-in apenas das linhas do fundo (sem \n final para evitar scroll)
        for (size_t i = 0; i+1 < frameBackground.size(); ++i) {
            std::cout << "\033[" << (i + 1) << ";1H" << frameBackground[i] << "\033[K\n";
        }
        if (!frameBackground.empty()) {
            int y = static_cast<int>(frameBackground.size()) - 1;
            std::cout << "\033[" << (y + 1) << ";1H" << frameBackground[y] << "\033[K";
        }
        
        Appearance::animateFadeIn(15, 30, [&](int /*frame*/, int intensity) {
            float ratio = intensity / 255.0f;
            std::ostringstream buffer;
            for (size_t i = 0; i < frameBackground.size(); ++i) {
                buffer << "\033[" << (i + 1) << ";1H" << Appearance::fatarLineAnsi(frameBackground[i], ratio) << "\033[K";
                if (i + 1 < frameBackground.size()) buffer << "\n";
            }
            std::cout << buffer.str() << std::flush;
        });

        class SceneIntroductionCombat3D : public ScreenScene3D {
        private:
            Character* currentPlayer;
            std::vector<Character*> enemies;
            std::vector<std::string> frameBackground;
            
        public:
            SceneIntroductionCombat3D(Character* player, const std::vector<Character*>& enemies, const std::vector<std::string>& backgroundFree)
                : currentPlayer(player), enemies(enemies), frameBackground(backgroundFree) {}
                
        protected:
            std::vector<std::string> getLogo() const override {
                return ScreenCombatLayouts::getLogoCombat();
            }

            std::string getLogoColor() const override {
                return "\033[1;31m";
            }

            std::vector<std::string> getBackgroundInitial() const override {
                return frameBackground;
            }

            std::vector<std::string> composeBackgroundWithElements(float opacity) const override {
                auto now = std::chrono::steady_clock::now();
                int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
                return RaycasterRendererCombat::renderFrame(
                    CombatScreen::context.titleMapCurrent, 
                    currentPlayer, 
                    enemies,
                    nullptr, 0, 0, -1, false, timeMs, false, {}, opacity
                );
            }
        };

        SceneIntroductionCombat3D scene(currentPlayer, enemies, frameBackground);
        scene.execute();

    } else {
        // 2D: Titulo com fade padronizado (suave, Y=2)
        displayLogoForCombatScreen(title, true);

        // Captura o estado final dos inimigos (2D)
        std::ostringstream bufferEnemies;
        std::streambuf* oldCout = std::cout.rdbuf(bufferEnemies.rdbuf());
        displayEnemyHordeSideBySide(enemies, nullptr, 0, false, false);
        std::cout.rdbuf(oldCout);
        std::string enemiesEndWithColors = bufferEnemies.str();
        std::string enemiesEndWithoutColors = Appearance::removeANSIColors(enemiesEndWithColors);

        // Loop de animacao de Fade-in dos Inimigos (2D)
        Appearance::animateFadeIn(15, 100, [&](int /*frame*/, int intensity) {
            std::string colorRGB = Appearance::getColorRGBFade(Color::WHITE, intensity);
            std::string contentFrame = colorRGB + enemiesEndWithoutColors + Appearance::color(Color::RESET);
            std::ostringstream buffer;
            buffer << "\033[H";
            {
                std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
                displayLogoForCombatScreen(title, false);
                std::cout << contentFrame;
                std::cout.rdbuf(oldCout);
            }
            std::cout << buffer.str() << std::flush;
        });
    }

    std::cout << "\n\033[0m";
    Appearance::printCentralized("Prepare-se! O combate esta prestes a comecar...", Appearance::color(Color::RED));
    InputControl::waitForEnter();
}

std::vector<std::string> IDECombatScreen::getPlayerStatusBarLines(Character* currentPlayer, Color colorHighlight, int damageAnimation, int frameAnimation, bool isHealing) 
{
    if (currentPlayer == nullptr) return {};
    std::string weaponName = (currentPlayer->getWeapons()) ? currentPlayer->getWeapons()->getItemName() + currentPlayer->getWeapons()->getInfoStatus() : "Punhos";
    std::string shieldName = (currentPlayer->getShield()) ? currentPlayer->getShield()->getItemName() + currentPlayer->getShield()->getInfoStatus() : "Nenhum";
    std::string armorName = (currentPlayer->getArmor()) ? currentPlayer->getArmor()->getItemName() + currentPlayer->getArmor()->getInfoStatus() : "Trapos";
    
    Item* consumableQuickly = currentPlayer->getConsumableQuickly();
    std::string nameCureQuickly = "Vazio";
    if (consumableQuickly) {
        nameCureQuickly = consumableQuickly->getItemName() + " (" + std::to_string(currentPlayer->getInventory()->countItem(consumableQuickly->getItemName())) + "x)";
    }

    auto now = std::chrono::steady_clock::now();
    int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());

    double lifePercentage = static_cast<double>(currentPlayer->getHealth()) / currentPlayer->getMaxHealth();
    std::string colorOrange = Appearance::color(Color::YELLOW);
    std::string colorCyan = Appearance::color(Color::CYAN);
    std::string colorReset = Appearance::color(Color::RESET);
    
    std::string colorLife = (lifePercentage > 0.70) ? Appearance::color(Color::GREEN) : (lifePercentage > 0.30) ? colorOrange : Appearance::color(Color::RED);

    int speedBeat = (lifePercentage <= 0.30) ? 300 : 1000;
    int modBeat = timeMs % speedBeat;
    if (modBeat < 0) modBeat += speedBeat;
    int frameBeat = modBeat < 150 ? 1 : 0;

    std::string colorHeart = colorLife;
    if (frameBeat == 1) {
        if (lifePercentage > 0.70) colorHeart = "\033[38;2;150;255;150m"; // Verde mais claro
        else if (lifePercentage > 0.30) colorHeart = "\033[38;2;255;255;150m"; // Amarelo mais claro
        else colorHeart = "\033[38;2;255;100;100m"; // Vermelho mais claro
    }

    std::vector<std::string> heartArt;
    if (CombatScreen::context.isMode3D) {
        if (lifePercentage > 0.70) {
            if (frameBeat == 0) heartArt = { " ▄██▄ ▄██▄ ", "███████████", " ▀███████▀ ", "   ▀███▀   " };
            else                  heartArt = { "           ", "  ▄█████▄  ", "  ▀█████▀  ", "    ▀█▀    " };
        } else if (lifePercentage > 0.30) {
            if (frameBeat == 0) heartArt = { " ▄██▄ ▄▄   ", "██████████ ", " ▀██████▀  ", "   ▀███▀   " };
            else                  heartArt = { "           ", "  ▄████▄   ", "  ▀████▀   ", "    ▀█▀    " };
        } else {
            if (frameBeat == 0) heartArt = { " ▄█▄   ▄█▄ ", "████   ████", " ▀███████▀ ", "   ▀███▀   " };
            else                  heartArt = { "           ", "  ▄█▄ ▄█▄  ", "  ▀█████▀  ", "    ▀█▀    " };
        }
    } else {
        if (lifePercentage > 0.70) {
            if (frameBeat == 0) heartArt = { "   _   _   ", "  / \\_/ \\  ", "  \\     /  ", "   \\___/   " };
            else                  heartArt = { "   _   _   ", "  / \\_/ \\  ", "  \\ \\_/ /  ", "   \\___/   " };
        } else if (lifePercentage > 0.30) {
            if (frameBeat == 0) heartArt = { "   _   _   ", "  / \\// \\  ", "  \\  \\ /   ", "   \\___/   " };
            else                  heartArt = { "   _   _   ", "  / \\// \\  ", "  \\ \\/ /   ", "   \\___/   " };
        } else {
            if (frameBeat == 0) heartArt = { "  _     _  ", " / \\   / \\ ", " \\     \\_/ ", "  \\___/    " };
            else                  heartArt = { "  _     _  ", " / \\   / \\ ", " \\ \\_  \\_/ ", "  \\___/    " };
        }
    }
    for (auto& line : heartArt) line = colorHeart + line + colorReset;

    PlayerHUDVisualState& stateHUD = hudStates[currentPlayer];
    double lifeCurrent = currentPlayer->getHealth();
    if (stateHUD.hpPrevious == -1.0) stateHUD.hpGhost = lifeCurrent;
    if (lifeCurrent < stateHUD.hpGhost) {
        stateHUD.hpGhost -= std::max(0.5, (stateHUD.hpGhost - lifeCurrent) * 0.15);
        if (stateHUD.hpGhost < lifeCurrent) stateHUD.hpGhost = lifeCurrent;
    } else {
        stateHUD.hpGhost = lifeCurrent;
    }
    stateHUD.hpPrevious = lifeCurrent;

    double percentageGhost = stateHUD.hpGhost / currentPlayer->getMaxHealth();
    int sizeBar = 8;
    int qtyReal = static_cast<int>(lifePercentage * sizeBar * 8);
    int qtyGhost = static_cast<int>(percentageGhost * sizeBar * 8);
    std::string barHP = "";
    std::string colorGhost = "\033[38;2;255;100;100m";
    std::string colorBackgroundHP = Appearance::color(Color::GRAY);
    Color baseColorLife = (lifePercentage > 0.70) ? Color::GREEN : (lifePercentage > 0.30) ? Color::YELLOW : Color::RED;
    for (int i = 0; i < sizeBar; ++i) {
        int intensity = 130 + (125 * i) / std::max(1, sizeBar - 1);
        std::string colorCurrent = Appearance::getColorRGBFade(baseColorLife, intensity);
        int charIdx = i * 8;
        if (qtyReal >= charIdx + 4) barHP += colorCurrent + "█";
        else if (qtyGhost >= charIdx + 4) barHP += colorGhost + "█";
        else barHP += colorBackgroundHP + "░";
    }

    int goldCurrent = currentPlayer->getInventory()->getGold();
    int xpCurrent = currentPlayer->getCurrentXp();
    if (stateHUD.goldPrevious == -1) stateHUD.goldPrevious = goldCurrent;
    if (stateHUD.xpPrevious == -1) stateHUD.xpPrevious = xpCurrent;
    if (goldCurrent > stateHUD.goldPrevious) stateHUD.timeLastRewardGold = timeMs;
    if (xpCurrent > stateHUD.xpPrevious) stateHUD.timeLastRewardXp = timeMs;
    stateHUD.goldPrevious = goldCurrent;
    stateHUD.xpPrevious = xpCurrent;
    int modGold = (timeMs / 150) % 2;
    if (modGold < 0) modGold += 2;
    int modXp = (timeMs / 150) % 2;
    if (modXp < 0) modXp += 2;
    bool blinkGold = (timeMs - stateHUD.timeLastRewardGold < 1500) && (modGold == 0);
    bool blinkXp = (timeMs - stateHUD.timeLastRewardXp < 1500) && (modXp == 0);
    std::string colorGold = blinkGold ? "\033[38;2;255;255;100m" : colorOrange;
    std::string colorXpStr = blinkXp ? "\033[38;2;150;255;255m" : colorCyan;

    std::string xpBarArt = generateBarDeXp(currentPlayer, colorXpStr, colorReset);
    std::string statusStr = generateStringDeStatus(currentPlayer, timeMs);

    std::string fctPrint = "";
    if (damageAnimation > 0 && frameAnimation > 0 && isHealing) {
        std::string colorFCT;
        std::string textFloating = "+" + std::to_string(damageAnimation) + "!";
        if (frameAnimation <= 3) colorFCT = "\033[1;38;2;150;255;150m";
        else if (frameAnimation <= 6) colorFCT = "\033[1;38;2;50;255;50m";
        else if (frameAnimation <= 9) colorFCT = "\033[1;38;2;0;200;0m";
        else colorFCT = "\033[1;38;2;0;150;0m";
        fctPrint = "  " + colorFCT + textFloating + "\033[0m";
    }
    std::string emptyPadding(10, ' ');

    std::string parryPrint = "";
    if (g_parryStatus > 0 && frameAnimation > 0 && frameAnimation <= 12) {
        bool isBlinkColorful = (frameAnimation <= 6 && frameAnimation % 2 == 1) || (frameAnimation > 6);
        std::string txtParry;
        Color colorParry = Color::RESET;
        if (g_parryStatus == 1) { txtParry = "  [Parry Perfeito!]"; colorParry = Color::GREEN; }
        else if (g_parryStatus == 2) { txtParry = "  [Parry Efetivo!]"; colorParry = Color::YELLOW; }
        else if (g_parryStatus == 3) { txtParry = "  [Parry Falhou!]"; colorParry = Color::RED; }

        if (isBlinkColorful) parryPrint = Appearance::color(colorParry) + txtParry + Appearance::color(Color::RESET);
        else parryPrint = std::string(txtParry.length(), ' ');
    }

    std::string playerTag = (colorHighlight != Color::RESET) ? Appearance::color(colorHighlight) + currentPlayer->getName() + Appearance::color(Color::RESET) : currentPlayer->getName();

    std::vector<std::string> linesForPrint = {
        "║ " + heartArt[0] + " ║ " + playerTag + " (" + currentPlayer->getRace()->getRaceName() + "/" + currentPlayer->getClassName() + ") ║ HP: [" + barHP + colorReset + "] " + colorLife + std::to_string(currentPlayer->getHealth()) + colorReset + "/" + std::to_string(currentPlayer->getMaxHealth()) + parryPrint + fctPrint + emptyPadding,
        "║ " + heartArt[1] + " ║ NIVEL: " + std::to_string(currentPlayer->getLevel()) + " ║ XP: " + xpBarArt + " ║ OURO: " + colorGold + std::to_string(currentPlayer->getInventory()->getGold()) + "G" + colorReset + emptyPadding,
        "║ " + heartArt[2] + " ║ ARMA: " + weaponName + " ║ CURA RAP.: " + nameCureQuickly + emptyPadding,
        "║ " + heartArt[3] + " ║ ESC: " + shieldName + " ║ ARM: " + armorName + emptyPadding,
        "║ " + std::string(11, ' ') + " ║ STATUS: " + statusStr + emptyPadding
    };

    return linesForPrint;
}

void IDECombatScreen::displayEnemyHordeSideBySide(const std::vector<Character*>& enemies, Character* targetAnimation, int frameAnimation, bool isHealing, bool animateEmergence, bool isDeath, Item* /*armaAtacante*/, int damageAnimation, const std::vector<std::string>& dropsAnimation) 
{
    if (enemies.empty()) return;
    int terminalWidth = Appearance::getTerminalWidth();
    int terminalHeight = Appearance::getTerminalHeight();
    const std::vector<std::string>& originalEnemyArt = enemies[0]->getRace()->getAppearanceRace();
    
    std::vector<std::string> artReducedLocation;
    const std::vector<std::string>* enemyArtPtr = &originalEnemyArt;
    
    // Aplica o padrao global de compressao para inimigos (se a arte for maior que algumas linhas minimas)
    if (static_cast<int>(originalEnemyArt.size()) > 10) {
        artReducedLocation = Appearance::reduceScaleAscii(originalEnemyArt, Appearance::FACTOR_COMPRESSION_GLOBAL, Appearance::FACTOR_COMPRESSION_GLOBAL);
        enemyArtPtr = &artReducedLocation;
    }
    const std::vector<std::string>& enemyArt = *enemyArtPtr;
    
    int totalEnemiesQuantityInHorde = static_cast<int>(enemies.size());
    int widthSeparateForEachColumn = terminalWidth / totalEnemiesQuantityInHorde; 

    auto splitUTF8 = [](const std::string& s) {
        std::vector<std::string> chars;
        for (size_t i = 0; i < s.length(); ) {
            int len = 1;
            unsigned char c = static_cast<unsigned char>(s[i]);
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            chars.push_back(s.substr(i, len));
            i += len;
        }
        return chars;
    };

    auto now = std::chrono::steady_clock::now();
    int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
    
    std::vector<int> offsetsIdle;
    for (size_t idx = 0; idx < enemies.size(); ++idx) {
        Character* enemy = enemies[idx];
        if (enemy->getHealth() <= 0 || enemy->getDeathLively() || (enemy == targetAnimation && frameAnimation > 0 && !isDeath && !isHealing)) {
            offsetsIdle.push_back(0); // Mantem estatico durante animacoes de dano ou morte
        } else {
            // Ciclo de balanco: Move horizontalmente de forma suave (esquerda e direita)
            int cycle = ((timeMs + idx * 500) / 200) % 8;
            if (cycle < 0) cycle += 8;
            int offset = 0;
            if (cycle == 1 || cycle == 3) offset = 1;
            else if (cycle == 2) offset = 2;
            else if (cycle == 5 || cycle == 7) offset = -1;
            else if (cycle == 6) offset = -2;
            offsetsIdle.push_back(offset);
        }
    }

    auto printLineHorde = [&](const std::function<std::pair<std::string, std::string>(Character*, size_t)>& generator) {
        for (size_t i = 0; i < enemies.size(); ++i) {
            auto [textVisual, textPrint] = generator(enemies[i], i);
            int compVisual = Appearance::getVisualLength(textVisual);
            int spacesLeft = std::max(0, (widthSeparateForEachColumn - compVisual) / 2);
            spacesLeft += offsetsIdle[i];
            if (spacesLeft < 0) spacesLeft = 0;
            
            std::cout << std::string(spacesLeft, ' ') << textPrint;
            if (i < enemies.size() - 1) {
                int spacesSay = std::max(0, widthSeparateForEachColumn - spacesLeft - compVisual);
                std::cout << std::string(spacesSay, ' ');
            }
        }
        std::cout << "\n";
    };

    auto formatFadeOut = [&](Character* enemy, const std::string& textVisual, const std::string& textPrint) -> std::pair<std::string, std::string> {
        int compVisual = Appearance::getVisualLength(textVisual);
        if (enemy->getDeathLively()) {
            return std::makeup_pair(std::string(compVisual, ' '), std::string(compVisual, ' '));
        }

        if (isDeath && enemy == targetAnimation && frameAnimation > 0) {
            int maxFrames = static_cast<int>(enemy->getRace()->getAppearanceRace().size());
            double progress = std::min(1.0, static_cast<double>(frameAnimation) / maxFrames);
            int intensity = std::max(0, 255 - static_cast<int>(255.0 * progress));
            
            if (intensity < 30) {
                return std::makeup_pair(std::string(compVisual, ' '), std::string(compVisual, ' '));
            } else {
                // Retira as cores internas originais e aplica a escala de cinza de forma agressiva!
                std::string colorFade = "\033[38;2;" + std::to_string(intensity) + ";" + std::to_string(intensity) + ";" + std::to_string(intensity) + "m";
                return std::makeup_pair(textVisual, colorFade + textVisual + "\033[0m");
            }
        }
        return std::makeup_pair(textVisual, textPrint);
    };

    for (int fctLine = 0; fctLine < 2; ++fctLine) {
        printLineHorde([&](Character* enemy, size_t /*i*/) {
            std::string visualStr = "", printStr = "";
            if (enemy == targetAnimation && damageAnimation > 0 && !isDeath && frameAnimation > 0) {
                int targetLine = (frameAnimation < 4) ? 1 : 0;
                if (fctLine == targetLine) {
                    std::string colorFCT;
                    std::string textFloating;
                    if (isHealing) {
                        textFloating = "+" + std::to_string(damageAnimation) + "!";
                        if (frameAnimation <= 2) colorFCT = "\033[1;38;2;150;255;150m";
                        else if (frameAnimation <= 4) colorFCT = "\033[1;38;2;50;255;50m";
                        else colorFCT = "\033[1;38;2;0;200;0m";
                    } else {
                        textFloating = "-" + std::to_string(damageAnimation) + "!";
                        if (frameAnimation <= 2) colorFCT = "\033[1;38;2;255;200;0m";
                        else if (frameAnimation <= 4) colorFCT = "\033[1;38;2;255;100;0m";
                        else if (frameAnimation <= 6) colorFCT = "\033[1;38;2;255;0;0m";
                        else colorFCT = "\033[1;38;2;150;0;0m";
                    }
                    visualStr = textFloating;
                    printStr = colorFCT + textFloating + "\033[0m";
                }
            }
            return std::makeup_pair(visualStr, printStr);
        });
    }

    bool hordeHasDebuffs = false;
    for (auto* enemy : enemies) {
        std::vector<EffectID> effs; enemy->getIDsEffectsAssets(effs);
        if (!effs.empty()) { hordeHasDebuffs = true; break; }
    }

    if (hordeHasDebuffs) {
        printLineHorde([&](Character* enemy, size_t /*i*/) {
            std::vector<EffectID> effectsAssets;
            enemy->getIDsEffectsAssets(effectsAssets);
            std::string visualStr = "", printStr = "";
            for (size_t e = 0; e < effectsAssets.size(); ++e) {
                auto disp = getDisplayEffect(effectsAssets[e]);
                if (!disp.name.empty()) {
                    visualStr += "[" + disp.name + "]";
                    printStr += Appearance::color(disp.color) + "[" + disp.name + "]" + Appearance::color(Color::RESET);
                    if (e < effectsAssets.size() - 1) { visualStr += " "; printStr += " "; }
                }
            }
            return formatFadeOut(enemy, visualStr, printStr);
        });
    }

    printLineHorde([&](Character* enemy, size_t i) {
        std::string tag = enemy->getName();
        if (enemies.size() > 1) {
            tag += " (" + std::to_string(i + 1) + ")";
        }
        std::string printTag = tag;
        if (CombatScreen::context.selectionTargetCurrent == static_cast<int>(i)) {
            tag = "> " + tag + " <";
            if (CombatScreen::context.blinkSelection) {
                printTag = Appearance::color(Color::YELLOW) + tag + Appearance::color(Color::RESET);
            } else {
                printTag = Appearance::color(Color::GRAY) + tag + Appearance::color(Color::RESET);
            }
        } else if (enemy == g_enemyAttackerParry) {
            printTag = "\033[38;2;255;140;0m" + tag + Appearance::color(Color::RESET); // Laranja Escuro
        }
        return formatFadeOut(enemy, tag, printTag);
    });

    printLineHorde([&](Character* enemy, size_t /*i*/) {
        double pctLife = static_cast<double>(enemy->getHealth()) / std::max(1, enemy->getMaxHealth());
        Color colorLifeEnemy = (pctLife > 0.7) ? Color::GREEN : (pctLife > 0.3) ? Color::YELLOW : Color::RED;
        std::string barPrint = BaseScreen::generateBarGradient(pctLife, 8, colorLifeEnemy);
        std::string barVisual = Appearance::removeANSIColors(barPrint);
        
        std::string hpStr = std::to_string(enemy->getHealth()) + "/" + std::to_string(enemy->getMaxHealth());
        std::string textVisual = "HP: [" + barVisual + "] " + hpStr;
        std::string printHp = "HP: [" + barPrint + Appearance::color(Color::RESET) + "] " + hpStr;
        
        if (enemy == g_enemyAttackerParry) {
            printHp = "HP: [" + barPrint + Appearance::color(Color::RESET) + "] \033[38;2;255;140;0m" + hpStr + Appearance::color(Color::RESET);
        }
        return formatFadeOut(enemy, textVisual, printHp);
    });


    
    std::cout << "\n";
        
    std::vector<std::string> artLines;
    for (size_t artLineIndex = 0; artLineIndex < enemyArt.size(); artLineIndex++) 
    {
        std::string lineCurrent = "";
        for (size_t enemyDrawIndex = 0; enemyDrawIndex < enemies.size(); enemyDrawIndex++) 
        {
            Character* enemyCurrent = enemies[enemyDrawIndex];
            
            int offset = offsetsIdle[enemyDrawIndex];
            int lineReal = static_cast<int>(artLineIndex);
            std::string lineArt;
            int visibleLen;
            
            if (lineReal >= 0 && lineReal < static_cast<int>(enemyArt.size())) {
                lineArt = enemyArt[lineReal];
                auto characters = splitUTF8(lineArt);
                visibleLen = static_cast<int>(characters.size());
            } else {
                visibleLen = static_cast<int>(splitUTF8(enemyArt[0]).size()); // Margem de seguranca de tamanho
                lineArt = std::string(visibleLen, ' ');
            }
            
            int spacesForCentralizeAArt = (widthSeparateForEachColumn - visibleLen) / 2;
            spacesForCentralizeAArt += offset; // Aplica o deslocamento horizontal na margem da arte
            if (spacesForCentralizeAArt < 0) spacesForCentralizeAArt = 0; // Evita crashes com margens negativas
            std::string spaces(spacesForCentralizeAArt, ' ');
            
            if (enemyCurrent->getDeathLively()) {
                lineArt = std::string(visibleLen, ' ');
            } else if (isDeath && enemyCurrent == targetAnimation) {
                int totalLinesArt = static_cast<int>(enemyArt.size());
                if (totalLinesArt <= 0) totalLinesArt = 1;
                
                if (frameAnimation >= totalLinesArt) {
                    lineArt = std::string(visibleLen, ' ');
                    int maxTextLen = 10; // Tamanho de "DERROTADO!"
                    for (const auto& d : dropsAnimation) {
                        int compVisual = Appearance::getVisualLength(d);
                        if (compVisual > maxTextLen) maxTextLen = compVisual;
                    }
                    if (maxTextLen > visibleLen - 4) maxTextLen = visibleLen - 4;
                    if (maxTextLen < 0) maxTextLen = 0;

                    int totalBoxLines = static_cast<int>(dropsAnimation.size()) + 3; // +1 texto base, +2 bordas
                    int startDropLine = (totalLinesArt - totalBoxLines) / 2;
                    if (startDropLine < 0) startDropLine = 0;
                    
                    int currentLineIndex = static_cast<int>(artLineIndex) - startDropLine;
                    if (currentLineIndex >= 0 && currentLineIndex < totalBoxLines) {
                        std::string colorEdge = Appearance::color(Color::GRAY);
                        std::string colorReset = Appearance::color(Color::RESET);
                        
                        int space = (visibleLen - (maxTextLen + 4)) / 2;
                        if (space < 0) space = 0;
                        int rem = visibleLen - space - (maxTextLen + 4);
                        if (rem < 0) rem = 0;
                        
                        if (currentLineIndex == 0) {
                            std::string b = "╔"; for(int k=0; k<maxTextLen+2; ++k) b += "═"; b += "╗";
                            lineArt = std::string(space, ' ') + colorEdge + b + colorReset + std::string(rem, ' ');
                        } else if (currentLineIndex == totalBoxLines - 1) {
                            std::string b = "╚"; for(int k=0; k<maxTextLen+2; ++k) b += "═"; b += "╝";
                            lineArt = std::string(space, ' ') + colorEdge + b + colorReset + std::string(rem, ' ');
                        } else {
                            int textIdx = currentLineIndex - 1;
                            std::string innerTxt = (textIdx == 0) ? "DERROTADO!" : dropsAnimation[textIdx - 1];
                            
                            std::string colorDrop = Appearance::color(Color::WHITE);
                            if (textIdx == 0) colorDrop = "\033[5m" + Appearance::color(Color::RED); // Piscar Vermelho
                            else if (innerTxt.find("XP") != std::string::npos) colorDrop = Appearance::color(Color::CYAN);
                            else if (innerTxt.find("G") != std::string::npos) colorDrop = Appearance::color(Color::YELLOW);
                            
                            int txtLen = Appearance::getVisualLength(innerTxt);
                            if (txtLen > maxTextLen) {
                                innerTxt = Appearance::removeANSIColors(innerTxt).substr(0, maxTextLen);
                                txtLen = maxTextLen;
                            }
                            
                            int pLeft = (maxTextLen - txtLen) / 2;
                            int pSay = maxTextLen - txtLen - pLeft;
                            
                            std::string middle = colorEdge + "║ " + std::string(pLeft, ' ') + colorDrop + innerTxt + colorReset + colorEdge + std::string(pSay, ' ') + " ║" + colorReset;
                            lineArt = std::string(space, ' ') + middle + std::string(rem, ' ');
                        }
                    }
                } else {
                    // Efeito de desintegracao dithered (virando poeira) + fade de cor na visao top-down (2D)
                    double progress = std::min(1.0, static_cast<double>(frameAnimation) / totalLinesArt);
                    int baseR = 200, baseG = 200, baseB = 200; // Cinza claro solicitado pelo usuario
                    double fade = 1.0 - progress;
                    int rMod = static_cast<int>(baseR * fade);
                    int gMod = static_cast<int>(baseG * fade);
                    int bMod = static_cast<int>(baseB * fade);
                    
                    auto characters = splitUTF8(lineArt);
                    std::string lineWithEffect = "";
                    for (size_t k = 0; k < characters.size(); ++k) {
                        if (characters[k] == " " || characters[k] == "") {
                            lineWithEffect += " ";
                        } else {
                            int hash = (static_cast<int>(k) * 37 + lineReal * 57) % 100;
                            if (hash < progress * 100) {
                                if (progress < 0.8 && (hash % 3 == 0)) {
                                    int dustIntensity = static_cast<int>(100 * (1.0 - progress));
                                    lineWithEffect += "\033[38;2;" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + ";" + std::to_string(dustIntensity) + "m.\033[0m";
                                } else {
                                    lineWithEffect += " ";
                                }
                            } else {
                                    lineWithEffect += "\033[38;2;" + std::to_string(rMod) + ";" + std::to_string(gMod) + ";" + std::to_string(bMod) + "m" + characters[k] + "\033[0m";
                            }
                        }
                    }
                    lineArt = lineWithEffect;
                }
            }

            lineCurrent += spaces;
            
            if (enemyCurrent == targetAnimation && frameAnimation > 0 && !isDeath) {
                std::string baseLine;
                std::string colorHighlight = isHealing ? Appearance::color(Color::GREEN) : Appearance::color(Color::RED);
                
                bool isBlinkColorful = (frameAnimation <= 4 && frameAnimation % 2 == 1);
                bool isInvisible = (frameAnimation <= 4 && frameAnimation % 2 == 0);

                if (!isInvisible) {
                    baseLine = lineArt;
                } else {
                    baseLine = std::string(visibleLen, ' ');
                }

                if (enemyCurrent->getDeathLively()) {
                    lineCurrent += lineArt; // Adiciona espacos vazios
                } else if (isBlinkColorful) {
                    lineCurrent += colorHighlight + baseLine + Appearance::color(Color::RESET);
                } else if (CombatScreen::context.selectionTargetCurrent == static_cast<int>(enemyDrawIndex)) {
                    lineCurrent += (CombatScreen::context.blinkSelection ? Appearance::color(Color::YELLOW) : Appearance::color(Color::GRAY)) + baseLine + Appearance::color(Color::RESET);
                } else if (enemyCurrent == g_enemyAttackerParry) {
                    lineCurrent += "\033[38;2;255;140;0m" + baseLine + Appearance::color(Color::RESET);
                } else {
                    lineCurrent += baseLine;
                }
            } else {
                if (enemyCurrent->getDeathLively()) {
                    lineCurrent += lineArt; // Adiciona espacos vazios
                } else if (CombatScreen::context.selectionTargetCurrent == static_cast<int>(enemyDrawIndex)) {
                    lineCurrent += (CombatScreen::context.blinkSelection ? Appearance::color(Color::YELLOW) : Appearance::color(Color::GRAY)) + lineArt + Appearance::color(Color::RESET);
                } else if (enemyCurrent == g_enemyAttackerParry) {
                    lineCurrent += "\033[38;2;255;140;0m" + lineArt + Appearance::color(Color::RESET);
                } else {
                    lineCurrent += lineArt;
                }
            }
            
            int spacesRight = widthSeparateForEachColumn - spacesForCentralizeAArt - visibleLen;
            if (enemyDrawIndex < enemies.size() - 1) {
                lineCurrent += std::string(spacesRight > 0 ? spacesRight : 0, ' ');
            }
        }
        artLines.push_back(lineCurrent);
    }
    
    if (animateEmergence) {
        Appearance::printVectorExcited(artLines, 12);
    } else {
        for (const auto& line : artLines) std::cout << line << "\n";
    }

    std::cout << "\n";
}

void IDECombatScreen::animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation)
{
    if (CombatScreen::context.isTerminalView) {
        std::string nameTarget = targetAnimation ? targetAnimation->getName() : "desconhecido";
        std::string nameAttacker = attacker ? attacker->getName() : "desconhecido";
        std::vector<std::string> logMsg = {
            IDETheme::comment("// " + nameAttacker + " ataca " + nameTarget),
            IDETheme::type("int") + " " + IDETheme::variable("dano") + " = " + IDETheme::number(std::to_string(damageAnimation)) + ";",
            IDETheme::comment("// " + nameTarget + " recebe " + std::to_string(damageAnimation) + " de dano")
        };
        IDECombatScreen::displayLogCombatTurnal(logMsg);
        return;
    }

    Item* weaponAttacker = (attacker != nullptr) ? attacker->getWeapons() : nullptr;
    
    executeAnimation(8, 100, 1, [&](int frame) {
        renderScenePattern(combatTitle, enemies, targetAnimation, frame, false, false, weaponAttacker, currentPlayer, allies, nullptr, Color::RESET, damageAnimation);
    }, [&]() {
        renderScenePattern(combatTitle, enemies, targetAnimation, 0, false, false, nullptr, currentPlayer, allies);
    });
}

void IDECombatScreen::animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation)
{
    if (CombatScreen::context.isTerminalView) {
        std::string nameTarget = targetAnimation ? targetAnimation->getName() : "desconhecido";
        std::vector<std::string> logMsg = {
            IDETheme::type("int") + " " + IDETheme::variable("curaRecebida") + " = " + IDETheme::number(std::to_string(healingAnimation)) + ";",
            IDETheme::comment("// " + nameTarget + " recupera " + std::to_string(healingAnimation) + " de vida")
        };
        IDECombatScreen::displayLogCombatTurnal(logMsg);
        return;
    }

    executeAnimation(12, 100, 1, [&](int frame) {
        Color colorApplied = (frame % 2 == 1 && frame <= 6) ? Color::GREEN : Color::RESET;
        renderScenePattern(combatTitle, enemies, nullptr, frame, true, false, nullptr, currentPlayer, allies, targetAnimation, colorApplied, healingAnimation);
    }, [&]() {
        renderScenePattern(combatTitle, enemies, nullptr, 0, false, false, nullptr, currentPlayer, allies);
    });
}

int IDECombatScreen::getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    setShiftVisible(currentTurn, characterActing->getName());
    CombatScreen::context.characterHUD = characterActing;
    CombatScreen::context.selectionActionCurrent = 0;
    
    if (CombatScreen::context.isTerminalView) {
        int action = IDECombatScreen::displayMenuActions(characterActing, enemies);
        if (action < 0) return -1;
        
        /*
         * 0=Atacar, 1=Defender, 2=Inventario/Habilidade, 3=Fuga, 4=Habilidade, 5=Escudo
         * Mapear para os valores originais
         */
        if (action == 0) return 1; // Atacar
        if (action == 1) return 2; // Defender
        if (action == 2) return 4; // Inventario
        if (action == 3) return 8; // Fugir
        if (action == 4) return 3; // Habilidade
        if (action == 5) return 6; // Escudo
        return 0;
    }
    
    auto buildOptions = [&]() {
        CombatScreen::context.optionsMenuCurrent.clear();
        if (CombatScreen::context.characterHUD != nullptr && CombatScreen::context.characterHUD != characterActing) {
            CombatScreen::context.optionsMenuCurrent.push_back("Voltar a Acao");
            CombatScreen::context.optionsMenuCurrent.push_back("Ver Aliados");
        } else {
            CombatScreen::context.optionsMenuCurrent.push_back("Atacar");
            CombatScreen::context.optionsMenuCurrent.push_back("Defender");
            
            std::string skillOption = "Habilidade";
            if (characterActing->getTypeClass() == TypeClass::NECROMANCER) {
                size_t souls = characterActing->getSoulCount();
                if (souls == 0) skillOption += Appearance::color(Color::RED) + " (0 Almas)" + Appearance::color(Color::RESET);
                else skillOption += " (" + std::to_string(souls) + " Alma" + (souls > 1 ? "s" : "") + ")";
            }
            CombatScreen::context.optionsMenuCurrent.push_back(skillOption);
            
            bool hasInventory = (characterActing->getInventory() != nullptr && characterActing->getInventory()->getAllItems().size() > 0);
            if (hasInventory || characterActing == currentPlayer) {
                CombatScreen::context.optionsMenuCurrent.push_back("Inventario");
            }
            
            CombatScreen::context.optionsMenuCurrent.push_back("Ficha");
            if (!allies.empty()) CombatScreen::context.optionsMenuCurrent.push_back("Ver Aliados");
            CombatScreen::context.optionsMenuCurrent.push_back("Diario");
            CombatScreen::context.optionsMenuCurrent.push_back("Log Batalha");
        }
    };

    buildOptions();
    auto lastUpdate = std::chrono::steady_clock::now();
    updateScreenStatic("", enemies, currentPlayer, allies);

    while (true) {
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count() >= 150) {
            updateScreenStatic("", enemies, currentPlayer, allies);
            lastUpdate = now;
        }

        if (InputControl::pressedKey()) {
            unsigned char key = static_cast<unsigned char>(InputControl::readKey());
            
            if (key == 224 || key == 0) {
                unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
                if (nextKey == 72 || nextKey == 'A') key = 'w';
                else if (nextKey == 80 || nextKey == 'B') key = 's';
            } else if (key == '\033') {
                if (InputControl::pressedKey()) {
                    unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
                    if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
                    if (nextKey == 72 || nextKey == 'A') key = 'w';
                    else if (nextKey == 80 || nextKey == 'B') key = 's';
                    else if (nextKey == 27) key = '\033';
                }
            }

            int totalOptions = static_cast<int>(CombatScreen::context.optionsMenuCurrent.size());

            if (key == 'w' || key == 'W') { 
                CombatScreen::context.selectionActionCurrent--; 
                if (CombatScreen::context.selectionActionCurrent < 0) CombatScreen::context.selectionActionCurrent = totalOptions - 1; 
                lastUpdate = std::chrono::steady_clock::now();
                updateScreenStatic("", enemies, currentPlayer, allies);
            }
            else if (key == 's' || key == 'S') { 
                CombatScreen::context.selectionActionCurrent++; 
                if (CombatScreen::context.selectionActionCurrent >= totalOptions) CombatScreen::context.selectionActionCurrent = 0; 
                lastUpdate = std::chrono::steady_clock::now();
                updateScreenStatic("", enemies, currentPlayer, allies);
            }
            else if (key == '\r' || key == '\n') { 
                std::string op = CombatScreen::context.optionsMenuCurrent[CombatScreen::context.selectionActionCurrent];
                if (op == "Ver Aliados") {
                    selectHUDAlly(currentPlayer, allies);
                    CombatScreen::context.selectionActionCurrent = 0; 
                    buildOptions();
                    updateScreenStatic("", enemies, currentPlayer, allies);
                    continue;
                }
                if (op == "Voltar a Acao") {
                    CombatScreen::context.characterHUD = characterActing;
                    CombatScreen::context.selectionActionCurrent = 0;
                    buildOptions();
                    updateScreenStatic("", enemies, currentPlayer, allies);
                    continue;
                }

                CombatScreen::context.selectionActionCurrent = -1; // Remove cursor para as animacoes manterem a tela limpa
                CombatScreen::context.characterHUD = nullptr; // Garante que o HUD volte a mostrar o personagem agindo de fato nas proximas animacoes
                updateScreenStatic("", enemies, currentPlayer, allies);
                
                if (op == "Atacar") return 1;
                if (op == "Defender") return 2;
                if (op.find("Habilidade") != std::string::npos) return 3;
                if (op == "Inventario") return 4;
                if (op == "Ficha") return 5;
                if (op == "Diario") return 6;
                if (op == "Log Batalha") return 7;
                return 0;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

int IDECombatScreen::getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    if (CombatScreen::context.isTerminalView) {
        int target = IDECombatScreen::displaySelectionTarget(enemies);
        return target;
    }
    
    CombatScreen::context.selectionTargetCurrent = 0;
    int totalEnemies = static_cast<int>(enemies.size());
    int oldAction = CombatScreen::context.selectionActionCurrent;
    CombatScreen::context.selectionActionCurrent = -1; // Esconde o cursor do menu de acoes

    CombatScreen::context.blinkSelection = true;
    updateScreenStatic(combatTitle, enemies, currentPlayer, allies);
    auto timeBlink = std::chrono::steady_clock::now();
    auto lastUpdate = std::chrono::steady_clock::now();

    while (true) {
        auto now = std::chrono::steady_clock::now();
        
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - timeBlink).count() >= 150) {
            CombatScreen::context.blinkSelection = !CombatScreen::context.blinkSelection;
            timeBlink = now;
        }

        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate).count() >= 33) {
            lastUpdate = now;
            updateScreenStatic(combatTitle, enemies, currentPlayer, allies);
        }

        if (InputControl::pressedKey()) {
            unsigned char key = static_cast<unsigned char>(InputControl::readKey());
            
            if (key == 224 || key == 0) {
                unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
                if (nextKey == 75 || nextKey == 'D') key = 'a'; // Seta Esquerda
                else if (nextKey == 77 || nextKey == 'C') key = 'd'; // Seta Direita
            } else if (key == '\033') {
                if (InputControl::pressedKey()) {
                    unsigned char nextKey = static_cast<unsigned char>(InputControl::readKey());
                    if (nextKey == '[') nextKey = static_cast<unsigned char>(InputControl::readKey());
                    if (nextKey == 75 || nextKey == 'D') key = 'a'; // Seta Esquerda
                    else if (nextKey == 77 || nextKey == 'C') key = 'd'; // Seta Direita
                    else if (nextKey == 27) key = '\033'; // Esc
                }
            }

            if (key == 'a' || key == 'A') { 
                CombatScreen::context.selectionTargetCurrent--; 
                if (CombatScreen::context.selectionTargetCurrent < 0) CombatScreen::context.selectionTargetCurrent = totalEnemies - 1; 
                CombatScreen::context.blinkSelection = true;
                timeBlink = std::chrono::steady_clock::now();
                updateScreenStatic(combatTitle, enemies, currentPlayer, allies);
            }
            else if (key == 'd' || key == 'D') { 
                CombatScreen::context.selectionTargetCurrent++; 
                if (CombatScreen::context.selectionTargetCurrent >= totalEnemies) CombatScreen::context.selectionTargetCurrent = 0; 
                CombatScreen::context.blinkSelection = true;
                timeBlink = std::chrono::steady_clock::now();
                updateScreenStatic(combatTitle, enemies, currentPlayer, allies);
            }
            else if (key == '\r' || key == '\n') { 
                int choice = CombatScreen::context.selectionTargetCurrent;
                CombatScreen::context.selectionTargetCurrent = -1; 
                CombatScreen::context.selectionActionCurrent = oldAction;
                CombatScreen::context.blinkSelection = true;
                updateScreenStatic(combatTitle, enemies, currentPlayer, allies);
                return choice; 
            }
            else if (key == '\033' || key == 'x' || key == 'X' || key == '0' || key == '\b') {
                CombatScreen::context.selectionTargetCurrent = -1;
                CombatScreen::context.selectionActionCurrent = oldAction;
                CombatScreen::context.blinkSelection = true;
                updateScreenStatic(combatTitle, enemies, currentPlayer, allies);
                return -1;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    }
}

void IDECombatScreen::selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies) {
    std::vector<std::string> optionsHUD;
    optionsHUD.push_back(currentPlayer->getName());
    for (auto* ally : allies) {
        optionsHUD.push_back(ally->getName());
    }
    optionsHUD.push_back("Voltar");

    std::cout << "\n" << CombatScreen::combatMargin() << "═══ SELECIONE O PERSONAGEM PARA VER INFO ═══\n";
    int choice = InputControl::readSelectionMenuWithArrows(optionsHUD, false, CombatScreen::combatMargin());
    
    if (choice >= 0 && choice < static_cast<int>(optionsHUD.size()) - 1) {
        if (choice == 0) CombatScreen::context.characterHUD = currentPlayer;
        else CombatScreen::context.characterHUD = allies[choice - 1];
    }
}

int IDECombatScreen::getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return getTargetAttack(combatTitle, enemies, currentPlayer, allies);
}

int IDECombatScreen::chooseShield(const std::string& characterName, const std::vector<Item*>& shields) {
    if (CombatScreen::context.isTerminalView) {
        int choice = IDECombatScreen::displaySelectionShield(CombatScreen::context.characterHUD, shields);
        if (choice < 0) return 0;
        return choice + 1;
    }
    
    std::cout << "\n" << CombatScreen::combatMargin() << "═══ SELECIONE UM ESCUDO PARA " << characterName << " ═══\n";
    std::vector<std::string> options;
    for (size_t index = 0; index < shields.size(); index++) {
        options.push_back(shields[index]->getItemName() + shields[index]->getInfoStatus());
    }
    options.push_back("Cancelar");
    int choice = InputControl::readSelectionMenuWithArrows(options, false, CombatScreen::combatMargin());
    if (choice == static_cast<int>(options.size()) - 1) return 0;
    return choice + 1;
}

void IDECombatScreen::notifyEnemiesMoreAct() {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayMessageCombat("Inimigos sao mais ageis e atacam primeiro!", 0);
        return;
    }
    std::string msg = DialogueFunctions::formatSystemMsg("Os inimigos sao mais ageis e atacam primeiro!", Color::RED);
    std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
    Appearance::registerBattleLog(msg);
    InputControl::waitForEnter();
}

void IDECombatScreen::notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayMessageCombat("Turno extra! Agilidade (" + std::to_string(dexterityPlayer) + " VS " + std::to_string(maxEnemyDexterity) + ")", 0);
        return;
    }
    std::string msg = DialogueFunctions::formatSystemMsg("Sua agilidade extrema (" + std::to_string(dexterityPlayer) + " VS " + std::to_string(maxEnemyDexterity) + ") permite que voce aja novamente!", Color::CYAN);
    std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
    Appearance::registerBattleLog(msg);
    InputControl::waitForEnter();
}

void IDECombatScreen::notifyUnpreventionInventory() {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayMessageCombat("Inimigo te pegou desprevenido enquanto usava o inventario!", 0);
        return;
    }
    std::string msg = DialogueFunctions::formatSystemMsg("O inimigo te pegou desprevinido enquanto voce usava o inventario!");
    std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
    Appearance::registerBattleLog(msg);
}

void IDECombatScreen::notifyWithoutShields(const std::string& characterName) {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayMessageCombat(characterName + " nao possui escudos no inventario!", 0);
        return;
    }
    std::cout << "\n" << CombatScreen::combatMargin() << DialogueFunctions::formatSystemMsg(characterName + " nao possui escudos no inventario para usar!", Color::RED) << "\n";
}

void IDECombatScreen::notifyImbalanceDefense(const std::string& characterName) {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayImbalanceDefense(nullptr);
        return;
    }
    std::string msg = DialogueFunctions::formatSystemMsg(characterName + " se desequilibrou e precisa de 1 turno para poder defender novamente!", Color::RED);
    std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
    Appearance::registerBattleLog(msg);
    InputControl::waitForEnter();
}

void IDECombatScreen::notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayPostureDefensive(nullptr, nameShield);
        return;
    }
    std::string msg = DialogueFunctions::formatSystemMsg(characterName + " assumiu uma postura defensiva com " + nameShield + "!", Color::WHITE);
    std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
    Appearance::registerBattleLog(msg);
    InputControl::waitForEnter();
}

void IDECombatScreen::notifyActionInvalidates() {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayMessageCombat("Acao invalida!", 0);
        return;
    }
    std::cout << "\n" << CombatScreen::combatMargin() << DialogueFunctions::formatSystemMsg("Acao invalida!", Color::RED) << "\n";
    InputControl::waitForEnter();
}

void IDECombatScreen::notifyCancellationItem() {
    if (CombatScreen::context.isTerminalView) {
        IDECombatScreen::displayMessageCombat("Uso do item cancelado.", 0);
        return;
    }
    std::cout << "\n" << CombatScreen::combatMargin() << DialogueFunctions::formatSystemMsg("Uso do item cancelado. Ele retornou para a mochila.") << "\n";
}

void IDECombatScreen::notifyUnmetRequirement(const std::string& requirementMessage) {
    if (CombatScreen::context.isTerminalView) {
        std::string msg = requirementMessage;
        if (msg.substr(0, 1) == "\n") msg = msg.substr(1);
        IDECombatScreen::displayMessageCombat(msg, 0);
        return;
    }
    if (requirementMessage.substr(0, 1) == "\n") {
        std::cout << "\n" << CombatScreen::combatMargin() << requirementMessage.substr(1);
    } else {
        std::cout << CombatScreen::combatMargin() << requirementMessage;
    }
    InputControl::waitForEnter();
}

void IDECombatScreen::updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance)
{
    renderScenePattern(combatTitle, enemies, nullptr, 0, false, false, nullptr, currentPlayer, allies, nullptr, Color::RESET, -1, {}, animateEntrance);
}

void IDECombatScreen::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops)
{
    if (enemies.empty()) return;

    if (CombatScreen::context.isTerminalView) {
        CombatScreen::context.enemyDeadWithDrops = enemyDead;
        CombatScreen::context.dropsAssets = drops;
        IDECombatScreen::displayDeathEnemy(enemyDead->getName(), 0, drops);
        CombatScreen::context.enemyDeadWithDrops = nullptr;
        CombatScreen::context.dropsAssets.clear();
        return;
    }
    
    CombatScreen::context.enemyDeadWithDrops = enemyDead;
    CombatScreen::context.dropsAssets = drops;
    
    int totalLines = 0;
    int stepAnime = 1;
    int intervalMs = 12;

    if (CombatScreen::context.isMode3D) {
        totalLines = 12; // 12 frames
        intervalMs = 15; // 15ms por frame. Com o tempo de desenho de tela (~60-80ms), a animacao dura cerca de 1,0 segundo no total.
    } else {
        totalLines = static_cast<int>(enemyDead->getRace()->getAppearanceRace().size());
        if (totalLines <= 0) totalLines = 6;
        intervalMs = 25; // 25ms por frame
    }

    executeAnimation(totalLines, intervalMs, stepAnime, [&](int frame) {
        renderScenePattern(combatTitle, enemies, enemyDead, frame, false, true, nullptr, currentPlayer, allies);
    }, [&]() {
        // Frame final para garantir que toda a arte seja apagada e os DROPS aparecam centralizados no fantasma da imagem!
        renderScenePattern(combatTitle, enemies, enemyDead, totalLines, false, true, nullptr, currentPlayer, allies, nullptr, Color::RESET, -1, drops);
    });
}

void IDECombatScreen::animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation)
{
    if (CombatScreen::context.isTerminalView) {
        std::string nameTarget = targetAnimation ? targetAnimation->getName() : "desconhecido";
        std::vector<std::string> logMsg = {
            IDETheme::type("int") + " " + IDETheme::variable("cura") + " = " + IDETheme::number(std::to_string(healingAnimation)) + ";",
            IDETheme::comment("// " + nameTarget + " recupera " + std::to_string(healingAnimation) + " de vida")
        };
        IDECombatScreen::displayLogCombatTurnal(logMsg);
        return;
    }

    executeAnimation(4, 100, 1, [&](int frame) {
        renderScenePattern(combatTitle, enemies, targetAnimation, frame, true, false, nullptr, currentPlayer, allies, nullptr, Color::RESET, healingAnimation);
    }, [&]() {
        renderScenePattern(combatTitle, enemies, targetAnimation, 0, true, false, nullptr, currentPlayer, allies);
    });
}

void IDECombatScreen::animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation)
{
    if (CombatScreen::context.isTerminalView) {
        std::string nameTarget = targetAnimation ? targetAnimation->getName() : currentPlayer ? currentPlayer->getName() : "desconhecido";
        std::vector<std::string> logMsg = {
            IDETheme::comment("// " + nameTarget + " sofre dano"),
            IDETheme::type("int") + " " + IDETheme::variable("danoRecebido") + " = " + IDETheme::number(std::to_string(damageAnimation)) + ";",
            IDETheme::comment("// " + nameTarget + " recebe " + std::to_string(damageAnimation) + " de dano")
        };
        IDECombatScreen::displayLogCombatTurnal(logMsg);
        return;
    }

    Color colorHighlight = isParry ? Color::CYAN : Color::RED;

    executeAnimation(12, 100, 1, [&](int frame) {
        Color colorApplied = (frame % 2 == 1 && frame <= 6) ? colorHighlight : Color::RESET;
        renderScenePattern(combatTitle, enemies, nullptr, frame, false, false, nullptr, currentPlayer, allies, targetAnimation, colorApplied, damageAnimation);
    }, [&]() {
        renderScenePattern(combatTitle, enemies, nullptr, 0, false, false, nullptr, currentPlayer, allies);
    });
}

// === Old IDECombatScreen methods ===

void IDECombatScreen::display(Character* currentPlayer, const std::vector<Character*>& enemies, const std::string& combatTitle) {
    Appearance::clearScreen();
    std::string titleIDE = "\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mCombate" + std::to_string(enemies.size()) + "\033[0m {";
    Appearance::printCentralized(titleIDE);
    double pctLife = static_cast<double>(currentPlayer->getHealth()) / currentPlayer->getMaxHealth();
    std::string barLife = AppearanceTerminal::generateBarLifeASCII(pctLife, 12);
    std::string lineLife = std::string("    ") + "\033[38;2;156;220;254mint\033[0m " + std::string("\033[38;2;156;220;254mvidaAtual\033[0m = ") + barLife + std::string(";");
    Appearance::printCentralized(lineLife);
    std::vector<std::string> barsEnemies;
    for (const auto& enemy : enemies) {
        double pctEnemy = static_cast<double>(enemy->getHealth()) / enemy->getMaxHealth();
        std::string barEnemy = AppearanceTerminal::generateBarLifeASCII(pctEnemy, 12);
        std::string nameEnemy = std::string("\033[38;2;156;220;254mstd::string\033[0m ") + std::string("\033[38;2;156;220;254mnome\033[0m = \"") + enemy->getName() + std::string("\";");
        std::string lifeEnemy = std::string("    ") + std::string("\033[38;2;156;220;254mint\033[0m ") + std::string("\033[38;2;156;220;254mvida\033[0m = ") + barEnemy + std::string(";");
        std::vector<std::string> lineEnemy = {nameEnemy, lifeEnemy};
        barsEnemies.insert(barsEnemies.end(), lineEnemy.begin(), lineEnemy.end());
    }
    Appearance::printBlockCentralized(barsEnemies);
    std::vector<std::string> code = {
        "\033[38;2;86;156;214mpublic:\033[0m",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mturnoAtual\033[0m = \033[38;2;181;206;168m0\033[0m;",
        "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mtitulo\033[0m = \033[38;2;214;157;133m\"" + combatTitle + "\"\033[0m;",
        "    \033[38;2;78;201;176mbool\033[0m \033[38;2;156;220;254mataqueRealizado\033[0m = \033[38;2;86;156;214mfalse\033[0m;",
        "",
        "    \033[38;2;220;220;170mvoid\033[0m \033[38;2;220;220;170executar\033[0m() {",
        "        " + IDETheme::variable(currentPlayer->getName()) + "->" + IDETheme::function("atacar") + "(inimigo);",
        "    }"
    };
    Appearance::printBlockCentralizedTyping(code, 15);
    std::vector<std::string> logEnemies;
    for (const auto& enemy : enemies) {
        logEnemies.push_back(std::string("\033[38;2;96;139;78m// Inimigo detectado: ") + std::string("\033[38;2;156;220;254m") + enemy->getName() + std::string("\033[0m ") + std::string("\033[38;2;96;139;78m// HP:") + std::string("\033[38;2;181;206;168m") + std::to_string(enemy->getHealth()) + std::string("\033[0m"));
    }
    std::cout << "\n";
    Appearance::printBlockCentralized(logEnemies);
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para iniciar o combate..."));
    InputControl::waitForEnter();
}

void IDECombatScreen::displayLogCombat(const std::vector<std::string>& messages) {
    Appearance::clearScreen();
    std::vector<std::string> log;
    log.push_back("\033[38;2;86;156;214mnamespace\033[0m \033[38;2;78;201;176mCombateLog\033[0m {");
    for (const auto& msg : messages) {
        std::string part1 = std::string("    ") + std::string("\033[38;2;86;156;214mauto\033[0m ") + std::string("\033[38;2;156;220;254mmsg\033[0m = ");
        std::string part2 = std::string("\033[38;2;214;157;133m\"") + msg + std::string("\"\033[0m;");
        log.push_back(part1 + part2);
    }
    log.push_back("}");
    Appearance::printBlockCentralized(log);
    Appearance::printCentralized(IDETheme::comment("Combate encerrado."));
}

void IDECombatScreen::displayStatsCombat(int shifts, int damageCaused, int damageReceived, int healing) {
    Appearance::clearScreen();
    std::vector<std::string> stats = {
        "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mStatsCombate\033[0m {",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mturnos\033[0m = \033[38;2;181;206;168m" + std::to_string(shifts) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdanoCausado\033[0m = \033[38;2;181;206;168m" + std::to_string(damageCaused) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mdanoRecebido\033[0m = \033[38;2;181;206;168m" + std::to_string(damageReceived) + "\033[0m;",
        "    \033[38;2;78;201;176mint\033[0m \033[38;2;156;220;254mcuraRecebida\033[0m = \033[38;2;181;206;168m" + std::to_string(healing) + "\033[0m;",
        "};",
        "",
        "// Total de " + std::to_string(shifts) + " turnos",
        "// Dano total causado: " + std::to_string(damageCaused),
        "// Dano recebido: " + std::to_string(damageReceived),
        "// Cura total: " + std::to_string(healing)
    };
    Appearance::printBlockCentralized(stats);
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

void IDECombatScreen::displayDrops(const std::vector<std::string>& drops) {
    Appearance::clearScreen();
    std::vector<std::string> boxDrops;
    boxDrops.push_back("\033[38;2;86;156;214mclass\033[0m \033[38;2;78;201;176mDropReward\033[0m {");
    boxDrops.push_back("\033[38;2;86;156;214mpublic:\033[0m");
    for (const auto& drop : drops) {
        std::string cleanDrop = Appearance::removeANSIColors(drop);
        boxDrops.push_back("    \033[38;2;86;156;214mauto\033[0m \033[38;2;156;220;254mitem\033[0m = \033[38;2;214;157;133m\"" + cleanDrop + "\"\033[0m;");
    }
    boxDrops.push_back("};");
    Appearance::printBlockCentralized(boxDrops);
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

int IDECombatScreen::displayMenuActions(Character* player, const std::vector<Character*>& enemies) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mEscolhaAcao\033[0m() {";
    Appearance::printCentralized(title);
    std::vector<std::string> options = {
        "\033[38;2;220;220;170matacar\033[0m() - Atacar inimigo",
        "\033[38;2;220;220;170mdefender\033[0m() - Postura defensiva",
        "\033[38;2;220;220;170musarItem\033[0m() - Usar item do inventario",
        "\033[38;2;220;220;170mfugar\033[0m() - Tentar fugir do combate",
        "\033[38;2;220;220;170mhabilidade\033[0m() - Usar habilidade especial",
        "\033[38;2;220;220;170mescudo\033[0m() - Equipar escudo"
    };
    Appearance::printBlockCentralized(options);
    std::string prompt = "\033[38;2;96;139;78m// Selecione uma opcao (0-" + std::to_string(options.size()-1) + ")\033[0m";
    Appearance::printCentralized(prompt);
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para confirmar..."));
    int selected = 0;
    int maxOptions = static_cast<int>(options.size());
    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        Appearance::printCentralized(title);
        Appearance::printBlockCentralized(options);
        Appearance::printCentralized(prompt);
        std::string opSelected = "\033[38;2;181;206;168m> " + options[selected] + " <\033[0m";
        Appearance::printCentralized(opSelected);
        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0) {
            unsigned char next = static_cast<unsigned char>(InputControl::readKey());
            if (next == 72 || next == 'A') key = 'w';
            else if (next == 80 || next == 'B') key = 's';
        } else if (key == '\033') {
            if (InputControl::pressedKey()) {
                unsigned char next = static_cast<unsigned char>(InputControl::readKey());
                if (next == '[') {
                    next = static_cast<unsigned char>(InputControl::readKey());
                    if (next == 72 || next == 'A') key = 'w';
                    else if (next == 80 || next == 'B') key = 's';
                }
            }
        }
        if (key == 'w') {
            selected = (selected - 1 + maxOptions) % maxOptions;
        } else if (key == 's') {
            selected = (selected + 1) % maxOptions;
        } else if (key == '\r' || key == '\n') {
            return selected;
        } else if (key == '\033') {
            return -1;
        }
    }
}

int IDECombatScreen::displaySelectionTarget(const std::vector<Character*>& enemies) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mEscolhaUmAlvo\033[0m() {";
    Appearance::printCentralized(title);
    std::vector<std::string> listEnemies;
    for (size_t i = 0; i < enemies.size(); ++i) {
        double pctLife = static_cast<double>(enemies[i]->getHealth()) / enemies[i]->getMaxHealth();
        std::string bar = AppearanceTerminal::generateBarLifeASCII(pctLife, 12);
        std::string line = "\033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254malvo\033[0m = \033[38;2;181;206;168m" + std::to_string(i) + "\033[0m; // " + enemies[i]->getName() + " (HP: " + bar + ")";
        listEnemies.push_back(line);
    }
    Appearance::printBlockCentralized(listEnemies);
    Appearance::printCentralized(IDETheme::comment("Selecione o alvo com < / > e confirme com ENTER..."));
    int selected = 0;
    int maxEnemies = static_cast<int>(enemies.size());
    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        Appearance::printCentralized(title);
        std::vector<std::string> listFormatted;
        for (size_t i = 0; i < enemies.size(); ++i) {
            double pctLife = static_cast<double>(enemies[i]->getHealth()) / enemies[i]->getMaxHealth();
            std::string bar = AppearanceTerminal::generateBarLifeASCII(pctLife, 12);
            std::string line = "\033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254malvo\033[0m = \033[38;2;181;206;168m" + std::to_string(i) + "\033[0m; // " + enemies[i]->getName() + " (HP: " + bar + ")";
            if (i == static_cast<size_t>(selected)) {
                line = "\033[38;2;181;206;168m> " + line + " <\033[0m";
            }
            listFormatted.push_back(line);
        }
        Appearance::printBlockCentralized(listFormatted);
        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0) {
            unsigned char next = static_cast<unsigned char>(InputControl::readKey());
            if (next == 75 || next == 'D') key = 'a';
            else if (next == 77 || next == 'C') key = 'd';
        } else if (key == '\033') {
            if (InputControl::pressedKey()) {
                unsigned char next = static_cast<unsigned char>(InputControl::readKey());
                if (next == '[') {
                    next = static_cast<unsigned char>(InputControl::readKey());
                    if (next == 75 || next == 'D') key = 'a';
                    else if (next == 77 || next == 'C') key = 'd';
                }
            }
        }
        if (key == 'a' || key == 'd') {
            selected = (selected + 1) % maxEnemies;
        } else if (key == '\r' || key == '\n') {
            return selected;
        } else if (key == '\033') {
            return -1;
        }
    }
}

void IDECombatScreen::displayShift(Character* character, const std::vector<Character*>& enemies, int shift) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mexecutar\033[0m() {";
    Appearance::printCentralized(title);
    std::string lineShift = "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mturno\033[0m = \033[38;2;181;206;168m" + std::to_string(shift) + "\033[0m;";
    Appearance::printCentralized(lineShift);
    std::string lineCharacter = "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mvezDe\033[0m = \033[38;2;214;157;133m\"" + character->getName() + "\"\033[0m;";
    Appearance::printCentralized(lineCharacter);
    std::vector<std::string> logEnemies;
    for (const auto& enemy : enemies) {
        logEnemies.push_back(std::string("\033[38;2;96;139;78m// Inimigo: ") + std::string("\033[38;2;156;220;254m") + enemy->getName() + std::string("\033[0m"));
    }
    Appearance::printBlockCentralized(logEnemies);
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

void IDECombatScreen::displayLogCombatTurnal(const std::vector<std::string>& messages) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mnamespace\033[0m \033[38;2;78;201;176mLogCombate\033[0m {";
    Appearance::printCentralized(title);
    std::vector<std::string> log;
    for (const auto& msg : messages) {
        log.push_back("    \033[38;2;86;156;214mauto\033[0m \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mmsg\033[0m = \033[38;2;214;157;133m\"" + msg + "\"\033[0m;");
    }
    Appearance::printBlockCentralized(log);
    Appearance::printCentralized("}");
    Appearance::printCentralized(IDETheme::comment("Combate em andamento..."));
}

void IDECombatScreen::displayDeathEnemy(const std::string& nameEnemy, int xpGain, const std::vector<std::string>& drops) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mInimigoDerrotado\033[0m() {";
    Appearance::printCentralized(title);
    std::string lineName = "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254minimigoMorto\033[0m = \033[38;2;214;157;133m\"" + nameEnemy + "\"\033[0m;";
    Appearance::printCentralized(lineName);
    std::string lineXP = "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mXP\033[0m = \033[38;2;181;206;168m" + std::to_string(xpGain) + "\033[0m;";
    Appearance::printCentralized(lineXP);
    std::vector<std::string> dropsList;
    for (const auto& drop : drops) {
        std::string cleanDrop = Appearance::removeANSIColors(drop);
        dropsList.push_back("    \033[38;2;86;156;214mauto\033[0m \033[38;2;156;220;254mitem\033[0m = \033[38;2;214;157;133m\"" + cleanDrop + "\"\033[0m;");
    }
    Appearance::printBlockCentralized(dropsList);
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

void IDECombatScreen::displayEffectsAssets(Character* character) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mstruct\033[0m \033[38;2;78;201;176mEfeitosAtivos\033[0m {";
    Appearance::printCentralized(title);
    std::vector<std::string> effects;
    std::vector<EffectID> IDsEffects;
    character->getIDsEffectsAssets(IDsEffects);
    for (EffectID id : IDsEffects) {
        const StatusEffect* effect = character->findEffect(id);
        std::string effectName = effect ? effect->getName() : "desconhecido";
        effects.push_back("    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mefeito\033[0m = \033[38;2;181;206;168m" + std::to_string(static_cast<int>(id)) + "\033[0m; // " + effectName);
    }
    if (effects.empty()) {
        effects.push_back("    // Nenhum efeito ativo");
    }
    Appearance::printBlockCentralized(effects);
    Appearance::printCentralized("}");
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

int IDECombatScreen::displaySelectionItem(Character* player, int category) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mSelecionarItem\033[0m() {";
    Appearance::printCentralized(title);
    std::string lineCategory = "    \033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mcategoria\033[0m = \033[38;2;181;206;168m" + std::to_string(category) + "\033[0m;";
    Appearance::printCentralized(lineCategory);
    std::vector<Item*> listItems;
    if (player != nullptr) {
        listItems = player->getInventory()->getAllItems();
    }
    if (listItems.empty()) {
        std::vector<std::string> msg = {
            "    // Nenhum item disponivel na categoria " + std::to_string(category),
            "    // Adicione itens ao inventario e tente novamente"
        };
        Appearance::printBlockCentralized(msg);
        Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para voltar..."));
        InputControl::waitForEnter();
        return -1;
    }
    std::vector<std::string> items;
    for (size_t i = 0; i < listItems.size(); ++i) {
        std::string name = listItems[i]->getItemName();
        items.push_back("\033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mit" + std::to_string(i) + "\033[0m = \033[38;2;181;206;168m" + std::to_string(i) + "\033[0m; // " + name);
    }
    Appearance::printBlockCentralized(items);
    Appearance::printCentralized(IDETheme::comment("Selecione o item com < / > e confirme com ENTER..."));
    int selected = 0;
    int maxItems = static_cast<int>(listItems.size());
    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        Appearance::printCentralized(title);
        Appearance::printCentralized(lineCategory);
        std::vector<std::string> itemsFormatted;
        for (size_t i = 0; i < listItems.size(); ++i) {
            std::string name = listItems[i]->getItemName();
            std::string line = "\033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mit" + std::to_string(i) + "\033[0m = \033[38;2;181;206;168m" + std::to_string(i) + "\033[0m; // " + name;
            if (i == static_cast<size_t>(selected)) {
                line = "\033[38;2;181;206;168m> " + line + " <\033[0m";
            }
            itemsFormatted.push_back(line);
        }
        Appearance::printBlockCentralized(itemsFormatted);
        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0) {
            unsigned char next = static_cast<unsigned char>(InputControl::readKey());
            if (next == 72 || next == 'A') key = 'w';
            else if (next == 80 || next == 'B') key = 's';
            else if (next == 75 || next == 'D') key = 'a';
            else if (next == 77 || next == 'C') key = 'd';
        } else if (key == '\033') {
            if (InputControl::pressedKey()) {
                unsigned char next = static_cast<unsigned char>(InputControl::readKey());
                if (next == '[') {
                    next = static_cast<unsigned char>(InputControl::readKey());
                    if (next == 72 || next == 'A') key = 'w';
                    else if (next == 80 || next == 'B') key = 's';
                    else if (next == 75 || next == 'D') key = 'a';
                    else if (next == 77 || next == 'C') key = 'd';
                }
            }
        }
        if (key == 'w' || key == 'a') {
            selected = (selected - 1 + maxItems) % maxItems;
        } else if (key == 's' || key == 'd') {
            selected = (selected + 1) % maxItems;
        } else if (key == '\r' || key == '\n') {
            return selected;
        } else if (key == '\033') {
            return -1;
        }
    }
}

int IDECombatScreen::displaySelectionShield(Character* player, const std::vector<Item*>& shields) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mSelecionarEscudo\033[0m() {";
    Appearance::printCentralized(title);
    if (shields.empty()) {
        std::vector<std::string> msg = {
            "    // Nenhum escudo disponivel",
            "    // Este personagem nao possui escudos em seu inventario"
        };
        Appearance::printBlockCentralized(msg);
        Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para cancelar..."));
        InputControl::waitForEnter();
        return 0;
    }
    std::vector<std::string> shieldsList;
    for (size_t i = 0; i < shields.size(); ++i) {
        std::string nameShield = shields[i]->getItemName();
        shieldsList.push_back("\033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mescudo\033[0m = \033[38;2;181;206;168m" + std::to_string(i) + "\033[0m; // " + nameShield);
    }
    Appearance::printBlockCentralized(shieldsList);
    Appearance::printCentralized(IDETheme::comment("Selecione o escudo com < / > e confirme com ENTER..."));
    int selected = 0;
    int maxShields = static_cast<int>(shields.size());
    while (true) {
        std::cout << "\033[?25l";
        Appearance::clearScreen();
        Appearance::printCentralized(title);
        std::vector<std::string> shieldsFormatted;
        for (size_t i = 0; i < shields.size(); ++i) {
            std::string nameShield = shields[i]->getItemName();
            std::string line = "\033[38;2;156;220;254mint\033[0m \033[38;2;156;220;254mescudo\033[0m = \033[38;2;181;206;168m" + std::to_string(i) + "\033[0m; // " + nameShield;
            if (i == static_cast<size_t>(selected)) {
                line = "\033[38;2;181;206;168m> " + line + " <\033[0m";
            }
            shieldsFormatted.push_back(line);
        }
        Appearance::printBlockCentralized(shieldsFormatted);
        unsigned char key = static_cast<unsigned char>(InputControl::readKey());
        if (key == 224 || key == 0) {
            unsigned char next = static_cast<unsigned char>(InputControl::readKey());
            if (next == 75 || next == 'D') key = 'a';
            else if (next == 77 || next == 'C') key = 'd';
        } else if (key == '\033') {
            if (InputControl::pressedKey()) {
                unsigned char next = static_cast<unsigned char>(InputControl::readKey());
                if (next == '[') {
                    next = static_cast<unsigned char>(InputControl::readKey());
                    if (next == 75 || next == 'D') key = 'a';
                    else if (next == 77 || next == 'C') key = 'd';
                }
            }
        }
        if (key == 'a' || key == 'd') {
            selected = (selected + 1) % maxShields;
        } else if (key == '\r' || key == '\n') {
            return selected;
        } else if (key == '\033') {
            return -1;
        }
    }
}

void IDECombatScreen::displayPostureDefensive(Character* player, const std::string& nameShield) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mPosturaDefensiva\033[0m() {";
    Appearance::printCentralized(title);
    std::string lineShield = "    \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mescudoEquipado\033[0m = \033[38;2;214;157;133m\"" + nameShield + "\"\033[0m;";
    Appearance::printCentralized(lineShield);
    Appearance::printCentralized(IDETheme::comment("Postura defensiva ativada! Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

void IDECombatScreen::displayImbalanceDefense(Character* player) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mvoid\033[0m \033[38;2;220;220;170mDesequilibrioDefesa\033[0m() {";
    Appearance::printCentralized(title);
    Appearance::printCentralized(IDETheme::comment("Defesa desequilibrada! Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}

void IDECombatScreen::displayMessageCombat(const std::string& message, int color) {
    Appearance::clearScreen();
    std::string title = "\033[38;2;86;156;214mnamespace\033[0m \033[38;2;78;201;176mMensagemCombate\033[0m {";
    Appearance::printCentralized(title);
    std::string lineMessage = "    \033[38;2;86;156;214mauto\033[0m \033[38;2;156;220;254mstring\033[0m \033[38;2;156;220;254mmsg\033[0m = \033[38;2;214;157;133m\"" + message + "\"\033[0m;";
    Appearance::printCentralized(lineMessage);
    Appearance::printCentralized("}");
    Appearance::printCentralized(IDETheme::comment("Pressione [ENTER] para continuar..."));
    InputControl::waitForEnter();
}
