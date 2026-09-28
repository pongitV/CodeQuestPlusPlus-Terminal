#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Domain/Items/Item.h"
#include "Domain/Characters/Character.h"

enum class TypeRace 
{
    None,
    Dwarf,
    Elf,
    Human,
    Ork,
    ExiledOrc,
    Goblin,
    Fairy,
    Slime,
    ForestAbomination,
    Mimic,
    Troll,
    Mahoraga
};

struct InfoBestiary {
    std::string map;
    std::string habitat;
    std::string lore;
    std::string factCurious;
    std::vector<std::string> drops;
    int difficulty;
};

class BaseRace {
public:
    virtual ~BaseRace() = default;

    virtual std::string getRaceName() const = 0;
    virtual TypeRace getTypeRace() const = 0;
    virtual const std::vector<std::string>& getAppearanceRace() const = 0;

    virtual Attributes getAttributesRace() const = 0;

    virtual std::string getNameSkillRace() const = 0;
    virtual std::string getDescriptionSkillRace() const = 0;

    virtual InfoBestiary getInfoBestiary() const { return {"Desconhecido", "Desconhecido", "", "", {}, 1}; }

    virtual std::vector<std::unique_ptr<Item>> getEquipmentRace() const { return {}; }

    virtual int processDamageOffensive(int damageBase, Character* /*attacker*/) {
        return damageBase;
    }

    virtual int processDamageDefensive(int finalDamage, Character* /*defender*/) {
        return finalDamage;
    }
    
    virtual void onSufferPerfectParry() {}
    virtual bool ignoreParry() const { return false; }
    virtual bool ignoreShield() const { return false; }

    virtual void performDrops(Character* /*enemy*/, Character* /*currentPlayer*/, std::vector<std::string>& /*obtainedItems*/, int& /*totalGold*/, int& /*totalXp*/) {
        // Implementacao padrao sem recompensas de drop
    }

    virtual void onCausingDamage(Character* /*attacker*/, Character* /*target*/, int /*damageDealt*/) {}

    virtual bool tryUseSkillActive(Character* /*thisEnemy*/, Character* /*target*/, int /*difficulty*/) {
        // Por padrao, inimigos nao possuem habilidades ativas que consomem o turno
        return false;
    }

private:
};
