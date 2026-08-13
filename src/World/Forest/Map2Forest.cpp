#include "World/Forest/Map2Forest.h"
#include "Core/Engine/Debug.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <functional>
#include <chrono>
#include <thread>

#include "UI/Screens/Menu/MenuScreen.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Equipment/ArmorEquipment.h"
#include "Domain/Items/Items/ConsumableItem.h"
#include "Domain/Items/Items/MaterialItem.h"
#include "Core/Engine/EnemyCreator.h"
#include "Systems/Inventory/CombatInventory.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Bestiary/BestiaryScreen.h"
#include "Systems/Combat/Combat.h"
#include "World/MapControl.h"
#include "World/Systems/MapAnimator.h"
#include "World/Systems/MapLoader.h"
#include "Domain/NPCs/NPCMage.h"
#include "Domain/Enemies/ForestAbomination.h"
#include "Domain/Enemies/Mahoraga.h"
#include "Domain/Enemies/BaseEnemyClass.h"
#include "Core/Utils/Appearance.h"
#include "Systems/Progression/Diary.h"
#include "Core/Utils/DialogFunctions.h"
#include "World/MapControl.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/RandomGenerator.h"
#include "World/Kingdom/Map3KingdomBridge.h"
#include "World/Forest/Map2ForestLayout.h"

Map2Forest::Map2Forest(Character* playerCharacter) :
    playerPositionX(31), 
    playerPositionY(17),
    currentPlayer(playerCharacter), 
    savedPositionXBeforeEnteringSubMap(0), 
    savedPositionYBeforeEnteringSubMap(0),
    playerIsInsideSubMap(false),
    heartOfTreeAlreadyVisited(false), 
    mazeAlreadyVisited(false),
    bossRoomAlreadyVisited(false),
    isExplorationActive(true), 
    currentMapTitle("FLORESTA"),
    nextMap(NextMapTransition::None)
{
    currentMapMatrix = Map2ForestLayouts::getForestLayout();
    MapLoader::standardizeMapSize(currentMapMatrix);
}

Map2Forest::~Map2Forest() = default;

namespace {
    class SlimeInteraction : public ForestInteraction {
    public:
        void process(ForestInteractionContext& ctx) override {
            if (ctx.nextPositionX > 0 && ctx.self->currentMapMatrix[ctx.nextPositionY][ctx.nextPositionX-1] != '^') {
                MapControl::processCombat(ctx.self->currentPlayer, ctx.self->currentMapMatrix, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->isExplorationActive, "ENCONTRO PEGAJOSO", "Voce encontrou Slimes selvagens!", EnemyCreator::createSlimeEnemy(RandomGenerator::getInteger(1, 3)), ctx.nextPositionX, ctx.nextPositionY, ctx.nextPositionX, 1, ctx.terminalWidth, ctx.restoreScreen);
            } else {
                ctx.self->playerPositionX = ctx.nextPositionX;
                ctx.self->playerPositionY = ctx.nextPositionY;
            }
        }
    };

    class FairyInteraction : public ForestInteraction {
    public:
        void process(ForestInteractionContext& ctx) override {
            MapControl::processCombat(ctx.self->currentPlayer, ctx.self->currentMapMatrix, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->isExplorationActive, "ENCONTRO MAGICO", "Voce encontrou Fadas hostis!", EnemyCreator::createFairyEnemy(RandomGenerator::getInteger(1, 3)), ctx.nextPositionX, ctx.nextPositionY, ctx.nextPositionX, 1, ctx.terminalWidth, ctx.restoreScreen);
        }
    };

    class AbominationInteraction : public ForestInteraction {
    public:
        void process(ForestInteractionContext& ctx) override {
            MapControl::processCombat(ctx.self->currentPlayer, ctx.self->currentMapMatrix, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->isExplorationActive, "ENCONTRO BOSS", "Voce encontrou a Abominacao da Floresta!", EnemyCreator::createForestAbominationEnemy(1), ctx.nextPositionX, ctx.nextPositionY, ctx.nextPositionX, 1, ctx.terminalWidth, ctx.restoreScreen);
        }
    };

    class MorganaInteraction : public ForestInteraction {
    public:
        void process(ForestInteractionContext& ctx) override {
            NPCMage morganaInteraction;
            morganaInteraction.interact(ctx.self->currentPlayer);
            Diary::instance().registerNPC("Morgana (Bruxa)");
            if (ctx.self->isExplorationActive && !MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
        }
    };

    class ChestInteraction : public ForestInteraction {
    public:
        void process(ForestInteractionContext& ctx) override {
            if (ctx.self->currentMapTitle == "LABIRINTO SUBTERRANEO") {
                Appearance::startPopupInteraction();
                
                std::vector<std::string> msgText = { "Voce encontrou um Bau ancestral!" };
                std::vector<std::string> chestOptions = { "Nao", "Abrir!" };
                int option = InputControl::readMenuSelectionInPopup("TESOURO ESCONDIDO", msgText, chestOptions, Color::GREEN);

                if (option == 1) {
                    if (RandomGenerator::rollChance(25)) {
                        Appearance::startPopupInteraction();
                        Appearance::displayPopup("CILADA!", {"O bau se revela uma criatura viva!", "E UM MIMICO!"}, Color::RED);
                        MapControl::processCombat(ctx.self->currentPlayer, ctx.self->currentMapMatrix, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->isExplorationActive, "CILADA!", "O Bau era um Mimico!", EnemyCreator::createMimicEnemy(1), ctx.nextPositionX, ctx.nextPositionY, ctx.nextPositionX, 1, ctx.terminalWidth, ctx.restoreScreen);
                    } else {
                    std::vector<std::string> lootMsg = { "O bau se abre rangendo...", "Voce obteve itens valiosos!", "" };

                    int qtyPotions = RandomGenerator::getInteger(2, 4);
                    for (int i = 0; i < qtyPotions; ++i) {
                        auto potion = std::make_unique<ConsumableItem>("Pocao de Cura (30%VM)");
                        potion->addProperty(Property::HealingConsumable);
                        ctx.self->currentPlayer->getInventory()->addItem(std::move(potion));
                    }
                    lootMsg.push_back("+ " + std::to_string(qtyPotions) + "x Pocoes de Cura (30%VM)");

                    int qtyGold = RandomGenerator::getInteger(150, 300);
                    ctx.self->currentPlayer->getInventory()->addGold(qtyGold);
                    lootMsg.push_back("+ " + std::to_string(qtyGold) + "G");

                    bool isFury = RandomGenerator::rollChance(50);
                    std::string buffName = isFury ? "Pocao de Furia (Buff)" : "Elixir Arcano (Buff)";
                    auto buff = std::make_unique<ConsumableItem>(buffName);
                    buff->addProperty(Property::BuffConsumable);
                    ctx.self->currentPlayer->getInventory()->addItem(std::move(buff));
                    lootMsg.push_back("+ 1x " + buffName);

                    ctx.self->currentPlayer->getInventory()->addItem(std::make_unique<MaterialItem>("Pedra magica de upgrade"));
                    lootMsg.push_back("+ 1x Pedra magica de upgrade");

                    ctx.self->currentMapMatrix[ctx.nextPositionY][ctx.nextPositionX] = ' ';
                    ctx.self->playerPositionX = ctx.nextPositionX;
                    ctx.self->playerPositionY = ctx.nextPositionY;
                    Appearance::startPopupInteraction();
                    Appearance::displayPopup("TESOURO ESCONDIDO", lootMsg, Color::GREEN);
                    }
                }
                ctx.self->playerPositionX = ctx.nextPositionX;
                ctx.self->playerPositionY = ctx.nextPositionY;

                if (ctx.self->isExplorationActive && !MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
            } else {
                ctx.self->playerPositionX = ctx.nextPositionX;
                ctx.self->playerPositionY = ctx.nextPositionY;
            }
        }
    };

    class TeleportInteraction : public ForestInteraction {
    public:
        void process(ForestInteractionContext& ctx) override {
            int px = ctx.nextPositionX;
            int py = ctx.nextPositionY;
            std::string title = ctx.self->currentMapTitle;
            
            // 2. Voltar para a Vila a partir da Floresta
            if (px < 40 && py < 20 && !ctx.self->playerIsInsideSubMap) {
                ctx.self->isExplorationActive = false;
                ctx.self->nextMap = NextMapTransition::Village;
            }
            // 3. Entrar no Coracao da Arvore a partir da Floresta
            else if (px > 80 && py > 20 && !ctx.self->playerIsInsideSubMap) {
                MapLoader::enterSubMap(ctx.self->currentMapMatrix, ctx.self->savedMainMapMatrix, ctx.self->savedPositionXBeforeEnteringSubMap, ctx.self->savedPositionYBeforeEnteringSubMap, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->playerIsInsideSubMap, ctx.self->currentMapTitle, ctx.self->savedTreeHeartMapMatrix, ctx.self->heartOfTreeAlreadyVisited, Map2ForestLayouts::getTreeHeartLayout(), 10, 3, "CORACAO DA ARVORE", ctx.restoreScreen);
            }
            // 4. Ir para o Reino a partir da Floresta
            else if (px < 40 && py > 20 && !ctx.self->playerIsInsideSubMap) {
                ctx.self->isExplorationActive = false;
                ctx.self->nextMap = NextMapTransition::Kingdom;
            }
            // 5. Entrar no Labirinto a partir da Floresta
            else if (px > 100 && py < 20 && title == "FLORESTA") {
                if (!ctx.self->currentPlayer->getUnlockedMaze()) {
                    Appearance::startPopupInteraction();
                    std::vector<std::string> msg = {
                        "A passagem esta selada por magia.",
                        "Fale com Morgana."
                    };
                    Appearance::displayPopup("PASSAGEM BLOQUEADA", msg, Color::MAGENTA);
                    ctx.self->playerPositionX = 129;
                    ctx.self->playerPositionY = 10;
                    if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
                    return;
                }

                MapLoader::enterSubMap(ctx.self->currentMapMatrix, ctx.self->savedMainMapMatrix, ctx.self->savedPositionXBeforeEnteringSubMap, ctx.self->savedPositionYBeforeEnteringSubMap, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->playerIsInsideSubMap, ctx.self->currentMapTitle, ctx.self->savedMazeMapMatrix, ctx.self->mazeAlreadyVisited, Map2ForestLayouts::getMazeLayout(), 4, 11, "LABIRINTO SUBTERRANEO", ctx.restoreScreen);
            }
            // 6. Sair de Submapas
            else if ((title == "CORACAO DA ARVORE") ||
                     ((px == 1 || px == 2) && py == 11 && title == "LABIRINTO SUBTERRANEO") ||
                     (title == "SALA DO CHEFE")) {
                
                if (title == "CORACAO DA ARVORE") {
                    ctx.self->heartOfTreeAlreadyVisited = false; 
                }
                else if (title == "LABIRINTO SUBTERRANEO") {
                    ctx.self->savedMazeMapMatrix = ctx.self->currentMapMatrix;
                }
                else if (title == "SALA DO CHEFE") {
                    ctx.self->savedBossRoomMapMatrix = ctx.self->currentMapMatrix;
                }

                if (title == "SALA DO CHEFE") {
                    ctx.self->currentMapMatrix = ctx.self->savedMazeMapMatrix;
                    ctx.self->playerPositionX = 76; 
                    ctx.self->playerPositionY = 11;
                    ctx.self->currentMapTitle = "LABIRINTO SUBTERRANEO";
                } else {
                    ctx.self->currentMapMatrix = ctx.self->savedMainMapMatrix;
                    ctx.self->playerPositionX = ctx.self->savedPositionXBeforeEnteringSubMap;
                    ctx.self->playerPositionY = ctx.self->savedPositionYBeforeEnteringSubMap;
                    ctx.self->playerIsInsideSubMap = false;
                    ctx.self->currentMapTitle = "FLORESTA";
                }
                if (!MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
            }
            // 7. Fim do Labirinto (Escadaria para Boss)
            else if ((px == 77 || px == 78) && py == 11 && title == "LABIRINTO SUBTERRANEO") {
                Appearance::startPopupInteraction();
                std::vector<std::string> mazeMsg = {
                    "Voce encontrou a saida do labirinto!",
                    "A sua frente, uma escadaria desce para uma caverna escura.",
                    "No fundo, parece haver um mar de liquido preto raso..."
                };
                std::vector<std::string> pathOptions = { "Descer a escadaria", "Voltar para a Floresta" };
                int choice = InputControl::readMenuSelectionInPopup("FIM DO LABIRINTO", mazeMsg, pathOptions, Color::GREEN);

                if (choice == 0) {
                    std::vector<std::string> bossMsg = {
                        "O ar aqui embaixo e gelado, cortante.",
                        "O liquido preto no chao e raso e liso como vidro.",
                        "Tudo e escuridao, exceto pelo brilho pulsante da",
                        "enorme runa magica desenhada no fundo da caverna."
                    };
                    
                    std::vector<std::string> bossOptions = {
                        Appearance::color(Color::RED) + "Seguir em frente" + Appearance::color(Color::RESET),
                        Appearance::color(Color::WHITE) + "Voltar para a seguranca da Floresta" + Appearance::color(Color::RESET)
                    };
                    int bossChoice = InputControl::readMenuSelectionInPopup("CAVERNA SOMBRIA", bossMsg, bossOptions, Color::RED);
                    
                    if (bossChoice == 0) {
                        ctx.self->savedMazeMapMatrix = ctx.self->currentMapMatrix;
                        if (!ctx.self->bossRoomAlreadyVisited) {
                            ctx.self->currentMapMatrix = Map2ForestLayouts::getBossRoomLayout();
                            MapLoader::standardizeMapSize(ctx.self->currentMapMatrix);
                            ctx.self->bossRoomAlreadyVisited = true;
                        } else {
                            ctx.self->currentMapMatrix = ctx.self->savedBossRoomMapMatrix;
                        }
                        ctx.self->playerPositionX = 53;
                        ctx.self->playerPositionY = 53;
                        ctx.self->currentMapTitle = "SALA DO CHEFE";
                        if (!MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
                    } else {
                        ctx.self->savedMazeMapMatrix = ctx.self->currentMapMatrix;
                        ctx.self->currentMapMatrix = ctx.self->savedMainMapMatrix;
                        ctx.self->playerPositionX = ctx.self->savedPositionXBeforeEnteringSubMap;
                        ctx.self->playerPositionY = ctx.self->savedPositionYBeforeEnteringSubMap;
                        ctx.self->playerIsInsideSubMap = false;
                        ctx.self->currentMapTitle = "FLORESTA";
                    }
                } else {
                    ctx.self->savedMazeMapMatrix = ctx.self->currentMapMatrix;
                    ctx.self->currentMapMatrix = ctx.self->savedMainMapMatrix;
                    ctx.self->playerPositionX = ctx.self->savedPositionXBeforeEnteringSubMap;
                    ctx.self->playerPositionY = ctx.self->savedPositionYBeforeEnteringSubMap;
                    ctx.self->playerIsInsideSubMap = false;
                    ctx.self->currentMapTitle = "FLORESTA";
                }
                if (ctx.self->isExplorationActive && !MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
            } else {
                ctx.self->playerPositionX = ctx.nextPositionX;
                ctx.self->playerPositionY = ctx.nextPositionY;
            }
        }
    };

    std::vector<std::string> getOriginalForestLayout(const std::string& title) {
        if (title == "CORACAO DA ARVORE") return Map2ForestLayouts::getTreeHeartLayout();
        if (title == "LABIRINTO SUBTERRANEO") return Map2ForestLayouts::getMazeLayout();
        if (title == "SALA DO CHEFE") return Map2ForestLayouts::getBossRoomLayout();
        return Map2ForestLayouts::getForestLayout();
    }
}

void Map2Forest::initializeInteractions() {
    interactions['S'] = std::make_unique<SlimeInteraction>();
    interactions['F'] = std::make_unique<FairyInteraction>();
    interactions['A'] = std::make_unique<AbominationInteraction>();
    interactions['M'] = std::make_unique<MorganaInteraction>();
    interactions['B'] = std::make_unique<ChestInteraction>();
    interactions['^'] = std::make_unique<TeleportInteraction>();
}

NextMapTransition Map2Forest::startExplorationLoop()
{
    // Resgata o player se ele usou Viagem Rapida enquanto estava dentro de um submapa
    if (playerIsInsideSubMap) {
        currentMapMatrix = savedMainMapMatrix;
        playerPositionX = savedPositionXBeforeEnteringSubMap;
        playerPositionY = savedPositionYBeforeEnteringSubMap;
        playerIsInsideSubMap = false;
        currentMapTitle = "FLORESTA";
    }

    initializeInteractions();

    MapLoader::standardizeMapSize(currentMapMatrix);

    Appearance::hideCursor();

    auto formatter = [&](char cell, int x, int y) -> std::string {
        if (x == playerPositionX && y == playerPositionY) {
            char ic = Appearance::customPlayerIcon;
            if (ic <= 32 || ic > 126) ic = '@'; 
            return Appearance::color(Appearance::customPlayerColor) + std::string(1, ic) + Appearance::color(Color::RESET);
        }
        return MapControl::formatCell(cell, x, y, currentMapTitle, currentMapMatrix, false);
    };

    bool needsRender = false;
    int initialLineToDrawMap = 0;

    auto restoreScreen = [&]() {
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, {}, 0, {}, 0, Color::GREEN, currentMapMatrix, playerPositionX, playerPositionY, formatter, false, true, nullptr);
        needsRender = true;
    };

    auto animateScreen = [&]() {
        std::vector<std::string> titleArt;
        int artWidth = 0;
        std::vector<std::string> transArt;
        int transWidth = 0;

        if (currentMapTitle == "FLORESTA") {
            titleArt = Map2ForestLayouts::getForestLogo();
            artWidth = 100;
            transArt = Map2ForestLayouts::getForestTransitionArt();
            transWidth = 87;
        }

        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, titleArt, artWidth, transArt, transWidth, Color::GREEN, currentMapMatrix, playerPositionX, playerPositionY, formatter, true, true, nullptr);
        needsRender = false;
    };

    animateScreen();

    auto processInteraction = [&](int nextPositionX, int nextPositionY, int terminalWidth)
    {
        char destinationMapCell = currentMapMatrix[nextPositionY][nextPositionX];
        
        if (currentMapTitle == "SALA DO CHEFE" && (destinationMapCell == 'M' || destinationMapCell == 'A' || destinationMapCell == 'H' || destinationMapCell == 'O' || destinationMapCell == 'R' || destinationMapCell == 'G')) {
            std::vector<std::unique_ptr<Character>> bossMaho;
            auto bossMahoraga = std::make_unique<Character>("Mahoraga", std::make_unique<Mahoraga>(), std::make_unique<BaseEnemyClass>());
            bossMahoraga->calculateAttributes();
            bossMahoraga->modifyHealth(bossMahoraga->getMaxHealth());
            bossMaho.push_back(std::move(bossMahoraga));

            int startX = nextPositionX;
            while (startX > 0 && (currentMapMatrix[nextPositionY][startX-1] == 'M' || currentMapMatrix[nextPositionY][startX-1] == 'A' || currentMapMatrix[nextPositionY][startX-1] == 'H' || currentMapMatrix[nextPositionY][startX-1] == 'O' || currentMapMatrix[nextPositionY][startX-1] == 'R' || currentMapMatrix[nextPositionY][startX-1] == 'G')) startX--;

            MapControl::processCombat(currentPlayer, currentMapMatrix, playerPositionX, playerPositionY, isExplorationActive, "O GENERAL DIVINO", "A Roda comeca a girar... Mahoraga despertou!", std::move(bossMaho), nextPositionX, nextPositionY, startX, 8, terminalWidth, restoreScreen);
            return;
        }
        
        auto it = interactions.find(destinationMapCell);
        if (it != interactions.end()) {
            ForestInteractionContext ctx = {this, nextPositionX, nextPositionY, terminalWidth, restoreScreen, destinationMapCell, animateScreen};
            it->second->process(ctx);
        } else {
            bool isWall = false;
            if (currentMapTitle == "LABIRINTO SUBTERRANEO") {
                isWall = (destinationMapCell != ' ' && destinationMapCell != '^' && destinationMapCell != 'S' && destinationMapCell != 'E' && destinationMapCell != 'B');
            } else if (currentMapTitle == "SALA DO CHEFE") {
                isWall = (destinationMapCell == ' ');
            } else {
                std::string wallCharacters = "*#|_[]{}-=";
                isWall = (wallCharacters.find(destinationMapCell) != std::string::npos);
            }
            if (!isWall || Debug::isNoclipActive) {
                playerPositionX = nextPositionX;
                playerPositionY = nextPositionY;
            }
        }
    };

    NextMapTransition fastTravelDestination = MapControl::executeExplorationLoop(
        currentPlayer, currentMapMatrix, playerPositionX, playerPositionY,
        isExplorationActive, currentMapTitle,
        [this]() { return (currentMapTitle == "SALA DO CHEFE") ? "" : "SFA"; },
        [this]() { return getOriginalForestLayout(currentMapTitle); },
        processInteraction, formatter, restoreScreen,
        initialLineToDrawMap, needsRender
    );

    if (fastTravelDestination != NextMapTransition::None) {
        return fastTravelDestination;
    }

    if (currentPlayer->getHealth() <= 0 || currentPlayer->getReturnToMenu()) {
        return NextMapTransition::ReturnMenu;
    }
    return nextMap;
}
