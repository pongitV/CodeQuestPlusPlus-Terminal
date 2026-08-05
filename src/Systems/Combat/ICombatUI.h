/*
 * Arquivo: ICombatUI.h
 * Propósito: Interface abstrata para renderização da interface visual de combate (Console 2D e Raycaster 3D).
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

class Character;
class Item;

/*
 * Interface de exibição e controle visual de combate.
 */
class ICombatUI {
public:
    virtual ~ICombatUI() = default;

    virtual void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) = 0;
    
    virtual void cheerIntroductionCombat(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer) = 0;
    virtual void updateScreenStatic(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool animateEntrance = false) = 0;
    
    virtual void cheerDamageNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int damageAnimation) = 0;
    virtual void cheerCureNoEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) = 0;
    
    virtual void cheerDamageNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, bool isParry, int damageAnimation) = 0;
    virtual void cheerCureNoPlayer(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& listDeAllies, int cureAnimation) = 0;
    
    virtual void cheerDeathEnemy(const std::string& titleCombat, const std::vector<Character*>& listDeEnemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& listDeAllies, const std::vector<std::string>& drops) = 0;

    virtual void cleanContextCharacterHUD() = 0;
    virtual void cleanContextEnemyDeadEDrops() = 0;

    virtual std::string combatMargin() = 0;

    virtual void addFixedMessage(const std::string& msg) = 0;
    virtual void cleanMessagesFixed() = 0;
    virtual void setShiftVisible(int shift, const std::string& name) = 0;
    
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

    virtual void displayScreenVictory(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat, const std::vector<std::string>& obtainedItems, const std::vector<std::string>& enemiesDefeated, int parriesPerfect, int biggerDamage, int parriesTempted, int parriesEffective, int itemsConsumed, const std::vector<std::string>& newDiscoveries) = 0;
    virtual void displayScreenDefeat(Character* currentPlayer, int quantityDeGoldObtained, int quantityDeXpObtained, int totalDeDamageCaused, int totalDeDamageReceived, int cureTotalReceived, int shiftsCombat) = 0;
    
    virtual void displayScreenAttributes(Character* character) = 0;
    virtual void displayScreenDiary(Character* character) = 0;

    virtual void clearScreen() = 0;
};
