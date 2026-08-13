#pragma once
#include <string>
#include <vector>
#include <functional>
#include "Domain/Characters/Character.h"
#include "Domain/Items/Item.h"

class ICombatScreenUI {
public:
    virtual ~ICombatScreenUI() = default;

    virtual void displayLogoForCombatScreen(const std::string& screenTitle = "", bool animate = true) = 0;
    virtual void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr) = 0;
    virtual std::vector<std::string> getPlayerStatusBarLines(Character* currentPlayer, Color colorHighlight = Color::RESET, int damageAnimation = -1, int frameAnimation = 0, bool isHealing = false) = 0;
    virtual void displayEnemyHordeSideBySide(const std::vector<Character*>& enemies, Character* targetAnimation = nullptr, int frameAnimation = 0, bool isHealing = false, bool animateEmergence = false, bool isDeath = false, Item* weaponAttacker = nullptr, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {}) = 0;
    virtual void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation = -1) = 0;
    virtual void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation = 0) = 0;
    virtual void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, bool isParry = false, int damageAnimation = -1) = 0;
    virtual void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, int healingAnimation = 0) = 0;
    virtual void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops = {}) = 0;
    virtual void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance = false, std::function<void(std::vector<std::string>&)> callbackOverlay = nullptr) = 0;
    virtual void addFixedMessage(const std::string& msg) = 0;
    virtual void cleanMessagesFixed() = 0;
    virtual void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) = 0;
    virtual void setShiftVisible(int shift, const std::string& name) = 0;
    virtual void selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) = 0;
    virtual int chooseShield(const std::string& characterName, const std::vector<Item*>& shields) = 0;
    virtual void notifyEnemiesMoreAct() = 0;
    virtual void notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) = 0;
    virtual void notifyUnpreventionInventory() = 0;
    virtual void notifyWithoutShields(const std::string& characterName) = 0;
    virtual void notifyImbalanceDefense(const std::string& characterName) = 0;
    virtual void notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) = 0;
    virtual void notifyActionInvalidates() = 0;
    virtual void notifyCancellationItem() = 0;
    virtual void notifyUnmetRequirement(const std::string& requirementMessage) = 0;
};
