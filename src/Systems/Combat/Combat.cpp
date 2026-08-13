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
    parriesAttempted = 0;
    effectiveParries = 0;
    perfectParries = 0;
    highestDamageCaused = 0;
    stats_itemsConsumed = 0;
    newDiscoveries.clear();
}

Combat::Combat(Character* combatPlayer, std::vector<std::unique_ptr<Character>>&& combatEnemies, std::unique_ptr<ICombatUI> interfaceVisual) 
    : currentPlayer(combatPlayer), enemies(std::move(combatEnemies)), goldObtained(0), xpObtained(0), totalDamageCaused(0), totalDamageReceived(0), currentTurnCount(1),
      ui(interfaceVisual ? std::move(interfaceVisual) : std::make_unique<CombatUIImpl>())
{

    int difficultyLevel = static_cast<int>(currentPlayer->getDifficulty());
    double enemyDifficultyMultiplier = 1.0;

    if (difficultyLevel == 2) {
        enemyDifficultyMultiplier = 1.5;
    } else if (difficultyLevel == 3) {
        enemyDifficultyMultiplier = 2.0;
    }

    for (auto& enemyCurrentPtr : this->enemies) 
    {
        enemyCurrentPtr->applyMultiplierDifficulty(enemyDifficultyMultiplier);
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
    this->allies = std::move(allies);
}

void Combat::addAllyInCombat(std::unique_ptr<Character> ally) {
    allies.push_back(std::move(ally));
}

Combat::~Combat()
{
    Parry::onUpdateScreen = nullptr;
    InputControl::onWaitEnterUpdate = nullptr;
}

std::string Combat::getCombatTitle() const
{
    std::string title = "EM COMBATE (";
    for (size_t i = 0; i < enemies.size(); ++i) {
        title += enemies[i]->getName();
        if (i < enemies.size() - 1) title += ", ";
    }
    title += ")";
    return title;
}

bool Combat::isPlayerOrAlly(Character* character) const {
    if (character == currentPlayer) return true;
    for (const auto& allyCurrent : allies) {
        if (allyCurrent.get() == character) return true;
    }
    return false;
}

std::vector<Character*> Combat::getEnemiesRaw() const
{
    std::vector<Character*> pointersEnemies(enemies.size());
    std::transform(enemies.begin(), enemies.end(), pointersEnemies.begin(), [](const std::unique_ptr<Character>& ptr) { return ptr.get(); });
    return pointersEnemies;
}

void Combat::displayCombatScreen(bool animateEntrance) const
{
    ui->updateScreenStatic(getCombatTitle(), getEnemiesRaw(), currentPlayer, getAlliesAliveRaw(), animateEntrance);
}

std::vector<Character*> Combat::getAlliesAliveRaw() const {
    std::vector<Character*> alliesAlive;
    for (const auto& ally : allies) {
        if (ally->getHealth() > 0) alliesAlive.push_back(ally.get());
    }
    return alliesAlive;
}

void Combat::prepareShiftCharacter(Character* character) {
    ui->cleanMessagesFixed();
    registerLog("");
    registerLog("═══ TURNO " + std::to_string(currentTurnCount) + " ║ VEZ DE " + character->getName() + " ═══");
    ui->setShiftVisible(currentTurnCount, character->getName());
    character->reduceCooldowns();
    character->processEffectsHomeShift();
}

bool Combat::executePlayerOrAllyTurn(Character* character, bool& firstRendering, bool processEffectsHome) {
    if (processEffectsHome) {
        prepareShiftCharacter(character);
    }
    if (character->getHealth() <= 0) return false;

    if (character == currentPlayer) {
        bool cleanedAlly = false;
        for (auto& ally : allies) {
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
            std::erase_if(allies, [](const auto& a) { return a->getHealth() <= 0; });
        }
    }

    bool shiftConsumed = false;
    bool usedInventory = false;

    while (!shiftConsumed && character->getHealth() > 0 && !enemies.empty()) {
        displayCombatScreen(firstRendering);
        firstRendering = false;
        processPlayerActionMenu(character, shiftConsumed, usedInventory);
        
        cleanEnemiesDead();
        if (checkVictoryOrDefeatCondition()) return true; 
    }

    if (usedInventory) {
        displayCombatScreen();
        ui->notifyUnpreventionInventory();
    }
    return false;
}

void Combat::startCombat() 
{
    Parry::onUpdateScreen = [this]() {
        this->displayCombatScreen(false);
    };
    InputControl::onWaitEnterUpdate = [this]() {
        this->displayCombatScreen(false);
    };
    resetStatisticsAdvanced();
    currentPlayer->prepareForNewBattle();
    Appearance::cleanLogBattle();
    ui->cleanMessagesFixed();

    for (auto& ally : allies) {
        ally->prepareForNewBattle();
    }
    ui->animateCombatIntro(getCombatTitle(), getEnemiesRaw(), currentPlayer);

    ui->clearScreen();

    int maxEnemyDexterity = ShiftManager::calculateMaxDexterityEnemies(enemies);
    for (const auto& enemyPtr : enemies) {
        Bestiary::instance().registerFirstView(enemyPtr->getRace()->getRaceName());
        Diary::instance().registerRace(enemyPtr->getRace()->getRaceName());
        if (enemyPtr->getClassName() != "Monstro") {
            Diary::instance().registerClass(enemyPtr->getClassName());
        }
    }
    
    bool shiftExtraFirstTurn = ShiftManager::playerHasExtraTurnAtStart(currentPlayer, maxEnemyDexterity);
    bool firstRendering = false; // Modificado, pois ja animamos na intro
    
    if (ShiftManager::enemiesActFirst(currentPlayer, maxEnemyDexterity)) {
        displayCombatScreen(firstRendering);
        firstRendering = false;
        
        if (ShiftManager::enemiesHaveDoubleAgility(currentPlayer, maxEnemyDexterity)) {
            std::string msg = DialogueFunctions::formatSystemMsg("A agilidade extrema dos inimigos (" + std::to_string(maxEnemyDexterity) + " VS " + std::to_string(currentPlayer->getDexterity()) + ") permite que eles ataquem duas vezes seguidas!", Color::RED);
            std::cout << "\n" << ui->combatMargin() << msg << "\n";
            Appearance::registerBattleLog(msg);
            InputControl::waitForEnter();

            executeTurnForAllEnemies();
            cleanEnemiesDead();
            if (checkVictoryOrDefeatCondition()) return;
            executeTurnForAllEnemies();
            cleanEnemiesDead();
            if (checkVictoryOrDefeatCondition()) return;
            
            currentTurnCount++; // Jogador comeca no Turno 2
        } else {
            ui->notifyEnemiesMoreAct();
            executeTurnForAllEnemies();
            cleanEnemiesDead();
            if (checkVictoryOrDefeatCondition()) return;
        }
    }

    while (currentPlayer->getHealth() > 0 && !enemies.empty()) {
        // Turno do Jogador
        if (currentPlayer->getHealth() > 0) {
            if (executePlayerOrAllyTurn(currentPlayer, firstRendering)) return;

            if (shiftExtraFirstTurn && currentTurnCount == 1) {
                ui->notifyShiftExtra(currentPlayer->getDexterity(), maxEnemyDexterity);
                shiftExtraFirstTurn = false;
                if (executePlayerOrAllyTurn(currentPlayer, firstRendering, false)) return;
            }
        }
        
        // Turnos dos Aliados
        for (size_t i = 0; i < allies.size(); ++i) {
            Character* ally = allies[i].get();
            if (ally->getHealth() <= 0 || enemies.empty()) continue;
            
            bool isFirstRender = false;
            if (executePlayerOrAllyTurn(ally, isFirstRender)) return;
        }
        
        executeTurnForAllEnemies();
        cleanEnemiesDead();
        if (checkVictoryOrDefeatCondition()) return;

        currentTurnCount++;
    }
}

void Combat::processPlayerActionMenu(Character* characterActing, bool& shiftWasConsumed, bool& inventoryUsedThisTurn)
{
    int actionChosen = ui->getPlayerAction(currentTurnCount, characterActing, getEnemiesRaw(), currentPlayer, getAlliesAliveRaw());
    
    ui->cleanContextCharacterHUD(); // Forca reset visual ao retornar para evitar bugs de persistencia de interface

    switch (actionChosen) 
    {
        case 1: processActionAttack(characterActing, shiftWasConsumed); break;
        case 2: processActionDefend(characterActing, shiftWasConsumed); break;
        case 3: processActionSkill(characterActing, shiftWasConsumed); break;
        case 4: processInventoryAction(characterActing, shiftWasConsumed, inventoryUsedThisTurn); break;
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
        performPhysicalAttack(characterActing, nullptr, currentTurnCount);
        shiftWasConsumed = true;
    }
    else 
    {
        int targetIndexChosen = ui->getTargetAttack(getCombatTitle(), getEnemiesRaw(), currentPlayer, getAlliesAliveRaw());
        if (targetIndexChosen == -1) return;

        performPhysicalAttack(characterActing, enemies[targetIndexChosen].get(), currentTurnCount);
        shiftWasConsumed = true;
    }
}

Item* Combat::selectShield(Character* characterActing) 
{
    std::vector<Item*> shields;
    for (auto* item : characterActing->getInventory()->getAllItems()) 
    {
        if (item->getType() == EquipmentType::SHIELD) {
            shields.push_back(item);
        }
    }

    if (shields.empty()) 
    {
        ui->notifyWithoutShields(characterActing->getName());
        return nullptr;
    }

    int optionChosen = ui->chooseShield(characterActing->getName(), shields);
    return (optionChosen == 0) ? nullptr : shields[optionChosen - 1];
}

void Combat::processActionDefend(Character* characterActing, bool& shiftWasConsumed)
{
    if (characterActing->getRechargeDefense()) 
    {
        ui->notifyImbalanceDefense(characterActing->getName());
        return; 
    }
    
    Item* chosenShield = selectShield(characterActing);
    if (chosenShield != nullptr) 
    {
        if (chosenShield->getDurabilityCurrentShield() <= 0) {
            std::string msg = DialogueFunctions::formatSystemMsg("O escudo [" + chosenShield->getItemName() + "] esta quebrado e nao pode ser usado!", Color::RED);
            std::cout << "\n" << ui->combatMargin() << msg << "\n";
            InputControl::waitForEnter();
            return; // Nao consome o turno
        }

        if (!chosenShield->canBeEquippedBy(characterActing)) {
            ui->notifyUnmetRequirement(chosenShield->getMessageRequirement());
            return;
        }

        characterActing->equipItem(chosenShield);
        characterActing->setDefending(true);
        ui->notifyPostureDefensive(characterActing->getName(), chosenShield->getItemName());
        shiftWasConsumed = true;
    }
}

void Combat::processActionSkill(Character* characterActing, bool& shiftWasConsumed)
{
    std::vector<Character*> targetsRaw = getEnemiesRaw();
    
    characterActing->setSkillCanceled(false);
    characterActing->getClass()->useSkillClass(this, characterActing, targetsRaw);
    
    if (characterActing->getSkillCanceled()) return;

    if (characterActing->classSkillConsumesTurn()) shiftWasConsumed = true;
    else InputControl::waitForEnter();
}

void Combat::processInventoryAction(Character* characterActing, bool& shiftWasConsumed, bool& inventoryUsedThisTurn)
{
    int lifeBefore = characterActing->getHealth();
    bool inventoryConsumed = false;
    
    CombatInventory::manageInventory(characterActing, &inventoryConsumed);
    if (inventoryConsumed) {
        shiftWasConsumed = true;
        inventoryUsedThisTurn = true;
    }
    
    if (characterActing->getHealth() > lifeBefore) {
        ui->animateCureToPlayer(getCombatTitle(), getEnemiesRaw(), characterActing, currentPlayer, getAlliesAliveRaw(), characterActing->getHealth() - lifeBefore);
    }

    if (characterActing->getItemSelectedForUse() != nullptr) 
    {
        Item* itemSelected = characterActing->getItemSelectedForUse();
        
        int targetIndexChosen = ui->getTargetItem(getCombatTitle(), getEnemiesRaw(), currentPlayer, getAlliesAliveRaw());

        if (targetIndexChosen == -1) 
        {
            ui->notifyCancellationItem();
            characterActing->setItemSelectedForUse(nullptr);
        } 
        else 
        {
            Character* target = enemies[targetIndexChosen].get();
            
            itemSelected->use(characterActing, target);
            
            if (characterActing->getConsumableQuickly() == itemSelected) {
                characterActing->unequipConsumable();
                std::string thisItemName = itemSelected->getItemName();
                for (auto* otherItem : characterActing->getInventory()->getAllItems()) {
                    if (otherItem != itemSelected && otherItem->getItemName() == thisItemName) {
                        characterActing->equipItem(otherItem);
                        break;
                    }
                }
            }
            
            characterActing->getInventory()->removeItem(itemSelected);
            characterActing->setItemSelectedForUse(nullptr);
            shiftWasConsumed = true;
            inventoryUsedThisTurn = true;
            stats_itemsConsumed++;
        }
    }
}

void Combat::cleanEnemiesDead()
{
    for (auto& enemyPtr : enemies) 
    {
        if (enemyPtr->getHealth() <= 0) 
        {
                int xpBefore = xpObtained;
                int goldBefore = goldObtained;
                size_t itemsBefore = obtainedItems.size();

                processEnemyDeath(enemyPtr.get());

                int xpDrop = xpObtained - xpBefore;
                int goldDrop = goldObtained - goldBefore;
                
                std::vector<std::string> deathDrops;
                if (xpDrop > 0) deathDrops.push_back("+" + std::to_string(xpDrop) + " XP");
                if (goldDrop > 0) deathDrops.push_back("+" + std::to_string(goldDrop) + "G");
                
                std::map<std::string, int> countItems;
                for (size_t i = itemsBefore; i < obtainedItems.size(); ++i) {
                    countItems[obtainedItems[i]]++;
                }
                for (auto const& [name, qty] : countItems) {
                    deathDrops.push_back("+" + std::to_string(qty) + "x " + name);
                }

                std::vector<Character*> alliesAlive = getAlliesAliveRaw();
                ui->animateEnemyDeath(getCombatTitle(), getEnemiesRaw(), enemyPtr.get(), currentPlayer, alliesAlive, deathDrops);
                enemyPtr->setDeathLively(true);
                if (enemies.size() > 1) {
                    InputControl::waitForEnter();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                ui->cleanContextEnemyDeathAndDrops();
        }
    }

    std::erase_if(enemies, [](const auto& enemy) { return enemy->getHealth() <= 0; });
}

void Combat::executeTurnForAllEnemies() 
{
    ui->cleanMessagesFixed();
    if (currentPlayer->getJumpShiftEnemy()) 
    {
        registerLog(DialogueFunctions::formatStatusMsg("Os inimigos estao atordoados e nao podem agir!", Color::GREEN));
        currentPlayer->setJumpShiftEnemy(false); 
    }
    else
    {
        std::string textShiftEnemies = "═══ TURNO " + std::to_string(currentTurnCount) + " ║ VEZ DOS INIMIGOS ═══";
        registerLog("");
        registerLog(textShiftEnemies);
            ui->setShiftVisible(currentTurnCount, "INIMIGOS");
            displayCombatScreen(false); // Forca o HUD a atualizar o nome do Turno para os inimigos antes do ataque iniciar
        for (size_t i = 0; i < enemies.size(); ++i) 
        {
            auto& enemyCurrentPtr = enemies[i];
            if (currentPlayer->getHealth() <= 0) break; // Interrompe se o player morrer
            
            Character* enemyCurrent = enemyCurrentPtr.get();
            enemyCurrent->processEffectsHomeShift();
            if (enemyCurrent->getHealth() <= 0) continue;

            bool acted = false;
            std::string reasonDisability;
            if (enemyCurrent->canAct(reasonDisability)) 
            {
                acted = true;

                // Logica de escolha de alvo do inimigo
                Character* target = EnemyMechanics::chooseTarget(getAlliesAliveRaw(), currentPlayer);

                bool shiftConsumedBySkill = enemyCurrent->getRace()->tryUseSkillActive(enemyCurrent, target, static_cast<int>(currentPlayer->getDifficulty()));
                
                if (!shiftConsumedBySkill) {
                    performPhysicalAttack(enemyCurrent, target, currentTurnCount);
                }
            }
            else
            {
                registerLog(DialogueFunctions::formatStatusMsg(enemyCurrent->getName() + " esta sob efeito de " + reasonDisability + " e nao pode agir!", Color::GREEN));
            }

            if (acted && i < enemies.size() - 1 && currentPlayer->getHealth() > 0) {
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

void Combat::performPhysicalAttack(Character* attackingCharacter, Character* defenderCharacter, int currentCombatTurn) 
{
    auto [damageBaseCalculated, damagePiercing] = DamageCalculator::calculateDamageOffensiveBase(attackingCharacter);

    bool isAttackerPlayerOrAlly = isPlayerOrAlly(attackingCharacter);

    if (isAttackerPlayerOrAlly || static_cast<int>(currentPlayer->getDifficulty()) >= 2) 
    {
        damageBaseCalculated = attackingCharacter->getRace()->processDamageOffensive(damageBaseCalculated, attackingCharacter);
    }

    auto callbackApplyDamage = [this, currentCombatTurn](Character* attacker, Character* target, int damageGross, int piercing) {
        this->applyDamageToTarget(attacker, target, damageGross, piercing, currentCombatTurn);
    };

    bool applyPassiveClass = isAttackerPlayerOrAlly || static_cast<int>(currentPlayer->getDifficulty()) == 3;

    attackingCharacter->getClass()->executeAttackWithClassPassive(attackingCharacter, defenderCharacter, damageBaseCalculated, damagePiercing, enemies, callbackApplyDamage, applyPassiveClass);
}



void Combat::processPostDamage(Character* attacker, Character* target, int finalDamage, bool triedParry, bool parrySuccess) {
    std::vector<Character*> alliesAlive = getAlliesAliveRaw();

    g_enemyAttackerParry = attacker;
    g_parryStatus = 0;
    if (triedParry) {
        if (parrySuccess) {
            if (finalDamage <= 0) g_parryStatus = 1;
            else g_parryStatus = 2;
        } else {
            g_parryStatus = 3;
        }
    }

    if (finalDamage > 0) 
    {
        // ANIMACAO DO DANO NO INIMIGO (Piscar Vermelho + Flicker)
        if (!isPlayerOrAlly(target)) {
            ui->animateDamageToEnemy(getCombatTitle(), getEnemiesRaw(), target, attacker, currentPlayer, alliesAlive, finalDamage);
        }
        else {
            ui->animateDamageToPlayer(getCombatTitle(), getEnemiesRaw(), target, currentPlayer, alliesAlive, false, finalDamage);
        }

        // Aplicacao dos efeitos no acerto
        int lifeAttackerBefore = attacker->getHealth();
        
        if (attacker->getWeapons()) {
            attacker->getWeapons()->onCausingDamage(attacker, target, finalDamage);
        }
        attacker->getRace()->onCausingDamage(attacker, target, finalDamage);
        
        // Verifica se o atacante se curou (Ex: Passiva da Abominacao)
        if (attacker->getHealth() > lifeAttackerBefore) {
            if (!isPlayerOrAlly(attacker)) {
                ui->animateCureToEnemy(getCombatTitle(), getEnemiesRaw(), attacker, currentPlayer, alliesAlive, attacker->getHealth() - lifeAttackerBefore);
            } else {
                ui->animateCureToPlayer(getCombatTitle(), getEnemiesRaw(), attacker, currentPlayer, alliesAlive, attacker->getHealth() - lifeAttackerBefore);
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
    else if (triedParry && parrySuccess && isPlayerOrAlly(target)) {
        ui->animateDamageToPlayer(getCombatTitle(), getEnemiesRaw(), target, currentPlayer, alliesAlive, true, finalDamage);
    } else {
        ui->updateScreenStatic(getCombatTitle(), getEnemiesRaw(), currentPlayer, alliesAlive);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    g_enemyAttackerParry = nullptr;
    g_parryStatus = 0;
}

void Combat::applyDamageToTarget(Character* attackingCharacter, Character* targetCharacter, int grossDamage, int damagePiercing, int /*turnoAtualDoCombate*/) 
{
    if (targetCharacter->ownsEffect(EffectID::Invincible))
    {
        std::string dodgeMessage = targetCharacter->getName() + " evitou o ataque de " + attackingCharacter->getName();
        registerLog(DialogueFunctions::formatCombatMsg(dodgeMessage, Color::CYAN));
        
        std::vector<Character*> alliesAlive = getAlliesAliveRaw();
        ui->updateScreenStatic(getCombatTitle(), getEnemiesRaw(), currentPlayer, alliesAlive);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return;
    }

    // Logica da Quebra de Resistencia (Po Magico)
    if (attackingCharacter->getWeapons()) attackingCharacter->getWeapons()->beforeCausingDamage(attackingCharacter, targetCharacter);

    int damageBaseMitigated = DamageCalculator::calculateMitigationDefensive(targetCharacter, grossDamage, damagePiercing);
    int parryReducedDamage = 0;
    bool triedParry = false;
    bool parryWasWellSuccessful = false;
    
    bool attackUnstoppable = attackingCharacter && attackingCharacter->getRace()->ignoreParry();

    // Logica do Parry
    if (targetCharacter->getParryActivated() && !targetCharacter->getDefending()) 
    {
        if (attackUnstoppable) {
            std::string msgUnstoppable = DialogueFunctions::formatCombatMsg(attackingCharacter->getName() + " desfere um ATAQUE IMPARAVEL! O Parry foi ignorado!", Color::BG_RED);
            registerLog(msgUnstoppable);
            ui->addFixedMessage(ui->combatMargin() + msgUnstoppable + "\n");
        } else {
            triedParry = true;
            parryWasWellSuccessful = Parry::tryParry(attackingCharacter, targetCharacter, damageBaseMitigated, parryReducedDamage);
            parriesAttempted++;
            if (parryWasWellSuccessful) effectiveParries++;
        }
    }

    bool applyPassive = (isPlayerOrAlly(targetCharacter) || static_cast<int>(currentPlayer->getDifficulty()) >= 2);

    DamageResult res = targetCharacter->receiveDamage(grossDamage, damagePiercing, parryReducedDamage, attackingCharacter, applyPassive);

    // Logica de adaptacao do Mahoraga ao ter seu ataque bloqueado por escudo
    if (res.damageBlocked > 0 && attackingCharacter->getTypeRace() == TypeRace::Mahoraga) {
        // Precisamos de um cast para chamar o metodo especifico da raca Mahoraga
        auto* mahoraga = dynamic_cast<Mahoraga*>(attackingCharacter->getRace());
        if (mahoraga) {
            mahoraga->onAttackBlockedByShield();
        }
    }

    // Burlar o limite de "minimo de 1 de dano" do sistema base caso o Parry absorva todo o impacto
    if (triedParry && parryWasWellSuccessful && parryReducedDamage >= damageBaseMitigated) 
    {
        if (targetCharacter == currentPlayer) perfectParries++;
        if (res.finalDamage > 0) 
        {
            targetCharacter->modifyHealth(res.finalDamage); // Restaura o HP retirado pela trava de minimo de dano
            res.finalDamage = 0; // Anula o dano para ativar a Reflexao de Parry Perfeito
        }
    }

    displayAttackResult(targetCharacter, res.finalDamage, triedParry, parryWasWellSuccessful, res.damageBlocked, res.shieldBroken, res.brokenShieldName);

    processPostDamage(attackingCharacter, targetCharacter, res.finalDamage, triedParry, parryWasWellSuccessful);

    if (triedParry && parryWasWellSuccessful && res.finalDamage <= 0 && isPlayerOrAlly(targetCharacter) && attackingCharacter) {
        attackingCharacter->getRace()->onSufferPerfectParry();

        int damageReflected = std::max(1, (grossDamage + damagePiercing) / 2);
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
        
        std::string reflectionMessage = DialogueFunctions::formatCombatMsg("Reflexao! Inimigo tomou " + std::to_string(damageReflected) + " de dano", Color::YELLOW);
        registerLog(reflectionMessage);
        
        std::vector<Character*> alliesAlive = getAlliesAliveRaw();
        if (!isPlayerOrAlly(attackingCharacter)) {
            ui->animateDamageToEnemy(getCombatTitle(), getEnemiesRaw(), attackingCharacter, targetCharacter, currentPlayer, alliesAlive, damageReflected);
            totalDamageCaused += damageReflected;
        } else {
            ui->animateDamageToPlayer(getCombatTitle(), getEnemiesRaw(), attackingCharacter, currentPlayer, alliesAlive, false, damageReflected);
        }
    }
}

void Combat::displayAttackResult(Character* target, int finalDamage, bool triedParry, bool parrySuccess, int damageBlocked, bool shieldBroken, const std::string& brokenShieldName)
{
    bool targetIsPlayerOrAlly = isPlayerOrAlly(target);

    if (damageBlocked > 0) {
        std::string defenseMessage = DialogueFunctions::formatCombatMsg("O escudo bloqueou " + std::to_string(damageBlocked) + " de dano!", Color::BLUE);
        registerLog(defenseMessage);
        
        if (shieldBroken) {
            std::string breakMessage = DialogueFunctions::formatCombatMsg("ALERTA: O escudo " + brokenShieldName + " foi DESTRUIDO em pedacos e desequipado!", Color::BLUE);
            registerLog(breakMessage);
            target->unequipShield();
        }
    }

    if (targetIsPlayerOrAlly) 
    {
        if (triedParry) {
            std::string messageParryLog = Parry::getMessageFeedback(parrySuccess, finalDamage);
            registerLog(DialogueFunctions::formatCombatMsg(messageParryLog, Color::ORANGE));
        }
        else if (finalDamage > 0) 
        {
            registerLog(DialogueFunctions::formatCombatMsg(target->getName() + " recebeu " + std::to_string(finalDamage) + " de dano", Color::ORANGE));
        }
        else if (finalDamage == 0 && target->getDefending()) 
        {
            registerLog(DialogueFunctions::formatCombatMsg("O dano foi totalmente absorvido pela defesa de " + target->getName() + "!", Color::BLUE));
        }
        
        if (finalDamage > 0 && target == currentPlayer) totalDamageReceived += finalDamage;
    }
    else if (finalDamage > 0) 
    {
        if (finalDamage > highestDamageCaused) highestDamageCaused = finalDamage;
        if (target != currentPlayer) totalDamageCaused += finalDamage;
        registerLog(DialogueFunctions::formatCombatMsg(target->getName() + " recebeu " + std::to_string(finalDamage) + " de dano", Color::RED));
    }
}

bool Combat::checkVictoryOrDefeatCondition() 
{
    bool isVictory = enemies.empty();
    bool isDefeat = currentPlayer->getHealth() <= 0;

    if (isVictory || isDefeat) 
    { 
        currentPlayer->cleanEffects(); // Remove buffs e debuffs ao final da batalha
        InputControl::onWaitEnterUpdate = nullptr; // Impede que o aguardarEnter da tela de vitoria/derrota redesenhe o combate
        if (isVictory) {
            ui->displayVictoryScreen(currentPlayer, goldObtained, xpObtained, totalDamageCaused, 
                                totalDamageReceived, currentPlayer->getTotalCureReceived(), currentTurnCount, 
                                obtainedItems, enemiesDefeated, perfectParries, highestDamageCaused, parriesAttempted, effectiveParries, stats_itemsConsumed, newDiscoveries);
        } else {
            ui->displayDefeatScreen(currentPlayer, goldObtained, xpObtained, totalDamageCaused, totalDamageReceived, currentPlayer->getTotalCureReceived(), currentTurnCount); 
        }
        currentPlayer->finishBattle();
        return true; 
    }
    return false;
}

void Combat::processEnemyDeath(Character* enemy)
{
    registerLog(DialogueFunctions::formatCombatMsg(enemy->getName() + " derrotado!", Color::RED));
    enemiesDefeated.push_back(enemy->getName());

    std::string raceName = enemy->getRace()->getRaceName();
    if (!Bestiary::instance().alreadyDefeated(raceName)) {
        newDiscoveries.push_back("Novo monstro catalogado: " + raceName);
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
    enemy->executeDrops(currentPlayer, obtainedItems, goldObtained, xpObtained);
    for (size_t i = itemsBefore; i < obtainedItems.size(); ++i) {
        if (!Bestiary::instance().jaCollectedDrop(raceName, obtainedItems[i])) {
            newDiscoveries.push_back("Novo drop descoberto: " + obtainedItems[i]);
        }
        Bestiary::instance().registerDrop(enemy->getRace()->getRaceName(), obtainedItems[i]);
    }
}
