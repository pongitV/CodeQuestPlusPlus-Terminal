#include "UI/Screens/Combat/CombatScreen.h"
#include "UI/PerspectiveManager.h"

#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"

ContextCombat CombatScreen::context;

void CombatScreen::displaySoonForScreenDeCombat(const std::string& titleDaScreen, bool animate) {
    PerspectiveManager::getScreenCombatUI().displaySoonForScreenDeCombat(titleDaScreen, animate);
}

void CombatScreen::animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    PerspectiveManager::getScreenCombatUI().animateCombatIntro(title, enemies, currentPlayer);
}

std::vector<std::string> CombatScreen::getLinesBarDeStatusDoPlayer(Character* currentPlayer, Color colorHighlight, int damageAnimation, int frameAnimation, bool isCure) {
    return PerspectiveManager::getScreenCombatUI().getLinesBarDeStatusDoPlayer(currentPlayer, colorHighlight, damageAnimation, frameAnimation, isCure);
}

void CombatScreen::displayHordeDeEnemiesSideASide(const std::vector<Character*>& enemies, Character* targetAnimation, int frameAnimation, bool isCure, bool cheerEmergence, bool isDeath, Item* weaponAttacker, int damageAnimation, const std::vector<std::string>& dropsAnimation) {
    PerspectiveManager::getScreenCombatUI().displayHordeDeEnemiesSideASide(enemies, targetAnimation, frameAnimation, isCure, cheerEmergence, isDeath, weaponAttacker, damageAnimation, dropsAnimation);
}

void CombatScreen::animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) {
    PerspectiveManager::getScreenCombatUI().animateDamageToEnemy(combatTitle, enemies, targetAnimation, attacker, currentPlayer, allies, damageAnimation);
}

void CombatScreen::animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) {
    PerspectiveManager::getScreenCombatUI().animateCureToEnemy(combatTitle, enemies, targetAnimation, currentPlayer, allies, cureAnimation);
}

void CombatScreen::animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) {
    PerspectiveManager::getScreenCombatUI().animateDamageToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, isParry, damageAnimation);
}

void CombatScreen::animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) {
    PerspectiveManager::getScreenCombatUI().animateCureToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, cureAnimation);
}

void CombatScreen::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) {
    PerspectiveManager::getScreenCombatUI().animateEnemyDeath(combatTitle, enemies, enemyDead, currentPlayer, allies, drops);
}

void CombatScreen::updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance) {
    PerspectiveManager::getScreenCombatUI().updateScreenStatic(combatTitle, enemies, currentPlayer, allies, animateEntrance);
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

void CombatScreen::selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies) {
    PerspectiveManager::getScreenCombatUI().selectHUDAlly(currentPlayer, allies);
}

void CombatScreen::setShiftVisible(int shift, const std::string& name) {
    context.shiftCurrentVisible = shift;
    context.nameShiftVisible = name;
}

int CombatScreen::getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return PerspectiveManager::getScreenCombatUI().getPlayerAction(currentTurn, characterActing, enemies, currentPlayer, allies);
}

int CombatScreen::getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return PerspectiveManager::getScreenCombatUI().getTargetAttack(combatTitle, enemies, currentPlayer, allies);
}

int CombatScreen::getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return PerspectiveManager::getScreenCombatUI().getTargetItem(combatTitle, enemies, currentPlayer, allies);
}

int CombatScreen::chooseShield(const std::string& characterName, const std::vector<Item*>& shields) {
    return PerspectiveManager::getScreenCombatUI().chooseShield(characterName, shields);
}

void CombatScreen::notifyEnemiesMoreAct() {
    PerspectiveManager::getScreenCombatUI().notifyEnemiesMoreAct();
}

void CombatScreen::notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) {
    PerspectiveManager::getScreenCombatUI().notifyShiftExtra(dexterityPlayer, maxEnemyDexterity);
}

void CombatScreen::notifyUnpreventionInventory() {
    PerspectiveManager::getScreenCombatUI().notifyUnpreventionInventory();
}

void CombatScreen::notifyWithoutShields(const std::string& characterName) {
    PerspectiveManager::getScreenCombatUI().notifyWithoutShields(characterName);
}

void CombatScreen::notifyImbalanceDefense(const std::string& characterName) {
    PerspectiveManager::getScreenCombatUI().notifyImbalanceDefense(characterName);
}

void CombatScreen::notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) {
    PerspectiveManager::getScreenCombatUI().notifyPostureDefensive(characterName, nameShield);
}

void CombatScreen::notifyActionInvalidates() {
    PerspectiveManager::getScreenCombatUI().notifyActionInvalidates();
}

void CombatScreen::notifyCancellationItem() {
    PerspectiveManager::getScreenCombatUI().notifyCancellationItem();
}

void CombatScreen::notifyUnmetRequirement(const std::string& requirementMessage) {
    PerspectiveManager::getScreenCombatUI().notifyUnmetRequirement(requirementMessage);
}

std::string CombatScreen::combatMargin() {
    int widthHUD = 91;
    int terminalWidth = Appearance::getTerminalWidth();
    int widthRef = std::min(widthHUD, terminalWidth);
    return Appearance::spacesToCenter(widthRef);
}
