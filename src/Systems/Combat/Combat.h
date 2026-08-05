/*
 * Arquivo: Combat.h
 * Propósito: Gerenciamento do fluxo de combate por turnos, ações dos personagens, cálculo de recompensas e estatísticas.
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
 * Gerenciador da sessão ativa de combate por turnos.
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
    std::vector<std::unique_ptr<Character>> listDeEnemies;
    std::vector<std::unique_ptr<Character>> listDeAllies;

    // Interface visual de combate (Injecao de Dependencia)
    std::unique_ptr<ICombatUI> ui;

    // Estatisticas gerais e controle da sessao de combate
    int quantityDeGoldObtained;
    int quantityDeXpObtained;
    int totalDeDamageCaused;
    int totalDeDamageReceived;
    int accountantDoShiftCurrent;
    std::vector<std::string> obtainedItems;
    std::vector<std::string> enemiesDefeated;

    // Estatisticas Avancadas da Sessao
    int stats_parriesTempted;
    int stats_parriesEffective;
    int stats_parriesPerfect;
    int stats_biggerDamageCaused;
    int stats_itemsConsumed;
    std::vector<std::string> stats_newDiscoveries;
    void resetStatisticsAdvanced();

    void applyDamageAoTarget(Character* attackingCharacter, Character* characterTarget, int quantityDeDamageGross, int damagePiercing, int shiftCurrentDoCombat);
    void processDeathDeEnemy(Character* enemy);
    void displayResultDoAttack(Character* target, int damageEnd, bool triedParry, bool parrySuccess, int damageBlocked, bool shieldBroke, const std::string& nameShieldBroken);

    void prepareShiftCharacter(Character* character);
    void processPostDamage(Character* attacker, Character* target, int damageEnd, bool triedParry, bool parrySuccess);
    bool isCharacterPlayerOuAlly(Character* character) const;
    void processMenuDeActionsDoPlayer(Character* characterActing, bool& shiftWasConsumed, bool& usedInventoryNoShift);
    void processActionAttack(Character* characterActing, bool& shiftWasConsumed);
    void processActionDefend(Character* characterActing, bool& shiftWasConsumed);
    void processActionSkill(Character* characterActing, bool& shiftWasConsumed);
    void processActionInventory(Character* characterActing, bool& shiftWasConsumed, bool& usedInventoryNoShift);
    void cleanEnemiesDead();
    Item* selectShield(Character* characterActing);

    std::string getTitleDoCombat() const;
    std::vector<Character*> getEnemiesRaw() const;
    void displayScreenDeCombat(bool animateEntrance = false) const;

public:
    Combat(Character* playerForOCombat, std::vector<std::unique_ptr<Character>>&& enemiesForOCombat, std::unique_ptr<ICombatUI> interfaceVisual = nullptr);
    virtual ~Combat();

    void set3DContext(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title);

    std::vector<Character*> getAlliesAliveRaw() const;
    bool executeShiftPlayerOuAlly(Character* character, bool& firstRendering, bool processEffectsHome = true);
    void addAllyEmCombat(std::unique_ptr<Character> ally);
    void addAllies(std::vector<std::unique_ptr<Character>> allies);
    /**
     * @brief Inicia o laco principal de combate.
     */
    void startCombat();

    /**
     * @brief Executa a inteligencia e as acoes de todos os inimigos presentes.
     */
    void executeShiftDeEveryoneOsEnemies();

    /**
     * @brief Verifica se todos os inimigos estao mortos ou se o jogador morreu.
     * @return true se o combate deve acabar.
     */
    bool checkConditionDeVictoryOuDefeat();

    /**
     * @brief Aplica o fluxo completo de dano fisico de um personagem a outro.
     * @param personagemAtacante Ponteiro para quem ataca.
     * @param personagemDefensor Ponteiro para quem defende.
     * @param turnoAtualDoCombate Turno em que a acao ocorre.
     */
    void performAttackPhysical(Character* attackingCharacter, Character* characterDefender, int shiftCurrentDoCombat);

    // Getters para Estatisticas Avancadas
    int getParriesTempted() const { return stats_parriesTempted; }
    int getParriesEffective() const { return stats_parriesEffective; }
    int getBiggerDamageCaused() const { return stats_biggerDamageCaused; }
    int getItemsConsumed() const { return stats_itemsConsumed; }
    const std::vector<std::string>& getNewDiscoveries() const { return stats_newDiscoveries; }
};
