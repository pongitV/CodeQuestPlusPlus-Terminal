#include "Necromante.h"

#include <iostream>
#include <memory>

#include "../../Perspectiva/TelasBase/Combate/TelaCombate.h"
#include "../../Sistemas/Combate/Combate.h"
#include "../../Core/Controladores/Status.h"
#include "../../Sistemas/Inventario/FabricaItens.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Entidades/Racas/RacaBase.h"

// --- INFORMACOES DA CLASSE ---
std::string Necromante::obterNomeClasse() const {
    return "Necromante";
}

const std::vector<std::string>& Necromante::obterAparenciaClasseMenu() const {
    static std::vector<std::string> aparencia;
    if (aparencia.empty()) {
        std::vector<std::string> aparenciaOriginal = {
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
        aparencia = Aparencia::reduzirEscalaAscii(aparenciaOriginal, 3);
    }
    return aparencia;
}

Atributos Necromante::obterAtributosClasse() const {
    return {-20, 5, 5, 3, 10, 10, 20};
}

std::vector<std::unique_ptr<Item>> Necromante::obterEquipamentoClasse() const {
    auto equipamentos = FabricaItens::criarKitPocoes();
    equipamentos.push_back(FabricaItens::criarItem(ItemID::CajadoOsso));
    equipamentos.push_back(FabricaItens::criarItem(ItemID::RoupasRitualista));
    return equipamentos;
}

// --- PASSIVA DA CLASSE ---
std::string Necromante::obterNomePassivaClasse() const {
    return "Toque Necrotico";
}

std::string Necromante::obterDescricaoPassivaClasse() const {
    return "Ataques aplicam Necrose, causando 5% da Vida Max. do alvo como dano por 3 turnos.\n"
           "Ao derrotar um inimigo, coleta sua alma.";
}

void Necromante::executarAtaqueComPassivaDaClasse(Personagem* atacante, Personagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<Personagem>>& listaDeInimigos, const std::function<void(Personagem*, Personagem*, int, int)>& aplicarDano, bool aplicarPassiva) {
    // Comportamento padrão: apenas ataca o alvo principal ou todos se a arma for de área
    ClasseBase::executarAtaqueComPassivaDaClasse(atacante, defensor, danoBase, danoPerfurante, listaDeInimigos,
        [&](Personagem* atk, Personagem* def, int dmg, int perf) {
            // Callback para aplicar o dano e depois o efeito da passiva
            aplicarDano(atk, def, dmg, perf);
            if (def->obterVida() > 0 && aplicarPassiva) {
                int danoNecrose = static_cast<int>(def->obterVidaMaxima() * 0.05);
                if (danoNecrose < 1) danoNecrose = 1;
                def->adicionarEfeito(std::make_unique<EfeitoNecrose>(3, danoNecrose));
                std::string msg = FuncoesDialogo::formatarMsgHabilidade("Necrose! " + def->obterNome() + " perdera " + std::to_string(danoNecrose) + " de HP por 3 turnos.", Cor::MAGENTA);
                this->notificarMensagemCombate(msg, msg);
            }
        }, aplicarPassiva);
}


// --- HABILIDADE DA CLASSE ---
std::string Necromante::obterRecargaHabilidadeClasse() const {
    return "Recarga: Nenhuma (consome 1 alma).";
}

std::string Necromante::obterNomeHabilidadeClasse() const {
    return "Invocacao de Morto-Vivo";
}

std::string Necromante::obterDescricaoHabilidadeClasse() const {
    return "Usa uma alma para invocar um clone com 80% dos atributos (Chefes 60%). Max: 3 lacaios.\nLacaios perdem 15% de sua Vida Max a cada turno do jogador.";
}

void Necromante::usarHabilidadeClasse(Combate* combate, Personagem* personagemUsuario, std::vector<Personagem*>& /*listaDeInimigos*/) {
    bool temMiniBoss = false;
    int minionCount = 0;
    for (const auto& aliado : combate->obterAliadosVivosRaw()) {
        if (aliado->isMinion()) {
            minionCount++;
            std::string raca = aliado->obterRaca()->obterNomeRaca();
            if (raca == "Abominacao da Floresta" || raca == "Ork Exilado" || raca == "Troll" || raca.find("Mahoraga") != std::string::npos) {
                temMiniBoss = true;
            }
        }
    }

    if (temMiniBoss) {
        std::string msg = FuncoesDialogo::formatarMsgSistema("Seu Morto-Vivo Chefe exige todo o seu controle! Nao e possivel invocar mais lacaios.", Cor::VERMELHO);
        std::cout << "\n" << TelaCombate::margemCombate() << msg << "\n";
        Aparencia::registrarLogBatalha(msg);
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    if (minionCount >= 3) {
        std::string msg = FuncoesDialogo::formatarMsgSistema("Limite maximo de 3 lacaios atingido!", Cor::VERMELHO);
        std::cout << "\n" << TelaCombate::margemCombate() << msg << "\n";
        Aparencia::registrarLogBatalha(msg);
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    if (personagemUsuario->obterNumeroDeAlmas() == 0) {
        std::string msg = FuncoesDialogo::formatarMsgSistema("Voce nao possui almas para invocar!", Cor::VERMELHO);
        std::cout << "\n" << TelaCombate::margemCombate() << msg << "\n";
        Aparencia::registrarLogBatalha(msg);
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int maxPossivel = std::min(3 - minionCount, static_cast<int>(personagemUsuario->obterNumeroDeAlmas()));

    std::cout << "\n" << TelaCombate::margemCombate() << "═══ QUANTIDADE DE INVOCACOES ═══\n";
    std::vector<std::string> opcoesQtd;
    for (int i = 1; i <= maxPossivel; ++i) {
        if (i == 1) opcoesQtd.push_back("1 Morto-Vivo");
        else opcoesQtd.push_back(std::to_string(i) + " Mortos-Vivos (Inimigo atua imediatamente)");
    }
    opcoesQtd.push_back("Cancelar");

    int qtdEscolhida = ControleDeInput::lerSelecaoMenuComSetas(opcoesQtd, false, TelaCombate::margemCombate());
    if (qtdEscolhida == static_cast<int>(opcoesQtd.size()) - 1 || qtdEscolhida == -1) {
        personagemUsuario->definirHabilidadeCancelada(true);
        return;
    }

    int quantidadeParaInvocar = qtdEscolhida + 1;
    std::vector<Personagem*> minionsRecemInvocados;

    for (int i = 0; i < quantidadeParaInvocar; ++i) {
        std::vector<std::string> opcoes;
        auto& almas = personagemUsuario->obterAlmas();

        struct GrupoAlma {
            std::string nome;
            std::string raca;
            int quantidade;
            int primeiroIndice;
        };
        std::vector<GrupoAlma> grupos;

        for (size_t j = 0; j < almas.size(); ++j) {
            bool encontrou = false;
            for (auto& g : grupos) {
                if (g.nome == almas[j]->obterNome()) {
                    g.quantidade++;
                    encontrou = true;
                    break;
                }
            }
            if (!encontrou) grupos.push_back({almas[j]->obterNome(), almas[j]->obterRaca()->obterNomeRaca(), 1, static_cast<int>(j)});
        }

        for (const auto& g : grupos) {
            std::string raca = g.raca;
            std::string prefixo = "";
            std::string cor = Aparencia::cor(Cor::BRANCO);
            
            if (raca.find("Mahoraga") != std::string::npos) {
                prefixo = "[CHEFE] ";
                cor = Aparencia::cor(Cor::VERMELHO);
            } else if (raca == "Abominacao da Floresta" || raca == "Ork Exilado" || raca == "Troll") {
                prefixo = "[MINI-CHEFE] ";
                cor = Aparencia::cor(Cor::AMARELO);
            }
            
            opcoes.push_back(cor + std::to_string(g.quantidade) + "x " + prefixo + "Morto-Vivo de " + g.nome + Aparencia::cor(Cor::RESET));
        }
        opcoes.push_back("Cancelar Restante");

        std::cout << "\n" << TelaCombate::margemCombate() << "═══ ESCOLHA UMA ALMA PARA INVOCAR (" << (i+1) << "/" << quantidadeParaInvocar << ") ═══\n";
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, TelaCombate::margemCombate());

        if (escolha == static_cast<int>(opcoes.size()) - 1 || escolha == -1) {
            if (i == 0) {
                personagemUsuario->definirHabilidadeCancelada(true);
                return;
            }
            break; // Para as invocacoes mas mantem as que ja foram feitas
        }

        int indiceRealParaRemover = grupos[escolha].primeiroIndice;
        auto minion = personagemUsuario->removerAlma(indiceRealParaRemover);
        std::string nomeOriginal = minion->obterNome();
        std::string racaOriginal = minion->obterRaca()->obterNomeRaca();
        
        double fatorEscala = 0.8;
        if (racaOriginal.find("Mahoraga") != std::string::npos) {
            fatorEscala = 0.6;
        }
        
        minion->escalarAtributos(fatorEscala);
        minion->setAsMinion(true);
        minion->alterarNome("Morto-Vivo (" + nomeOriginal + ")");

        std::string msg = FuncoesDialogo::formatarMsgHabilidade(personagemUsuario->obterNome() + " ergueu um Morto-Vivo de " + nomeOriginal + "!", Cor::MAGENTA);
        notificarMensagemCombate(msg, msg);

        Personagem* minionPtr = minion.get();
        std::string racaInvocada = minionPtr->obterRaca()->obterNomeRaca();
        combate->adicionarAliadoEmCombate(std::move(minion));
        minionsRecemInvocados.push_back(minionPtr);

        if (racaInvocada == "Abominacao da Floresta" || racaInvocada == "Ork Exilado" || racaInvocada == "Troll" || racaInvocada.find("Mahoraga") != std::string::npos) {
            if (i < quantidadeParaInvocar - 1) {
                std::string msgBoss = FuncoesDialogo::formatarMsgSistema("A invocacao de um Chefe consumiu seu foco! Invocacoes adicionais canceladas.", Cor::AMARELO);
                notificarMensagemCombate(msgBoss, msgBoss);
                std::cout << "\n" << TelaCombate::margemCombate() << msgBoss << "\n";
            }
            break; // Interrompe o laco, impedindo que os proximos mortos-vivos selecionados sejam invocados no mesmo turno
        }
    }
    
    // Se invocou mais de um minion na mesma acao, eles saltam seu turno ("Stun") para que o inimigo atue de imediato!
    if (minionsRecemInvocados.size() > 1) {
        std::string msg = FuncoesDialogo::formatarMsgSistema("A invocacao multipla exauriu seu controle! O turno inimigo comecara imediatamente!", Cor::VERMELHO_CLARO);
        notificarMensagemCombate(msg, msg);
        std::cout << "\n" << TelaCombate::margemCombate() << msg << "\n";
        ControleDeInput::aguardarEnter();
        for (auto* m : minionsRecemInvocados) {
            m->adicionarEfeito(std::make_unique<EfeitoAtordoamento>(1));
        }
    }
}
