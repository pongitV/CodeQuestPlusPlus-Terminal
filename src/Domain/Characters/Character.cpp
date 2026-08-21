#include "Domain/Characters/Character.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <cassert>

#include "Domain/Characters/Classes/BaseClass.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Domain/Characters/Classes/NecroClone.h"
#include "Core/Utils/Constants.h"
#include "UI/Screens/Combat/CombatScreen.h"
#include "Core/Utils/Appearance.h"
#include "Core/Engine/Debug.h"

std::unordered_set<Character*> Character::charactersAssets;

bool Character::isValid(Character* p) {
    return charactersAssets.find(p) != charactersAssets.end();
}

Character::Character(const Character& others)
    : characterName(others.characterName),
      lifeCurrent(others.lifeCurrent),
      race(std::make_unique<RaceClone>(others.race ? others.race->getRaceName() : "Desconhecido", others.race ? others.race->getAppearanceRace() : std::vector<std::string>())),
      classObj(std::make_unique<ClassClone>()),
      finalStats(others.finalStats),
      backpack(std::make_unique<Inventory>()),
      itemSelectedForUse(nullptr),
      levelSystem(std::make_unique<LevelSystem>(others.levelSystem->getLevel(), others.levelSystem->getCurrentXp(), others.levelSystem->getXpForRise()))
{
    system = others.system;
    
    combat.isDefending = others.combat.isDefending;
    // [PT-BR] Almas coletadas nao sao duplicadas na copia
    // [EN-US] Collected souls are not duplicated during copy
    combat.defenseRecharge = others.combat.defenseRecharge;
    combat.rechargeSkill = others.combat.rechargeSkill;
    combat.jumpShiftEnemy = others.combat.jumpShiftEnemy;
    combat.skillCanceled = others.combat.skillCanceled;
    combat.deathLively = others.combat.deathLively;
    combat.multiplierCurrent = others.combat.multiplierCurrent;
    combat.totalCureReceived = others.combat.totalCureReceived;
    combat.lifeMaximumFixed = others.combat.lifeMaximumFixed;
    combat.cooldownsAssets = others.combat.cooldownsAssets;

    cache_ = others.cache_;
    charactersAssets.insert(this);

    // [PT-BR] Copia dos Itens (mantem o inventario duplicando as instancias)
    // [EN-US] Copies Items (maintains inventory by cloning instances)
    for (const auto& pair : others.equipment) {
        if (pair.second) {
            auto copyItem = ItemFactory::createItem(Appearance::removeANSIColors(pair.second->getItemName()));
            if (copyItem) { 
                this->equipment[pair.first] = copyItem.get(); 
                this->backpack->addItem(std::move(copyItem)); 
            }
        }
    }
    updateCacheIfNecessary();
}

Character::Character(const std::string& name, std::unique_ptr<BaseRace> chosenRace, std::unique_ptr<BaseClass> chosenClass)
    : characterName(name),
      lifeCurrent(0),
      race(std::move(chosenRace)),
      classObj(std::move(chosenClass)),
      finalStats{ 0, 0, 0, 0, 0, 0, 0 },
      backpack(std::make_unique<Inventory>()),
      itemSelectedForUse(nullptr),
      levelSystem(std::make_unique<LevelSystem>(1, 0, Constants::BASE_XP_TO_LEVEL_UP))
{
    auto receiveEquipKit = [this](std::vector<std::unique_ptr<Item>> kit) {
        for (auto& itemUnique : kit) {
            Item* ptr = itemUnique.get();
            this->backpack->addItem(std::move(itemUnique)); 
            this->equipItem(ptr);            
        }
    };

    receiveEquipKit(this->classObj->getEquipmentClass());
    receiveEquipKit(this->race->getEquipmentRace());

    calculateAttributes();
    charactersAssets.insert(this);
}

Character::~Character() 
{
    charactersAssets.erase(this);
}  

std::unique_ptr<Character> Character::clone() const {
    return std::make_unique<Character>(*this);
}

void Character::climbAttributes(double factor) {
    finalStats.health = std::max(1, static_cast<int>(finalStats.health * factor));
    finalStats.strength = static_cast<int>(finalStats.strength * factor);
    finalStats.dexterity = static_cast<int>(finalStats.dexterity * factor);
    finalStats.resistance = static_cast<int>(finalStats.resistance * factor);
    finalStats.constitution = static_cast<int>(finalStats.constitution * factor);
    finalStats.intelligence = static_cast<int>(finalStats.intelligence * factor);
    finalStats.wisdom = static_cast<int>(finalStats.wisdom * factor);
    combat.lifeMaximumFixed = finalStats.health; 
    forceCacheRecalculation();
    lifeCurrent = getMaxHealth();
}

void Character::addSoul(std::unique_ptr<Character> soul) { combat.soulsCollected.push_back(std::move(soul)); }

std::vector<std::unique_ptr<Character>>& Character::getSouls() { return combat.soulsCollected; }

size_t Character::getSoulCount() const { return combat.soulsCollected.size(); }

std::unique_ptr<Character> Character::removeSoul(int index) {
    if (index < 0 || index >= static_cast<int>(combat.soulsCollected.size())) return nullptr;
    auto soul = std::move(combat.soulsCollected[index]);
    combat.soulsCollected.erase(combat.soulsCollected.begin() + index);
    return soul;
}

int* Character::getPointerAttributeStatic(AttributeType attribute) {
    switch (attribute) {
        case AttributeType::Strength: return &finalStats.strength;
        case AttributeType::Dexterity: return &finalStats.dexterity;
        case AttributeType::Resistance: return &finalStats.resistance;
        case AttributeType::Constitution: return &finalStats.constitution;
        case AttributeType::Intelligence: return &finalStats.intelligence;
        case AttributeType::Wisdom: return &finalStats.wisdom;
        default: return nullptr;
    }
}

bool Character::levelUp(AttributeType attribute)
{
    if (levelSystem->getCurrentXp() < levelSystem->getXpForRise()) return false;

    if (attribute == AttributeType::Life) {
        finalStats.health += Constants::HEALTH_GAIN_PER_LEVEL;
        lifeCurrent += Constants::HEALTH_GAIN_PER_LEVEL;
    } else if (int* attr = getPointerAttributeStatic(attribute)) {
        *attr += Constants::ATTRIBUTE_GAIN_PER_LEVEL;
    } else {
        return false;
    }

    levelSystem->setCurrentXp(levelSystem->getCurrentXp() - levelSystem->getXpForRise());
    levelSystem->setXpForRise(static_cast<int>(std::min(levelSystem->getXpForRise() * Constants::XP_MULTIPLIER_PER_LEVEL, Constants::MAX_XP)));
    levelSystem->setLevel(levelSystem->getLevel() + 1);
    cache_.dirty = true;
    return true;
}

void Character::changeStaticAttribute(AttributeType attribute, int value)
{
    if (int* attr = getPointerAttributeStatic(attribute)) {
        *attr = std::max(0, *attr + value);
        cache_.dirty = true;
    }
}

void Character::reduceCooldowns()
{
    if (combat.defenseRecharge) combat.defenseRecharge = false;
    if (combat.rechargeSkill) combat.rechargeSkill = false;
    if (combat.cooldownsAssets.empty()) return;
    for (auto& pair : combat.cooldownsAssets)
    {
        if (pair.second > 0) pair.second--;
    }
}

void Character::prepareForNewBattle()
{
    combat.reset();
    combat.lifeMaximumFixed = getMaxHealth();
    cleanEffects();
    
    if (getArmor() && getArmor()->hasProperty(Property::AdaptationArmor)) {
        addEffect(std::make_unique<AdaptationWheelEffect>());
    }
}

void Character::calculateAttributes()
{
    this->finalStats.addAttributes(race->getAttributesRace());
    this->finalStats.addAttributes(classObj->getAttributesClass());
    this->lifeCurrent = getMaxHealth();
    cache_.dirty = true;
}

void Character::updateCacheIfNecessary() const {
    if (!cache_.dirty) return;
    
    double mult = system.difficultyMultiplier;
    auto applyMult = [mult](int val) { return static_cast<int>(val * mult); };

    cache_.lifeMaximum = applyMult(finalStats.health);
    cache_.strength = applyMult(finalStats.strength);
    cache_.resistance = applyMult(finalStats.resistance);
    cache_.constitution = applyMult(finalStats.constitution);
    cache_.intelligence = applyMult(finalStats.intelligence);
    cache_.wisdom = applyMult(finalStats.wisdom);

    int penalty = getArmor() ? (getArmor()->getReductionFixed() / 3) : 0;
    if (getArmor() && getArmor()->getItemName() == "Armadura de bau") penalty = 10;
    if (classObj) penalty = classObj->processPenaltyArmorPassiveArcher(penalty);
    
    int dexterityBase = static_cast<int>(finalStats.dexterity * mult);
    int dexterityEnd = dexterityBase - penalty;
    cache_.dexterity = dexterityEnd > 0 ? dexterityEnd : 0;

    int bonusArmor = getArmor() ? getArmor()->getReductionFixed() : 0;
    int reduction = cache_.resistance + bonusArmor;
    
    double percentageReduction = cache_.constitution / 100.0;
    if (percentageReduction > 0.50) percentageReduction = 0.50;
    cache_.reductionPercentage = static_cast<int>(reduction * (1.0 - percentageReduction));

    cache_.dirty = false;
}

void Character::setMultiplier(double newMultiplier) 
{ 
    if (classObj) {
        combat.multiplierCurrent = classObj->processMultiplierBuffPassiveBard(newMultiplier);
    } else {
        combat.multiplierCurrent = newMultiplier;
    }
}

void Character::applyMultiplierDifficulty(double mult)
{
    if (mult <= 1.0) return;
    system.difficultyMultiplier = mult;
    cache_.dirty = true;
    this->lifeCurrent = getMaxHealth();
}

void Character::modifyHealth(int value) 
{
    assert(this->classObj != nullptr && "Erro de Integridade: A classe do personagem nao deve ser nula ao modificar a vida!");
    if (value > 0 && classObj) value = classObj->processCurePassiveBard(value);

    int lifeBefore = this->lifeCurrent;
    this->lifeCurrent = std::clamp(this->lifeCurrent + value, 0, getMaxHealth());

    if (this->lifeCurrent > lifeBefore) 
    {
        combat.totalCureReceived += (this->lifeCurrent - lifeBefore);
    }
}

const StatusEffect* Character::findEffect(EffectID id) const {
    auto it = std::find_if(effectsAssets.begin(), effectsAssets.end(), [id](const auto& ef) {
        return ef->getID() == id;
    });
    return it != effectsAssets.end() ? it->get() : nullptr;
}

bool Character::ownsEffect(EffectID id) const {
    return findEffect(id) != nullptr;
}

int Character::getShiftsEffect(EffectID id) const {
    const StatusEffect* ef = findEffect(id);
    return ef ? ef->getRemainingTurns() : 0;
}

void Character::showStatus() const 
{
    std::cout << "[" << characterName << "] HP: " << lifeCurrent << "/" << getMaxHealth() << std::endl;
}

std::string Character::getClassName() const 
{
    return this->classObj->getClassName();
}

TypeClass Character::getTypeClass() const 
{
    if (this->classObj) return this->classObj->getTypeClass();
    return TypeClass::None;
}

TypeRace Character::getTypeRace() const 
{
    if (this->race) return this->race->getTypeRace();
    return TypeRace::None;
}

void Character::equipItem(Item* item)
{
    if (item == nullptr) return;
    if (item->getType() == EquipmentType::WEAPONS) this->equipment[SlotEquipment::MAIN_HAND] = item;
    else if (item->getType() == EquipmentType::SHIELD) this->equipment[SlotEquipment::OFF_HAND] = item;
    else if (item->getType() == EquipmentType::ARMOR)
    {
        this->equipment[SlotEquipment::ARMOR] = item;
        if (combat.lifeMaximumFixed > 0 && item->hasProperty(Property::AdaptationArmor)) {
            if (!ownsEffect(EffectID::AdaptationWheel)) {
                addEffect(std::make_unique<AdaptationWheelEffect>());
            }
        }
    }
    else if (item->getType() == EquipmentType::CONSUMABLE) this->equipment[SlotEquipment::CONSUMABLE] = item;
    cache_.dirty = true;
}

BaseRace* Character::getRace() const 
{
    return this->race.get();
}

BaseClass* Character::getClass() const 
{
    return this->classObj.get();
}

TypeAttack Character::getTypeAttack() const 
{
    if (this->classObj) return this->classObj->getTypeAttack();
    return TypeAttack::UNIQUE;
}

bool Character::classSkillConsumesTurn() const 
{
    if (this->classObj) return this->classObj->skillConsumeShift();
    return true;
}

int Character::calculateDefenseBase(int damageGross, int damagePiercing) {
    int damageWithoutDrilling = std::max(0, damageGross - damagePiercing);

    updateCacheIfNecessary();

    int finalDamage = static_cast<int>(damageWithoutDrilling - cache_.reductionPercentage);
    if (finalDamage < 1 && damageWithoutDrilling > 0) finalDamage = 1;
    else if (damageWithoutDrilling == 0) finalDamage = 0;

    return finalDamage + damagePiercing;
}

DamageResult Character::receiveDamage(int damageGross, int damagePiercing, int damageReducedParry, IAttacker* attacker, bool applyPassive) {
    DamageResult result;

    if (Debug::isGodModeActive && getTypeClass() != TypeClass::None) {
        result.finalDamage = 0;
        return result;
    }

    int finalDamage = calculateDefenseBase(damageGross, damagePiercing);

    for (auto& effect : effectsAssets) {
        finalDamage = effect->processReceivedDamage(finalDamage);
    }

    finalDamage = std::max(0, finalDamage - damageReducedParry);

    if (combat.isDefending && getShield() != nullptr) {
        Item* shield = getShield();
        result.damageBlocked = shield->getReductionDamageFixedShield();
        finalDamage = std::max(0, finalDamage - result.damageBlocked);

        shield->reduceDurability(1);
        if (shield->getDurabilityCurrentShield() <= 0) {
            result.shieldBroken = true;
            result.brokenShieldName = shield->getItemName();
            backpack->removeItem(shield);
            unequipShield();
        }
    }

    if (applyPassive && race) finalDamage = race->processDamageDefensive(finalDamage, this);
    
    if (attacker) finalDamage = attacker->ensureDamageMinimum(finalDamage);

    if (finalDamage > 0) modifyHealth(-finalDamage);

    result.finalDamage = finalDamage;
    return result;
}

void Character::addEffect(std::unique_ptr<StatusEffect> effect) {
    effect->onEnterMap(this);
    if (processingEffects) {
        effectsQueueAddition.push_back(std::move(effect));
    } else {
        effectsAssets.push_back(std::move(effect));
    }
    cache_.dirty = true;
}

void Character::processEffectsHomeShift() {
    processingEffects = true;
    for (auto& ef : effectsAssets) {
        ef->applyTurnStart(this);
        ef->decrementTurn();
    }

    effectsAssets.erase(
        std::remove_if(effectsAssets.begin(), effectsAssets.end(),
            [this](const std::unique_ptr<StatusEffect>& ef) {
                if (ef->expired()) {
                    ef->onExitMap(this);
                    cache_.dirty = true;
                    return true;
                }
                return false;
            }),
        effectsAssets.end()
    );
    processingEffects = false;

    for (EffectID id : effectsQueueRemoval) {
        removeEffect(id);
    }
    effectsQueueRemoval.clear();

    for (auto& ef : effectsQueueAddition) {
        effectsAssets.push_back(std::move(ef));
    }
    effectsQueueAddition.clear();
}

void Character::cleanEffects() {
    for (auto& ef : effectsAssets) {
        // [PT-BR] Garante que os atributos modificados (ex: Forca e Destreza) sejam restaurados ao remover o efeito
        // [EN-US] Ensures modified attributes (e.g., Strength and Dexterity) are restored upon effect removal
        ef->onExitMap(this);
    }
    effectsAssets.clear();
    effectsQueueAddition.clear();
    effectsQueueRemoval.clear();
    cache_.dirty = true;
}

void Character::removeEffect(EffectID id) {
    if (processingEffects) {
        effectsQueueRemoval.push_back(id);
        return;
    }
    auto it = std::find_if(effectsAssets.begin(), effectsAssets.end(),
        [id](const std::unique_ptr<StatusEffect>& ef) {
            return ef->getID() == id;
        });
    if (it != effectsAssets.end()) {
        (*it)->onExitMap(this);
        effectsAssets.erase(it);
        cache_.dirty = true;
    }
}

bool Character::canAct(std::string& reasonDisability) const {
    auto it = std::find_if(effectsAssets.begin(), effectsAssets.end(), [](const auto& ef) {
        return ef->preventsAction();
    });
    if (it != effectsAssets.end()) {
        reasonDisability = (*it)->getName();
        return false;
    }
    return true;
}

void Character::getIDsEffectsAssets(std::vector<EffectID>& outIDs) const {
    outIDs.clear();
    outIDs.reserve(effectsAssets.size());
    std::transform(effectsAssets.begin(), effectsAssets.end(), std::back_inserter(outIDs), [](const auto& ef) {
        return ef->getID();
    });
}

void Character::executeDrops(Character* currentPlayer, std::vector<std::string>& obtainedItems, int& totalGold, int& totalXp) {
    if (race) {
        race->performDrops(this, currentPlayer, obtainedItems, totalGold, totalXp);
    }
}

int Character::ensureDamageMinimum(int damageCurrent) {
    if (getWeapons()) {
        return getWeapons()->ensureDamageMinimum(damageCurrent);
    }
    return damageCurrent;
}

std::pair<int, int> Character::calculateDamageOffensiveBase() {
    if (Debug::isGodModeActive && getTypeClass() != TypeClass::None) {
        return { 999999, 999999 };
    }

    double attributeMultiplier = getMultiplier();

    int weaponPhysicalDamage = 1;
    int weaponMagicalDamage = 0;
    int piercingCurrent = 0;

    if (getWeapons()) 
    {
        weaponPhysicalDamage = getWeapons()->getPhysicsDamage();
        weaponMagicalDamage = getWeapons()->getMagicalDamage();

        if (getWeapons()->hasProperty(Property::Magic)) {
            int magicBonus = weaponPhysicalDamage / 2;
            double scaledBonus = magicBonus * (1.0 + (getWisdom() / 100.0));
            piercingCurrent = static_cast<int>(scaledBonus * attributeMultiplier);
        }
    }

    int strengthEffective = getStrength();
    int dexterityEffective = getDexterity();
    int intelligenceEffective = getIntelligence();
    int wisdomEffective = getWisdom();

    if (weaponPhysicalDamage == 0 && weaponMagicalDamage > 0) {
        strengthEffective /= 10; dexterityEffective /= 10;
    } else if (weaponPhysicalDamage > 0 && weaponMagicalDamage == 0) {
        intelligenceEffective /= 10; wisdomEffective /= 10;
    }

    int damagePhysicalCalculated = std::max(0, static_cast<int>((weaponPhysicalDamage + strengthEffective) * (1.0 + (dexterityEffective / 100.0))));
    int damageMagicianCalculated = std::max(0, static_cast<int>((weaponMagicalDamage + intelligenceEffective) * (1.0 + (wisdomEffective / 100.0))));
    
    int total = std::max(1, damagePhysicalCalculated + damageMagicianCalculated);
    int finalTotal = static_cast<int>(total * attributeMultiplier);
    int finalPiercing = piercingCurrent;

    if (getWeapons() && getWeapons()->hasProperty(Property::IgnoreDefense)) {
        finalPiercing = finalTotal;
    }

    if (race && race->ignoreShield()) {
        finalPiercing = finalTotal;
    }

    if (ownsEffect(EffectID::SureStrike)) {
        finalTotal *= 2;
        finalPiercing *= 2;
        removeEffect(EffectID::SureStrike);
    }

    return { finalTotal, finalPiercing };
}

void Character::finishBattle() { 
    combat.lifeMaximumFixed = 0; 
    if (system.ownsRegenerationTroll && lifeCurrent > 0 && lifeCurrent < getMaxHealth()) {
        modifyHealth(getMaxHealth());
        std::cout << "\n" << CombatScreen::combatMargin() << Appearance::color(Color::GREEN) << "[SISTEMA]: Seu Orgao regenerador curou completamente suas feridas apos a batalha!" << Appearance::color(Color::RESET) << "\n";
        InputControl::waitForEnter();
    }
}

bool Character::isBoss() const {
    TypeRace t = getTypeRace();
    return (t == TypeRace::Mahoraga || 
            t == TypeRace::ExiledOrc || 
            t == TypeRace::Troll || 
            t == TypeRace::ForestAbomination);
}
