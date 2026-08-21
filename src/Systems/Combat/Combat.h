// [PT-BR] Arquivo: Combat.h
// [PT-BR] Proposito: Gerenciamento do fluxo de combate por turnos, acoes dos personagens, calculo de recompensas e estatisticas.
// [EN-US] File: Combat.h
// [EN-US] Purpose: Management of turn-based combat flow, character actions, reward calculation, and statistics.

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include "Domain/Characters/Character.h"
#include "Systems/Combat/ICombatUI.h"

// [PT-BR] Gerenciador da sessao ativa de combate por turnos.
// [EN-US] Manager for the active turn-based combat session.
class Combat 
{
public:
    struct PostCombatContext3D {
        bool enabled = false;
        std::vector<std::string> mapMatrix;
        float playerX = 0.0f;
        float playerY = 0.0f;
        float playerAngle = 0.0f;
        std::string placeTitle = "";
    };

private:
    // [PT-BR] Referencias aos participantes do combate ativo
    // [EN-US] References to active combat participants
    Character* currentPlayer;
    std::vector<std::unique_ptr<Character>> enemies;
    std::vector<std::unique_ptr<Character>> allies;

    // [PT-BR] Interface visual de combate (Injecao de Dependencia)
    // [EN-US] Combat visual interface (Dependency Injection)
    std::unique_ptr<ICombatUI> ui;

    // [PT-BR] Estatisticas gerais e controle da sessao de combate
    // [EN-US] General statistics and combat session control
    int goldObtained;
    int xpObtained;
    int totalDamageCaused;
    int totalDamageReceived;
    int currentTurnCount;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> enemiesDefeated;

    // [PT-BR] Estatisticas Avancadas da Sessao
    // [EN-US] Advanced Session Statistics
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
    
    // [PT-BR] Inicia o laco principal de combate
    // [EN-US] Starts main combat loop
    void startCombat();

    // [PT-BR] Executa a inteligencia e as acoes de todos os inimigos presentes
    // [EN-US] Executes AI and actions of all present enemies
    void executeTurnForAllEnemies();

    // [PT-BR] Verifica condicao de vitoria ou derrota no combate
    // [EN-US] Checks victory or defeat condition in combat
    bool checkVictoryOrDefeatCondition();

    // [PT-BR] Aplica o fluxo completo de ataque fisico de um personagem a outro
    // [EN-US] Applies full physical attack flow from one character to another
    void performPhysicalAttack(Character* attackingCharacter, Character* defenderCharacter, int currentCombatTurn);

    // [PT-BR] Getters para Estatisticas Avancadas
    // [EN-US] Getters for Advanced Statistics
    int getParriesAttempted() const { return parriesAttempted; }
    int getEffectiveParries() const { return effectiveParries; }
    int getHighestDamageCaused() const { return highestDamageCaused; }
    int getItemsConsumed() const { return stats_itemsConsumed; }
    const std::vector<std::string>& getNewDiscoveries() const { return newDiscoveries; }
};
