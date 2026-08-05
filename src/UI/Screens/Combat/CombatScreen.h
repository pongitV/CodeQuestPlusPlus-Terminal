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
    static void cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr);
    static std::vector<std::string> getLinesBarDeStatusDoPlayer(Character* currentPlayer, Color colorHighlight = Color::RESET, int damageAnimation = -1, int frameAnimation = 0, bool isCure = false);
    static void displayHordeDeEnemiesSideASide(const std::vector<Character*>& listDeEnemies, Character* targetAnimation = nullptr, int frameAnimation = 0, bool isCure = false, bool cheerEmergence = false, bool isDeath = false, Item* weaponAttacker = nullptr, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {});
    static void cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation = -1);
    static void cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation = 0);
    static void cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies = {}, bool isParry = false, int damageAnimation = -1);
    static void cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies = {}, int cureAnimation = 0);
    static void cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops = {});
    static void updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance = false);

    static void addFixedMessage(const std::string& msg);
    static void cleanMessagesFixed();

    static void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title);
    static void setShiftVisible(int shift, const std::string& name);
    static void selectHUDDeAlly(Character* currentPlayer, const std::vector<Character*>& allies);

    static int getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies);
    static int getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies);
    static int getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies);
    static int getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields);
    static void notifyEnemiesMoreAct();
    static void notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies);
    static void notifyUnpreventionInventory();
    static void notifyWithoutShields(const std::string& nameCharacter);
    static void notifyImbalanceDefense(const std::string& nameCharacter);
    static void notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield);
    static void notifyActionInvalidates();
    static void notifyCancellationItem();
    static void notifyRequirementNoServed(const std::string& messageRequirement);

    static std::string combatMargin();
};
