#pragma once

#include <string>
#include <vector>

#include "Core/Utils/Appearance.h"

class Character;
class Item;

class IDECombatScreen 
{
public:
    static void displayLogoForCombatScreen(const std::string& screenTitle = "", bool animate = true);
    static void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr);
    static std::vector<std::string> getPlayerStatusBarLines(Character* currentPlayer, Color colorHighlight = Color::RESET, int damageAnimation = -1, int frameAnimation = 0, bool isHealing = false);
    static void displayEnemyHordeSideBySide(const std::vector<Character*>& enemies, Character* targetAnimation = nullptr, int frameAnimation = 0, bool isHealing = false, bool animateEmergence = false, bool isDeath = false, Item* weaponAttacker = nullptr, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {});
    static void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation = -1);
    static void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation = 0);
    static void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, bool isParry = false, int damageAnimation = -1);
    static void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, int healingAnimation = 0);
    static void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops = {});
    static void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance = false);

    static void addFixedMessage(const std::string& msg);
    static void cleanMessagesFixed();

    static void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title);

    static void setShiftVisible(int shift, const std::string& name);
    static void selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies);

    // [PT-BR] Menus e interacoes de texto do combate
    // [EN-US] Combat menus and text interactions
    static int getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies);
    static int getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies);
    static int getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies);
    static int chooseShield(const std::string& characterName, const std::vector<Item*>& shields);
    static void notifyEnemiesMoreAct();
    static void notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity);
    static void notifyUnpreventionInventory();
    static void notifyWithoutShields(const std::string& characterName);
    static void notifyImbalanceDefense(const std::string& characterName);
    static void notifyPostureDefensive(const std::string& characterName, const std::string& nameShield);
    static void notifyActionInvalidates();
    static void notifyCancellationItem();
    static void notifyUnmetRequirement(const std::string& requirementMessage);

    // [PT-BR] Metodos auxiliares internos de renderizacao de combate IDE
    // [EN-US] Internal helper methods for IDE combat rendering
    static void display(Character* currentPlayer, const std::vector<Character*>& enemies, const std::string& combatTitle = "Combate");
    static void displayLogCombat(const std::vector<std::string>& messages);
    static void displayStatsCombat(int shifts, int damageCaused, int damageReceived, int healing);
    static void displayDrops(const std::vector<std::string>& drops);
    static int displayMenuActions(Character* player, const std::vector<Character*>& enemies);
    static int displaySelectionTarget(const std::vector<Character*>& enemies);
    static void displayShift(Character* character, const std::vector<Character*>& enemies, int shift);
    static void displayLogCombatTurnal(const std::vector<std::string>& messages);
    static void displayDeathEnemy(const std::string& nameEnemy, int xpGain, const std::vector<std::string>& drops);
    static void displayEffectsAssets(Character* character);
    static int displaySelectionItem(Character* player, int category);
    static int displaySelectionShield(Character* player, const std::vector<Item*>& shields);
    static void displayPostureDefensive(Character* player, const std::string& nameShield);
    static void displayImbalanceDefense(Character* player);
    static void displayMessageCombat(const std::string& message, int color);
};
