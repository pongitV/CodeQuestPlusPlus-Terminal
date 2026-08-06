#pragma once
#include <string>
#include <vector>
#include <functional>
#include "Domain/Characters/Character.h"
#include "Domain/Items/Item.h"

class IScreenCombatUI {
public:
    virtual ~IScreenCombatUI() = default;

    virtual void displaySoonForScreenDeCombat(const std::string& titleDaScreen = "", bool animate = true) = 0;
    virtual void cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr) = 0;
    virtual std::vector<std::string> getLinesBarDeStatusDoPlayer(Character* currentPlayer, Color colorHighlight = Color::RESET, int damageAnimation = -1, int frameAnimation = 0, bool isCure = false) = 0;
    virtual void displayHordeDeEnemiesSideASide(const std::vector<Character*>& listDeEnemies, Character* targetAnimation = nullptr, int frameAnimation = 0, bool isCure = false, bool cheerEmergence = false, bool isDeath = false, Item* weaponAttacker = nullptr, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {}) = 0;
    virtual void cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation = -1) = 0;
    virtual void cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation = 0) = 0;
    virtual void cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies = {}, bool isParry = false, int damageAnimation = -1) = 0;
    virtual void cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies = {}, int cureAnimation = 0) = 0;
    virtual void cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops = {}) = 0;
    virtual void updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance = false, std::function<void(std::vector<std::string>&)> callbackOverlay = nullptr) = 0;
    virtual void addFixedMessage(const std::string& msg) = 0;
    virtual void cleanMessagesFixed() = 0;
    virtual void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) = 0;
    virtual void setShiftVisible(int shift, const std::string& name) = 0;
    virtual void selectHUDDeAlly(Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields) = 0;
    virtual void notifyEnemiesMoreAct() = 0;
    virtual void notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies) = 0;
    virtual void notifyUnpreventionInventory() = 0;
    virtual void notifyWithoutShields(const std::string& nameCharacter) = 0;
    virtual void notifyImbalanceDefense(const std::string& nameCharacter) = 0;
    virtual void notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield) = 0;
    virtual void notifyActionInvalidates() = 0;
    virtual void notifyCancellationItem() = 0;
    virtual void notifyRequirementNoServed(const std::string& messageRequirement) = 0;
};
