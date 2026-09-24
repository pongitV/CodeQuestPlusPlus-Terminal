#pragma once

#include "UI/Interfaces/ICombatScreenUI.h"
#include <vector>
#include <string>

class IDECombatScreen : public ICombatScreenUI {
public:
    IDECombatScreen() = default;
    ~IDECombatScreen() override = default;

    void displayLogoForCombatScreen(const std::string& screenTitle = "", bool animate = true) override;
    void animateCombatIntro(const std::string& title, const std::vector<Character*>& enemies, Character* currentPlayer = nullptr) override;
    std::vector<std::string> getPlayerStatusBarLines(Character* currentPlayer, Color colorHighlight = Color::RESET, int damageAnimation = -1, int frameAnimation = 0, bool isHealing = false) override;
    void displayEnemyHordeSideBySide(const std::vector<Character*>& enemies, Character* targetAnimation = nullptr, int frameAnimation = 0, bool isHealing = false, bool animateEmergence = false, bool isDeath = false, Item* weaponAttacker = nullptr, int damageAnimation = -1, const std::vector<std::string>& dropsAnimation = {}) override;
    void animateDamageToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* attacker, Character* currentPlayer, const std::vector<Character*>& allies, int damageAnimation = -1) override;
    void animateCureToEnemy(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies, int healingAnimation = 0) override;
    void animateDamageToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, bool isParry = false, int damageAnimation = -1) override;
    void animateCureToPlayer(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* targetAnimation, Character* currentPlayer, const std::vector<Character*>& allies = {}, int healingAnimation = 0) override;
    void animateEnemyDeath(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* enemyDead, Character* currentPlayer, const std::vector<Character*>& allies, const std::vector<std::string>& drops = {}) override;
    void updateScreenStatic(const std::string& combatTitle, const std::vector<Character*>& enemies, Character* currentPlayer, const std::vector<Character*>& allies, bool animateEntrance = false, std::function<void(std::vector<std::string>&)> callbackOverlay = nullptr) override;
    
    void addFixedMessage(const std::string& msg) override;
    void cleanMessagesFixed() override;
    void configureContext3D(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) override;
    void setShiftVisible(int shift, const std::string& name) override;
    void selectHUDAlly(Character* currentPlayer, const std::vector<Character*>& allies) override;

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

    // [PT-BR] Instancia Singleton compartilhada
    // [EN-US] Shared singleton instance
    static IDECombatScreen& instance() {
        static IDECombatScreen s_instance;
        return s_instance;
    }

private:
    void renderCombatFrame(
        const std::string& combatTitle,
        const std::vector<Character*>& enemies,
        Character* currentPlayer,
        const std::vector<std::string>& activePanelLines = {},
        Character* targetAnimation = nullptr,
        int frameAnimation = 0,
        bool isHealing = false,
        bool isDeath = false,
        int damageAnimation = -1
    );

    std::vector<std::string> m_fixedMessages;
};

