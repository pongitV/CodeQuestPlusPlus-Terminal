#include "Systems/Combat/CombatUIImpl.h"

#include "UI/Screens/Combat/CombatScreen.h"
#include "UI/Screens/Victory/VictoryScreen.h"
#include "UI/Screens/Defeat/DefeatScreen.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"
#include "Core/Utils/Appearance.h"

void CombatUIImpl::configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    CombatScreen::configureContext3D(mode3D, matrix, postX, postY, angle, title);
}

void CombatUIImpl::animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    CombatScreen::animateCombatIntro(title, enemies, currentPlayer);
}

void CombatUIImpl::updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance) {
    CombatScreen::updateScreenStatic(combatTitle, enemies, currentPlayer, allies, animateEntrance);
}

void CombatUIImpl::animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) {
    CombatScreen::animateDamageToEnemy(combatTitle, enemies, targetAnimation, attacker, currentPlayer, allies, damageAnimation);
}

void CombatUIImpl::animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) {
    CombatScreen::animateCureToEnemy(combatTitle, enemies, targetAnimation, currentPlayer, allies, healingAnimation);
}

void CombatUIImpl::animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) {
    CombatScreen::animateDamageToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, isParry, damageAnimation);
}

void CombatUIImpl::animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) {
    CombatScreen::animateCureToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, healingAnimation);
}

void CombatUIImpl::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) {
    CombatScreen::animateEnemyDeath(combatTitle, enemies, enemyDead, currentPlayer, allies, drops);
}

void CombatUIImpl::cleanContextCharacterHUD() {
    CombatScreen::context.characterHUD = nullptr;
}

void CombatUIImpl::cleanContextEnemyDeathAndDrops() {
    CombatScreen::context.enemyDeadWithDrops = nullptr;
    CombatScreen::context.dropsAssets.clear();
}

std::string CombatUIImpl::combatMargin() {
    return CombatScreen::combatMargin();
}

void CombatUIImpl::addFixedMessage(const std::string& msg) {
    CombatScreen::addFixedMessage(msg);
}

void CombatUIImpl::cleanMessagesFixed() {
    CombatScreen::cleanMessagesFixed();
}

void CombatUIImpl::setShiftVisible(int shift, const std::string& name) {
    CombatScreen::setShiftVisible(shift, name);
}

int CombatUIImpl::getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return CombatScreen::getPlayerAction(currentTurn, characterActing, enemies, currentPlayer, allies);
}

int CombatUIImpl::getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return CombatScreen::getTargetAttack(combatTitle, enemies, currentPlayer, allies);
}

int CombatUIImpl::getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return CombatScreen::getTargetItem(combatTitle, enemies, currentPlayer, allies);
}

int CombatUIImpl::chooseShield(const std::string& characterName, const std::vector<Item*>& shields) {
    return CombatScreen::chooseShield(characterName, shields);
}

void CombatUIImpl::notifyEnemiesMoreAct() {
    CombatScreen::notifyEnemiesMoreAct();
}

void CombatUIImpl::notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) {
    CombatScreen::notifyShiftExtra(dexterityPlayer, maxEnemyDexterity);
}

void CombatUIImpl::notifyUnpreventionInventory() {
    CombatScreen::notifyUnpreventionInventory();
}

void CombatUIImpl::notifyWithoutShields(const std::string& characterName) {
    CombatScreen::notifyWithoutShields(characterName);
}

void CombatUIImpl::notifyImbalanceDefense(const std::string& characterName) {
    CombatScreen::notifyImbalanceDefense(characterName);
}

void CombatUIImpl::notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) {
    CombatScreen::notifyPostureDefensive(characterName, nameShield);
}

void CombatUIImpl::notifyActionInvalidates() {
    CombatScreen::notifyActionInvalidates();
}

void CombatUIImpl::notifyCancellationItem() {
    CombatScreen::notifyCancellationItem();
}

void CombatUIImpl::notifyUnmetRequirement(const std::string& requirementMessage) {
    CombatScreen::notifyUnmetRequirement(requirementMessage);
}

void CombatUIImpl::displayVictoryScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int perfectParries, int highestDamage, int parriesAttempted, int effectiveParries, int itemsConsumed, const std::vector<std::string>& newDiscoveries) {
    VictoryScreen::display(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, totalCureReceived, combatTurns, obtainedItems, enemiesDefeated, perfectParries, highestDamage, parriesAttempted, effectiveParries, itemsConsumed, newDiscoveries);
}

void CombatUIImpl::displayDefeatScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns) {
    DefeatScreen::display(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, totalCureReceived, combatTurns);
}

void CombatUIImpl::displayScreenAttributes(Character* character) {
    AttributesScreen::managePlayerCharacterSheet(character);
}

void CombatUIImpl::displayScreenDiary(Character* character) {
    DiaryScreen::display(character);
}

void CombatUIImpl::clearScreen() {
    Appearance::clearScreen();
}
