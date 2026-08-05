/*
 * Arquivo: Status.h
 * Propósito: Define o sistema de efeitos de status (buffs e debuffs) aplicáveis aos personagens.
 */

#pragma once

#include <string>

// Enumeração de identificadores de efeitos temporários de status
enum class EffectID {
    None,
    Bleeding,
    Slowness,
    Weakness,
    ResistanceBreak,
    AdaptationWheel,
    Invincible,
    HalfDamage,
    BloodSuck,
    Stun,
    AttributesBuff,
    WarCry,
    Necrosis,
    SureStrike
};

class Character;

/*
 * Classe base para representação de um efeito de status ativo.
 */
class StatusEffect {
protected:
    EffectID id;
    std::string name;
    int remainingTurns;
public:
    StatusEffect(EffectID id, const std::string& effectName, int durationTurns) : id(id), name(effectName), remainingTurns(durationTurns) {}
    virtual ~StatusEffect() = default;
    
    EffectID getID() const { return id; }
    std::string getName() const { return name; }
    int getRemainingTurns() const { return remainingTurns; }
    void decrementTurn() { remainingTurns--; }
    bool expired() const { return remainingTurns <= 0; }
    
    virtual void onEnterMap(Character* /*alvo*/) {}
    virtual void applyTurnStart(Character* /*alvo*/) {}
    virtual void onExitMap(Character* /*alvo*/) {}
    
    virtual int processReceivedDamage(int damage) { return damage; }
    virtual bool preventsAction() const { return false; }
};

class StunEffect : public StatusEffect {
public:
    StunEffect(int durationTurns) : StatusEffect(EffectID::Stun, "Atordoamento", durationTurns) {}
    bool preventsAction() const override { return true; }
};

class BloodSuckEffect : public StatusEffect {
private:
    Character* attacker;
public:
    BloodSuckEffect(int durationTurns, Character* attackingCharacter) : StatusEffect(EffectID::BloodSuck, "SugaSangue", durationTurns), attacker(attackingCharacter) {}
    void applyTurnStart(Character* target) override;
};

class SlownessEffect : public StatusEffect {
public:
    SlownessEffect(int durationTurns) : StatusEffect(EffectID::Slowness, "Lentidao", durationTurns) {}
    void onEnterMap(Character* target) override;
    void onExitMap(Character* target) override;
};

class WeaknessEffect : public StatusEffect {
private:
    int lostStrength;
public:
    WeaknessEffect(int durationTurns) : StatusEffect(EffectID::Weakness, "Fraqueza", durationTurns), lostStrength(0) {}
    void onEnterMap(Character* target) override;
    void onExitMap(Character* target) override;
};

class ResistanceBreakEffect : public StatusEffect {
private:
    int lostResistance;
    int lostConstitution;
public:
    ResistanceBreakEffect() : StatusEffect(EffectID::ResistanceBreak, "QuebraResistencia", 9999), lostResistance(0), lostConstitution(0) {}
    void onEnterMap(Character* target) override;
    void onExitMap(Character* target) override;
    void applyTurnStart(Character* target) override;
};

class BleedingEffect : public StatusEffect {
private:
    int damagePerTurn;
public:
    BleedingEffect(int durationTurns, int damage) : StatusEffect(EffectID::Bleeding, "Sangramento", durationTurns), damagePerTurn(damage) {}
    void applyTurnStart(Character* target) override;
};

class NecrosisEffect : public StatusEffect {
private:
    int damagePerTurn;
public:
    NecrosisEffect(int durationTurns, int damage) : StatusEffect(EffectID::Necrosis, "Necrose", durationTurns), damagePerTurn(damage) {}
    void applyTurnStart(Character* target) override;
};

class HalfDamageEffect : public StatusEffect {
public:
    HalfDamageEffect(int durationTurns) : StatusEffect(EffectID::HalfDamage, "MetadeDano", durationTurns) {}
    int processReceivedDamage(int damage) override;
};

class AttributesBuffEffect : public StatusEffect {
public:
    AttributesBuffEffect(int durationTurns) : StatusEffect(EffectID::AttributesBuff, "BuffAtributos", durationTurns) {}
    void onExitMap(Character* target) override;
};

class InvincibleEffect : public StatusEffect {
public:
    InvincibleEffect(int durationTurns) : StatusEffect(EffectID::Invincible, "Inviolavel", durationTurns) {}
    void onExitMap(Character* target) override;
};

class SureStrikeEffect : public StatusEffect {
public:
    SureStrikeEffect(int durationTurns) : StatusEffect(EffectID::SureStrike, "Mira Certeira", durationTurns) {}
};

class WarCryEffect : public StatusEffect {
private:
    int strengthBonus;
    int dexterityBonus;
public:
    WarCryEffect(int durationTurns, int strengthBonus, int dexterityBonus) : StatusEffect(EffectID::WarCry, "GritoDeGuerra", durationTurns), strengthBonus(strengthBonus), dexterityBonus(dexterityBonus) {}
    void onEnterMap(Character* target) override;
    void onExitMap(Character* target) override;
};

class AdaptationWheelEffect : public StatusEffect {
private:
    int bStrength = 0;
    int bDexterity = 0;
    int bResistance = 0;
    int bConstitution = 0;
    int bIntelligence = 0;
    int bWisdom = 0;
public:
    AdaptationWheelEffect() : StatusEffect(EffectID::AdaptationWheel, "Adaptacao Divina", 9999) {}
    bool preventsAction() const override { return false; }
    void applyTurnStart(Character* target) override;
    void onExitMap(Character* target) override;
    void adapt(Character* target, Character* enemy);
};
