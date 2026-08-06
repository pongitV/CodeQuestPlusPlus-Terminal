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
    void display(Character* player) override { ScreenAttributesRaycaster::display(player); }
    void displayDetailsAttributes(Character* currentPlayer) override { ScreenAttributesRaycaster::displayDetailsAttributes(currentPlayer); }
    void managePlayerCharacterSheet(Character* currentPlayer) override { ScreenAttributesRaycaster::managePlayerCharacterSheet(currentPlayer); }
};

class BestiaryUIAdapter : public IBestiaryUI {
    void display(const std::vector<Character*>& enemies) override { ScreenBestiaryRaycaster::display(enemies); }
    void displayDetail(Character* enemy) override { ScreenBestiaryRaycaster::displayDetail(enemy); }
};

class ScreenCombatUIAdapter : public IScreenCombatUI {
    void displaySoonForScreenDeCombat(const std::string& titleDaScreen, bool animate) override { ScreenCombatRaycaster::displaySoonForScreenDeCombat(titleDaScreen, animate); }
    void cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) override { ScreenCombatRaycaster::cheerIntroductionCombat(title, enemies, currentPlayer); }
    std::vector<std::string> getLinesBarDeStatusDoPlayer(Character* currentPlayer, Color colorHighlight, int damageAnimation, int frameAnimation, bool isCure) override { return ScreenCombatRaycaster::getLinesBarDeStatusDoPlayer(currentPlayer, colorHighlight, damageAnimation, frameAnimation, isCure); }
    void displayHordeDeEnemiesSideASide(const std::vector<Character*>& listDeEnemies, Character* targetAnimation, int frameAnimation, bool isCure, bool cheerEmergence, bool isDeath, Item* weaponAttacker, int damageAnimation, const std::vector<std::string>& dropsAnimation) override { ScreenCombatRaycaster::displayHordeDeEnemiesSideASide(listDeEnemies, targetAnimation, frameAnimation, isCure, cheerEmergence, isDeath, weaponAttacker, damageAnimation, dropsAnimation); }
    void cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation) override { ScreenCombatRaycaster::cheerDamageNoEnemy(titleCombat, listDeEnemies, targetAnimation, attacker, currentPlayer, listDeAllies, damageAnimation); }
    void cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) override { ScreenCombatRaycaster::cheerCureNoEnemy(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation); }
    void cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool isParry, int damageAnimation) override { ScreenCombatRaycaster::cheerDamageNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, isParry, damageAnimation); }
    void cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) override { ScreenCombatRaycaster::cheerCureNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation); }
    void cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops) override { ScreenCombatRaycaster::cheerDeathEnemy(titleCombat, listDeEnemies, enemyDead, currentPlayer, listDeAllies, drops); }
    void updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance, std::function<void(std::vector<std::string>&)> callbackOverlay) override { ScreenCombatRaycaster::updateScreenStatic(titleCombat, listDeEnemies, currentPlayer, listDeAllies, animateEntrance, callbackOverlay); }
    void addFixedMessage(const std::string& msg) override { ScreenCombatRaycaster::addFixedMessage(msg); }
    void cleanMessagesFixed() override { ScreenCombatRaycaster::cleanMessagesFixed(); }
    void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) override { ScreenCombatRaycaster::configureContext3D(mode3D, matrix, postX, postY, angle, title); }
    void setShiftVisible(int shift, const std::string& name) override { ScreenCombatRaycaster::setShiftVisible(shift, name); }
    void selectHUDDeAlly(Character* currentPlayer, const std::vector<Character*>& allies) override { ScreenCombatRaycaster::selectHUDDeAlly(currentPlayer, allies); }
    int getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override { return ScreenCombatRaycaster::getActionDoPlayer(shiftCurrent, characterActing, enemies, currentPlayer, allies); }
    int getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override { return ScreenCombatRaycaster::getTargetAttack(titleCombat, enemies, currentPlayer, allies); }
    int getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override { return ScreenCombatRaycaster::getTargetItem(titleCombat, enemies, currentPlayer, allies); }
    int getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields) override { return ScreenCombatRaycaster::getChooseDeShield(nameCharacter, listDeShields); }
    void notifyEnemiesMoreAct() override { ScreenCombatRaycaster::notifyEnemiesMoreAct(); }
    void notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies) override { ScreenCombatRaycaster::notifyShiftExtra(dexterityPlayer, maxDexterityEnemies); }
    void notifyUnpreventionInventory() override { ScreenCombatRaycaster::notifyUnpreventionInventory(); }
    void notifyWithoutShields(const std::string& nameCharacter) override { ScreenCombatRaycaster::notifyWithoutShields(nameCharacter); }
    void notifyImbalanceDefense(const std::string& nameCharacter) override { ScreenCombatRaycaster::notifyImbalanceDefense(nameCharacter); }
    void notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield) override { ScreenCombatRaycaster::notifyPostureDefensive(nameCharacter, nameShield); }
    void notifyActionInvalidates() override { ScreenCombatRaycaster::notifyActionInvalidates(); }
    void notifyCancellationItem() override { ScreenCombatRaycaster::notifyCancellationItem(); }
    void notifyRequirementNoServed(const std::string& messageRequirement) override { ScreenCombatRaycaster::notifyRequirementNoServed(messageRequirement); }
};

class DefeatUIAdapter : public IDefeatUI {
    void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat) override { ScreenDefeatRaycaster::display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat); }
};

class VictoryUIAdapter : public IVictoryUI {
    void display(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::pair<std::string, int>>& dropsUnique, bool canRiseLevel, const std::vector<std::string>& newDiscoveries, const std::string& titleMap) override { ScreenVictoryRaycaster::display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat, enemiesDefeated, parriesPerfect, biggerDamage, parriesTempted, parriesEffective, itemsConsumed, dropsUnique, canRiseLevel, newDiscoveries, titleMap); }
};

class PauseUIAdapter : public IPauseUI {
    int renderMenuPause() override { return ScreenPauseRaycaster::renderMenuPause(); }
    int renderMenuConfiguracoes(Character* player) override { return ScreenPauseRaycaster::renderMenuConfiguracoes(player); }
    int renderMenuAppearance(Character* player) override { return ScreenPauseRaycaster::renderMenuAppearance(player); }
    int renderMenuBackground(int colorBackgroundCurrentIndex) override { return ScreenPauseRaycaster::renderMenuBackground(colorBackgroundCurrentIndex); }
    int renderMenuSensitivity(int percX, int percY) override { return ScreenPauseRaycaster::renderMenuSensitivity(percX, percY); }
};

class MapWorldUIAdapter : public IMapWorldUI {
    void renderPopup(const std::vector<std::string>& art, const std::vector<std::string>& places, int selection, bool redesignComplete) override { ScreenMapWorldRaycaster::renderPopup(art, places, selection, redesignComplete); }
};

PerspectiveManager::PerspectiveManager() : m_visa3DActive(true) {
}

void PerspectiveManager::boot() {
    m_renderer3D = std::make_unique<RaycasterRenderer>();
    m_screens3D = std::make_unique<ManagerScreensRaycaster>();
    m_visa3DActive = true;
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
    static ScreenDiaryRaycaster diaryUI;
    return diaryUI;
}

IInventoryUI& PerspectiveManager::getInventoryUI() {
    static ScreenInventoryRaycaster inventoryUI;
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

IScreenCombatUI& PerspectiveManager::getScreenCombatUI() {
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

IMapWorldUI& PerspectiveManager::getMapWorldUI() {
    static MapWorldUIAdapter adapter;
    return adapter;
}
