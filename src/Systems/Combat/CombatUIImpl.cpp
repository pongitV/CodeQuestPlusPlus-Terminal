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

void CombatUIImpl::cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    CombatScreen::cheerIntroductionCombat(title, enemies, currentPlayer);
}

void CombatUIImpl::updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance) {
    CombatScreen::updateScreenStatic(titleCombat, listDeEnemies, currentPlayer, listDeAllies, animateEntrance);
}

void CombatUIImpl::cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation) {
    CombatScreen::cheerDamageNoEnemy(titleCombat, listDeEnemies, targetAnimation, attacker, currentPlayer, listDeAllies, damageAnimation);
}

void CombatUIImpl::cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) {
    CombatScreen::cheerCureNoEnemy(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation);
}

void CombatUIImpl::cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool isParry, int damageAnimation) {
    CombatScreen::cheerDamageNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, isParry, damageAnimation);
}

void CombatUIImpl::cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) {
    CombatScreen::cheerCureNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation);
}

void CombatUIImpl::cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops) {
    CombatScreen::cheerDeathEnemy(titleCombat, listDeEnemies, enemyDead, currentPlayer, listDeAllies, drops);
}

void CombatUIImpl::cleanContextCharacterHUD() {
    CombatScreen::context.characterHUD = nullptr;
}

void CombatUIImpl::cleanContextEnemyDeadEDrops() {
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

int CombatUIImpl::getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return CombatScreen::getActionDoPlayer(shiftCurrent, characterActing, enemies, currentPlayer, allies);
}

int CombatUIImpl::getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return CombatScreen::getTargetAttack(titleCombat, enemies, currentPlayer, allies);
}

int CombatUIImpl::getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return CombatScreen::getTargetItem(titleCombat, enemies, currentPlayer, allies);
}

int CombatUIImpl::getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields) {
    return CombatScreen::getChooseDeShield(nameCharacter, listDeShields);
}

void CombatUIImpl::notifyEnemiesMoreAct() {
    CombatScreen::notifyEnemiesMoreAct();
}

void CombatUIImpl::notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies) {
    CombatScreen::notifyShiftExtra(dexterityPlayer, maxDexterityEnemies);
}

void CombatUIImpl::notifyUnpreventionInventory() {
    CombatScreen::notifyUnpreventionInventory();
}

void CombatUIImpl::notifyWithoutShields(const std::string& nameCharacter) {
    CombatScreen::notifyWithoutShields(nameCharacter);
}

void CombatUIImpl::notifyImbalanceDefense(const std::string& nameCharacter) {
    CombatScreen::notifyImbalanceDefense(nameCharacter);
}

void CombatUIImpl::notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield) {
    CombatScreen::notifyPostureDefensive(nameCharacter, nameShield);
}

void CombatUIImpl::notifyActionInvalidates() {
    CombatScreen::notifyActionInvalidates();
}

void CombatUIImpl::notifyCancellationItem() {
    CombatScreen::notifyCancellationItem();
}

void CombatUIImpl::notifyRequirementNoServed(const std::string& messageRequirement) {
    CombatScreen::notifyRequirementNoServed(messageRequirement);
}

void CombatUIImpl::displayScreenVictory(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries) {
    ScreenVictory::display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat, obtainedItems, enemiesDefeated, parriesPerfect, biggerDamage, parriesTempted, parriesEffective, itemsConsumed, newDiscoveries);
}

void CombatUIImpl::displayScreenDefeat(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat) {
    ScreenDefeat::display(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, cureTotalReceived, shiftsCombat);
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
