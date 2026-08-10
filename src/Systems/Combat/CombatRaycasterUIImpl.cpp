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

void CombatRaycasterUIImpl::animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    ScreenCombatRaycaster::animateCombatIntro(title, enemies, currentPlayer);
}

void CombatRaycasterUIImpl::updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance) {
    ScreenCombatRaycaster::updateScreenStatic(combatTitle, enemies, currentPlayer, allies, animateEntrance, nullptr);
}

void CombatRaycasterUIImpl::animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) {
    ScreenCombatRaycaster::animateDamageToEnemy(combatTitle, enemies, targetAnimation, attacker, currentPlayer, allies, damageAnimation);
}

void CombatRaycasterUIImpl::animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) {
    ScreenCombatRaycaster::animateCureToEnemy(combatTitle, enemies, targetAnimation, currentPlayer, allies, cureAnimation);
}

void CombatRaycasterUIImpl::animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) {
    ScreenCombatRaycaster::animateDamageToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, isParry, damageAnimation);
}

void CombatRaycasterUIImpl::animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) {
    ScreenCombatRaycaster::animateCureToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, cureAnimation);
}

void CombatRaycasterUIImpl::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) {
    ScreenCombatRaycaster::animateEnemyDeath(combatTitle, enemies, enemyDead, currentPlayer, allies, drops);
}

void CombatRaycasterUIImpl::cleanContextCharacterHUD() {
    CombatScreen::context.characterHUD = nullptr;
}

void CombatRaycasterUIImpl::cleanContextEnemyDeathAndDrops() {
    CombatScreen::context.enemyDeadWithDrops = nullptr;
    CombatScreen::context.dropsAssets.clear();
}

std::string CombatRaycasterUIImpl::combatMargin() {
    return ""; // TelaCombateRaycaster usualmente utiliza sua propria formatacao ou margem vazia para mensagens fixas em 3D
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

int CombatRaycasterUIImpl::getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return ScreenCombatRaycaster::getPlayerAction(currentTurn, characterActing, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return ScreenCombatRaycaster::getTargetAttack(combatTitle, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return ScreenCombatRaycaster::getTargetItem(combatTitle, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::chooseShield(const std::string& characterName, const std::vector<Item*>& shields) {
    return ScreenCombatRaycaster::chooseShield(characterName, shields);
}

void CombatRaycasterUIImpl::notifyEnemiesMoreAct() {
    ScreenCombatRaycaster::notifyEnemiesMoreAct();
}

void CombatRaycasterUIImpl::notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) {
    ScreenCombatRaycaster::notifyShiftExtra(dexterityPlayer, maxEnemyDexterity);
}

void CombatRaycasterUIImpl::notifyUnpreventionInventory() {
    ScreenCombatRaycaster::notifyUnpreventionInventory();
}

void CombatRaycasterUIImpl::notifyWithoutShields(const std::string& characterName) {
    ScreenCombatRaycaster::notifyWithoutShields(characterName);
}

void CombatRaycasterUIImpl::notifyImbalanceDefense(const std::string& characterName) {
    ScreenCombatRaycaster::notifyImbalanceDefense(characterName);
}

void CombatRaycasterUIImpl::notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) {
    ScreenCombatRaycaster::notifyPostureDefensive(characterName, nameShield);
}

void CombatRaycasterUIImpl::notifyActionInvalidates() {
    ScreenCombatRaycaster::notifyActionInvalidates();
}

void CombatRaycasterUIImpl::notifyCancellationItem() {
    ScreenCombatRaycaster::notifyCancellationItem();
}

void CombatRaycasterUIImpl::notifyUnmetRequirement(const std::string& requirementMessage) {
    ScreenCombatRaycaster::notifyUnmetRequirement(requirementMessage);
}

void CombatRaycasterUIImpl::displayVictoryScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int perfectParries, int highestDamage, int parriesAttempted, int effectiveParries, int itemsConsumed, const std::vector<std::string>& newDiscoveries) {
    std::unordered_map<std::string, int> frequencyDrops;
    for (const auto& item : obtainedItems) {
        frequencyDrops[item]++;
    }
    std::vector<std::pair<std::string, int>> dropsUnique;
    for (const auto& pair : frequencyDrops) {
        dropsUnique.push_back(pair);
    }

    bool canLevelUp = currentPlayer->getCurrentXp() + xpObtained >= currentPlayer->getXpForRise();

    ScreenVictoryRaycaster::display(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, totalCureReceived, combatTurns, enemiesDefeated, perfectParries, highestDamage, parriesAttempted, effectiveParries, itemsConsumed, dropsUnique, canLevelUp, newDiscoveries, "");
}

void CombatRaycasterUIImpl::displayDefeatScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns) {
    ScreenDefeatRaycaster::display(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, totalCureReceived, combatTurns);
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
