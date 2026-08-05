/*
 * Arquivo: InputControl.cpp
 * Propósito: Implementação das rotinas de baixo nível e menus interativos de entrada.
 */

#include "Core/Terminal/InputOutputControl/InputControl.h"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "UI/PerspectiveManager.h"
#include "UI/PerspectiveChangedException.h"
#include "UI/Screens/BaseScreen.h"

#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <terms.h>
    #include <fcntl.h>
#endif
#include "Core/Terminal/Appearance/Appearance.h"

std::function<void()> InputControl::onWaitEnterUpdate = nullptr;
std::string InputControl::enterPromptText = "";

bool InputControl::pressedKey() 
{
#ifdef _WIN32
    return _kbhit() != 0;
#else
    return false; // Ambiente POSIX requer implementacao detalhada non-blocking
#endif
}

char InputControl::readKey() 
{
    char key;
#ifdef _WIN32
    key = static_cast<char>(_getch());
#else
    key = static_cast<char>(std::cin.get());
#endif
    
    return key;
}

void InputControl::clearBuffer() 
{
#ifdef _WIN32
    while (_kbhit()) _getch();
#endif
}

MapCommand InputControl::translateKeyToCommand(char key)
{
    if (key == 'w' || key == 'W' || key == 72) return MapCommand::Up;
    if (key == 's' || key == 'S' || key == 80) return MapCommand::Down;
    if (key == 'a' || key == 'A' || key == 75) return MapCommand::Left;
    if (key == 'd' || key == 'D' || key == 77) return MapCommand::Right;
    if (key == 'i' || key == 'I') return MapCommand::Inventory;
    if (key == 'c' || key == 'C') return MapCommand::CharacterSheet;
    if (key == 'b' || key == 'B') return MapCommand::Bestiary;
    return MapCommand::None;
}

#ifdef _WIN32
#include <windows.h>
#endif

void InputControl::enableMouseInput() {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);
    mode = (mode & ~ENABLE_QUICK_EDIT_MODE) | ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
    SetConsoleMode(hStdin, mode);
#endif
}

bool InputControl::readStateDragHorizontalMouse(int& deltaX) {
    deltaX = 0;
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD inaEvents = 0;
    GetNumberOfConsoleInputEvents(hStdin, &inaEvents);
    if (inaEvents == 0) return false;

    INPUT_RECORD ir[128];
    DWORD inaRead;
    // We use ReadConsoleInput, but we must restore KEYBOARD_EVENTs so _kbhit() doesn't break
    ReadConsoleInput(hStdin, ir, 128, &inaRead);
    
    static int lastMouseX = -1;
    static bool isDragging = false;
    bool evMouse = false;
    
    std::vector<INPUT_RECORD> nonMouseEvents;

    for (DWORD i = 0; i < inaRead; ++i) {
        if (ir[i].EventType == MOUSE_EVENT) {
            evMouse = true;
            MOUSE_EVENT_RECORD mouseEvent = ir[i].Event.MouseEvent;
            
            if (mouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                if (!isDragging) {
                    isDragging = true;
                    lastMouseX = mouseEvent.dwMousePosition.X;
                } else {
                    deltaX += (int)mouseEvent.dwMousePosition.X - lastMouseX;
                    lastMouseX = mouseEvent.dwMousePosition.X;
                }
            } else {
                isDragging = false;
                lastMouseX = -1;
            }
        } else {
            nonMouseEvents.push_back(ir[i]);
        }
    }
    
    if (!nonMouseEvents.empty()) {
        DWORD written = 0;
        WriteConsoleInput(hStdin, nonMouseEvents.data(), (DWORD)nonMouseEvents.size(), &written);
    }
    
    return evMouse;
#else
    return false;
#endif
}


std::string InputControl::readEntryProtected(const std::string& promptMessage) {
    if (!promptMessage.empty()) {
        Appearance::displayPrompt(promptMessage);
    }
    std::cout << "\033[s";
    std::string entry;
    while (true) {
        if (!std::getline(std::cin, entry)) std::cin.clear();
        
        if (!entry.empty()) {
            entry.erase(0, entry.find_first_not_of(" \n\r\t"));
            entry.erase(entry.find_last_not_of(" \n\r\t") + 1);
        }
        
        if (entry.empty()) {
            std::cout << "\033[u\033[J";
            continue;
        }
        return entry;
    }
}

int InputControl::readIntegerWithLimits(const std::string& promptMessage, int minimum, int maximum, bool centralizePrompt, const std::string& marginPersonalized) {
    int value;
    if (centralizePrompt) Appearance::displayPrompt(promptMessage);
    else std::cout << marginPersonalized << promptMessage;

    while (true) {
        std::string entry = readEntryProtected();
        try {
            value = std::stoi(entry);
            if (value >= minimum && value <= maximum) break;
        } catch (...) {}
        std::cout << "\033[u\033[J"; // Apenas limpa a entrada invalida
    }
    return value;
}

int InputControl::readSelectionMenuWithArrows(const std::vector<std::string>& options, bool centralize, const std::string& marginPersonalized, const std::vector<std::string>& panelRight) {
    if (options.empty()) return -1;
    
    int selectionCurrent = 0;
    int totalOptions = static_cast<int>(options.size());
    int totalDir = static_cast<int>(panelRight.size());

    // Pula para a primeira opcao que nao seja HEADER (evita que comece focado nas bordas da caixa)
    while (selectionCurrent < totalOptions && options[selectionCurrent].find("#HEADER#") == 0) {
        selectionCurrent++;
    }
    if (selectionCurrent >= totalOptions) selectionCurrent = 0;

    int maxLines = std::max(totalOptions, totalDir);
    std::string margin = marginPersonalized;
    
    int maxWidth = 0;
    for (const std::string& op : options) {
        std::string text = op;
        if (text.find("#HEADER#") == 0) text = text.substr(8);
        int comp = Appearance::getVisualLength(text);
        if (comp > maxWidth) maxWidth = comp;
    }
    int larguraMenuEsq = maxWidth + 15; // 3 para " > " + 12 de espacamento

    if (centralize) {
        margin = Appearance::spacesToCenter(larguraMenuEsq + (totalDir > 0 ? 40 : 0));
    }

    // Oculta o cursor do console temporariamente para evitar piscadas visuais na atualização
    Appearance::hideCursor();

    while (true) {
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::string cursorIcon = ((timeMs / 400) % 2 == 0) ? ">  " : " > ";

        for (int i = 0; i < maxLines; ++i) {
            std::string lineLeft = "";
            int lenEsqReal = 0;
            if (i < totalOptions) {
                std::string text = options[i];
                bool isHeader = false;
                if (text.find("#HEADER#") == 0) {
                    isHeader = true;
                    text = text.substr(8);
                }

                if (isHeader) {
                    lineLeft = "   " + text;
                } else if (i == selectionCurrent) {
                    lineLeft = Appearance::color(Color::GREEN) + cursorIcon + text + Appearance::color(Color::RESET);
                } else {
                    lineLeft = "   " + text;
                }
                lenEsqReal = Appearance::getVisualLength(text) + 3;
            }
            
            std::string lineSay = (i < totalDir) ? panelRight[i] : "";
            
            int padding = larguraMenuEsq - lenEsqReal;
            if (padding < 0) padding = 0;
            
            if (PerspectiveManager::getInstance().is3DViewActive()) {
                std::string margemMove = margin.empty() ? "" : "\033[" + std::to_string(margin.length()) + "C";
                std::cout << "\033[0m" << margemMove << lineLeft << std::string(padding, ' ') << lineSay << "\n";
            } else {
                std::cout << margin << lineLeft << std::string(padding, ' ') << lineSay << "\033[K\n";
            }
        }

        std::cout << std::flush; // Garante que a tela sempre atualize antes de esperar a tecla

        if (pressedKey()) {
            unsigned char key = static_cast<unsigned char>(readKey());
            
            if (key == 224 || key == 0 || key == '\033') {
                unsigned char nextKey = static_cast<unsigned char>(readKey());
                if (nextKey == '[') nextKey = static_cast<unsigned char>(readKey()); // Para lidar com sequências de escape POSIX (\033[A)
                
                if (nextKey == 72 || nextKey == 'A') key = 'w';
                else if (nextKey == 80 || nextKey == 'B') key = 's';
            }

            if (key == 'w' || key == 'W') { 
                int home = selectionCurrent;
                do {
                    selectionCurrent--; 
                    if (selectionCurrent < 0) selectionCurrent = totalOptions - 1; 
                } while (options[selectionCurrent].find("#HEADER#") == 0 && selectionCurrent != home);
            }
            else if (key == 's' || key == 'S') { 
                int home = selectionCurrent;
                do {
                    selectionCurrent++; 
                    if (selectionCurrent >= totalOptions) selectionCurrent = 0; 
                } while (options[selectionCurrent].find("#HEADER#") == 0 && selectionCurrent != home);
            }
            else if (key == '\r' || key == '\n') { Appearance::concertCursor(); return selectionCurrent; } // Restaura o cursor

            std::cout << "\r\033[" << maxLines << "A"; // Retorna o cursor para cima a fim de reescrever o texto
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout << "\r\033[" << maxLines << "A"; // Retorna o cursor para cima a fim de reescrever o texto
        }
    }
}

int InputControl::readMenuSelectionInPopup(const std::string& title, const std::vector<std::string>& text, const std::vector<std::string>& options, Color themeColor, const std::vector<std::string>& artOriginal, bool /*animarEntrada*/) {
    if (options.empty()) return -1;
        
    std::vector<std::string> art = artOriginal;
    if (static_cast<int>(art.size()) > 10) {
        art = Appearance::reduceScaleAscii(artOriginal, Appearance::FACTOR_COMPRESSION_GLOBAL, Appearance::FACTOR_COMPRESSION_GLOBAL);
    }

    int selectionCurrent = 0;
    int totalOptions = static_cast<int>(options.size());
    std::string bgPopup = "\033[48;2;25;25;25m";
    std::cout << "\033[?25l";
    
    InputControl::clearBuffer();
    bool firstTime = true;
    while (true) {
        if (firstTime) {
            Appearance::cleanPopupPrevious();
            firstTime = false;
        }

        std::vector<std::string> linesText = text;
        linesText.push_back("");
        for (int i = 0; i < totalOptions; ++i) {
            if (i == selectionCurrent) linesText.push_back(Appearance::color(Color::GREEN) + " > " + options[i] + Appearance::color(Color::RESET));
            else linesText.push_back("   " + options[i]);
        }

        std::vector<std::string> box;
        if (art.empty()) {
            box = BaseScreen::createBox(linesText, title, 0, themeColor, bgPopup);
        } else {
            box = BaseScreen::createBoxWithArt(art, linesText, title, 0, themeColor, bgPopup);
        }

        int endBoxWidth = Appearance::getVisualLength(box[0]);
        int endBoxHeight = box.size();
        
        int widthTerm = Appearance::getTerminalWidth();
        int heightTerm = Appearance::getTerminalHeight();
        int startX = (widthTerm - endBoxWidth) / 2;
        int startY = (heightTerm - endBoxHeight) / 2;
        if (startX < 0) startX = 0;
        if (startY < 0) startY = 0;
        
        for (int i = 0; i < endBoxHeight; ++i) {
            Appearance::moveCursor(startX, startY + i);
            std::cout << box[i];
        }
        std::cout << std::flush;

        Appearance::setLastPopup(startX, startY, endBoxWidth, endBoxHeight, bgPopup);
        
        char key = readKey();
        if (key == 224 || key == 0 || key == '\033') {
            char nextKey = readKey();
            if (nextKey == '[') nextKey = readKey();
            if (nextKey == 72 || nextKey == 'A') key = 'w';
            else if (nextKey == 80 || nextKey == 'B') key = 's';
        }

        if (key == 'w' || key == 'W') { selectionCurrent--; if (selectionCurrent < 0) selectionCurrent = totalOptions - 1; }
        else if (key == 's' || key == 'S') { selectionCurrent++; if (selectionCurrent >= totalOptions) selectionCurrent = 0; }
        else if (key == '\r' || key == '\n') { 
            return selectionCurrent;
        }
    }
}

void InputControl::executeLoopMenuPopup(
    const std::function<void()>& displayDialogue,
    const std::function<std::vector<std::string>()>& getOptions,
    const std::function<bool(const std::string&)>& processOption,
    const std::string& title,
    Color themeColor,
    const std::vector<std::string>& asciiArt
) {
    Appearance::startPopupInteraction();
    if (displayDialogue) displayDialogue();

    while (true) {
        auto options = getOptions();
        if (options.empty()) break;
        
        int choice = readMenuSelectionInPopup(title, {"O que deseja fazer?"}, options, themeColor, asciiArt);
        if (choice == -1 || choice == static_cast<int>(options.size()) - 1) break;
        if (!processOption(options[choice])) break;
    }
}

void InputControl::waitForEnter(const std::string& message) {
    Appearance::hideCursor();
    if (InputControl::onWaitEnterUpdate) {
        InputControl::enterPromptText = message;
        InputControl::clearBuffer();
        while (true) {
            InputControl::onWaitEnterUpdate();
            if (InputControl::pressedKey()) {
                char c = InputControl::readKey();
                if (c == '\r' || c == '\n') break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
        InputControl::enterPromptText = "";
        InputControl::onWaitEnterUpdate();
    } else {
        if (!message.empty()) {
            std::cout << "\n\033[0m" << Appearance::spacesToCenter(Appearance::getVisualLength(message)) << "\033[5m" << message << "\033[0m\n";
        }
        InputControl::clearBuffer();
        while (true) {
            char c = InputControl::readKey();
            if (c == '\r' || c == '\n') break;
        }
    }
}
