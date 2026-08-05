#include "UI/Screens/Combat/CombatScreen.h"
#include "UI/PerspectiveManager.h"

#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"

ContextCombat CombatScreen::context;

void CombatScreen::displaySoonForScreenDeCombat(const std::string& titleDaScreen, bool animate) {
    PerspectiveManager::getScreenCombatUI().displaySoonForScreenDeCombat(titleDaScreen, animate);
}

void CombatScreen::cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    PerspectiveManager::getScreenCombatUI().cheerIntroductionCombat(title, enemies, currentPlayer);
}

std::vector<std::string> CombatScreen::getLinesBarDeStatusDoPlayer(Character* currentPlayer, Color colorHighlight, int damageAnimation, int frameAnimation, bool isCure) {
    return PerspectiveManager::getScreenCombatUI().getLinesBarDeStatusDoPlayer(currentPlayer, colorHighlight, damageAnimation, frameAnimation, isCure);
}

void CombatScreen::displayHordeDeEnemiesSideASide(const std::vector<Character*>& listDeEnemies, Character* targetAnimation, int frameAnimation, bool isCure, bool cheerEmergence, bool isDeath, Item* weaponAttacker, int damageAnimation, const std::vector<std::string>& dropsAnimation) {
    PerspectiveManager::getScreenCombatUI().displayHordeDeEnemiesSideASide(listDeEnemies, targetAnimation, frameAnimation, isCure, cheerEmergence, isDeath, weaponAttacker, damageAnimation, dropsAnimation);
}

void CombatScreen::cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation) {
    PerspectiveManager::getScreenCombatUI().cheerDamageNoEnemy(titleCombat, listDeEnemies, targetAnimation, attacker, currentPlayer, listDeAllies, damageAnimation);
}

void CombatScreen::cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) {
    PerspectiveManager::getScreenCombatUI().cheerCureNoEnemy(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation);
}

void CombatScreen::cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool isParry, int damageAnimation) {
    PerspectiveManager::getScreenCombatUI().cheerDamageNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, isParry, damageAnimation);
}

void CombatScreen::cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) {
    PerspectiveManager::getScreenCombatUI().cheerCureNoPlayer(titleCombat, listDeEnemies, targetAnimation, currentPlayer, listDeAllies, cureAnimation);
}

void CombatScreen::cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops) {
    PerspectiveManager::getScreenCombatUI().cheerDeathEnemy(titleCombat, listDeEnemies, enemyDead, currentPlayer, listDeAllies, drops);
}

void CombatScreen::updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance) {
    PerspectiveManager::getScreenCombatUI().updateScreenStatic(titleCombat, listDeEnemies, currentPlayer, listDeAllies, animateEntrance);
}

void CombatScreen::addFixedMessage(const std::string& msg) {
    PerspectiveManager::getScreenCombatUI().addFixedMessage(msg);
}

void CombatScreen::cleanMessagesFixed() {
    PerspectiveManager::getScreenCombatUI().cleanMessagesFixed();
}

void CombatScreen::configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    context.isMode3D = mode3D;
    context.currentMapMatrix = matrix;
    context.playerPostX = postX;
    context.playerPostY = postY;
    context.playerAngle = angle;
    context.titleMapCurrent = title;
}

void CombatScreen::selectHUDDeAlly(Character* currentPlayer, const std::vector<Character*>& allies) {
    PerspectiveManager::getScreenCombatUI().selectHUDDeAlly(currentPlayer, allies);
}

void CombatScreen::setShiftVisible(int shift, const std::string& name) {
    context.shiftCurrentVisible = shift;
    context.nameShiftVisible = name;
}

int CombatScreen::getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return PerspectiveManager::getScreenCombatUI().getActionDoPlayer(shiftCurrent, characterActing, enemies, currentPlayer, allies);
}

int CombatScreen::getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return PerspectiveManager::getScreenCombatUI().getTargetAttack(titleCombat, enemies, currentPlayer, allies);
}

int CombatScreen::getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return PerspectiveManager::getScreenCombatUI().getTargetItem(titleCombat, enemies, currentPlayer, allies);
}

int CombatScreen::getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields) {
    return PerspectiveManager::getScreenCombatUI().getChooseDeShield(nameCharacter, listDeShields);
}

void CombatScreen::notifyEnemiesMoreAct() {
    PerspectiveManager::getScreenCombatUI().notifyEnemiesMoreAct();
}

void CombatScreen::notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies) {
    PerspectiveManager::getScreenCombatUI().notifyShiftExtra(dexterityPlayer, maxDexterityEnemies);
}

void CombatScreen::notifyUnpreventionInventory() {
    PerspectiveManager::getScreenCombatUI().notifyUnpreventionInventory();
}

void CombatScreen::notifyWithoutShields(const std::string& nameCharacter) {
    PerspectiveManager::getScreenCombatUI().notifyWithoutShields(nameCharacter);
}

void CombatScreen::notifyImbalanceDefense(const std::string& nameCharacter) {
    PerspectiveManager::getScreenCombatUI().notifyImbalanceDefense(nameCharacter);
}

void CombatScreen::notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield) {
    PerspectiveManager::getScreenCombatUI().notifyPostureDefensive(nameCharacter, nameShield);
}

void CombatScreen::notifyActionInvalidates() {
    PerspectiveManager::getScreenCombatUI().notifyActionInvalidates();
}

void CombatScreen::notifyCancellationItem() {
    PerspectiveManager::getScreenCombatUI().notifyCancellationItem();
}

void CombatScreen::notifyRequirementNoServed(const std::string& messageRequirement) {
    PerspectiveManager::getScreenCombatUI().notifyRequirementNoServed(messageRequirement);
}

std::string CombatScreen::combatMargin() {
    int widthHUD = 91;
    int terminalWidth = Appearance::getTerminalWidth();
    int widthRef = std::min(widthHUD, terminalWidth);
    return Appearance::spacesToCenter(widthRef);
}
