#include "Systems/Combat/CombatRaycasterUIImpl.h"

#include "UI/Renderers/3D/RaycasterScreens/Combat/RaycasterCombatScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Victory/RaycasterVictoryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Defeat/RaycasterDefeatScreen.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

void CombatRaycasterUIImpl::configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    ScreenCombatRaycaster::configureContext3D(mode3D, matrix, postX, postY, angle, title);
}

void CombatRaycasterUIImpl::cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    ScreenCombatRaycaster::cheerIntroductionCombat(title, enemies, currentPlayer);
}

void CombatRaycasterUIImpl::updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance) {
    ScreenCombatRaycaster::updateScreenStatic(titleCombat, listDeEnemies, currentPlayer, listDeAllies, animateEntrance, nullptr);
}

void CombatRaycasterUIImpl::cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation) {
    ScreenCombatRaycaster::cheerDamageNoEnemy(titleCombat, listDeEnemies, targetAnimation, attacker, currentPlayer, listDeAllies, damageAnimation);
}

void CombatRaycasterUIImpl::cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) {
    ScreenCombatRaycaster::cheerCureNoEnemy(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation);
}

void CombatRaycasterUIImpl::cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool isParry, int damageAnimation) {
    ScreenCombatRaycaster::cheerDamageNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, isParry, damageAnimation);
}

void CombatRaycasterUIImpl::cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) {
    ScreenCombatRaycaster::cheerCureNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation);
}

void CombatRaycasterUIImpl::cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops) {
    ScreenCombatRaycaster::cheerDeathEnemy(titleCombat, listDeEnemies, enemyDead, currentPlayer, listDeAllies, drops);
}

void CombatRaycasterUIImpl::cleanContextCharacterHUD() {
    CombatScreen::context.characterHUD = nullptr;
}

void CombatRaycasterUIImpl::cleanContextEnemyDeadEDrops() {
    CombatScreen::context.enemyDeadWithDrops = nullptr;
    CombatScreen::context.dropsAssets.clear();
}

std::string CombatRaycasterUIImpl::combatMargin() {
    return ""; // TelaCombateRaycaster usually uses its own formatting or empty margin for fixed messages in 3D
}

void CombatRaycasterUIImpl::addFixedMessage(const std::string& msg) {
    ScreenCombatRaycaster::addFixedMessage(msg);
}

void CombatRaycasterUIImpl::cleanMessagesFixed() {
    ScreenCombatRaycaster::cleanMessagesFixed();
}

void CombatRaycasterUIImpl::setShiftVisible(int shift, const std::string& name) {
    ScreenCombatRaycaster::setShiftVisible(shift, name);
}

int CombatRaycasterUIImpl::getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return ScreenCombatRaycaster::getActionDoPlayer(shiftCurrent, characterActing, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return ScreenCombatRaycaster::getTargetAttack(titleCombat, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return ScreenCombatRaycaster::getTargetItem(titleCombat, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields) {
    return ScreenCombatRaycaster::getChooseDeShield(nameCharacter, listDeShields);
}

void CombatRaycasterUIImpl::notifyEnemiesMoreAct() {
    ScreenCombatRaycaster::notifyEnemiesMoreAct();
}

void CombatRaycasterUIImpl::notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies) {
    ScreenCombatRaycaster::notifyShiftExtra(dexterityPlayer, maxDexterityEnemies);
}

void CombatRaycasterUIImpl::notifyUnpreventionInventory() {
    ScreenCombatRaycaster::notifyUnpreventionInventory();
}

void CombatRaycasterUIImpl::notifyWithoutShields(const std::string& nameCharacter) {
    ScreenCombatRaycaster::notifyWithoutShields(nameCharacter);
}

void CombatRaycasterUIImpl::notifyImbalanceDefense(const std::string& nameCharacter) {
    ScreenCombatRaycaster::notifyImbalanceDefense(nameCharacter);
}

void CombatRaycasterUIImpl::notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield) {
    ScreenCombatRaycaster::notifyPostureDefensive(nameCharacter, nameShield);
}

void CombatRaycasterUIImpl::notifyActionInvalidates() {
    ScreenCombatRaycaster::notifyActionInvalidates();
}

void CombatRaycasterUIImpl::notifyCancellationItem() {
    ScreenCombatRaycaster::notifyCancellationItem();
}

void CombatRaycasterUIImpl::notifyRequirementNoServed(const std::string& messageRequirement) {
    ScreenCombatRaycaster::notifyRequirementNoServed(messageRequirement);
}

void CombatRaycasterUIImpl::displayScreenVictory(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries) {
    std::unordered_map<std::string, int> frequencyDrops;
    for (const auto& item : obtainedItems) {
        frequencyDrops[item]++;
    }
    std::vector<std::pair<std::string, int>> dropsUnique;
    for (const auto& pair : frequencyDrops) {
        dropsUnique.push_back(pair);
    }

    bool canRiseLevel = currentPlayer->getCurrentXp() + quantityDeXpObtained >= currentPlayer->getXpForRise();

    ScreenVictoryRaycaster::display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat, enemiesDefeated, parriesPerfect, biggerDamage, parriesTempted, parriesEffective, itemsConsumed, dropsUnique, canRiseLevel, newDiscoveries, "");
}

void CombatRaycasterUIImpl::displayScreenDefeat(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat) {
    ScreenDefeatRaycaster::display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat);
}

void CombatRaycasterUIImpl::displayScreenAttributes(Character* character) {
    Appearance::clearScreen();
    AttributesScreen::managePlayerCharacterSheet(character);
    Appearance::clearScreen();
}

void CombatRaycasterUIImpl::displayScreenDiary(Character* character) {
    Appearance::clearScreen();
    DiaryScreen::display(character);
    Appearance::clearScreen();
}

void CombatRaycasterUIImpl::clearScreen() {
    Appearance::clearScreen();
}
