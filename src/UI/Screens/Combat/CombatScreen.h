#pragma once

#include <string>
#include <vector>
#include "UI/Screens/Combat/CombatContext.h"
#include "Core/Utils/Appearance.h"

class Character;
class Item;

class CombatScreen {
public:
    static ContextCombat context;

    static void displaySoonForScreenDeCombat(const std::string& titleDaScreen = "", bool animate = true);
    static void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr);
    static std::vector<std::string> getLinesBarDeStatusDoPlayer(Character* currentPlayer, Color colorHighlight = Color::RESET, int damageAnimation = -1, int frameAnimation = 0, bool isCure = false);
    static void displayHordeDeEnemiesSideASide(const std::vector<Character*>& enemies, Character* targetAnimation = nullptr, int frameAnimation = 0, bool isCure = false, bool cheerEmergence = false, bool isDeath = false, Item* weaponAttacker = nullptr, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {});
    static void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation = -1);
    static void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation = 0);
    static void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, bool isParry = false, int damageAnimation = -1);
    static void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, int cureAnimation = 0);
    static void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops = {});
    static void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance = false);

    static void addFixedMessage(const std::string& msg);
    static void cleanMessagesFixed();

    static void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title);
    static void setShiftVisible(int shift, const std::string& name);
    static void selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies);

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

    static std::string combatMargin();
};
