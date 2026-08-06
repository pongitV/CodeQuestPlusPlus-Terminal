/*
 * Arquivo: Combat.cpp
 * Proposito: Implementacao do loop de combate, gerenciamento de turnos e integracao com a UI.
 */

#include "Systems/Combat/Combat.h"
#include "Systems/Combat/CombatUIImpl.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <map>
#include <chrono>

#include "Domain/Characters/Classes/BaseClass.h"
#include "Systems/Inventory/CombatInventory.h"
#include "Domain/Items/Item.h"
#include "Domain/Items/Equipment/ShieldEquipment.h"
#include "Domain/Enemies/Mahoraga.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Systems/Progression/Bestiary.h"
#include "Systems/Progression/Diary.h"
#include "Systems/Progression/Progression.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Systems/Combat/Parry.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/RandomGenerator.h"
#include "Domain/Characters/Classes/BaseClass.h"
#include "Core/Utils/InputControl.h"
#include "Core/Utils/DialogFunctions.h"
#include "Systems/Combat/Mechanics/DamageCalculator.h"
#include "Systems/Combat/Mechanics/EnemyMechanics.h"
#include "Systems/Combat/Mechanics/TurnManager.h"

namespace {
    void registerLog(const std::string& text, Color color = Color::RESET) {
        if (color != Color::RESET) {
            Appearance::registerBattleLog(Appearance::color(color) + text + Appearance::color(Color::RESET));
        } else {
            Appearance::registerBattleLog(text);
        }
    }
}

Character* g_enemyAttackerParry = nullptr;
int g_parryStatus = 0;

void Combat::resetStatisticsAdvanced() {
    stats_parriesTempted = 0;
    stats_parriesEffective = 0;
    stats_parriesPerfect = 0;
    stats_biggerDamageCaused = 0;
    stats_itemsConsumed = 0;
    stats_newDiscoveries.clear();
}

Combat::Combat(Character* playerForOCombat, std::vector<std::unique_ptr<Character>>&& enemiesForOCombat, std::unique_ptr<ICombatUI> interfaceVisual) 
    : currentPlayer(playerForOCombat), listDeEnemies(std::move(enemiesForOCombat)), quantityDeGoldObtained(0), quantityDeXpObtained(0), totalDeDamageCaused(0), totalDeDamageReceived(0), accountantDoShiftCurrent(1),
      ui(interfaceVisual ? std::move(interfaceVisual) : std::make_unique<CombatUIImpl>())
{

    int levelDeDifficulty = static_cast<int>(currentPlayer->getDifficulty());
    double multiplierDeDifficultyOfEnemies = 1.0;

    if (levelDeDifficulty == 2) {
        multiplierDeDifficultyOfEnemies = 1.5;
    } else if (levelDeDifficulty == 3) {
        multiplierDeDifficultyOfEnemies = 2.0;
    }

    for (auto& enemyCurrentPtr : this->listDeEnemies) 
    {
        enemyCurrentPtr->applyMultiplierDifficulty(multiplierDeDifficultyOfEnemies);
        enemyCurrentPtr->prepareForNewBattle();
    }
}

void Combat::set3DContext(bool mode3D, const std::vector<std::string>& matrix, float postX, float postY, float angle, const std::string& title) {
    if (ui) {
        ui->configureContext3D(mode3D, matrix, postX, postY, angle, title);
    }
}

void Combat::addAllies(std::vector<std::unique_ptr<Character>> allies)
{
    listDeAllies = std::move(allies);
}

void Combat::addAllyEmCombat(std::unique_ptr<Character> ally) {
    listDeAllies.push_back(std::move(ally));
}

Combat::~Combat()
{
    Parry::onUpdateScreen = nullptr;
    InputControl::onWaitEnterUpdate = nullptr;
}

std::string Combat::getTitleDoCombat() const
{
    std::string title = "EM COMBATE (";
    for (size_t i = 0; i < listDeEnemies.size(); ++i) {
        title += listDeEnemies[i]->getName();
        if (i < listDeEnemies.size() - 1) title += ", ";
    }
    title += ")";
    return title;
}

bool Combat::isCharacterPlayerOuAlly(Character* character) const {
    if (character == currentPlayer) return true;
    for (const auto& allyCurrent : listDeAllies) {
        if (allyCurrent.get() == character) return true;
    }
    return false;
}

std::vector<Character*> Combat::getEnemiesRaw() const
{
    std::vector<Character*> pointersEnemies(listDeEnemies.size());
    std::transform(listDeEnemies.begin(), listDeEnemies.end(), pointersEnemies.begin(), [](const std::unique_ptr<Character>& ptr) { return ptr.get(); });
    return pointersEnemies;
}

void Combat::displayScreenDeCombat(bool animateEntrance) const
{
    ui->updateScreenStatic(getTitleDoCombat(), getEnemiesRaw(), currentPlayer, getAlliesAliveRaw(), animateEntrance);
}

std::vector<Character*> Combat::getAlliesAliveRaw() const {
    std::vector<Character*> alliesAlive;
    for (const auto& ally : listDeAllies) {
        if (ally->getHealth() > 0) alliesAlive.push_back(ally.get());
    }
    return alliesAlive;
}

void Combat::prepareShiftCharacter(Character* character) {
    ui->cleanMessagesFixed();
    registerLog("");
    registerLog("═══ TURNO " + std::to_string(accountantDoShiftCurrent) + " ║ VEZ DE " + character->getName() + " ═══");
    ui->setShiftVisible(accountantDoShiftCurrent, character->getName());
    character->reduceCooldowns();
    character->processEffectsHomeShift();
}

bool Combat::executeShiftPlayerOuAlly(Character* character, bool& firstRendering, bool processEffectsHome) {
    if (processEffectsHome) {
        prepareShiftCharacter(character);
    }
    if (character->getHealth() <= 0) return false;

    if (character == currentPlayer) {
        bool cleanedAlly = false;
        for (auto& ally : listDeAllies) {
            if (ally->isMinion() && ally->getHealth() > 0) {
                int damage = std::max(1, static_cast<int>(ally->getMaxHealth() * 0.15));
                ally->modifyHealth(-damage);
                registerLog(DialogueFunctions::formatStatusMsg(ally->getName() + " perdeu " + std::to_string(damage) + " HP (decomposicao).", Color::MAGENTA));
                
                if (ally->getHealth() <= 0) {
                    registerLog(DialogueFunctions::formatStatusMsg(ally->getName() + " se decompos durante o combate", Color::RED));
                    cleanedAlly = true;
                }
            }
        }
        // Remove definitivamente da memoria os aliados que morreram pelo dreno
        if (cleanedAlly) {
            std::erase_if(listDeAllies, [](const auto& a) { return a->getHealth() <= 0; });
        }
    }

    bool shiftConsumed = false;
    bool usedInventory = false;

    while (!shiftConsumed && character->getHealth() > 0 && !listDeEnemies.empty()) {
        displayScreenDeCombat(firstRendering);
        firstRendering = false;
        processMenuDeActionsDoPlayer(character, shiftConsumed, usedInventory);
        
        cleanEnemiesDead();
        if (checkConditionDeVictoryOuDefeat()) return true; 
    }

    if (usedInventory) {
        displayScreenDeCombat();
        ui->notifyUnpreventionInventory();
    }
    return false;
}

void Combat::startCombat() 
{
    Parry::onUpdateScreen = [this]() {
        this->displayScreenDeCombat(false);
    };
    InputControl::onWaitEnterUpdate = [this]() {
        this->displayScreenDeCombat(false);
    };
    resetStatisticsAdvanced();
    currentPlayer->prepareForNewBattle();
    Appearance::cleanLogBattle();
    ui->cleanMessagesFixed();

    for (auto& ally : listDeAllies) {
        ally->prepareForNewBattle();
    }
    ui->cheerIntroductionCombat(getTitleDoCombat(), getEnemiesRaw(), currentPlayer);

    ui->clearScreen();

    int maxDexterityEnemies = ManagerShifts::calculateMaxDexterityEnemies(listDeEnemies);
    for (const auto& enemyPtr : listDeEnemies) {
        Bestiary::instance().registerFirstView(enemyPtr->getRace()->getRaceName());
        Diary::instance().registerRace(enemyPtr->getRace()->getRaceName());
        if (enemyPtr->getClassName() != "Monstro") {
            Diary::instance().registerClass(enemyPtr->getClassName());
        }
    }
    
    bool shiftExtraFirstTurn = ManagerShifts::playerHasShiftExtraNoHome(currentPlayer, maxDexterityEnemies);
    bool firstRendering = false; // Modificado, pois ja animamos na intro
    
    if (ManagerShifts::enemiesAreMoreAct(currentPlayer, maxDexterityEnemies)) {
        displayScreenDeCombat(firstRendering);
        firstRendering = false;
        
        if (ManagerShifts::enemiesHasDoubleDeAgility(currentPlayer, maxDexterityEnemies)) {
            std::string msg = DialogueFunctions::formatSystemMsg("A agilidade extrema dos inimigos (" + std::to_string(maxDexterityEnemies) + " VS " + std::to_string(currentPlayer->getDexterity()) + ") permite que eles ataquem duas vezes seguidas!", Color::RED);
            std::cout << "\n" << ui->combatMargin() << msg << "\n";
            Appearance::registerBattleLog(msg);
            InputControl::waitForEnter();

            executeShiftDeEveryoneOsEnemies();
            cleanEnemiesDead();
            if (checkConditionDeVictoryOuDefeat()) return;
            executeShiftDeEveryoneOsEnemies();
            cleanEnemiesDead();
            if (checkConditionDeVictoryOuDefeat()) return;
            
            accountantDoShiftCurrent++; // Jogador comeca no Turno 2
        } else {
            ui->notifyEnemiesMoreAct();
            executeShiftDeEveryoneOsEnemies();
            cleanEnemiesDead();
            if (checkConditionDeVictoryOuDefeat()) return;
        }
    }

    while (currentPlayer->getHealth() > 0 && !listDeEnemies.empty()) {
        // Turno do Jogador
        if (currentPlayer->getHealth() > 0) {
            if (executeShiftPlayerOuAlly(currentPlayer, firstRendering)) return;

            if (shiftExtraFirstTurn && accountantDoShiftCurrent == 1) {
                ui->notifyShiftExtra(currentPlayer->getDexterity(), maxDexterityEnemies);
                shiftExtraFirstTurn = false;
                if (executeShiftPlayerOuAlly(currentPlayer, firstRendering, false)) return;
            }
        }
        
        // Turnos dos Aliados
        for (size_t i = 0; i < listDeAllies.size(); ++i) {
            Character* ally = listDeAllies[i].get();
            if (ally->getHealth() <= 0 || listDeEnemies.empty()) continue;
            
            bool isFirstRender = false;
            if (executeShiftPlayerOuAlly(ally, isFirstRender)) return;
        }
        
        executeShiftDeEveryoneOsEnemies();
        cleanEnemiesDead();
        if (checkConditionDeVictoryOuDefeat()) return;

        accountantDoShiftCurrent++;
    }
}

void Combat::processMenuDeActionsDoPlayer(Character* characterActing, bool& shiftWasConsumed, bool& usedInventoryNoShift)
{
    int actionChosen = ui->getActionDoPlayer(accountantDoShiftCurrent, characterActing, getEnemiesRaw(), currentPlayer, getAlliesAliveRaw());
    
    ui->cleanContextCharacterHUD(); // Forca reset visual ao retornar para evitar bugs de persistencia de interface

    switch (actionChosen) 
    {
        case 1: processActionAttack(characterActing, shiftWasConsumed); break;
        case 2: processActionDefend(characterActing, shiftWasConsumed); break;
        case 3: processActionSkill(characterActing, shiftWasConsumed); break;
        case 4: processActionInventory(characterActing, shiftWasConsumed, usedInventoryNoShift); break;
        case 5: ui->displayScreenAttributes(characterActing); break;
        case 6: ui->displayScreenDiary(characterActing); break;
        case 7: Appearance::displayHistoryComplete(); break;
        default: 
            ui->notifyActionInvalidates();
            break;
    }
}

void Combat::processActionAttack(Character* characterActing, bool& shiftWasConsumed)
{
    if (characterActing->getTypeAttack() == TypeAttack::AREA) 
    {
        performAttackPhysical(characterActing, nullptr, accountantDoShiftCurrent);
        shiftWasConsumed = true;
    }
    else 
    {
        int indexDoTargetChosen = ui->getTargetAttack(getTitleDoCombat(), getEnemiesRaw(), currentPlayer, getAlliesAliveRaw());
        if (indexDoTargetChosen == -1) return;

        performAttackPhysical(characterActing, listDeEnemies[indexDoTargetChosen].get(), accountantDoShiftCurrent);
        shiftWasConsumed = true;
    }
}

Item* Combat::selectShield(Character* characterActing) 
{
    std::vector<Item*> listDeShields;
    for (auto* item : characterActing->getInventory()->getAllItems()) 
    {
        if (item->getType() == EquipmentType::SHIELD) {
            listDeShields.push_back(item);
        }
    }

    if (listDeShields.empty()) 
    {
        ui->notifyWithoutShields(characterActing->getName());
        return nullptr;
    }

    int optionChosen = ui->getChooseDeShield(characterActing->getName(), listDeShields);
    return (optionChosen == 0) ? nullptr : listDeShields[optionChosen - 1];
}

void Combat::processActionDefend(Character* characterActing, bool& shiftWasConsumed)
{
    if (characterActing->getRechargeDefense()) 
    {
        ui->notifyImbalanceDefense(characterActing->getName());
        return; 
    }
    
    Item* shieldChosen = selectShield(characterActing);
    if (shieldChosen != nullptr) 
    {
        if (shieldChosen->getDurabilityCurrentShield() <= 0) {
            std::string msg = DialogueFunctions::formatSystemMsg("O escudo [" + shieldChosen->getItemName() + "] esta quebrado e nao pode ser usado!", Color::RED);
            std::cout << "\n" << ui->combatMargin() << msg << "\n";
            InputControl::waitForEnter();
            return; // Nao consome o turno
        }

        if (!shieldChosen->canBeEquippedBy(characterActing)) {
            ui->notifyRequirementNoServed(shieldChosen->getMessageRequirement());
            return;
        }

        characterActing->equipItem(shieldChosen);
        characterActing->setDefending(true);
        ui->notifyPostureDefensive(characterActing->getName(), shieldChosen->getItemName());
        shiftWasConsumed = true;
    }
}

void Combat::processActionSkill(Character* characterActing, bool& shiftWasConsumed)
{
    std::vector<Character*> targetsRaw = getEnemiesRaw();
    
    characterActing->setSkillCanceled(false);
    characterActing->getClass()->useSkillClass(this, characterActing, targetsRaw);
    
    if (characterActing->getSkillCanceled()) return;

    if (characterActing->skillDaClassConsumeShift()) shiftWasConsumed = true;
    else InputControl::waitForEnter();
}

void Combat::processActionInventory(Character* characterActing, bool& shiftWasConsumed, bool& usedInventoryNoShift)
{
    int lifeBefore = characterActing->getHealth();
    bool inventoryConsumiu = false;
    
    CombatInventory::manageInventory(characterActing, &inventoryConsumiu);
    if (inventoryConsumiu) {
        shiftWasConsumed = true;
        usedInventoryNoShift = true;
    }
    
    if (characterActing->getHealth() > lifeBefore) {
        ui->cheerCureNoPlayer(getTitleDoCombat(), getEnemiesRaw(), characterActing, currentPlayer, getAlliesAliveRaw(), characterActing->getHealth() - lifeBefore);
    }

    if (characterActing->getItemSelectedForUse() != nullptr) 
    {
        Item* itemSelected = characterActing->getItemSelectedForUse();
        
        int indexDoTargetChosen = ui->getTargetItem(getTitleDoCombat(), getEnemiesRaw(), currentPlayer, getAlliesAliveRaw());

        if (indexDoTargetChosen == -1) 
        {
            ui->notifyCancellationItem();
            characterActing->setItemSelectedForUse(nullptr);
        } 
        else 
        {
            Character* target = listDeEnemies[indexDoTargetChosen].get();
            
            itemSelected->use(characterActing, target);
            
            if (characterActing->getConsumableQuickly() == itemSelected) {
                characterActing->unequipConsumable();
                std::string nameOfthisItem = itemSelected->getItemName();
                for (auto* otherItem : characterActing->getInventory()->getAllItems()) {
                    if (otherItem != itemSelected && otherItem->getItemName() == nameOfthisItem) {
                        characterActing->equipItem(otherItem);
                        break;
                    }
                }
            }
            
            characterActing->getInventory()->removeItem(itemSelected);
            characterActing->setItemSelectedForUse(nullptr);
            shiftWasConsumed = true;
            usedInventoryNoShift = true;
            stats_itemsConsumed++;
        }
    }
}

void Combat::cleanEnemiesDead()
{
    for (auto& enemyPtr : listDeEnemies) 
    {
        if (enemyPtr->getHealth() <= 0) 
        {
                int xpBefore = quantityDeXpObtained;
                int goldBefore = quantityDeGoldObtained;
                size_t itemsBefore = obtainedItems.size();

                processDeathDeEnemy(enemyPtr.get());

                int xpDrop = quantityDeXpObtained - xpBefore;
                int goldDrop = quantityDeGoldObtained - goldBefore;
                
                std::vector<std::string> dropsDaDeath;
                if (xpDrop > 0) dropsDaDeath.push_back("+" + std::to_string(xpDrop) + " XP");
                if (goldDrop > 0) dropsDaDeath.push_back("+" + std::to_string(goldDrop) + "G");
                
                std::map<std::string, int> countItems;
                for (size_t i = itemsBefore; i < obtainedItems.size(); ++i) {
                    countItems[obtainedItems[i]]++;
                }
                for (auto const& [name, qty] : countItems) {
                    dropsDaDeath.push_back("+" + std::to_string(qty) + "x " + name);
                }

                std::vector<Character*> alliesAlive = getAlliesAliveRaw();
                ui->cheerDeathEnemy(getTitleDoCombat(), getEnemiesRaw(), enemyPtr.get(), currentPlayer, alliesAlive, dropsDaDeath);
                enemyPtr->setDeathLively(true);
                if (listDeEnemies.size() > 1) {
                    InputControl::waitForEnter();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                ui->cleanContextEnemyDeadEDrops();
        }
    }

    std::erase_if(listDeEnemies, [](const auto& enemy) { return enemy->getHealth() <= 0; });
}

void Combat::executeShiftDeEveryoneOsEnemies() 
{
    ui->cleanMessagesFixed();
    if (currentPlayer->getJumpShiftEnemy()) 
    {
        registerLog(DialogueFunctions::formatStatusMsg("Os inimigos estao atordoados e nao podem agir!", Color::GREEN));
        currentPlayer->setJumpShiftEnemy(false); 
    }
    else
    {
        std::string textShiftEnemies = "═══ TURNO " + std::to_string(accountantDoShiftCurrent) + " ║ VEZ DOS INIMIGOS ═══";
        registerLog("");
        registerLog(textShiftEnemies);
            ui->setShiftVisible(accountantDoShiftCurrent, "INIMIGOS");
            displayScreenDeCombat(false); // Forca o HUD a atualizar o nome do Turno para os inimigos antes do ataque iniciar
        for (size_t i = 0; i < listDeEnemies.size(); ++i) 
        {
            auto& enemyCurrentPtr = listDeEnemies[i];
            if (currentPlayer->getHealth() <= 0) break; // Interrompe se o jogador morrer
            
            Character* enemyCurrent = enemyCurrentPtr.get();
            enemyCurrent->processEffectsHomeShift();
            if (enemyCurrent->getHealth() <= 0) continue;

            bool acted = false;
            std::string reasonDisability;
            if (enemyCurrent->canAct(reasonDisability)) 
            {
                acted = true;

                // Logica de escolha de alvo do inimigo
                Character* target = MechanicsEnemy::chooseTarget(getAlliesAliveRaw(), currentPlayer);

                bool shiftConsumedBySkill = enemyCurrent->getRace()->tryUseSkillActive(enemyCurrent, target, static_cast<int>(currentPlayer->getDifficulty()));
                
                if (!shiftConsumedBySkill) {
                    performAttackPhysical(enemyCurrent, target, accountantDoShiftCurrent);
                }
            }
            else
            {
                registerLog(DialogueFunctions::formatStatusMsg(enemyCurrent->getName() + " esta sob efeito de " + reasonDisability + " e nao pode agir!", Color::GREEN));
            }

            if (acted && i < listDeEnemies.size() - 1 && currentPlayer->getHealth() > 0) {
                InputControl::waitForEnter("... o ataque continua ...");
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                InputControl::clearBuffer();
            }
        }
    }

    if (currentPlayer->getDefending())
    {
        currentPlayer->setDefending(false);
        currentPlayer->setRechargeDefense(true);
    }
    else if (currentPlayer->getRechargeDefense())
    {
        currentPlayer->setRechargeDefense(false);
    }



    if (currentPlayer->getRecharge()) currentPlayer->setRecharge(false);
    InputControl::waitForEnter();
}

void Combat::performAttackPhysical(Character* attackingCharacter, Character* characterDefender, int shiftCurrentDoCombat) 
{
    auto [damageBaseCalculated, damagePiercing] = CalculatorDamage::calculateDamageOffensiveBase(attackingCharacter);

    bool isAttackerPlayerOuAlly = isCharacterPlayerOuAlly(attackingCharacter);

    if (isAttackerPlayerOuAlly || static_cast<int>(currentPlayer->getDifficulty()) >= 2) 
    {
        damageBaseCalculated = attackingCharacter->getRace()->processDamageOffensive(damageBaseCalculated, attackingCharacter);
    }

    auto callbackApplyDamage = [this, shiftCurrentDoCombat](Character* attacker, Character* target, int damageGross, int piercing) {
        this->applyDamageAoTarget(attacker, target, damageGross, piercing, shiftCurrentDoCombat);
    };

    bool applyPassiveClass = isAttackerPlayerOuAlly || static_cast<int>(currentPlayer->getDifficulty()) == 3;

    attackingCharacter->getClass()->executeAttackWithPassiveDaClass(attackingCharacter, characterDefender, damageBaseCalculated, damagePiercing, listDeEnemies, callbackApplyDamage, applyPassiveClass);
}



void Combat::processPostDamage(Character* attacker, Character* target, int damageEnd, bool triedParry, bool parrySuccess) {
    std::vector<Character*> alliesAlive = getAlliesAliveRaw();

    g_enemyAttackerParry = attacker;
    g_parryStatus = 0;
    if (triedParry) {
        if (parrySuccess) {
            if (damageEnd <= 0) g_parryStatus = 1;
            else g_parryStatus = 2;
        } else {
            g_parryStatus = 3;
        }
    }

    if (damageEnd > 0) 
    {
        // ANIMACAO DO DANO NO INIMIGO (Piscar Vermelho + Flicker)
        if (!isCharacterPlayerOuAlly(target)) {
            ui->cheerDamageNoEnemy(getTitleDoCombat(), getEnemiesRaw(), target, attacker, currentPlayer, alliesAlive, damageEnd);
        }
        else {
            ui->cheerDamageNoPlayer(getTitleDoCombat(), getEnemiesRaw(), target, currentPlayer, alliesAlive, false, damageEnd);
        }

        // Aplicacao dos efeitos no acerto
        int lifeAttackerBefore = attacker->getHealth();
        
        if (attacker->getWeapons()) {
            attacker->getWeapons()->aoCauseDamage(attacker, target, damageEnd);
        }
        attacker->getRace()->aoCauseDamage(attacker, target, damageEnd);
        
        // Verifica se o atacante se curou (Ex: Passiva da Abominacao)
        if (attacker->getHealth() > lifeAttackerBefore) {
            if (!isCharacterPlayerOuAlly(attacker)) {
                ui->cheerCureNoEnemy(getTitleDoCombat(), getEnemiesRaw(), attacker, currentPlayer, alliesAlive, attacker->getHealth() - lifeAttackerBefore);
            } else {
                ui->cheerCureNoPlayer(getTitleDoCombat(), getEnemiesRaw(), attacker, currentPlayer, alliesAlive, attacker->getHealth() - lifeAttackerBefore);
            }
        }
        
        if (target->getArmor() && target->getArmor()->hasProperty(Property::AdaptationArmor)) {
            auto* ef = const_cast<StatusEffect*>(target->findEffect(EffectID::AdaptationWheel));
            if (ef) {
                auto* efWheel = dynamic_cast<AdaptationWheelEffect*>(ef);
                if (efWheel) efWheel->adapt(target, attacker);
            }
        }
    }
    else if (triedParry && parrySuccess && isCharacterPlayerOuAlly(target)) {
        ui->cheerDamageNoPlayer(getTitleDoCombat(), getEnemiesRaw(), target, currentPlayer, alliesAlive, true, damageEnd);
    } else {
        ui->updateScreenStatic(getTitleDoCombat(), getEnemiesRaw(), currentPlayer, alliesAlive);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    g_enemyAttackerParry = nullptr;
    g_parryStatus = 0;
}

void Combat::applyDamageAoTarget(Character* attackingCharacter, Character* characterTarget, int quantityDeDamageGross, int damagePiercing, int /*turnoAtualDoCombate*/) 
{
    if (characterTarget->ownsEffect(EffectID::Invincible))
    {
        std::string msgDodge = characterTarget->getName() + " evitou o ataque de " + attackingCharacter->getName();
        registerLog(DialogueFunctions::formatCombatMsg(msgDodge, Color::CYAN));
        
        std::vector<Character*> alliesAlive = getAlliesAliveRaw();
        ui->updateScreenStatic(getTitleDoCombat(), getEnemiesRaw(), currentPlayer, alliesAlive);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return;
    }

    // Logica da Quebra de Resistencia (Po Magico)
    if (attackingCharacter->getWeapons()) attackingCharacter->getWeapons()->beforeDeCauseDamage(attackingCharacter, characterTarget);

    int damageBaseMitigated = CalculatorDamage::calculateMitigationDefensive(characterTarget, quantityDeDamageGross, damagePiercing);
    int quantityDeDamageReducedFurParry = 0;
    bool triedParry = false;
    bool parryWasWellSuccessful = false;
    
    bool attackUnstoppable = attackingCharacter && attackingCharacter->getRace()->ignoreParry();

    // Logica do Parry
    if (characterTarget->getParryActivated() && !characterTarget->getDefending()) 
    {
        if (attackUnstoppable) {
            std::string msgUnstoppable = DialogueFunctions::formatCombatMsg(attackingCharacter->getName() + " desfere um ATAQUE IMPARAVEL! O Parry foi ignorado!", Color::BG_RED);
            registerLog(msgUnstoppable);
            ui->addFixedMessage(ui->combatMargin() + msgUnstoppable + "\n");
        } else {
            triedParry = true;
            parryWasWellSuccessful = Parry::tryParry(attackingCharacter, characterTarget, damageBaseMitigated, quantityDeDamageReducedFurParry);
            stats_parriesTempted++;
            if (parryWasWellSuccessful) stats_parriesEffective++;
        }
    }

    bool applyPassive = (isCharacterPlayerOuAlly(characterTarget) || static_cast<int>(currentPlayer->getDifficulty()) >= 2);

    ResultDamage res = characterTarget->receiveDamage(quantityDeDamageGross, damagePiercing, quantityDeDamageReducedFurParry, attackingCharacter, applyPassive);

    // Logica de adaptacao do Mahoraga ao ter seu ataque bloqueado por escudo
    if (res.damageBlocked > 0 && attackingCharacter->getTypeRace() == TypeRace::Mahoraga) {
        // Precisamos de um cast para chamar o metodo especifico da raca Mahoraga
        auto* mahoraga = dynamic_cast<Mahoraga*>(attackingCharacter->getRace());
        if (mahoraga) {
            mahoraga->aoHaveAttackBlockedByShield();
        }
    }

    // Burlar o limite de "minimo de 1 de dano" do sistema base caso o Parry absorva todo o impacto
    if (triedParry && parryWasWellSuccessful && quantityDeDamageReducedFurParry >= damageBaseMitigated) 
    {
        if (characterTarget == currentPlayer) stats_parriesPerfect++;
        if (res.damageEnd > 0) 
        {
            characterTarget->modifyHealth(res.damageEnd); // Restaura o HP retirado pela trava de minimo de dano
            res.damageEnd = 0; // Anula o dano para ativar a Reflexao de Parry Perfeito
        }
    }

    displayResultDoAttack(characterTarget, res.damageEnd, triedParry, parryWasWellSuccessful, res.damageBlocked, res.shieldBroke, res.nameShieldBroken);

    processPostDamage(attackingCharacter, characterTarget, res.damageEnd, triedParry, parryWasWellSuccessful);

    if (triedParry && parryWasWellSuccessful && res.damageEnd <= 0 && isCharacterPlayerOuAlly(characterTarget) && attackingCharacter) {
        attackingCharacter->getRace()->aoSufferParryPerfect();

        int damageReflected = std::max(1, (quantityDeDamageGross + damagePiercing) / 2);
        attackingCharacter->modifyHealth(-damageReflected);
        std::string attackerReflection = attackingCharacter->getName();
        auto enemiesRaw = getEnemiesRaw();
        int enemyIdx = -1;
        for (size_t k = 0; k < enemiesRaw.size(); ++k) {
            if (enemiesRaw[k] == attackingCharacter) {
                enemyIdx = static_cast<int>(k) + 1;
                break;
            }
        }
        if (enemyIdx != -1) {
            attackerReflection += "(" + std::to_string(enemyIdx) + ")";
        }
        
        std::string msgReflection = DialogueFunctions::formatCombatMsg("Reflexao! Inimigo tomou " + std::to_string(damageReflected) + " de dano", Color::YELLOW);
        registerLog(msgReflection);
        
        std::vector<Character*> alliesAlive = getAlliesAliveRaw();
        if (!isCharacterPlayerOuAlly(attackingCharacter)) {
            ui->cheerDamageNoEnemy(getTitleDoCombat(), getEnemiesRaw(), attackingCharacter, characterTarget, currentPlayer, alliesAlive, damageReflected);
            totalDeDamageCaused += damageReflected;
        } else {
            ui->cheerDamageNoPlayer(getTitleDoCombat(), getEnemiesRaw(), attackingCharacter, currentPlayer, alliesAlive, false, damageReflected);
        }
    }
}

void Combat::displayResultDoAttack(Character* target, int damageEnd, bool triedParry, bool parrySuccess, int damageBlocked, bool shieldBroke, const std::string& nameShieldBroken)
{
    bool isPlayerOuAlly = isCharacterPlayerOuAlly(target);

    if (damageBlocked > 0) {
        std::string defenseMsg = DialogueFunctions::formatCombatMsg("O escudo bloqueou " + std::to_string(damageBlocked) + " de dano!", Color::BLUE);
        registerLog(defenseMsg);
        
        if (shieldBroke) {
            std::string msgBreak = DialogueFunctions::formatCombatMsg("ALERTA: O escudo " + nameShieldBroken + " foi DESTRUIDO em pedacos e desequipado!", Color::BLUE);
            registerLog(msgBreak);
            target->unequipShield();
        }
    }

    if (isPlayerOuAlly) 
    {
        if (triedParry) {
            std::string messageParryLog = Parry::getMessageFeedback(parrySuccess, damageEnd);
            registerLog(DialogueFunctions::formatCombatMsg(messageParryLog, Color::ORANGE));
        }
        else if (damageEnd > 0) 
        {
            registerLog(DialogueFunctions::formatCombatMsg(target->getName() + " recebeu " + std::to_string(damageEnd) + " de dano", Color::ORANGE));
        }
        else if (damageEnd == 0 && target->getDefending()) 
        {
            registerLog(DialogueFunctions::formatCombatMsg("O dano foi totalmente absorvido pela defesa de " + target->getName() + "!", Color::BLUE));
        }
        
        if (damageEnd > 0 && target == currentPlayer) totalDeDamageReceived += damageEnd;
    }
    else if (damageEnd > 0) 
    {
        if (damageEnd > stats_biggerDamageCaused) stats_biggerDamageCaused = damageEnd;
        if (target != currentPlayer) totalDeDamageCaused += damageEnd;
        registerLog(DialogueFunctions::formatCombatMsg(target->getName() + " recebeu " + std::to_string(damageEnd) + " de dano", Color::RED));
    }
}

bool Combat::checkConditionDeVictoryOuDefeat() 
{
    bool isVictory = listDeEnemies.empty();
    bool isDefeat = currentPlayer->getHealth() <= 0;

    if (isVictory || isDefeat) 
    { 
        currentPlayer->cleanEffects(); // Remove buffs e debuffs ao final da batalha
        InputControl::onWaitEnterUpdate = nullptr; // Impede que o aguardarEnter da tela de vitoria/derrota redesenhe o combate
        if (isVictory) {
            ui->displayScreenVictory(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, 
                                totalDeDamageReceived, currentPlayer->getCureTotalReceived(), accountantDoShiftCurrent, 
                                obtainedItems, enemiesDefeated, stats_parriesPerfect, stats_biggerDamageCaused, stats_parriesTempted, stats_parriesEffective, stats_itemsConsumed, stats_newDiscoveries);
        } else {
            ui->displayScreenDefeat(currentPlayer, quantityDeGoldObtained, quantityDeXpObtained, totalDeDamageCaused, totalDeDamageReceived, currentPlayer->getCureTotalReceived(), accountantDoShiftCurrent); 
        }
        currentPlayer->finishBattle();
        return true; 
    }
    return false;
}

void Combat::processDeathDeEnemy(Character* enemy)
{
    registerLog(DialogueFunctions::formatCombatMsg(enemy->getName() + " derrotado!", Color::RED));
    enemiesDefeated.push_back(enemy->getName());

    std::string raceName = enemy->getRace()->getRaceName();
    if (!Bestiary::instance().jaDefeated(raceName)) {
        stats_newDiscoveries.push_back("Novo monstro catalogado: " + raceName);
    }

    Bestiary::instance().registerDefeat(enemy->getRace()->getRaceName());

    if (enemy->getName() == "Mahoraga") {
        Progression::instance().setFlag(Flags::Forest_MahoragaDefeated, true);
    }

    // Passiva do Necromante: Coletar alma
    if (currentPlayer->getTypeClass() == TypeClass::NECROMANCER) {
        currentPlayer->addSoul(enemy->clone());
        std::string msg = DialogueFunctions::formatSkillMsg("Voce coletou a alma de " + enemy->getName() + "!", Color::MAGENTA);
        registerLog(msg);
    }

    registerLog("═══ DROPS ═══", Color::YELLOW);

    size_t itemsBefore = obtainedItems.size();
    enemy->executeDrops(currentPlayer, obtainedItems, quantityDeGoldObtained, quantityDeXpObtained);
    for (size_t i = itemsBefore; i < obtainedItems.size(); ++i) {
        if (!Bestiary::instance().jaCollectedDrop(raceName, obtainedItems[i])) {
            stats_newDiscoveries.push_back("Novo drop descoberto: " + obtainedItems[i]);
        }
        Bestiary::instance().registerDrop(enemy->getRace()->getRaceName(), obtainedItems[i]);
    }
}
