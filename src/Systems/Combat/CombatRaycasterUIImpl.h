#pragma once

#include "Systems/Combat/ICombatUI.h"

class CombatRaycasterUIImpl : public ICombatUI {
public:
    CombatRaycasterUIImpl() = default;
    ~CombatRaycasterUIImpl() override = default;

    void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) override;
    
    void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) override;
    void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance = false) override;
    
    void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) override;
    void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) override;
    
    void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) override;
    void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) override;
    
    void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) override;

    void cleanContextCharacterHUD() override;
    void cleanContextEnemyDeathAndDrops() override;

    std::string combatMargin() override;

    void addFixedMessage(const std::string& msg) override;
    void cleanMessagesFixed() override;
    void setShiftVisible(int shift, const std::string& name) override;
    
    int getPlayerAction(int currentTurn, Character* characterActing, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getTargetAttack(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int getTargetItem(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies) override;
    int chooseShield(const std::string& characterName, const std::vector<Item*>& shields) override;
    
    void notifyEnemiesMoreAct() override;
    void notifyShiftExtra(int dexterityPlayer, int maxEnemyDexterity) override;
    void notifyUnpreventionInventory() override;
    void notifyWithoutShields(const std::string& characterName) override;
    void notifyImbalanceDefense(const std::string& characterName) override;
    void notifyPostureDefensive(const std::string& characterName, const std::string& nameShield) override;
    void notifyActionInvalidates() override;
    void notifyCancellationItem() override;
    void notifyUnmetRequirement(const std::string& requirementMessage) override;

    void displayVictoryScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int perfectParries, int highestDamage, int parriesAttempted, int effectiveParries, int itemsConsumed, const std::vector<std::string>& newDiscoveries) override;
    void displayDefeatScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns) override;

    void displayScreenAttributes(Character* character) override;
    void displayScreenDiary(Character* character) override;

    void clearScreen() override;
};
