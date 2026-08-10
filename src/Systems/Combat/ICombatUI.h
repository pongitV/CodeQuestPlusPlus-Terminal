/*
 * Arquivo: ICombatUI.h
 * Proposito: Interface abstrata para renderizacao da interface visual de combate (Console 2D e Raycaster 3D).
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

class Character;
class Item;

/*
 * Interface de exibicao e controle visual de combate.
 */
class ICombatUI {
public:
    virtual ~ICombatUI() = default;

    virtual void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) = 0;
    
    virtual void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) = 0;
    virtual void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance = false) = 0;
    
    virtual void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation) = 0;
    virtual void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) = 0;
    
    virtual void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, bool isParry, int damageAnimation) = 0;
    virtual void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int cureAnimation) = 0;
    
    virtual void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops) = 0;

    virtual void cleanContextCharacterHUD() = 0;
    virtual void cleanContextEnemyDeathAndDrops() = 0;

    virtual std::string combatMargin() = 0;

    virtual void addFixedMessage(const std::string& msg) = 0;
    virtual void cleanMessagesFixed() = 0;
    virtual void setShiftVisible(int shift, const std::string& name) = 0;
    
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

    virtual void displayVictoryScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int perfectParries, int highestDamage, int parriesAttempted, int effectiveParries, int itemsConsumed, const std::vector<std::string>& newDiscoveries) = 0;
    virtual void displayDefeatScreen(Character* currentPlayer, int goldObtained, int xpObtained, int totalDamageCaused, int totalDamageReceived, int totalCureReceived, int combatTurns) = 0;
    
    virtual void displayScreenAttributes(Character* character) = 0;
    virtual void displayScreenDiary(Character* character) = 0;

    virtual void clearScreen() = 0;
};
