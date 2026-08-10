/*
 * Arquivo: Character.h
 * Proposito: Classe base central do personagem (Jogador e Inimigos), atributos, status e inventario.
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <map>

#include "Domain/EquipmentSlot.h"
#include "Domain/Interfaces/IAttacker.h"
#include "Domain/Interfaces/IDamageable.h"

#include "Core/Engine/Status.h"
#include "Systems/Inventory/Inventory.h"
#include "Core/Utils/Appearance.h"
#include "Domain/LevelSystem.h"

/*
 * Estrutura de atributos base e finais de um personagem.
 */
struct Attributes 
{
    int health;       // Pontos de vida (HP) maximos do personagem
    int strength;     // Influencia o dano de ataques fisicos frontais e armas pesadas
    int dexterity;    // Define a ordem do turno, dano de armas ageis e acerto critico/esquiva
    int resistance;   // Reduz dano fisico recebido e atua como requisito para escudos pesados
    int constitution; // Vitalidade geral, reduz efetividade de debuffs e atua como requisito para armaduras
    int intelligence; // Multiplicador base do dano magico e requisito para usar cajados/varinhas
    int wisdom;       // Aumenta atributos magicos secundarios, forca das curas e defesa magica

    void addAttributes(const Attributes& other) 
    {
        this->health += other.health;
        this->strength += other.strength;
        this->dexterity += other.dexterity;
        this->resistance += other.resistance;
        this->constitution += other.constitution;
        this->intelligence += other.intelligence;
        this->wisdom += other.wisdom;
    }
};

enum class AttributeType 
{
    Life = 1,
    Strength,
    Dexterity,
    Resistance,
    Constitution,
    Intelligence,
    Wisdom
};

enum class TypeAttack 
{
    UNIQUE,
    AREA
};

class BaseRace;   
class BaseClass; 
enum class TypeClass;
enum class SkillID;
enum class TypeRace;

enum class GameDifficulty 
{
    Easy = 1,
    Normal = 2,
    Difficult = 3
};

/*
 * Classe central do jogo que representa qualquer entidade viva (Jogador, Inimigos, NPCs).
 * Agrega status, atributos, inventario e logica de persistencia e interacao.
 */
class Character : public IAttacker, public IDamageable
{
private:
    struct CombatControl {
        bool isDefending = false;
        std::vector<std::unique_ptr<Character>> soulsCollected;
        bool defenseRecharge = false;
        bool rechargeSkill = false;
        bool jumpShiftEnemy = false;
        bool skillCanceled = false;
        bool deathLively = false;
        double multiplierCurrent = 1.0;
        int totalCureReceived = 0;
        int lifeMaximumFixed = 0;
        std::unordered_map<SkillID, int> cooldownsAssets;
        
        void reset() {
            isDefending = false;
            defenseRecharge = false;
            rechargeSkill = false;
            jumpShiftEnemy = false;
            skillCanceled = false;
            deathLively = false;
            multiplierCurrent = 1.0;
            totalCureReceived = 0;
            lifeMaximumFixed = 0;
            cooldownsAssets.clear();
        }
    };

    struct ControlSystem {
        bool returnToMenuRequested = false;
        bool labyrinthUnlocked = false;
        bool canRevive = true;
        bool parryActivated = false;
        bool parryModern = true;
        bool ownsRegenerationTroll = false;
        bool godModeActive = false;
        bool noclipActive = false;
        bool isMinion = false;
        GameDifficulty currentDifficulty = GameDifficulty::Normal;
        double difficultyMultiplier = 1.0;
        char iconPlayer = '@';
        Color colorPlayer = Color::GREEN;
        Color colorBackgroundTerminal = Color::RESET;
    };

    static std::unordered_set<Character*> charactersAssets;

    CombatControl combat;
    ControlSystem system;

protected:
    std::string characterName;
    int lifeCurrent;
    std::unique_ptr<BaseRace> race;
    std::unique_ptr<BaseClass> classObj;
    Attributes finalStats;
    std::unique_ptr<Inventory> backpack;

    std::vector<std::unique_ptr<StatusEffect>> effectsAssets;
    std::vector<std::unique_ptr<StatusEffect>> effectsQueueAddition;
    std::vector<EffectID> effectsQueueRemoval;
    bool processingEffects = false;

    std::map<SlotEquipment, Item*> equipment;
    Item* itemSelectedForUse;

    /*
     * Cache de getters calculados
     * ATENCAO: Esta estrutura usando 'mutable' nao e thread-safe.
     * Caso o jogo passe a utilizar multi-threading (ex: IA rodando em background), e necessario proteger com std::mutex ou std::atomic.
     */
    struct CacheAttributes {
        int lifeMaximum = 0;
        int strength = 0;
        int dexterity = 0;
        int resistance = 0;
        int constitution = 0;
        int intelligence = 0;
        int wisdom = 0;
        int reductionPercentage = 0;
        bool dirty = true;
    };
    mutable CacheAttributes cache_;
    mutable std::mutex mutexCache_; // Protege o acesso ao cache em ambientes multithread
    void updateCacheIfNecessary() const;
    
    
    
    std::unique_ptr<LevelSystem> levelSystem;


    int* getPointerAttributeStatic(AttributeType attribute);

public:
    Character(const Character& others);
    Character(const std::string& name, std::unique_ptr<BaseRace> r, std::unique_ptr<BaseClass> c);
    virtual ~Character();

    static bool isValid(Character* p);

    void calculateAttributes();
    std::unique_ptr<Character> clone() const;
    void showStatus() const;
    void modifyHealth(int value);
    void changeName(const std::string& newName) { characterName = newName; }
    void equipItem(Item* item);

    // Getters e Setters em camelCase
    std::string getName() const { return characterName; }
    int getHealth() const { return lifeCurrent; }
    int getMaxHealth() const {
        if (combat.lifeMaximumFixed > 0) return combat.lifeMaximumFixed;
        updateCacheIfNecessary(); return cache_.lifeMaximum;
    }
    int getStrength() const { updateCacheIfNecessary(); return cache_.strength; }
    int getDexterity() const { updateCacheIfNecessary(); return cache_.dexterity; }
    int getResistance() const { updateCacheIfNecessary(); return cache_.resistance; }
    int getConstitution() const { updateCacheIfNecessary(); return cache_.constitution; }
    int getIntelligence() const { updateCacheIfNecessary(); return cache_.intelligence; }
    int getWisdom() const { updateCacheIfNecessary(); return cache_.wisdom; }
    
    int getLevel() const { return levelSystem->getLevel(); }
    int getCurrentXp() const { return levelSystem->getCurrentXp(); }
    int getXpForRise() const { return levelSystem->getXpForRise(); }
    void setLevel(int newLevel) { levelSystem->setLevel(newLevel); }
    void setCurrentXp(int newXp) { levelSystem->setCurrentXp(newXp); }
    void setXpForRise(int newXpForRise) { levelSystem->setXpForRise(newXpForRise); }
    void setHealth(int newLife) { lifeCurrent = newLife; }
    void gainXp(int value) { levelSystem->gainXp(value); }
    bool canLevelUp() const { return levelSystem->canLevelUp(); }
    bool levelUp(AttributeType attribute);
    void climbAttributes(double factor);
    void addSoul(std::unique_ptr<Character> soul);
    std::vector<std::unique_ptr<Character>>& getSouls();
    size_t getSoulCount() const;
    std::unique_ptr<Character> removeSoul(int index);

    void forceCacheRecalculation() { cache_.dirty = true; }
    
    int getTotalCureReceived() const { return combat.totalCureReceived; }

    void changeStaticAttribute(AttributeType attribute, int value);
    Attributes& getFinalAttributes() { return finalStats; }

    BaseRace* getRace() const;
    BaseClass* getClass() const;
    std::string getClassName() const;
    TypeClass getTypeClass() const;
    TypeRace getTypeRace() const;
    bool isBoss() const;
    
    Item* getWeapons() const { auto it = equipment.find(SlotEquipment::MAO_MAIN); return it != equipment.end() ? it->second : nullptr; }
    Item* getShield() const { auto it = equipment.find(SlotEquipment::MAO_SECONDARY); return it != equipment.end() ? it->second : nullptr; }
    Item* getArmor() const { auto it = equipment.find(SlotEquipment::ARMOR); return it != equipment.end() ? it->second : nullptr; }
    Item* getConsumableQuickly() const { auto it = equipment.find(SlotEquipment::CONSUMABLE); return it != equipment.end() ? it->second : nullptr; }
    void unequipConsumable() { equipment.erase(SlotEquipment::CONSUMABLE); cache_.dirty = true; }
    Inventory* getInventory() const { return backpack.get(); }
    Item* getItemSelectedForUse() const { return itemSelectedForUse; }
    bool isItemEquipped(Item* item) const { 
        if (!item) return false;
        for (const auto& pair : equipment) {
            if (pair.second == item) return true;
        }
        return false;
    }

    // Verifica se a entidade esta ativamente no loop de combate
    bool isInCombat() const;

    // Define o estado da entidade para combate
    void enterCombat();

    // Limpa o estado de combate da entidade (CDs, flags)
    void leaveCombat();

    // Inicializa a entidade antes da luta
    void prepareForCombat();

    void setItemSelectedForUse(Item* item) { itemSelectedForUse = item; }

    void gainGold(int value) { backpack->addGold(value); }

    void setMultiplier(double newMultiplier);
    double getMultiplier() const { return combat.multiplierCurrent; }

    bool canUseResurrection() const { return system.canRevive; }
    void consumeResurrection() { system.canRevive = false; }

    int getCooldown(SkillID skill) const 
    {
        auto it{combat.cooldownsAssets.find(skill)};
        return (it != combat.cooldownsAssets.end()) ? it->second : 0;
    }
    void setCooldown(SkillID skill, int shifts) 
    {
        combat.cooldownsAssets[skill] = shifts;
    }
    
    bool getSkillCanceled() const { return combat.skillCanceled; }
    void setSkillCanceled(bool wasCanceled) { combat.skillCanceled = wasCanceled; }

    void setRecharge(bool emRecharge) { combat.rechargeSkill = emRecharge; }
    bool getRecharge() const { return combat.rechargeSkill; }
    void setJumpShiftEnemy(bool jumpShift) { combat.jumpShiftEnemy = jumpShift; }
    bool getJumpShiftEnemy() const { return combat.jumpShiftEnemy; }
    
    void setReturnToMenu(bool returnVal) { system.returnToMenuRequested = returnVal; }
    bool getReturnToMenu() const { return system.returnToMenuRequested; }

    void unlockMaze() { system.labyrinthUnlocked = true; }
    bool getUnlockedMaze() const { return system.labyrinthUnlocked; }

    void unlockRegenerationTroll() { system.ownsRegenerationTroll = true; }
    bool ownsRegenerationTroll() const { return system.ownsRegenerationTroll; }

    void toggleGodMode() { system.godModeActive = !system.godModeActive; }
    bool isGodMode() const { return system.godModeActive; }

    void toggleNoclip() { system.noclipActive = !system.noclipActive; }
    bool isNoclip() const { return system.noclipActive; }

    void reduceCooldowns();
    void prepareForNewBattle();
    void finishBattle();

    bool ownsEffect(EffectID id) const;
    int getShiftsEffect(EffectID id) const;
    const StatusEffect* findEffect(EffectID id) const;


    void setDefending(bool d) { combat.isDefending = d; }
    bool getDefending() const { return combat.isDefending; }
    void setRechargeDefense(bool r) { combat.defenseRecharge = r; }
    bool getRechargeDefense() const { return combat.defenseRecharge; }
    void unequipShield() { equipment.erase(SlotEquipment::MAO_SECONDARY); cache_.dirty = true; }
    void unequipWeapon() { equipment.erase(SlotEquipment::MAO_MAIN); cache_.dirty = true; }
    void unequipArmor() { equipment.erase(SlotEquipment::ARMOR); cache_.dirty = true; }

    void setDeathLively(bool m) { combat.deathLively = m; }
    bool getDeathLively() const { return combat.deathLively; }

    void setParryEnabled(bool p) { system.parryActivated = p; }
    bool getParryActivated() const { return system.parryActivated; }
    void setModernParry(bool m) { system.parryModern = m; }
    bool getParryModern() const { return system.parryModern; }

    void setAsMinion(bool minion) { system.isMinion = minion; }
    bool isMinion() const { return system.isMinion; }

    void setDifficulty(GameDifficulty d) { system.currentDifficulty = d; }
    GameDifficulty getDifficulty() const { return system.currentDifficulty; }
    void applyMultiplierDifficulty(double mult);

    void setIconPlayer(char icon) { system.iconPlayer = icon; }
    char getIconPlayer() const { return system.iconPlayer; }

    void setColorPlayer(Color color) { system.colorPlayer = color; }
    Color getColorPlayer() const { return system.colorPlayer; }

    void setColorBackgroundTerminal(Color color) { system.colorBackgroundTerminal = color; Appearance::colorBackgroundActive = color; }
    Color getColorBackgroundTerminal() const { return system.colorBackgroundTerminal; }

    TypeAttack getTypeAttack() const;
    bool classSkillConsumesTurn() const;

    void addEffect(std::unique_ptr<StatusEffect> effect);
    void processEffectsHomeShift();
    bool canAct(std::string& reasonDisability) const;

    // Preenche o vetor com os IDs de todos os efeitos ativos (evita alocacoes indesejadas)
    void getIDsEffectsAssets(std::vector<EffectID>& outIDs) const;
    void cleanEffects();
    void removeEffect(EffectID id);

    int calculateDefenseBase(int damageGross, int damagePiercing) override;
    DamageResult receiveDamage(int damageGross, int damagePiercing, int damageReducedParry, IAttacker* attacker, bool applyPassive = true) override;
    std::pair<int, int> calculateDamageOffensiveBase() override;
    int ensureDamageMinimum(int damageCurrent) override;

    virtual void executeDrops(Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp);
};
