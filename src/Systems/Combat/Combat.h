/*
 * Arquivo: Combat.h
 * Proposito: Gerenciamento do fluxo de combate por turnos, acoes dos personagens, calculo de recompensas e estatisticas.
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "Systems/Combat/ICombatUI.h"
#include "Domain/Characters/Character.h"

/*
 * Gerenciador da sessao ativa de combate por turnos.
 */
class Combat 
{
public:
    enum class ActionCombat 
    { 
        Attack = 1, 
        Defend = 2, 
        Skill = 3, 
        Inventory = 4, 
        Player = 5, 
        Bestiary = 6 
    };

private:
    // Referencias aos participantes do combate ativo
    Character* currentPlayer;
    std::vector<std::unique_ptr<Character>> enemies;
    std::vector<std::unique_ptr<Character>> allies;

    // Interface visual de combate (Injecao de Dependencia)
    std::unique_ptr<ICombatUI> ui;

    // Estatisticas gerais e controle da sessao de combate
    int goldObtained;
    int xpObtained;
    int totalDamageCaused;
    int totalDamageReceived;
    int currentTurnCount;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> enemiesDefeated;

    // Estatisticas Avancadas da Sessao
    int parriesAttempted;
    int effectiveParries;
    int perfectParries;
    int highestDamageCaused;
    int stats_itemsConsumed;
    std::vector<std::string> newDiscoveries;
    void resetStatisticsAdvanced();

    void applyDamageToTarget(Character* attackingCharacter, Character* targetCharacter, int grossDamage, int damagePiercing, int currentCombatTurn);
    void processEnemyDeath(Character* enemy);
    void displayAttackResult(Character* target, int finalDamage, bool triedParry, bool parrySuccess, int damageBlocked, bool shieldBroken, const std::string& brokenShieldName);

    void prepareShiftCharacter(Character* character);
    void processPostDamage(Character* attacker, Character* target, int finalDamage, bool triedParry, bool parrySuccess);
    bool isPlayerOrAlly(Character* character) const;
    void processPlayerActionMenu(Character* characterActing, bool& shiftWasConsumed, bool& inventoryUsedThisTurn);
    void processActionAttack(Character* characterActing, bool& shiftWasConsumed);
    void processActionDefend(Character* characterActing, bool& shiftWasConsumed);
    void processActionSkill(Character* characterActing, bool& shiftWasConsumed);
    void processInventoryAction(Character* characterActing, bool& shiftWasConsumed, bool& inventoryUsedThisTurn);
    void cleanEnemiesDead();
    Item* selectShield(Character* characterActing);

    std::string getCombatTitle() const;
    std::vector<Character*> getEnemiesRaw() const;
    void displayCombatScreen(bool animateEntrance = false) const;

public:
    Combat(Character* combatPlayer, std::vector<std::unique_ptr<Character>>&& combatEnemies, std::unique_ptr<ICombatUI> interfaceVisual = nullptr);
    virtual ~Combat();

    void set3DContext(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title);

    std::vector<Character*> getAlliesAliveRaw() const;
    bool executePlayerOrAllyTurn(Character* character, bool& firstRendering, bool processEffectsHome = true);
    void addAllyInCombat(std::unique_ptr<Character> ally);
    void addAllies(std::vector<std::unique_ptr<Character>> allies);
    // Inicia o laco principal de combate
    void startCombat();

    // Executa a inteligencia e as acoes de todos os inimigos presentes
    void executeTurnForAllEnemies();

    // Verifica se todos os inimigos estao mortos ou se o player morreu. Retorna true se o combate deve acabar.
    bool checkVictoryOrDefeatCondition();

    // Aplica o fluxo completo de dano fisico de um personagem a outro
    void performPhysicalAttack(Character* attackingCharacter, Character* defenderCharacter, int currentCombatTurn);

    // Getters para Estatisticas Avancadas
    int getParriesAttempted() const { return parriesAttempted; }
    int getEffectiveParries() const { return effectiveParries; }
    int getHighestDamageCaused() const { return highestDamageCaused; }
    int getItemsConsumed() const { return stats_itemsConsumed; }
    const std::vector<std::string>& getNewDiscoveries() const { return newDiscoveries; }
};
