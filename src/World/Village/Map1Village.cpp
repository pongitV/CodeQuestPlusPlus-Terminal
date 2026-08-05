#include "World/Village/Map1Village.h"
#include "Core/Engine/Debug.h"

#include <iostream>
#include <vector>
#include <memory>
#include <utility>
#include <chrono>
#include <thread>

#include "World/Forest/Map2Forest.h"
#include "UI/Screens/Menu/MenuScreen.h"
#include "Core/Engine/EnemyCreator.h"
#include "Systems/Combat/Combat.h"
#include "Domain/Items/Item.h"
#include "Systems/Inventory/CombatInventory.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Bestiary/BestiaryScreen.h"
#include "Domain/NPCs/NPCBlacksmith.h"
#include "Domain/NPCs/NPCMerchant.h"
#include "Domain/Enemies/ExiledOrc.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "World/Systems/MapAnimator.h"
#include "World/MapControl.h"
#include "World/Systems/MapLoader.h"
#include "Core/Utils/RandomGenerator.h"
#include "World/InteractionMap.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Systems/Progression/Diary.h"
#include "World/Village/Map1VillageLayout.h"

Map1Village::Map1Village(Character* playerCharacter) :
    playerPositionX(4), 
    playerPositionY(5), 
    currentPlayer(playerCharacter), 
    isExplorationActive(true),
    currentMapTitle("CAMINHO DO INICIO"),
    savedPositionXBeforeEnteringSubMap(10), 
    savedPositionYBeforeEnteringSubMap(4),
    playerIsInsideSubMap(true),
    bjornRescued(Progression::instance().getFlag(Flags::Village_KissRescued)), 
    caveAlreadyVisited(false),
    spawnAlreadyVisited(true),
    nextMap(NextMapTransition::None),
    cameFromForest(false)
{
    savedMainMapMatrix = Map1VillageLayouts::getInitialVillageLayout();
    MapLoader::standardizeMapSize(savedMainMapMatrix);

    currentMapMatrix = Map1VillageLayouts::getSpawnLayout();
    MapLoader::standardizeMapSize(currentMapMatrix);

    villageBaseMap = Map1VillageLayouts::getInitialVillageLayout();
    MapLoader::standardizeMapSize(villageBaseMap);

    if (!bjornRescued) {
        for (auto& line : savedMainMapMatrix) {
            std::replace(line.begin(), line.end(), 'B', 'P');
        }
        for (auto& line : villageBaseMap) {
            std::replace(line.begin(), line.end(), 'B', 'P');
        }
    }
}

Map1Village::~Map1Village() = default;

namespace {
    class GoblinCombatInteraction : public VillageInteraction {
    public:
        void process(VillageInteractionContext& ctx) override {
            MapControl::processCombat(ctx.self->currentPlayer, ctx.self->currentMapMatrix, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->isExplorationActive, "ENCONTRO INESPERADO", "Voce encontrou uma horda de Goblins!", EnemyCreator::createGoblinEnemy(RandomGenerator::getInteger(1, 3)), ctx.nextPositionX, ctx.nextPositionY, ctx.nextPositionX, 1, ctx.terminalWidth, ctx.restoreScreen);
        }
    };

    class OrcCombatInteraction : public VillageInteraction {
    public:
        void process(VillageInteractionContext& ctx) override {
            MapControl::processCombat(ctx.self->currentPlayer, ctx.self->currentMapMatrix, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->isExplorationActive, "ENCONTRO NA CAVERNA", "Voce encontrou um Ork!", EnemyCreator::createExiledOrcEnemy(1), ctx.nextPositionX, ctx.nextPositionY, ctx.nextPositionX, 1, ctx.terminalWidth, ctx.restoreScreen);
        }
    };

    class BlacksmithNPCInteraction : public VillageInteraction {
    public:
        void process(VillageInteractionContext& ctx) override {
            if (ctx.self->currentMapTitle == "CAVERNA DO ORK") {
                Appearance::startPopupInteraction();
                std::vector<std::string> bjornLines = {
                    Appearance::color(Color::CYAN) + "Bjorn:" + Appearance::color(Color::RESET) + " Pelos deuses, muito obrigado por me salvar!",
                    Appearance::color(Color::CYAN) + "Bjorn:" + Appearance::color(Color::RESET) + " Passe na Forja e eu ajudarei voce!"
                };
                Appearance::displayPopup("RESGATE NA CAVERNA", bjornLines, Color::YELLOW);
                
                ctx.self->bjornRescued = true;
                Progression::instance().setFlag(Flags::Village_KissRescued, true);

                ctx.self->currentMapMatrix[ctx.nextPositionY][ctx.nextPositionX] = '.';
                
                // Atualiza os mapas salvos para que a Placa volte a ser o Bjorn na Vila
                for (auto& line : ctx.self->savedMainMapMatrix) {
                    std::replace(line.begin(), line.end(), 'P', 'B');
                }
                for (auto& line : ctx.self->villageBaseMap) {
                    std::replace(line.begin(), line.end(), 'P', 'B');
                }
            } else if (ctx.self->currentMapTitle == "VILA INICIAL") {
                NPCBlacksmith bjornInteraction;
                bjornInteraction.interact(ctx.self->currentPlayer);
                Diary::instance().registerNPC("Bjorn (Ferreiro)");
            } else {
                ctx.self->playerPositionX = ctx.nextPositionX;
                ctx.self->playerPositionY = ctx.nextPositionY;
            }
            if (ctx.self->isExplorationActive && !MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
        }
    };

    class MerchantNPCInteraction : public VillageInteraction {
    public:
        void process(VillageInteractionContext& ctx) override {
            NPCMerchant franchescoInteraction;
            franchescoInteraction.interact(ctx.self->currentPlayer);
            Diary::instance().registerNPC("Franchesco (Mercador)");
            if (ctx.self->isExplorationActive && !MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
        }
    };

    class SignInteraction : public VillageInteraction {
    public:
        void process(VillageInteractionContext& ctx) override {
            Appearance::startPopupInteraction();
            std::vector<std::string> msg = {
                "A Forja esta fechada.",
                "Uma placa diz: 'Fui a caverna a leste'."
            };
            Appearance::displayPopup("PLACA", msg, Color::BROWN);
            if (ctx.self->isExplorationActive && !MapControl::is3DExplorationActive()) if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
        }
    };

    class TeleportInteraction : public VillageInteraction {
    public:
        void process(VillageInteractionContext& ctx) override {
            
            char nextCell = ' ';
            if (ctx.nextPositionX + 1 < static_cast<int>(ctx.self->currentMapMatrix[ctx.nextPositionY].length())) {
                nextCell = ctx.self->currentMapMatrix[ctx.nextPositionY][ctx.nextPositionX+1];
            }
            int px = ctx.nextPositionX;
            int py = ctx.nextPositionY;
            
            // 1. Entrar no Caminho do Inicio (Spawn) a partir da Vila Inicial (X=18, Y=5 ou Y=4)
            if (px >= 17 && px <= 19 && py <= 6 && !ctx.self->playerIsInsideSubMap) {
                MapLoader::enterSubMap(ctx.self->currentMapMatrix, ctx.self->savedMainMapMatrix, ctx.self->savedPositionXBeforeEnteringSubMap, ctx.self->savedPositionYBeforeEnteringSubMap, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->playerIsInsideSubMap, ctx.self->currentMapTitle, ctx.self->savedSpawnMapMatrix, ctx.self->spawnAlreadyVisited, Map1VillageLayouts::getSpawnLayout(), 53, 7, "CAMINHO DO INICIO", ctx.animateScreen);
            }
            // 2. Entrar na Caverna a partir da Vila (Caverna fica a leste)
            else if (px > 50 && py < 30 && !ctx.self->playerIsInsideSubMap) {
                MapLoader::enterSubMap(ctx.self->currentMapMatrix, ctx.self->savedMainMapMatrix, ctx.self->savedPositionXBeforeEnteringSubMap, ctx.self->savedPositionYBeforeEnteringSubMap, ctx.self->playerPositionX, ctx.self->playerPositionY, ctx.self->playerIsInsideSubMap, ctx.self->currentMapTitle, ctx.self->savedCaveMapMatrix, ctx.self->caveAlreadyVisited, Map1VillageLayouts::getBasementLayout(ctx.self->bjornRescued), 14, 3, "CAVERNA DO ORK", ctx.animateScreen);
            }
            // 3. Retornar dos Interiores/Caverna de volta para a Vila Inicial
            else if (nextCell == 'S' && ctx.self->playerIsInsideSubMap) {
                if (ctx.self->currentMapTitle == "CAVERNA DO ORK") {
                    ctx.self->caveAlreadyVisited = false;
                }

                ctx.self->currentMapMatrix = ctx.self->savedMainMapMatrix;
                MapLoader::standardizeMapSize(ctx.self->currentMapMatrix);
                ctx.self->playerPositionX = ctx.self->savedPositionXBeforeEnteringSubMap;
                ctx.self->playerPositionY = ctx.self->savedPositionYBeforeEnteringSubMap;
                ctx.self->playerIsInsideSubMap = false;
                ctx.self->currentMapTitle = "VILA INICIAL";
                if (!MapControl::is3DExplorationActive()) ctx.restoreScreen();
            }
            // 3. Voltar para a Vila Inicial a partir do Caminho do Inicio (X=54, Y=7 ou Y=6)
            else if (px == 54 && (py == 7 || py == 6) && ctx.self->currentMapTitle == "CAMINHO DO INICIO") {
                ctx.self->savedSpawnMapMatrix = ctx.self->currentMapMatrix;
                ctx.self->currentMapMatrix = ctx.self->savedMainMapMatrix;
                MapLoader::standardizeMapSize(ctx.self->currentMapMatrix);
                ctx.self->playerPositionX = 17;
                ctx.self->playerPositionY = 5;
                ctx.self->playerIsInsideSubMap = false;
                ctx.self->currentMapTitle = "VILA INICIAL";
                if (!MapControl::is3DExplorationActive()) ctx.animateScreen();
            }
            // 5. Ir para a Floresta a partir da Vila
            else if (py >= 30 && !ctx.self->playerIsInsideSubMap) {
                if (!Progression::instance().getFlag(Flags::Village_KissRescued)) {
                    Appearance::startPopupInteraction();
                    std::vector<std::string> msg = {
                        "Voce precisa ajudar os habitantes da vila antes de seguir jornada.",
                        "(Dica: Explore a caverna a leste da vila)."
                    };
                    Appearance::displayPopup("CAMINHO BLOQUEADO", msg, Color::YELLOW);
                    return;
                }
                ctx.self->isExplorationActive = false;
                ctx.self->nextMap = NextMapTransition::Forest;
                ctx.self->cameFromForest = true;
            }
            else {
                ctx.self->playerPositionX = ctx.nextPositionX;
                ctx.self->playerPositionY = ctx.nextPositionY;
            }
        }
    };

    std::vector<std::string> getOriginalVillageLayout(const std::string& title, bool bjornRescued) {
        if (title == "CAVERNA DO ORK") return Map1VillageLayouts::getBasementLayout(bjornRescued);
        if (title == "CAMINHO DO INICIO") return Map1VillageLayouts::getSpawnLayout();
        auto layout = Map1VillageLayouts::getInitialVillageLayout();
        if (!bjornRescued) {
            for (auto& line : layout) {
                std::replace(line.begin(), line.end(), 'B', 'P');
            }
        }
        return layout;
    }
}

void Map1Village::initializeInteractions() {
    interactions['G'] = std::make_unique<GoblinCombatInteraction>();
    interactions['O'] = std::make_unique<OrcCombatInteraction>();
    interactions['B'] = std::make_unique<BlacksmithNPCInteraction>();
    interactions['F'] = std::make_unique<MerchantNPCInteraction>();
    interactions['P'] = std::make_unique<SignInteraction>();
    interactions['^'] = std::make_unique<TeleportInteraction>();
    interactions['S'] = std::make_unique<TeleportInteraction>();
}

NextMapTransition Map1Village::startExplorationLoop()
{
    initializeInteractions();

    if (cameFromForest) {
        currentMapMatrix = villageBaseMap;
        caveAlreadyVisited = false;
        cameFromForest = false;
    }

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
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, {}, 0, {}, 0, Color::YELLOW, currentMapMatrix, playerPositionX, playerPositionY, formatter, false, true, nullptr);
        needsRender = true;
    };

    auto animateScreen = [&]() {
        std::vector<std::string> titleArt;
        int artWidth = 0;
        std::vector<std::string> transArt;
        int transWidth = 0;
        std::function<void()> narrationAction = nullptr;
        bool useBannerAnimation = true;

        if (currentMapTitle == "VILA INICIAL") {
            titleArt = Map1VillageLayouts::getVillageSoon();
            artWidth = 125;
            transArt = Map1VillageLayouts::getVillageTransitionArt();
            transWidth = 75;
        } else if (currentMapTitle == "CAMINHO DO INICIO") {
            titleArt = Map1VillageLayouts::getSpawnSoon();
            artWidth = 105;
            if (savedSpawnMapMatrix.empty()) {
                useBannerAnimation = false;
            }
        }
        
        initialLineToDrawMap = MapAnimator::animateMapIntroduction(currentMapTitle, titleArt, artWidth, transArt, transWidth, Color::YELLOW, currentMapMatrix, playerPositionX, playerPositionY, formatter, true, useBannerAnimation, narrationAction);
        needsRender = false;
    };

    // Substitui o Bjorn por uma placa se ele ainda nao foi resgatado (Antes de animar a tela)
    if (currentMapTitle == "VILA INICIAL" && !bjornRescued) {
        for (auto& line : currentMapMatrix) {
            std::replace(line.begin(), line.end(), 'B', 'P');
        }
    }

    animateScreen();

    if (villageBaseMap.empty()) villageBaseMap = savedMainMapMatrix;

    auto processInteraction = [&](int nextPositionX, int nextPositionY, int terminalWidth) {
        char destinationMapCell = currentMapMatrix[nextPositionY][nextPositionX];
        
        auto it = interactions.find(destinationMapCell);
        bool isFalseF = (destinationMapCell == 'F' && nextPositionX > 0 && currentMapMatrix[nextPositionY][nextPositionX - 1] != '{');

        if (it != interactions.end() && !isFalseF) {
            VillageInteractionContext ctx = {this, nextPositionX, nextPositionY, terminalWidth, restoreScreen, destinationMapCell, animateScreen};
            it->second->process(ctx);
        } else {
            std::string collisions = "*#|_[]{}-=";
            bool isWall = (collisions.find(destinationMapCell) != std::string::npos);
            if (!isWall || Debug::isNoclipActive) {
                playerPositionX = nextPositionX;
                playerPositionY = nextPositionY;
            }
        }
    };

    NextMapTransition fastTravelDestination = MapControl::executeExplorationLoop(
        currentPlayer, currentMapMatrix, playerPositionX, playerPositionY,
        isExplorationActive, currentMapTitle,
        [this]() { return "GO"; },
        [this]() { return getOriginalVillageLayout(currentMapTitle, bjornRescued); },
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
