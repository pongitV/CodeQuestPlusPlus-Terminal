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

std::unordered_set<Character*> Character::charactersAssets;

bool Character::isValid(Character* p) {
    return charactersAssets.find(p) != charactersAssets.end();
}

Character::Character(const Character& others)
    : nameCharacter(others.nameCharacter),
      lifeCurrent(others.lifeCurrent),
      race(std::make_unique<RaceClone>(others.race ? others.race->getRaceName() : "Desconhecido", others.race ? others.race->getAppearanceRace() : std::vector<std::string>())),
      classObj(std::make_unique<ClassClone>()),
      statsFinals(others.statsFinals),
      backpack(std::make_unique<Inventory>()),
      itemSelectedForUse(nullptr),
      systemDeLevel(std::make_unique<SystemDeLevel>(others.systemDeLevel->getLevel(), others.systemDeLevel->getCurrentXp(), others.systemDeLevel->getXpForRise()))
{
    system = others.system;
    
    combat.thisDefending = others.combat.thisDefending;
    // almasColetadas nao sao copiadas
    combat.rechargeDefense = others.combat.rechargeDefense;
    combat.rechargeSkill = others.combat.rechargeSkill;
    combat.jumpShiftEnemy = others.combat.jumpShiftEnemy;
    combat.skillCanceled = others.combat.skillCanceled;
    combat.deathLively = others.combat.deathLively;
    combat.multiplierCurrent = others.combat.multiplierCurrent;
    combat.cureTotalReceived = others.combat.cureTotalReceived;
    combat.lifeMaximumFixed = others.combat.lifeMaximumFixed;
    combat.cooldownsAssets = others.combat.cooldownsAssets;

    cache_ = others.cache_;
    charactersAssets.insert(this);

    // Copia dos Itens (Conforme regra: "mas possui os mesmos items")
    for (const auto& pair : others.equipment) {
        if (pair.second) {
            auto copyItem = ItemFactory::createItem(Appearance::removeANSIColors(pair.second->getItemName()));
            if (copyItem) { 
                this->equipment[pair.first] = copyItem.get(); 
                this->backpack->addItem(std::move(copyItem)); 
            }
        }
    }
    updateCacheSeNecessary();
}

Character::Character(const std::string& name, std::unique_ptr<BaseRace> chosenRace, std::unique_ptr<BaseClass> chosenClass)
    : nameCharacter(name),
      lifeCurrent(0),
      race(std::move(chosenRace)),
      classObj(std::move(chosenClass)),
      statsFinals{ 0, 0, 0, 0, 0, 0, 0 },
      backpack(std::make_unique<Inventory>()),
      itemSelectedForUse(nullptr),
      systemDeLevel(std::make_unique<SystemDeLevel>(1, 0, Constants::BASE_XP_TO_LEVEL_UP))
{
    auto receiveEEquipKit = [this](std::vector<std::unique_ptr<Item>> kit) {
        for (auto& itemUnique : kit) {
            Item* ptr = itemUnique.get();
            this->backpack->addItem(std::move(itemUnique)); 
            this->equipItem(ptr);            
        }
    };

    receiveEEquipKit(this->classObj->getEquipmentClass());
    receiveEEquipKit(this->race->getEquipmentRace());

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
    statsFinals.health = std::max(1, static_cast<int>(statsFinals.health * factor));
    statsFinals.strength = static_cast<int>(statsFinals.strength * factor);
    statsFinals.dexterity = static_cast<int>(statsFinals.dexterity * factor);
    statsFinals.resistance = static_cast<int>(statsFinals.resistance * factor);
    statsFinals.constitution = static_cast<int>(statsFinals.constitution * factor);
    statsFinals.intelligence = static_cast<int>(statsFinals.intelligence * factor);
    statsFinals.wisdom = static_cast<int>(statsFinals.wisdom * factor);
    combat.lifeMaximumFixed = statsFinals.health; 
    forceCacheRecalculation();
    lifeCurrent = getMaxHealth();
}

void Character::addSoul(std::unique_ptr<Character> soul) { combat.soulsCollected.push_back(std::move(soul)); }

std::vector<std::unique_ptr<Character>>& Character::getSouls() { return combat.soulsCollected; }

size_t Character::getNumberDeSouls() const { return combat.soulsCollected.size(); }

std::unique_ptr<Character> Character::removeSoul(int index) {
    if (index < 0 || index >= static_cast<int>(combat.soulsCollected.size())) return nullptr;
    auto soul = std::move(combat.soulsCollected[index]);
    combat.soulsCollected.erase(combat.soulsCollected.begin() + index);
    return soul;
}

int* Character::getPointerAttributeStatic(AttributeType attribute) {
    switch (attribute) {
        case AttributeType::Strength: return &statsFinals.strength;
        case AttributeType::Dexterity: return &statsFinals.dexterity;
        case AttributeType::Resistance: return &statsFinals.resistance;
        case AttributeType::Constitution: return &statsFinals.constitution;
        case AttributeType::Intelligence: return &statsFinals.intelligence;
        case AttributeType::Wisdom: return &statsFinals.wisdom;
        default: return nullptr;
    }
}

bool Character::riseDeLevel(AttributeType attribute)
{
    if (systemDeLevel->getCurrentXp() < systemDeLevel->getXpForRise()) return false;

    if (attribute == AttributeType::Life) {
        statsFinals.health += Constants::HEALTH_GAIN_PER_LEVEL;
        lifeCurrent += Constants::HEALTH_GAIN_PER_LEVEL;
    } else if (int* attr = getPointerAttributeStatic(attribute)) {
        *attr += Constants::ATTRIBUTE_GAIN_PER_LEVEL;
    } else {
        return false;
    }

    systemDeLevel->setCurrentXp(systemDeLevel->getCurrentXp() - systemDeLevel->getXpForRise());
    systemDeLevel->setXpForRise(static_cast<int>(std::min(systemDeLevel->getXpForRise() * Constants::XP_MULTIPLIER_PER_LEVEL, Constants::MAX_XP)));
    systemDeLevel->setLevel(systemDeLevel->getLevel() + 1);
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
    if (combat.rechargeDefense) combat.rechargeDefense = false;
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
    this->statsFinals.addAttributes(race->getAttributesRace());
    this->statsFinals.addAttributes(classObj->getAttributesClass());
    this->lifeCurrent = getMaxHealth();
    cache_.dirty = true;
}

void Character::updateCacheSeNecessary() const {
    if (!cache_.dirty) return;
    
    double mult = system.difficultyMultiplier;
    auto applyMult = [mult](int val) { return static_cast<int>(val * mult); };

    cache_.lifeMaximum = applyMult(statsFinals.health);
    cache_.strength = applyMult(statsFinals.strength);
    cache_.resistance = applyMult(statsFinals.resistance);
    cache_.constitution = applyMult(statsFinals.constitution);
    cache_.intelligence = applyMult(statsFinals.intelligence);
    cache_.wisdom = applyMult(statsFinals.wisdom);

    int penalty = getArmor() ? (getArmor()->getReductionFixed() / 3) : 0;
    if (getArmor() && getArmor()->getItemName() == "Armadura de bau") penalty = 10;
    if (classObj) penalty = classObj->processPenaltyArmorPassiveArcher(penalty);
    
    int dexterityBase = static_cast<int>(statsFinals.dexterity * mult);
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
        combat.cureTotalReceived += (this->lifeCurrent - lifeBefore);
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
    std::cout << "[" << nameCharacter << "] HP: " << lifeCurrent << "/" << getMaxHealth() << std::endl;
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
    if (item->getType() == EquipmentType::WEAPONS) this->equipment[SlotEquipment::MAO_MAIN] = item;
    else if (item->getType() == EquipmentType::SHIELD) this->equipment[SlotEquipment::MAO_SECONDARY] = item;
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

bool Character::skillDaClassConsumeShift() const 
{
    if (this->classObj) return this->classObj->skillConsumeShift();
    return true;
}

int Character::calculateDefenseBase(int damageGross, int damagePiercing) {
    int damageWithoutDrilling = std::max(0, damageGross - damagePiercing);

    updateCacheSeNecessary();

    int damageEnd = static_cast<int>(damageWithoutDrilling - cache_.reductionPercentage);
    if (damageEnd < 1 && damageWithoutDrilling > 0) damageEnd = 1;
    else if (damageWithoutDrilling == 0) damageEnd = 0;

    return damageEnd + damagePiercing;
}

ResultDamage Character::receiveDamage(int damageGross, int damagePiercing, int damageReducedParry, IAttacker* attacker, bool applyPassive) {
    ResultDamage result;

    int damageEnd = calculateDefenseBase(damageGross, damagePiercing);

    for (auto& ef : effectsAssets) {
        damageEnd = ef->processReceivedDamage(damageEnd);
    }

    damageEnd = std::max(0, damageEnd - damageReducedParry);

    if (combat.thisDefending && getShield() != nullptr) {
        Item* esc = getShield();
        result.damageBlocked = esc->getReductionDamageFixedShield();
        damageEnd = std::max(0, damageEnd - result.damageBlocked);

        esc->reduceDurability(1);
        if (esc->getDurabilityCurrentShield() <= 0) {
            result.shieldBroke = true;
            result.nameShieldBroken = esc->getItemName();
            backpack->removeItem(esc);
            unequipShield();
        }
    }

    if (applyPassive && race) damageEnd = race->processDamageDefensive(damageEnd, this);
    
    if (attacker) damageEnd = attacker->ensureDamageMinimum(damageEnd);

    if (damageEnd > 0) modifyHealth(-damageEnd);

    result.damageEnd = damageEnd;
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
        ef->onExitMap(this); // Garante que os atributos (como Forca e Destreza) sejam restaurados
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

bool Character::canAct(std::string& outReasonDisability) const {
    auto it = std::find_if(effectsAssets.begin(), effectsAssets.end(), [](const auto& ef) {
        return ef->preventsAction();
    });
    if (it != effectsAssets.end()) {
        outReasonDisability = (*it)->getName();
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
    double multiplierDeAttributes = getMultiplier();

    int damagePhysicalDaWeapon = 1;
    int damageMagicianDaWeapon = 0;
    int piercingCurrent = 0;

    if (getWeapons()) 
    {
        damagePhysicalDaWeapon = getWeapons()->getPhysicsDamage();
        damageMagicianDaWeapon = getWeapons()->getMagicalDamage();

        if (getWeapons()->hasProperty(Property::Magic)) {
            int bonusMagician = damagePhysicalDaWeapon / 2;
            double bonusClimbed = bonusMagician * (1.0 + (getWisdom() / 100.0));
            piercingCurrent = static_cast<int>(bonusClimbed * multiplierDeAttributes);
        }
    }

    int gallowsEffective = getStrength();
    int dexterityEffective = getDexterity();
    int intelligenceEffective = getIntelligence();
    int wisdomEffective = getWisdom();

    if (damagePhysicalDaWeapon == 0 && damageMagicianDaWeapon > 0) {
        gallowsEffective /= 10; dexterityEffective /= 10;
    } else if (damagePhysicalDaWeapon > 0 && damageMagicianDaWeapon == 0) {
        intelligenceEffective /= 10; wisdomEffective /= 10;
    }

    int damagePhysicalCalculated = std::max(0, static_cast<int>((damagePhysicalDaWeapon + gallowsEffective) * (1.0 + (dexterityEffective / 100.0))));
    int damageMagicianCalculated = std::max(0, static_cast<int>((damageMagicianDaWeapon + intelligenceEffective) * (1.0 + (wisdomEffective / 100.0))));
    
    int total = std::max(1, damagePhysicalCalculated + damageMagicianCalculated);
    int totalEnd = static_cast<int>(total * multiplierDeAttributes);
    int piercingEnd = piercingCurrent;

    if (getWeapons() && getWeapons()->hasProperty(Property::IgnoreDefense)) {
        piercingEnd = totalEnd;
    }

    if (race && race->ignoreShield()) {
        piercingEnd = totalEnd;
    }

    if (ownsEffect(EffectID::SureStrike)) {
        totalEnd *= 2;
        piercingEnd *= 2;
        removeEffect(EffectID::SureStrike);
    }

    return { totalEnd, piercingEnd };
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
