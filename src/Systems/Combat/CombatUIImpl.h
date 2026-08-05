#pragma once

#include "Systems/Combat/ICombatUI.h"

class CombatUIImpl : public ICombatUI {
public:
    CombatUIImpl() = default;
    ~CombatUIImpl() override = default;

    void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) override;
    
    void cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) override;
    void updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance = false) override;
    
    void cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation) override;
    void cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) override;
    
    void cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool isParry, int damageAnimation) override;
    void cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) override;
    
    void cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops) override;

    void cleanContextCharacterHUD() override;
    void cleanContextEnemyDeadEDrops() override;

    std::string combatMargin() override;

    void addFixedMessage(const std::string& msg) override;
    void cleanMessagesFixed() override;
    void setShiftVisible(int shift, const std::string& name) override;
    
    int getActionDoPlayer(int shiftCurrent, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getTargetAttack(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getTargetItem(const std::string& titleCombat, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getChooseDeShield(const std::string& nameCharacter, const std::vector<Item*>& listDeShields) override;
    
    void notifyEnemiesMoreAct() override;
    void notifyShiftExtra(int dexterityPlayer, int maxDexterityEnemies) override;
    void notifyUnpreventionInventory() override;
    void notifyWithoutShields(const std::string& nameCharacter) override;
    void notifyImbalanceDefense(const std::string& nameCharacter) override;
    void notifyPostureDefensive(const std::string& nameCharacter, const std::string& nameShield) override;
    void notifyActionInvalidates() override;
    void notifyCancellationItem() override;
    void notifyRequirementNoServed(const std::string& messageRequirement) override;

    void displayScreenVictory(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries) override;
    void displayScreenDefeat(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat) override;

    void displayScreenAttributes(Character* character) override;
    void displayScreenDiary(Character* character) override;

    void clearScreen() override;
};
