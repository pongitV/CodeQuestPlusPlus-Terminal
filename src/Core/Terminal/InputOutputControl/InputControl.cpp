// [PT-BR] Arquivo: InputControl.cpp
// [PT-BR] Proposito: Implementacao das rotinas de baixo nivel e menus interativos de entrada.
// [EN-US] File: InputControl.cpp
// [EN-US] Purpose: Implementation of low-level routines and interactive input menus.

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

// [PT-BR] Verifica se ha alguma tecla aguardando no buffer de entrada (Non-blocking).
// [PT-BR] Utilizado primariamente pelo motor 3D e loops assincronos para evitar que a thread de renderizacao trave esperando input.
// [EN-US] Checks if there is any key waiting in the input buffer (Non-blocking).
// [EN-US] Used primarily by the 3D engine and asynchronous loops to prevent render thread from blocking on input.
bool InputControl::pressedKey() 
{
#ifdef _WIN32
    return _kbhit() != 0;
#else
    // [PT-BR] Ambiente POSIX requer implementacao detalhada non-blocking
    // [EN-US] POSIX environment requires detailed non-blocking implementation
    return false;
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

// [PT-BR] Captura e processa eventos complexos de mouse na API do Windows (Win32 Console).
// [PT-BR] Intercepta MOUSE_EVENTs diretamente da stream STDIN.
// [EN-US] Captures and processes complex mouse events in the Windows API (Win32 Console).
// [EN-US] Intercepts MOUSE_EVENT records directly from STDIN stream.
bool InputControl::pollMouseState(int& mouseX, int& mouseY, bool& isLeftPressed, bool& isRightPressed) {
#ifdef _WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD indexInEvents = 0;
    GetNumberOfConsoleInputEvents(hStdin, &indexInEvents);
    
    static int lastX = -1;
    static int lastY = -1;
    static bool lastLeft = false;
    static bool lastRight = false;
    bool evMouse = false;

    if (indexInEvents > 0) {
        INPUT_RECORD ir[128];
        DWORD indexInRead;
        ReadConsoleInput(hStdin, ir, 128, &indexInRead);
        
        std::vector<INPUT_RECORD> nonMouseEvents;

        for (DWORD i = 0; i < indexInRead; ++i) {
            if (ir[i].EventType == MOUSE_EVENT) {
                evMouse = true;
                MOUSE_EVENT_RECORD mouseEvent = ir[i].Event.MouseEvent;
                
                lastX = mouseEvent.dwMousePosition.X;
                lastY = mouseEvent.dwMousePosition.Y;
                lastLeft = (mouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) != 0;
                lastRight = (mouseEvent.dwButtonState & RIGHTMOST_BUTTON_PRESSED) != 0;
            } else {
                nonMouseEvents.push_back(ir[i]);
            }
        }
        
        if (!nonMouseEvents.empty()) {
            DWORD written = 0;
            WriteConsoleInput(hStdin, nonMouseEvents.data(), (DWORD)nonMouseEvents.size(), &written);
        }
    }
    
    mouseX = lastX;
    mouseY = lastY;
    isLeftPressed = lastLeft;
    isRightPressed = lastRight;
    
    return evMouse;
#else
    mouseX = -1; mouseY = -1; isLeftPressed = false; isRightPressed = false;
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
        // [PT-BR] Limpa a entrada invalida na tela
        // [EN-US] Clears the invalid input on screen
        std::cout << "\033[u\033[J";
    }
    return value;
}

// [PT-BR] Renderiza e controla um menu iterativo direto no console (ANSI).
// [PT-BR] Utiliza escape sequences para pular linhas (ex: \033[NA) e atualizar os icones sem que o terminal pisque inteiramente (Zero-Flicker approach).
// [EN-US] Renders and controls an interactive menu directly in console (ANSI).
// [EN-US] Uses escape sequences to jump lines (e.g., \033[NA) and update icons without causing full-terminal flicker (Zero-Flicker approach).
int InputControl::readSelectionMenuWithArrows(const std::vector<std::string>& options, bool centralize, const std::string& marginPersonalized, const std::vector<std::string>& panelRight) {
    if (options.empty()) return -1;
    
    int selectionCurrent = 0;
    int totalOptions = static_cast<int>(options.size());
    int totalRight = static_cast<int>(panelRight.size());

    // [PT-BR] Pula para a primeira opcao que nao seja HEADER (evita focar nas bordas)
    // [EN-US] Skips to the first non-HEADER option (avoids focusing on border headers)
    while (selectionCurrent < totalOptions && options[selectionCurrent].find("#HEADER#") == 0) {
        selectionCurrent++;
    }
    if (selectionCurrent >= totalOptions) selectionCurrent = 0;

    int maxLines = std::max(totalOptions, totalRight);
    std::string margin = marginPersonalized;
    
    int maxWidth = 0;
    for (const std::string& op : options) {
        std::string text = op;
        if (text.find("#HEADER#") == 0) text = text.substr(8);
        int comp = Appearance::getVisualLength(text);
        if (comp > maxWidth) maxWidth = comp;
    }
    int leftMenuWidth = maxWidth + 15;
    
    if (centralize) {
        margin = Appearance::spacesToCenter(leftMenuWidth + (totalRight > 0 ? 40 : 0));
    }

    Appearance::hideCursor();

#ifdef _WIN32
    int startLineY = -1;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        startLineY = csbi.dwCursorPosition.Y;
    }
#endif

    while (true) {
        auto now = std::chrono::steady_clock::now();
        int timeMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
        std::string cursorIcon = ((timeMs / 400) % 2 == 0) ? ">  " : " > ";

        for (int i = 0; i < maxLines; ++i) {
            std::string lineLeft = "";
            int realLeftLen = 0;
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
                realLeftLen = Appearance::getVisualLength(text) + 3;
            }
            
            std::string lineSay = (i < totalRight) ? panelRight[i] : "";
            
            int padding = leftMenuWidth - realLeftLen;
            if (padding < 0) padding = 0;
            
            if (PerspectiveManager::getInstance().is3DViewActive()) {
                std::string moveMargin = margin.empty() ? "" : "\033[" + std::to_string(margin.length()) + "C";
                std::cout << "\033[0m" << moveMargin << lineLeft << std::string(padding, ' ') << lineSay << "\n";
            } else {
                std::cout << margin << lineLeft << std::string(padding, ' ') << lineSay << "\033[K\n";
            }
        }

        std::cout << std::flush; 

#ifdef _WIN32
        int mouseX = -1, mouseY = -1;
        bool isLeftPressed = false, isRightPressed = false;
        if (pollMouseState(mouseX, mouseY, isLeftPressed, isRightPressed)) {
            if (startLineY != -1 && mouseY >= startLineY && mouseY < startLineY + totalOptions) {
                int hovered = mouseY - startLineY;
                if (hovered >= 0 && hovered < totalOptions && options[hovered].find("#HEADER#") != 0) {
                    selectionCurrent = hovered;
                    if (isLeftPressed) {
                        while (true) {
                            int mx, my; bool l, r;
                            pollMouseState(mx, my, l, r);
                            if (!l) break;
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        }
                        Appearance::concertCursor();
                        return selectionCurrent;
                    }
                }
            }
        }
#endif

        if (pressedKey()) {
            unsigned char key = static_cast<unsigned char>(readKey());
            
            if (key == 224 || key == 0 || key == '\033') {
                unsigned char nextKey = static_cast<unsigned char>(readKey());
                if (nextKey == '[') nextKey = static_cast<unsigned char>(readKey()); 
                
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
            else if (key == '\r' || key == '\n') { Appearance::concertCursor(); return selectionCurrent; } 

            std::cout << "\r\033[" << maxLines << "A"; 
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            std::cout << "\r\033[" << maxLines << "A"; 
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
        
#ifdef _WIN32
        int mouseX = -1, mouseY = -1;
        bool isLeft = false, isRight = false;
        if (pollMouseState(mouseX, mouseY, isLeft, isRight)) {
            int optionsStartY = startY + static_cast<int>(text.size()) + 2;
            if (mouseY >= optionsStartY && mouseY < optionsStartY + totalOptions) {
                int hovered = mouseY - optionsStartY;
                if (hovered >= 0 && hovered < totalOptions) {
                    selectionCurrent = hovered;
                    if (isLeft) {
                        while (true) {
                            int mx, my; bool l, r;
                            pollMouseState(mx, my, l, r);
                            if (!l) break;
                            std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        }
                        return selectionCurrent;
                    }
                }
            }
        }
#endif

        if (pressedKey()) {
            unsigned char key = static_cast<unsigned char>(readKey());
            if (key == 224 || key == 0 || key == 27) {
                unsigned char nextKey = static_cast<unsigned char>(readKey());
                if (nextKey == '[') nextKey = static_cast<unsigned char>(readKey());
                if (nextKey == 72 || nextKey == 'A') key = 'w';
                else if (nextKey == 80 || nextKey == 'B') key = 's';
            }

            if (key == 'w' || key == 'W') { selectionCurrent--; if (selectionCurrent < 0) selectionCurrent = totalOptions - 1; }
            else if (key == 's' || key == 'S') { selectionCurrent++; if (selectionCurrent >= totalOptions) selectionCurrent = 0; }
            else if (key == '\r' || key == '\n') { 
                return selectionCurrent;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
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
#ifdef _WIN32
            int mx = -1, my = -1;
            bool isLeft = false, isRight = false;
            if (pollMouseState(mx, my, isLeft, isRight) && isLeft) {
                while (true) {
                    int x, y; bool l, r;
                    pollMouseState(x, y, l, r);
                    if (!l) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                break;
            }
#endif
            if (InputControl::pressedKey()) {
                char c = InputControl::readKey();
                if (c == '\r' || c == '\n' || c == ' ' || c == 27) break;
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
#ifdef _WIN32
            int mx = -1, my = -1;
            bool isLeft = false, isRight = false;
            if (pollMouseState(mx, my, isLeft, isRight) && isLeft) {
                while (true) {
                    int x, y; bool l, r;
                    pollMouseState(x, y, l, r);
                    if (!l) break;
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
                break;
            }
#endif
            if (InputControl::pressedKey()) {
                char c = InputControl::readKey();
                if (c == '\r' || c == '\n' || c == ' ' || c == 27) break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }
}
