#include "UI/Screens/Pause/PauseScreen.h"
#include "UI/Screens/ScreenRegistry.h"
#include <iostream>
#include "Domain/Characters/Character.h"
#include "UI/PerspectiveManager.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/InputControl.h"

static int getChooseMenuPause() {
    return PerspectiveManager::getPauseUI().renderMenuPause();
}

static int getSettingsChoice(Character* player) {
    return PerspectiveManager::getPauseUI().renderSettingsMenu(player);
}

static int getChooseAppearance(Character* player) {
    return PerspectiveManager::getPauseUI().renderMenuAppearance(player);
}

static int getChooseBackground(int colorBackgroundCurrentIndex) {
    return PerspectiveManager::getPauseUI().renderMenuBackground(colorBackgroundCurrentIndex);
}

static int getChooseSensitivity(int percentX, int percentY) {
    return PerspectiveManager::getPauseUI().renderMenuSensitivity(percentX, percentY);
}

void PauseScreen::display(Character* player) {
    int colorBackgroundCurrentIndex = 0;
    bool keepRunning = true;

    while (keepRunning && !player->getReturnToMenu()) {
        int choice = getChooseMenuPause();

        if (choice == 0) {
            keepRunning = false;
        } else if (choice == 1) {
            bool configOpen = true;
            while (configOpen) {
                int confChoose = getSettingsChoice(player);

                if (confChoose == 0) {
                    int difficultyCurrent = static_cast<int>(player->getDifficulty());
                    difficultyCurrent++;
                    if (difficultyCurrent > 3) difficultyCurrent = 1;
                    player->setDifficulty(static_cast<GameDifficulty>(difficultyCurrent));
                } else if (confChoose == 1) {
                    player->setParryEnabled(!player->getParryActivated());
                } else if (confChoose == 2) {
                    player->setModernParry(!player->getParryModern());
                } else if (confChoose == 3) {
                    bool appearanceOpen = true;
                    while (appearanceOpen) {
                        int apChoose = getChooseAppearance(player);

                        if (apChoose == 0) {
                            Color colors[] = {Color::GREEN, Color::BLUE, Color::YELLOW, Color::WHITE, Color::MAGENTA, Color::CYAN, Color::RED};
                            int indexInColors = 7;
                            int idxCurrent = 0;
                            for (int i = 0; i < indexInColors; ++i) {
                                if (Appearance::customPlayerColor == colors[i]) {
                                    idxCurrent = i;
                                    break;
                                }
                            }
                            idxCurrent = (idxCurrent + 1) % indexInColors;
                            Appearance::customPlayerColor = colors[idxCurrent];
                        } else if (apChoose == 1) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo icone (1 caractere): ";
                            std::cout << Appearance::spacesToCenter(promptMsg.length()) << promptMsg;
                            std::string newIcon = InputControl::readEntryProtected();
                            if (!newIcon.empty() && newIcon[0] != ' ') {
                                Appearance::customPlayerIcon = newIcon[0];
                            }
                        } else {
                            appearanceOpen = false;
                        }
                    }
                } else if (confChoose == 4) {
                    bool backgroundOpen = true;
                    while (backgroundOpen) {
                        int backgroundChoose = getChooseBackground(colorBackgroundCurrentIndex);

                        if (backgroundChoose >= 0 && backgroundChoose <= 5) {
                            colorBackgroundCurrentIndex = backgroundChoose;
                            std::string hexColor;
                            switch (backgroundChoose) {
                                case 0: hexColor = "#0C0C0C"; break; case 1: hexColor = "#1A1A1A"; break;
                                case 2: hexColor = "#000022"; break; case 3: hexColor = "#220000"; break;
                                case 4: hexColor = "#002200"; break; case 5: hexColor = "#220022"; break;
                            }
                            Appearance::setColorBackgroundTerminal(hexColor);
                        } else {
                            backgroundOpen = false;
                        }
                    }
                } else if (confChoose == 5) {
                    bool sensitivityOpen = true;
                    while (sensitivityOpen) {
                        int percentX = (int)((PerspectiveManager::getSensitivityMouseX() / 0.002f) * 100);
                        int percentY = (int)((PerspectiveManager::getSensitivityMouseY() / 0.08f) * 100);

                        int sensChoose = getChooseSensitivity(percentX, percentY);

                        if (sensChoose == 0) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo valor em porcentagem (ex: 50, 100, 150): ";
                            std::cout << Appearance::spacesToCenter(promptMsg.length()) << promptMsg;
                            std::string entry = InputControl::readEntryProtected();
                            try {
                                int newValue = std::stoi(entry);
                                if (newValue > 0) PerspectiveManager::setSensitivityMouse((newValue / 100.0f) * 0.002f, PerspectiveManager::getSensitivityMouseY());
                            } catch (...) {}
                        } else if (sensChoose == 1) {
                            std::cout << "\n";
                            std::string promptMsg = "Digite o novo valor em porcentagem (ex: 50, 100, 150): ";
                            std::cout << Appearance::spacesToCenter(promptMsg.length()) << promptMsg;
                            std::string entry = InputControl::readEntryProtected();
                            try {
                                int newValue = std::stoi(entry);
                                if (newValue > 0) PerspectiveManager::setSensitivityMouse(PerspectiveManager::getSensitivityMouseX(), (newValue / 100.0f) * 0.08f);
                            } catch (...) {}
                        } else {
                            sensitivityOpen = false;
                        }
                    }
                } else {
                    configOpen = false;
                }
            }
        } else if (choice == 2) {
            if (ScreenRegistry::confirmExit()) {
                std::exit(0);
            }
        }
    }
}
