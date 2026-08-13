#include "UI/PerspectiveManager.h"
#include "UI/Renderers/3D/EngineRaycaster/Raycaster.h"
#include "UI/Renderers/3D/EngineRaycaster/RaycasterRenderer.h"
#include "Core/Utils/RendererProvider.h"
#include "Core/Utils/Appearance.h"
#include "UI/Renderers/3D/RaycasterScreenManager.h"
#include "UI/Renderers/3D/RaycasterScreens/Diary/RaycasterDiaryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Inventory/RaycasterInventoryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Attributes/RaycasterAttributesScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Bestiary/RaycasterBestiaryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Combat/RaycasterCombatScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Defeat/RaycasterDefeatScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Victory/RaycasterVictoryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Pause/RaycasterPauseScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Map/RaycasterWorldMapScreen.h"

/*
 * ─── UI Adapters ────────────────────────────────────────────────────────────
 * Wrappers leves que implementam as interfaces abstratas da UI por delegacao
 * para os metodos estaticos concretos dos renderizadores Raycaster (ou IDE).
 * Isso mantem as dependencias concretas confinadas a raiz de composicao.
 */

class AttributesUIAdapter : public IAttributesUI {
    void display(Character* player) override { RaycasterAttributesScreen::display(player); }
    void displayDetailsAttributes(Character* currentPlayer) override { RaycasterAttributesScreen::displayDetailsAttributes(currentPlayer); }
    void managePlayerCharacterSheet(Character* currentPlayer) override { RaycasterAttributesScreen::managePlayerCharacterSheet(currentPlayer); }
};

class BestiaryUIAdapter : public IBestiaryUI {
    void display(const std::vector<Character*>& enemies) override { RaycasterBestiaryScreen::display(enemies); }
    void displayDetail(Character* enemy) override { RaycasterBestiaryScreen::displayDetail(enemy); }
};

class ScreenCombatUIAdapter : public ICombatScreenUI {
    void displayLogoForCombatScreen(const std::string& screenTitle, bool animate) override { RaycasterCombatScreen::displayLogoForCombatScreen(screenTitle, animate); }
    void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) override { RaycasterCombatScreen::animateCombatIntro(title, enemies, currentPlayer); }
    std::vector<std::string> getPlayerStatusBarLines(Character* currentPlayer, Color colorHighlight, int damageAnimation, int frameAnimation, bool isHealing) override { return RaycasterCombatScreen::getPlayerStatusBarLines(currentPlayer, colorHighlight, damageAnimation, frameAnimation, isHealing); }
    void displayEnemyHordeSideBySide(const std::vector<Character*>& enemies, Character* targetAnimation, int frameAnimation, bool isHealing, bool animateEmergence, bool isDeath, Item* weaponAttacker, int damageAnimation, const std::vector<std::string>& dropsAnimation) override { RaycasterCombatScreen::displayEnemyHordeSideBySide(enemies, targetAnimation, frameAnimation, isHealing, animateEmergence, isDeath, weaponAttacker, damageAnimation, dropsAnimation); }
    void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) override { RaycasterCombatScreen::animateDamageToEnemy(combatTitle, enemies, targetAnimation, attacker, currentPlayer, allies, damageAnimation); }
    void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) override { RaycasterCombatScreen::animateCureToEnemy(combatTitle, enemies, targetAnimation, currentPlayer, allies, healingAnimation); }
    void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) override { RaycasterCombatScreen::animateDamageToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, isParry, damageAnimation); }
    void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) override { RaycasterCombatScreen::animateCureToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, healingAnimation); }
    void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) override { RaycasterCombatScreen::animateEnemyDeath(combatTitle, enemies, enemyDead, currentPlayer, allies, drops); }
    void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance, std::function<void(std::vector<std::string>&)> callbackOverlay) override { RaycasterCombatScreen::updateScreenStatic(combatTitle, enemies, currentPlayer, allies, animateEntrance, callbackOverlay); }
    void addFixedMessage(const std::string& msg) override { RaycasterCombatScreen::addFixedMessage(msg); }
    void cleanMessagesFixed() override { RaycasterCombatScreen::cleanMessagesFixed(); }
    void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) override { RaycasterCombatScreen::configureContext3D(mode3D, matrix, postX, postY, angle, title); }
    void setShiftVisible(int shift, const std::string& name) override { RaycasterCombatScreen::setShiftVisible(shift, name); }
    void selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies) override { RaycasterCombatScreen::selectHUDAlly(currentPlayer, allies); }
    int getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override { return RaycasterCombatScreen::getPlayerAction(currentTurn, characterActing, enemies, currentPlayer, allies); }
    int getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override { return RaycasterCombatScreen::getTargetAttack(combatTitle, enemies, currentPlayer, allies); }
    int getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override { return RaycasterCombatScreen::getTargetItem(combatTitle, enemies, currentPlayer, allies); }
    int chooseShield(const std::string& characterName, const std::vector<Item*>& shields) override { return RaycasterCombatScreen::chooseShield(characterName, shields); }
    void notifyEnemiesMoreAct() override { RaycasterCombatScreen::notifyEnemiesMoreAct(); }
    void notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) override { RaycasterCombatScreen::notifyShiftExtra(dexterityPlayer, maxEnemyDexterity); }
    void notifyUnpreventionInventory() override { RaycasterCombatScreen::notifyUnpreventionInventory(); }
    void notifyWithoutShields(const std::string& characterName) override { RaycasterCombatScreen::notifyWithoutShields(characterName); }
    void notifyImbalanceDefense(const std::string& characterName) override { RaycasterCombatScreen::notifyImbalanceDefense(characterName); }
    void notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) override { RaycasterCombatScreen::notifyPostureDefensive(characterName, nameShield); }
    void notifyActionInvalidates() override { RaycasterCombatScreen::notifyActionInvalidates(); }
    void notifyCancellationItem() override { RaycasterCombatScreen::notifyCancellationItem(); }
    void notifyUnmetRequirement(const std::string& requirementMessage) override { RaycasterCombatScreen::notifyUnmetRequirement(requirementMessage); }
};

class DefeatUIAdapter : public IDefeatUI {
    void display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity, int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns) override { RaycasterDefeatScreen::display(currentPlayer, obtainedGoldQuantity, obtainedXpQuantity, totalDamageCaused, totalDamageReceived, totalHealingReceived, combatTurns); }
};

class VictoryUIAdapter : public IVictoryUI {
    void display(Character* currentPlayer, int obtainedGoldQuantity, int obtainedXpQuantity, int totalDamageCaused, int totalDamageReceived, int totalHealingReceived, int combatTurns, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique, bool canRiseLevel, const std::vector<std::string>& newDiscoveries, const std::string& titleMap) override { RaycasterVictoryScreen::display(currentPlayer, obtainedGoldQuantity, obtainedXpQuantity, totalDamageCaused, totalDamageReceived, totalHealingReceived, combatTurns, enemiesDefeated, parriesPerfect, biggerDamage, parriesTempted, parriesEffective, itemsConsumed, dropsUnique, canRiseLevel, newDiscoveries, titleMap); }
};

class PauseUIAdapter : public IPauseUI {
    int renderMenuPause() override { return RaycasterPauseScreen::renderMenuPause(); }
    int renderSettingsMenu(Character* player) override { return RaycasterPauseScreen::renderSettingsMenu(player); }
    int renderMenuAppearance(Character* player) override { return RaycasterPauseScreen::renderMenuAppearance(player); }
    int renderMenuBackground(int colorBackgroundCurrentIndex) override { return RaycasterPauseScreen::renderMenuBackground(colorBackgroundCurrentIndex); }
    int renderMenuSensitivity(int percentX, int percentY) override { return RaycasterPauseScreen::renderMenuSensitivity(percentX, percentY); }
};

class MapWorldUIAdapter : public IWorldMapUI {
    void renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete) override { RaycasterWorldMapScreen::renderPopup(art, places, selection, redesignComplete); }
};

PerspectiveManager::PerspectiveManager() : m_view3DActive(true) {
}

void PerspectiveManager::boot() {
    m_renderer3D = std::make_unique<RaycasterRenderer>();
    m_screens3D = std::make_unique<RaycasterScreenManager>();
    m_view3DActive = true;
    RendererProvider::set(m_renderer3D.get());
}

void PerspectiveManager::toggleView() {
    Appearance::displayPopup(
        "PERSPECTIVA IDE",
        {"A perspective IDE esta em construcao!",
         "",
         "Em breve voce podera explorar o jogo",
         "no estilo de um terminal de programacao.",
         "Por enquanto, apenas a visao 3D esta disponivel."},
        Color::YELLOW
    );
}

bool PerspectiveManager::is3DViewActive() const {
    return true;
}

PerspectiveRenderer* PerspectiveManager::getRendererActive() const {
    return m_renderer3D.get();
}

IManagerScreens* PerspectiveManager::getManagerScreens() const {
    return m_screens3D.get();
}

float PerspectiveManager::getSensitivityMouseX() {
    return Raycaster::sensitivityX;
}

float PerspectiveManager::getSensitivityMouseY() {
    return Raycaster::sensitivityY;
}

void PerspectiveManager::setSensitivityMouse(float x, float y) {
    Raycaster::sensitivityX = x;
    Raycaster::sensitivityY = y;
}

IDiaryUI& PerspectiveManager::getDiaryUI() {
    static RaycasterDiaryScreen diaryUI;
    return diaryUI;
}

IInventoryUI& PerspectiveManager::getInventoryUI() {
    static RaycasterInventoryScreen inventoryUI;
    return inventoryUI;
}

IAttributesUI& PerspectiveManager::getAttributesUI() {
    static AttributesUIAdapter adapter;
    return adapter;
}

IBestiaryUI& PerspectiveManager::getBestiaryUI() {
    static BestiaryUIAdapter adapter;
    return adapter;
}

ICombatScreenUI& PerspectiveManager::getScreenCombatUI() {
    static ScreenCombatUIAdapter adapter;
    return adapter;
}

IDefeatUI& PerspectiveManager::getDefeatUI() {
    static DefeatUIAdapter adapter;
    return adapter;
}

IVictoryUI& PerspectiveManager::getVictoryUI() {
    static VictoryUIAdapter adapter;
    return adapter;
}

IPauseUI& PerspectiveManager::getPauseUI() {
    static PauseUIAdapter adapter;
    return adapter;
}

IWorldMapUI& PerspectiveManager::getMapWorldUI() {
    static MapWorldUIAdapter adapter;
    return adapter;
}
