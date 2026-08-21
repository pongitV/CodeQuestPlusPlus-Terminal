#include "Systems/Combat/CombatRaycasterUIImpl.h"

#include "UI/Renderers/3D/RaycasterScreens/Combat/RaycasterCombatScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Victory/RaycasterVictoryScreen.h"
#include "UI/Renderers/3D/RaycasterScreens/Defeat/RaycasterDefeatScreen.h"
#include "UI/Screens/Attributes/AttributesScreen.h"
#include "UI/Screens/Diary/DiaryScreen.h"
#include "Core/Utils/Appearance.h"
#include "UI/Screens/Combat/CombatScreen.h"

void CombatRaycasterUIImpl::configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    RaycasterCombatScreen::configureContext3D(mode3D, matrix, postX, postY, angle, title);
}

void CombatRaycasterUIImpl::animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) {
    RaycasterCombatScreen::animateCombatIntro(title, enemies, currentPlayer);
}

void CombatRaycasterUIImpl::updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance) {
    RaycasterCombatScreen::updateScreenStatic(combatTitle, enemies, currentPlayer, allies, animateEntrance, nullptr);
}

void CombatRaycasterUIImpl::animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) {
    RaycasterCombatScreen::animateDamageToEnemy(combatTitle, enemies, targetAnimation, attacker, currentPlayer, allies, damageAnimation);
}

void CombatRaycasterUIImpl::animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) {
    RaycasterCombatScreen::animateCureToEnemy(combatTitle, enemies, targetAnimation, currentPlayer, allies, healingAnimation);
}

void CombatRaycasterUIImpl::animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) {
    RaycasterCombatScreen::animateDamageToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, isParry, damageAnimation);
}

void CombatRaycasterUIImpl::animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation) {
    RaycasterCombatScreen::animateCureToPlayer(combatTitle, enemies, targetAnimation, currentPlayer, allies, healingAnimation);
}

void CombatRaycasterUIImpl::animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) {
    RaycasterCombatScreen::animateEnemyDeath(combatTitle, enemies, enemyDead, currentPlayer, allies, drops);
}

void CombatRaycasterUIImpl::cleanContextCharacterHUD() {
    CombatScreen::context.characterHUD = nullptr;
}

void CombatRaycasterUIImpl::cleanContextEnemyDeathAndDrops() {
    CombatScreen::context.enemyDeadWithDrops = nullptr;
    CombatScreen::context.dropsAssets.clear();
}

std::string CombatRaycasterUIImpl::combatMargin() {
    // [PT-BR] RaycasterCombatScreen utiliza formatacao propria em 3D
    // [EN-US] RaycasterCombatScreen uses its own 3D formatting
    return "";
}

void CombatRaycasterUIImpl::addFixedMessage(const std::string& msg) {
    RaycasterCombatScreen::addFixedMessage(msg);
}

void CombatRaycasterUIImpl::cleanMessagesFixed() {
    RaycasterCombatScreen::cleanMessagesFixed();
}

void CombatRaycasterUIImpl::setShiftVisible(int shift, const std::string& name) {
    RaycasterCombatScreen::setShiftVisible(shift, name);
}

int CombatRaycasterUIImpl::getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return RaycasterCombatScreen::getPlayerAction(currentTurn, characterActing, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return RaycasterCombatScreen::getTargetAttack(combatTitle, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) {
    return RaycasterCombatScreen::getTargetItem(combatTitle, enemies, currentPlayer, allies);
}

int CombatRaycasterUIImpl::chooseShield(const std::string& characterName, const std::vector<Item*>& shields) {
    return RaycasterCombatScreen::chooseShield(characterName, shields);
}

void CombatRaycasterUIImpl::notifyEnemiesMoreAct() {
    RaycasterCombatScreen::notifyEnemiesMoreAct();
}

void CombatRaycasterUIImpl::notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) {
    RaycasterCombatScreen::notifyShiftExtra(dexterityPlayer, maxEnemyDexterity);
}

void CombatRaycasterUIImpl::notifyUnpreventionInventory() {
    RaycasterCombatScreen::notifyUnpreventionInventory();
}

void CombatRaycasterUIImpl::notifyWithoutShields(const std::string& characterName) {
    RaycasterCombatScreen::notifyWithoutShields(characterName);
}

void CombatRaycasterUIImpl::notifyImbalanceDefense(const std::string& characterName) {
    RaycasterCombatScreen::notifyImbalanceDefense(characterName);
}

void CombatRaycasterUIImpl::notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) {
    RaycasterCombatScreen::notifyPostureDefensive(characterName, nameShield);
}

void CombatRaycasterUIImpl::notifyActionInvalidates() {
    RaycasterCombatScreen::notifyActionInvalidates();
}

void CombatRaycasterUIImpl::notifyCancellationItem() {
    RaycasterCombatScreen::notifyCancellationItem();
}

void CombatRaycasterUIImpl::notifyUnmetRequirement(const std::string& requirementMessage) {
    RaycasterCombatScreen::notifyUnmetRequirement(requirementMessage);
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

    RaycasterVictoryScreen::display(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, totalCureReceived, combatTurns, enemiesDefeated, perfectParries, highestDamage, parriesAttempted, effectiveParries, itemsConsumed, dropsUnique, canLevelUp, newDiscoveries, "");
}

void CombatRaycasterUIImpl::displayDefeatScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns) {
    RaycasterDefeatScreen::display(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, totalCureReceived, combatTurns);
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
