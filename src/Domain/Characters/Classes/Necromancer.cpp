#include "Domain/Characters/Classes/Necromancer.h"

#include <iostream>
#include <memory>

#include "UI/Screens/Combat/CombatScreen.h"
#include "Systems/Combat/Combat.h"
#include "Core/Engine/Status.h"
#include "Domain/Items/ItemFactory.h"
#include "Core/Utils/Appearance.h"
#include "Core/Utils/DialogFunctions.h"
#include "Core/Utils/InputControl.h"
#include "Domain/Characters/Races/BaseRace.h"

// --- INFORMACOES DA CLASSE ---
std::string Necromancer::getClassName() const {
    return "Necromante";
}

const std::vector<std::string>& Necromancer::getAppearanceClassMenu() const {
    static std::vector<std::string> appearance;
    if (appearance.empty()) {
        std::vector<std::string> appearanceOriginal = {
            "                   .      ..:-+                                                                     ",
            "                   :+:=##*+++=@                                                                     ",
            "                    @#*%@#%@%@#                                                                     ",
            "                    :=#-:=+*                                                                        ",
            "                     :-*@@=                                                                         ",
            "                     .  @#:                                                                         ",
            "                        *#.                                                                         ",
            "                        =#.                                                                         ",
            "                        =#                                                                          ",
            "                        +#                                                                          ",
            "                        **.                 .:++=#**#*=                                             ",
            "                      :-#*:               -=..:-=-*%#%%%-                                           ",
            "                      -+- :              -:. #++#%##%%#%@+.                                         ",
            "                      +*#@+:           *+ -*+*#%@%@%#%%%%%%:                                        ",
            "                      .=+%=:.          #%%%%#*%#%@@@@@##%%@@+                                       ",
            "                       =+@#-=        .%@@@@@@@@@##%@@@@%#*#@@@*                                     ",
            "                        **@*:        *@@@@@%%*##%@%%@@@@###*@@@@                                    ",
            "                        =#@%+       :@@@@@@@*--**#%@%%@@@#%%%@@@=                                   ",
            "                        =%%#+       %@@@@@@@@%.*=+%#%@%@@@%%%@@@#                                   ",
            "                        =%%#++     :@@@@@@@@@@@+ =+@@@%%%@@@@@@::+                                  ",
            "                        =#%%*+.    =@@@@@@@@@@@@@@@@@@%*%@@@@@@%@%+                                 ",
            "                        -#%%@@=     =%%@@@@@@@@@@@@##%@@@@@@@%+=-*%*                                ",
            "                        -%@@@@@      =*#%@@@@@@%#%@@@@@@@@@%#@@#%++#-     -:.   .                   ",
            "                        -%%@@@@+   .**:=%@@#**%@@@@@@@%##*#@@%%#%#*%*-  :+%#=-:::=                  ",
            "                        :%#@@@@@   =@*%@*+%@@@@@@@@%##*=%@@@#@%%#%*@%% .*=::-+=*#*-                 ",
            "                        .%#@@@@@#.:*@@@@@@@@@@@@%%%@@@@@%@@@@@%%%#@@@%=+#@+==---=*=                 ",
            "                        .%#%@@@@%:+@@@@@@@@@@@%@@@@@@@@@@@@#*%@@%@@@@%@##*@@@**#@#:                 ",
            "                         %#@@@@@@#@%@%=@@@@@@@@@@@@%@@@@%@@@@@@@@@@@@%@.+=%@@#%%#%.                 ",
            "                         ##@@@@@@@@%@@@@@@@@@@@@@@@@@@@@%@@@@@@@@@@@@%@:-+=#-*%*-                   ",
            "                         +#@@@@@@@@%@@@@@@@@@@@@@@%%@@@@@@@@%@@@@@@@@%%%##=-+##+:                   ",
            "                         :%@@@@@@@@@#@%@*@@:=%@@@*%@@@@@@%@%@@@@@@@@@%@@##+%%%%#@.                  ",
            "                         :%@@@@@@@@@#@@@=#+**#*@:%@@@@@@@@@@@@@@@@@@@@@@@@@@@@#@*#*                 ",
            "                         :%%@@@@@@@@%@@@*%=*#*#*@@@@@@@@@@@@@@@@@@@%@@%%%@@@@@@*+*%=                ",
            "                         :%*=@@@@@@@%@@@@@+==*%@@@@@@@@@@@@@@@%%@@@@@@#%%%%%#%*+==+%#-              ",
            "                         .%# @@@@@@+%@@@@@=:+=#@@@@@@@@@@@@@@@%%@@@%%@##%@@@%@#%@*-:.==:            ",
            "                    :   :+%* %@@@@@=%@@@@@@%%@@@@@@@@@@@@@@@@@%%@@@%%@@#%@@@%***+.::=#%*=           ",
            "                  --.:.  =%*. =%@@@*%@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@%%@@%%@@@@@#+*+:.:=##-           ",
            "                 .=::  . +%#:. +-@%#@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@@%%@@%%%@@@@@%=*+*#%#%+           ",
            "                 -*: .::.*@#=. +*%#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%@@@%%%%%@@@%%%%%%%:            ",
            "                 ==--- -*#@*:. .:#*@@@@@@@@@@@@@@@@@@@@@@@@@@@%@@@%%#@@@%*@@@@@@%%#####-            ",
            "                : :*=::=**@*:.....=@@@@@@@@@@@@@@@@@@@@@@@@@@@%@@@%%#%@@%#@@%%%@%#*-==--            ",
            "                  .+*=-%@%@++@- ..+@@@@@@@@@@@@@@@@@@@@@@@@@@@%@@@%##%@@%@%%%%#**%-:-=              ",
            "                 : .-:@%%%@=#@@%:.*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%*#%@@@@*%##==--*-:*              ",
            "                @+-:-+:++@@===%@=.#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%##%@@@@##*+==  ==-+              ",
            "              -@%%*#*+=##@%-=+*%@.%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%#%%%@@@%**+*+. .=-+:             ",
            "              -:+*####*#+@#=*+%%%#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%##%@@@@%-**+-   +*+*             ",
            "             + . .=+***#%@%#%#*#%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%@@@@%:++==. :**++             ",
            "             -....-+*=**#@%+++*#@%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%#**+*+=:  =#**.             ",
            "             #.-.:::+**##@%=+%#%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%*:.:..:-----==.=%#+              ",
            "             +* :::.:+*%@@@%#+*#%%@@@@@@@@@@@@@@@@@@@@@@@@@@*. .-==-=-------=-==-+*@%-              ",
            "             +*:-..::-:##@@#++###%@@@@@@@@@@@@@@@@@@@@@@@@@.=====++++=+-::-=-...-.::=:              ",
            "            .##  -.---:%#@@###*%#@@@@@@@@@@@@@@@@@@@@@@@@@@* =*=  ..:-++=:-=-.  .. :-:-             ",
            "             .*-:.:.:-+%%@@%#*##%@@@@@@@@@@@@@@@@@@@@@@@@@@@=*:   ....=+++-----: ...:--+            ",
            "              -=.--:+*#*%%@%##%##@@@@@@@@@@@@@@@@@@@@@@@@@@@#-  ......:=+++=-:-::-::.=-+-           ",
            "              :==:.:-**=#%@@%#%:=@@@@@@@@@@@@@@@@@@@@@@@@@@@#=.::::..:-=++====::--+-:.==+           ",
            "               :.:-:.:*+=%@@%%#:*@@@@@@@@@@@@@@@@@@@@@@@@@@@#----:---==*=**#=-:..===::-=+           ",
            "                 ::.:.-::*@@%%###@@@@@@@@@@@#@@@@@@@@@@@@@@@%++++===-=+***##+:=::===:--+*           ",
            "                 #%:-:::-:@@#%%##@@@@@@@@@@=@@%%@@@@@@@@@@@*++*++++===+=++#%*=:.:-:-*+*+*-          ",
            "                +#%%:-----@@%%%#+@@@@@@@@#=@@-@@@@@@@@@@@@@@@%##**=-=+-#++%##*=:---+#+*%#-          ",
            "                +*#%%-----@@###**@%@%@@@@##@+@@@@@%@@@@@@@@@@@@@--+=+**+=%%##*=-=--*#@%%@           ",
            "                .=*#*% ::-@@****%%%@@@@@%%##*@@=*%@@@@@@@@@@@@@@@@+##:+##@####+-===*%%%@- -         ",
            "         =+=     :=**:.---@@***+%%%%@@@@##**#@*@#@@@@@@@@@@@@@@@@#@%+*-+%%%#%#*+=++#@@%@            ",
            "       =+++==    .++---:-=@@#**+%%%%%%%@#%%*#*@#%@@@@@@@@@@@@@@@@*@@*@@%+%@@@##*=+=#%%@@            ",
            "      .++++==:    .++=-=+%@@#**+%%%%%%%@%#+***+%@@@@@@@@@@@@@@@@@*#%%+++*%@@@@**+=+#@%@=:**.        ",
            "       ++++=--    .+-=+**#@@#*+=%%%%%%%%%%%#*+*%@@@@@@@@@@@@@@@@@+@%@@@@@@@@@#=++++*@@#%++++        ",
            "       :==+++:: ..==+*****@@*++=%%%%%%%%%%##*+*%@@@@@@@@@@@@@@@@@###@@@@@@@@@+==+--#@%%%*++=        ",
            "        :==+.  :::-==++**#@@#+==%%%%%%%%%%%*+#%%@@@@@@@@@@@@@@@@@-#%%%%%@@@@@@:=+-=#@%%%#+==        ",
            "         ::    -:+%*#=***#%@%=-=%%%%%%%%%%%+%#%@@@@@@@@@@@@@@@@@-+***#%%%@@@@@-=-=+%%%%#%*-=        ",
            "         ....*=%#*-++**#+#@@@=-+#%%%%%%%%%@#*%#@@@@@@@@@@@@@@@@@++**#*##%%@@@@===++%@%#%#%--        ",
            "          .  -==-==+%#**++@@@=:=#%%%%%%%%%@#*%%@@@@@@@@@@@@@@@#:=+=*+++%%@@@@@*-=++@%%####+:        ",
            "             :+.=*@%@%%#*+%@@-:=#####%%%%%%#*%@@@@@@@@@@@@@@@*.:==--+#*#%@@@@@%=++*%%%####+.        ",
            "       .    ..=#%@##%%%%##%@@=:=####%%%#%#%##@%%@@@@@@@@@@@@%.. ..-++*+%%%@@@@%+++*#%#####-         ",
            "             :+%%%*@%@%%##%@@+.=#####%%%%%%%#@%@@@%@@@@@@@@@-=:::-+++*%%%@%@@@%+++==%####*-         ",
            "            =+++#%@%%%@%%##@@+.:*#####%#%%%%%@%@@%*@@@@@@@@@=+--+++=*%%@%%%%@%%+*+=:####*+-         ",
            "              -: =##@%*%%%%@@%.:=+#####%%%%%%%%@@@@@@@@@@@#=%+==+**%%@@%@@@%%%#*+++:####*+.         ",
            "          .: = =#-@#%%@%%##@@#.:. .+*#%%*=##%@@@@@@@@@@@@-*##*+*+##@@@@@@%%###*+*+=#****+:..        ",
            "          :-.=*%#####%@@#%=@@@.:.::=**+##%@%#%@@@@@@@@@-#=##%%@%%%%@@@%%####*===++=***+=::.:..      ",
            "          :-===**+.=+%@#%=*#%%:..:::*%%%%#%%%%@%@@%#=.-*=##%@@@@%%%######*+:-===+*+*+=-::::::.      ",
            "          :----=+*+-:---=-%**%#=++*##%#%%%%@%%%%%#++-*#%#%#%@@%%%%#####*-:-=-==+#*++=-::::--:.      ",
            "         ..:---==+*+++*=--+##*=+#***##%#%%%%%%@%%%%-++*++*%-@@%%%%#***+-=+*+*#***++==----=--..      ",
            "         ...::---=+=++*+****#%#*+***+===+++--#%%%%%=#%=*#-+%@%%%*+=@%=*#+*####***+++++++=++-:       ",
            "       ....:::-==+=--==-===*%%*=-:.:==-+:=====+***%%%%*%%%@@%%%%#%%*%*%##%##*****+==+=+*+---.       ",
            "      ....:-----=::--:--=+*#-=+-::-=+*-:=---++++*###%%%%#**#@%@###%%########***+++==+=====-:.       ",
            "       ....:::--...::.:==+**+++*+++-:+=-:::...::::-++*+*====:+*-:=+*#%%%%#==++==*#*+==++*=-..       ",
            "      .::::::--:::-=+*%%@%%#=-:. ... .-+.          ...::..:::------=++**#*%%%**+++*##+==--:.        ",
            "           :::---*%+---++%+=..:::..:..=++-::::.:=-:.:===+=--:::-==-::-*#####**++==--:::.            "
        };
        appearance = Appearance::reduceScaleAscii(appearanceOriginal, 3);
    }
    return appearance;
}

Attributes Necromancer::getAttributesClass() const {
    return {-20, 5, 5, 3, 10, 10, 20};
}

std::vector<std::unique_ptr<Item>> Necromancer::getEquipmentClass() const {
    auto equipment = ItemFactory::createKitPocoes();
    equipment.push_back(ItemFactory::createItem(ItemID::StaffBone));
    equipment.push_back(ItemFactory::createItem(ItemID::ClothesRitualist));
    return equipment;
}

// --- PASSIVA DA CLASSE ---
std::string Necromancer::getNamePassiveClass() const {
    return "Toque Necrotico";
}

std::string Necromancer::getDescriptionPassiveClass() const {
    return "Ataques aplicam Necrose, causando 5% da Vida Max. do alvo como dano por 3 turnos.\n"
           "Ao derrotar um inimigo, coleta sua alma.";
}

void Necromancer::executeAttackWithPassiveDaClass(Character* attacker, Character* defender, int damageBase, int damagePiercing, std::vector<std::unique_ptr<Character>>& listDeEnemies, const std::function<void(Character*, Character*, int, int)>& applyDamage, bool applyPassive) {
    // Comportamento padrao: apenas ataca o alvo principal ou todos se a arma for de area
    BaseClass::executeAttackWithPassiveDaClass(attacker, defender, damageBase, damagePiercing, listDeEnemies,
        [&](Character* atk, Character* def, int dmg, int perf) {
            // Callback para aplicar o dano e depois o efeito da passiva
            applyDamage(atk, def, dmg, perf);
            if (def->getHealth() > 0 && applyPassive) {
                int damageNecrosis = static_cast<int>(def->getMaxHealth() * 0.05);
                if (damageNecrosis < 1) damageNecrosis = 1;
                def->addEffect(std::make_unique<NecrosisEffect>(3, damageNecrosis));
                std::string msg = DialogueFunctions::formatSkillMsg("Necrose! " + def->getName() + " perdera " + std::to_string(damageNecrosis) + " de HP por 3 turnos.", Color::MAGENTA);
                this->notifyMessageCombat(msg, msg);
            }
        }, applyPassive);
}


// --- HABILIDADE DA CLASSE ---
std::string Necromancer::getRechargeSkillClass() const {
    return "Recarga: Nenhuma (consome 1 alma).";
}

std::string Necromancer::getNameSkillClass() const {
    return "Invocacao de Morto-Vivo";
}

std::string Necromancer::getDescriptionSkillClass() const {
    return "Usa uma alma para invocar um clone com 80% dos atributos (Chefes 60%). Max: 3 lacaios.\nLacaios perdem 15% de sua Vida Max a cada turno do jogador.";
}

void Necromancer::useSkillClass(Combat* combat, Character* characterUser, std::vector<Character*>& /*listaDeInimigos*/) {
    bool hasMiniBoss = false;
    int minionCount = 0;
    for (const auto& ally : combat->getAlliesAliveRaw()) {
        if (ally->isMinion()) {
            minionCount++;
            if (ally->isBoss()) {
                hasMiniBoss = true;
            }
        }
    }

    if (hasMiniBoss) {
        std::string msg = DialogueFunctions::formatSystemMsg("Seu Morto-Vivo Chefe exige todo o seu controle! Nao e possivel invocar mais lacaios.", Color::RED);
        std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
        Appearance::registerBattleLog(msg);
        characterUser->setSkillCanceled(true);
        return;
    }

    if (minionCount >= 3) {
        std::string msg = DialogueFunctions::formatSystemMsg("Limite maximo de 3 lacaios atingido!", Color::RED);
        std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
        Appearance::registerBattleLog(msg);
        characterUser->setSkillCanceled(true);
        return;
    }

    if (characterUser->getNumberDeSouls() == 0) {
        std::string msg = DialogueFunctions::formatSystemMsg("Voce nao possui almas para invocar!", Color::RED);
        std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
        Appearance::registerBattleLog(msg);
        characterUser->setSkillCanceled(true);
        return;
    }

    int maxPossible = std::min(3 - minionCount, static_cast<int>(characterUser->getNumberDeSouls()));

    std::cout << "\n" << CombatScreen::combatMargin() << "═══ QUANTIDADE DE INVOCACOES ═══\n";
    std::vector<std::string> qtyOptions;
    for (int i = 1; i <= maxPossible; ++i) {
        if (i == 1) qtyOptions.push_back("1 Morto-Vivo");
        else qtyOptions.push_back(std::to_string(i) + " Mortos-Vivos (Inimigo atua imediatamente)");
    }
    qtyOptions.push_back("Cancelar");

    int qtyChosen = InputControl::readSelectionMenuWithArrows(qtyOptions, false, CombatScreen::combatMargin());
    if (qtyChosen == static_cast<int>(qtyOptions.size()) - 1 || qtyChosen == -1) {
        characterUser->setSkillCanceled(true);
        return;
    }

    int quantityForInvoke = qtyChosen + 1;
    std::vector<Character*> minionsRecentlyInvoked;

    for (int i = 0; i < quantityForInvoke; ++i) {
        std::vector<std::string> options;
        auto& souls = characterUser->getSouls();

        struct GroupSoul {
            std::string name;
            TypeRace type;
            bool isBoss;
            int quantity;
            int firstIndex;
        };
        std::vector<GroupSoul> groups;

        for (size_t j = 0; j < souls.size(); ++j) {
            bool found = false;
            for (auto& g : groups) {
                if (g.name == souls[j]->getName()) {
                    g.quantity++;
                    found = true;
                    break;
                }
            }
            if (!found) groups.push_back({souls[j]->getName(), souls[j]->getTypeRace(), souls[j]->isBoss(), 1, static_cast<int>(j)});
        }

        for (const auto& g : groups) {
            std::string prefix = "";
            std::string color = Appearance::color(Color::WHITE);
            
            if (g.isBoss) {
                if (g.type == TypeRace::Mahoraga) {
                    prefix = "[CHEFE] ";
                    color = Appearance::color(Color::RED);
                } else {
                    prefix = "[MINI-CHEFE] ";
                    color = Appearance::color(Color::YELLOW);
                }
            }
            
            options.push_back(color + std::to_string(g.quantity) + "x " + prefix + "Morto-Vivo de " + g.name + Appearance::color(Color::RESET));
        }
        options.push_back("Cancelar Restante");

        std::cout << "\n" << CombatScreen::combatMargin() << "═══ ESCOLHA UMA ALMA PARA INVOCAR (" << (i+1) << "/" << quantityForInvoke << ") ═══\n";
        int choice = InputControl::readSelectionMenuWithArrows(options, false, CombatScreen::combatMargin());

        if (choice == static_cast<int>(options.size()) - 1 || choice == -1) {
            if (i == 0) {
                characterUser->setSkillCanceled(true);
                return;
            }
            break; // Para as invocacoes mas mantem as que ja foram feitas
        }

        int indexRealForRemove = groups[choice].firstIndex;
        auto minion = characterUser->removeSoul(indexRealForRemove);
        std::string nameOriginal = minion->getName();
        
        double factorScale = 0.8;
        if (minion->isBoss()) {
            factorScale = 0.6;
        }
        
        minion->climbAttributes(factorScale);
        minion->setAsMinion(true);
        minion->changeName("Morto-Vivo (" + nameOriginal + ")");

        std::string msg = DialogueFunctions::formatSkillMsg(characterUser->getName() + " ergueu um Morto-Vivo de " + nameOriginal + "!", Color::MAGENTA);
        notifyMessageCombat(msg, msg);

        Character* minionPtr = minion.get();
        bool wasBoss = minionPtr->isBoss();
        combat->addAllyEmCombat(std::move(minion));
        minionsRecentlyInvoked.push_back(minionPtr);

        if (wasBoss) {
            if (i < quantityForInvoke - 1) {
                std::string bossMsg = DialogueFunctions::formatSystemMsg("A invocacao de um Chefe consumiu seu foco! Invocacoes adicionais canceladas.", Color::YELLOW);
                notifyMessageCombat(bossMsg, bossMsg);
                std::cout << "\n" << CombatScreen::combatMargin() << bossMsg << "\n";
            }
            break; // Interrompe o laco, impedindo que os proximos mortos-vivos selecionados sejam invocados no mesmo turno
        }
    }
    
    // Se invocou mais de um minion na mesma acao, eles saltam seu turno ("Stun") para que o inimigo atue de imediato!
    if (minionsRecentlyInvoked.size() > 1) {
        std::string msg = DialogueFunctions::formatSystemMsg("A invocacao multipla exauriu seu controle! O turno inimigo comecara imediatamente!", Color::LIGHT_RED);
        notifyMessageCombat(msg, msg);
        std::cout << "\n" << CombatScreen::combatMargin() << msg << "\n";
        InputControl::waitForEnter();
        for (auto* m : minionsRecentlyInvoked) {
            m->addEffect(std::make_unique<StunEffect>(1));
        }
    }
}
