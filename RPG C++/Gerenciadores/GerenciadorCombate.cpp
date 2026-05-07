#include "GerenciadorCombate.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "../Classes/ClasseBase.h"
#include "../Inventario/InventarioCombate.h"
#include "../Inventario/Item.h"
#include "../Inventario/EquipamentoEscudo.h"
#include "../Racas/RacaBase.h"
#include "../Sistemas/SistemaBestiario.h"
#include "../Sistemas/SistemaParry.h"
#include "../Telas/TelaBestiario.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaCombate.h"
#include "../Telas/TelaDerrota.h"
#include "../Telas/TelaVitoria.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Telas/TelaMenu.h"

namespace {
    int lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt = false, bool usarMargemCombate = false) {
        int valor;
        std::string prefixo = usarMargemCombate ? Aparencia::margemCombate() : "";
        if (centralizarPrompt) Aparencia::exibirPrompt(promptMensagem);
        else std::cout << prefixo << promptMensagem;

        while (!(std::cin >> valor) || valor < minimo || valor > maximo) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (centralizarPrompt) {
                Aparencia::exibirPrompt("Entrada invalida! " + promptMensagem);
            } else {
                std::cout << prefixo << "Entrada invalida! " << promptMensagem;
            }
        }
        return valor;
    }
}

GerenciadorCombate::GerenciadorCombate(SistemaPersonagem* jogadorParaOCombate, std::vector<std::unique_ptr<SistemaPersonagem>>&& inimigosParaOCombate) 
    : jogadorAtual(jogadorParaOCombate), listaDeInimigos(std::move(inimigosParaOCombate)), contadorDoTurnoAtual(1), quantidadeDeOuroObtido(0), quantidadeDeXpObtido(0), totalDeDanoCausado(0), totalDeDanoRecebido(0)
{

    int nivelDeDificuldade = static_cast<int>(jogadorAtual->obterDificuldade());
    double multiplicadorDeDificuldadeDosInimigos = 1.0;

    if (nivelDeDificuldade == 3) {
        multiplicadorDeDificuldadeDosInimigos = 2.0;
    }

    for (auto& inimigoAtualPtr : this->listaDeInimigos) 
    {
        inimigoAtualPtr->aplicarMultiplicadorDificuldade(multiplicadorDeDificuldadeDosInimigos);
        inimigoAtualPtr->prepararParaNovaBatalha();
    }
}

void GerenciadorCombate::adicionarAliados(std::vector<std::unique_ptr<SistemaPersonagem>> aliados)
{
    listaDeAliados = std::move(aliados);
}

GerenciadorCombate::~GerenciadorCombate()
{
}

std::string GerenciadorCombate::obterTituloDoCombate() const
{
    std::string titulo = "EM COMBATE (";
    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
        titulo += listaDeInimigos[i]->obterNome();
        if (i < listaDeInimigos.size() - 1) titulo += ", ";
    }
    titulo += ")";
    return titulo;
}

bool GerenciadorCombate::isPersonagemJogadorOuAliado(SistemaPersonagem* personagem) const {
    if (personagem == jogadorAtual) return true;
    for (const auto& aliadoAtual : listaDeAliados) {
        if (aliadoAtual.get() == personagem) return true;
    }
    return false;
}

std::vector<SistemaPersonagem*> GerenciadorCombate::obterInimigosRaw() const
{
    std::vector<SistemaPersonagem*> ponteirosInimigos(listaDeInimigos.size());
    std::transform(listaDeInimigos.begin(), listaDeInimigos.end(), ponteirosInimigos.begin(), [](const std::unique_ptr<SistemaPersonagem>& ptr) { return ptr.get(); });
    return ponteirosInimigos;
}

void GerenciadorCombate::exibirTelaDeCombate(bool animarEntrada) const
{
    Aparencia::limparTela();
    TelaCombate::exibirLogoParaTelaDeCombate(obterTituloDoCombate());
    TelaCombate::exibirHordaDeInimigosLadoALado(obterInimigosRaw(), nullptr, 0, false, animarEntrada);
    TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual);
    for (const auto& aliado : listaDeAliados) {
        if (aliado->obterVida() > 0) {
            TelaCombate::exibirBarraDeStatusDoJogador(aliado.get());
        }
    }
    Aparencia::imprimirLinhaDivisoria();
}

void GerenciadorCombate::iniciarCombate() 
{
    jogadorAtual->prepararParaNovaBatalha();
    Aparencia::limparLogBatalha();

    for (auto& aliado : listaDeAliados) {
        aliado->prepararParaNovaBatalha();
    }

    int maxDestrezaInimigos = 0;
    for (const auto& inimigoPtr : listaDeInimigos) {
        SistemaBestiario::instancia().registrarPrimeiraVista(inimigoPtr->obterNome());
        if (inimigoPtr->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigoPtr->obterDestreza();
    }
    
    bool turnoExtraFirstTurn = (jogadorAtual->obterDestreza() > (maxDestrezaInimigos * 2));
    bool primeiraRenderizacao = true;
    
    if (maxDestrezaInimigos > jogadorAtual->obterDestreza()) {
        exibirTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << "[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!" << Aparencia::cor(Cor::RESET) << "\n";
        Aparencia::registrarLogBatalha("[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!");
        Aparencia::aguardarEnter();
        executarTurnoDeTodosOsInimigos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;
    }

    while (jogadorAtual->obterVida() > 0 && !listaDeInimigos.empty()) {
        // Turno do Jogador
        if (jogadorAtual->obterVida() > 0) {
            jogadorAtual->reduzirCooldowns();
            jogadorAtual->processarEfeitosInicioTurno();

            bool turnoFoiConsumido = false;
            bool usouInventarioNoTurno = false;

            while (!turnoFoiConsumido && jogadorAtual->obterVida() > 0 && !listaDeInimigos.empty()) {
                exibirTelaDeCombate(primeiraRenderizacao);
                primeiraRenderizacao = false;
                processarMenuDeAcoesDoJogador(jogadorAtual, turnoFoiConsumido, usouInventarioNoTurno);
                if (verificarCondicaoDeVitoriaOuDerrota()) return; 
            }

            if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) {
                std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::CIANO) << "[SISTEMA]: Sua agilidade extrema (" << jogadorAtual->obterDestreza() << " VS " << maxDestrezaInimigos << ") permite que voce aja novamente!" << Aparencia::cor(Cor::RESET) << "\n";
                Aparencia::registrarLogBatalha("[SISTEMA]: Sua agilidade extrema (" + std::to_string(jogadorAtual->obterDestreza()) + " VS " + std::to_string(maxDestrezaInimigos) + ") permite que voce aja novamente!");
                Aparencia::aguardarEnter();
                turnoExtraFirstTurn = false;
                continue;
            }

            if (usouInventarioNoTurno) {
                exibirTelaDeCombate();
                std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::AMARELO) << "[SISTEMA]: O inimigo te pegou desprevinido enquanto voce usava o inventario!" << Aparencia::cor(Cor::RESET) << "\n";
                Aparencia::registrarLogBatalha("[SISTEMA]: O inimigo te pegou desprevinido enquanto voce usava o inventario!");
            }
        }
        
        // Turnos dos Aliados
        for (auto& aliado : listaDeAliados) {
            if (aliado->obterVida() <= 0 || listaDeInimigos.empty()) continue;
            
            aliado->reduzirCooldowns();
            aliado->processarEfeitosInicioTurno();
            if (aliado->obterVida() <= 0) continue;

            bool turnoAliadoConsumido = false;
            bool usouInventarioAliado = false;

            while (!turnoAliadoConsumido && aliado->obterVida() > 0 && !listaDeInimigos.empty()) {
                exibirTelaDeCombate();
                processarMenuDeAcoesDoJogador(aliado.get(), turnoAliadoConsumido, usouInventarioAliado);
                if (verificarCondicaoDeVitoriaOuDerrota()) return;
            }
        }
        
        executarTurnoDeTodosOsInimigos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;

        contadorDoTurnoAtual++;
    }
}

void GerenciadorCombate::processarMenuDeAcoesDoJogador(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    TelaCombate::limparMensagensFixas();
    Aparencia::exibirUltimosLogs(5);
    std::string textoDoTurno = "--- TURNO " + std::to_string(contadorDoTurnoAtual) + " | VEZ DE " + personagemAgindo->obterNome() + " ---";
    std::string msgTurno = "\n" + Aparencia::espacosParaCentralizar(textoDoTurno.length()) + textoDoTurno + "\n";
    std::cout << msgTurno;
    TelaCombate::adicionarMensagemFixa(msgTurno);

    std::string textoAcoes = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | 6. Bestiario | 7. Log | Escolha: ";
    int acaoEscolhida = lerInteiroComLimites(textoAcoes, 1, 7, true);

    switch (acaoEscolhida) 
    {
        case 1: processarAcaoAtacar(personagemAgindo, turnoFoiConsumido); break;
        case 2: processarAcaoDefender(personagemAgindo, turnoFoiConsumido); break;
        case 3: processarAcaoHabilidade(personagemAgindo, turnoFoiConsumido); break;
        case 4: processarAcaoInventario(personagemAgindo, turnoFoiConsumido, usouInventarioNoTurno); break;
        case 5: TelaAtributos::gerenciarFichaDoJogador(personagemAgindo); break;
        case 6: TelaBestiario::exibirLista(personagemAgindo); break;
        case 7: Aparencia::exibirHistoricoCompleto(); break;
        default: 
            std::cout << "\n" << Aparencia::margemCombate() << "[ERRO] Acao invalida!\n"; 
            Aparencia::aguardarEnter(); 
            break;
    }
}

void GerenciadorCombate::processarAcaoAtacar(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterTipoAtaque() == TipoAtaque::AREA) 
    {
        realizarAtaqueFisico(personagemAgindo, nullptr, contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }
    else 
    {
        int maxIndice = static_cast<int>(listaDeInimigos.size()) - 1;
        int indiceDoAlvoEscolhido = lerInteiroComLimites("Escolha o alvo (0 a " + std::to_string(maxIndice) + "): ", 0, maxIndice, false, true);

        realizarAtaqueFisico(personagemAgindo, listaDeInimigos[indiceDoAlvoEscolhido].get(), contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }

    limparInimigosMortos();
}

Item* GerenciadorCombate::selecionarEscudo(SistemaPersonagem* personagemAgindo) 
{
    std::vector<Item*> listaDeEscudos;
    for (auto* item : personagemAgindo->obterInventario()->obterTodosOsItens()) 
    {
        if (item->obterTipo() == TipoEquipamento::ESCUDO) {
            listaDeEscudos.push_back(item);
        }
    }

    if (listaDeEscudos.empty()) 
    {
        std::cout << "\n" << Aparencia::margemCombate() << "[!] " << personagemAgindo->obterNome() << " nao possui escudos no inventario para usar!\n";
        return nullptr;
    }

    std::cout << "\n" << Aparencia::margemCombate() << "=== SELECIONE UM ESCUDO PARA " << personagemAgindo->obterNome() << " ===\n";
    for (size_t indice = 0; indice < listaDeEscudos.size(); indice++) 
    {
        std::cout << Aparencia::margemCombate() << " [" << indice + 1 << "] " << listaDeEscudos[indice]->obterNomeItem() << listaDeEscudos[indice]->obterInfoStatus() << "\n";
    }
    std::cout << Aparencia::margemCombate() << " [0] Cancelar\n\n";
    int opcaoEscolhida = lerInteiroComLimites("Escolha: ", 0, static_cast<int>(listaDeEscudos.size()), false, true);
    return (opcaoEscolhida == 0) ? nullptr : listaDeEscudos[opcaoEscolhida - 1];
}

void GerenciadorCombate::processarAcaoDefender(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterRecargaDefesa()) 
    {
        std::cout << "\n" << Aparencia::margemCombate() << "[ERRO]: " << personagemAgindo->obterNome() << " se desequilibrou e precisa de 1 turno para poder defender novamente!\n";
        Aparencia::registrarLogBatalha("[ERRO]: " + personagemAgindo->obterNome() + " se desequilibrou e precisa de 1 turno para poder defender novamente!");
        Aparencia::aguardarEnter();
        return; 
    }
    
    Item* escudoEscolhido = selecionarEscudo(personagemAgindo);
    if (escudoEscolhido != nullptr) 
    {
        if (!escudoEscolhido->podeSerEquipadoPor(personagemAgindo)) {
            std::string msg = escudoEscolhido->obterMensagemRequisito();
            if (msg.substr(0, 1) == "\n") {
                std::cout << "\n" << Aparencia::margemCombate() << msg.substr(1);
            } else {
                std::cout << Aparencia::margemCombate() << msg;
            }
            Aparencia::aguardarEnter();
            return;
        }

        personagemAgindo->equiparItem(escudoEscolhido);
        personagemAgindo->definirDefendendo(true);
        std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << personagemAgindo->obterNome() << " assumiu uma postura defensiva com " << escudoEscolhido->obterNomeItem() << "!\n";
        Aparencia::registrarLogBatalha("[SISTEMA]: " + personagemAgindo->obterNome() + " assumiu uma postura defensiva com " + escudoEscolhido->obterNomeItem() + "!");
        Aparencia::aguardarEnter();
        turnoFoiConsumido = true;
    }
}

void GerenciadorCombate::processarAcaoHabilidade(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido)
{
    std::vector<SistemaPersonagem*> alvosRaw = obterInimigosRaw();

    personagemAgindo->definirHabilidadeCancelada(false);
    personagemAgindo->obterClasse()->usarHabilidadeClasse(personagemAgindo, alvosRaw);
    
    if (personagemAgindo->obterHabilidadeCancelada()) return;

    if (personagemAgindo->habilidadeDaClasseConsomeTurno()) turnoFoiConsumido = true;
    else Aparencia::aguardarEnter();
}

void GerenciadorCombate::processarAcaoInventario(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    bool inventarioConsumiu = false;
    InventarioCombate::gerenciarInventario(personagemAgindo, &inventarioConsumiu);
    if (inventarioConsumiu) {
        turnoFoiConsumido = true;
        usouInventarioNoTurno = true;
    }
    if (personagemAgindo->obterItemSelecionadoParaUso() != nullptr) 
    {
        Item* itemSelecionado = personagemAgindo->obterItemSelecionadoParaUso();
        
        std::cout << "\n" << Aparencia::margemCombate() << "--- ESCOLHA UM ALVO ---\n";
        for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
            std::cout << Aparencia::margemCombate() << "[" << i << "] " << listaDeInimigos[i]->obterNome() << " (HP: " << listaDeInimigos[i]->obterVida() << ")\n";
        }
        
        int maxIndice = static_cast<int>(listaDeInimigos.size()) - 1;
        int indiceDoAlvoEscolhido = lerInteiroComLimites("Escolha (ou -1 para CANCELAR): ", -1, maxIndice, false, true);

        if (indiceDoAlvoEscolhido == -1) 
        {
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA] Uso do frasco cancelado. O item voltou para a mochila.\n";
            personagemAgindo->definirItemSelecionadoParaUso(nullptr);
        } 
        else 
        {
            SistemaPersonagem* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
            
            itemSelecionado->usar(personagemAgindo, alvo);
            
            personagemAgindo->obterInventario()->removerItem(itemSelecionado);
            personagemAgindo->definirItemSelecionadoParaUso(nullptr);
            turnoFoiConsumido = true;
            usouInventarioNoTurno = true;
        }
        Aparencia::aguardarEnter();
    }
}

void GerenciadorCombate::limparInimigosMortos()
{
    for (auto& inimigoPtr : listaDeInimigos) 
    {
        if (inimigoPtr->obterVida() <= 0) 
        {
                std::vector<SistemaPersonagem*> aliadosVivos;
                for (const auto& aliado : listaDeAliados) {
                    if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
                }
                TelaCombate::animarMorteInimigo(obterTituloDoCombate(), obterInimigosRaw(), inimigoPtr.get(), jogadorAtual, aliadosVivos);

            processarMorteDeInimigo(inimigoPtr.get());
            Aparencia::aguardarEnter();
        }
    }

    listaDeInimigos.erase(
        std::remove_if(listaDeInimigos.begin(), listaDeInimigos.end(),
            [](const std::unique_ptr<SistemaPersonagem>& inimigo) { return inimigo->obterVida() <= 0; }),
        listaDeInimigos.end()
    );
}

void GerenciadorCombate::executarTurnoDeTodosOsInimigos() 
{
    if (jogadorAtual->obterPularTurnoInimigo()) 
    {
        std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE) << "[EFEITO]: Os inimigos estao atordoados e nao podem agir!" << Aparencia::cor(Cor::RESET) << "\n";
        Aparencia::registrarLogBatalha("[EFEITO]: Os inimigos estao atordoados e nao podem agir!");
        jogadorAtual->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::string textoTurnoInimigos = "--- TURNO DOS INIMIGOS ---";
        std::string msgTurno = "\n" + Aparencia::espacosParaCentralizar(textoTurnoInimigos.length()) + textoTurnoInimigos + "\n";
        std::cout << msgTurno;
        TelaCombate::adicionarMensagemFixa(msgTurno);
        for (auto& inimigoAtualPtr : listaDeInimigos) 
        {
            if (jogadorAtual->obterVida() <= 0) break; // Interrompe se o jogador morrer
            
            SistemaPersonagem* inimigoAtual = inimigoAtualPtr.get();
            inimigoAtual->processarEfeitosInicioTurno();
            if (inimigoAtual->obterVida() <= 0) continue;

            std::string motivoIncapacidade;
            if (inimigoAtual->podeAgir(motivoIncapacidade)) 
            {
                SistemaPersonagem* alvo = jogadorAtual;
                std::vector<SistemaPersonagem*> aliadosVivos;
                for (auto& aliado : listaDeAliados) {
                    if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
                }
                if (!aliadosVivos.empty()) {
                    alvo = aliadosVivos[GeradorAleatorio::obterInteiro(0, static_cast<int>(aliadosVivos.size()) - 1)];
                }

                bool turnoConsumidoPorHabilidade = inimigoAtual->obterRaca()->tentarUsarHabilidadeAtiva(inimigoAtual, alvo, static_cast<int>(jogadorAtual->obterDificuldade()));
                
                if (!turnoConsumidoPorHabilidade) 
                {
                    realizarAtaqueFisico(inimigoAtual, alvo, contadorDoTurnoAtual);
                }
            }
            else
            {
                std::string msg = Aparencia::margemCombate() + Aparencia::cor(Cor::VERDE) + ">> [EFEITO]: " + inimigoAtual->obterNome() + " esta sob efeito de " + motivoIncapacidade + " e nao pode agir neste turno!" + Aparencia::cor(Cor::RESET) + "\n";
                std::cout << msg;
                TelaCombate::adicionarMensagemFixa(msg);
                Aparencia::registrarLogBatalha("[EFEITO]: " + inimigoAtual->obterNome() + " esta sob efeito de " + motivoIncapacidade + " e nao pode agir!");
            }
        }
    }

    if (jogadorAtual->obterDefendendo())
    {
        jogadorAtual->definirDefendendo(false);
        jogadorAtual->definirRecargaDefesa(true);
    }
    else if (jogadorAtual->obterRecargaDefesa())
    {
        jogadorAtual->definirRecargaDefesa(false);
    }



    if (jogadorAtual->obterRecarga()) jogadorAtual->definirRecarga(false);
    Aparencia::aguardarEnter();
}

void GerenciadorCombate::realizarAtaqueFisico(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemDefensor, int turnoAtualDoCombate) 
{
    auto [danoBaseCalculado, danoPerfurante] = calcularDanoBase(personagemAtacante);

    bool isAtacanteJogadorOuAliado = isPersonagemJogadorOuAliado(personagemAtacante);

    if (isAtacanteJogadorOuAliado || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2) 
    {
        danoBaseCalculado = personagemAtacante->obterRaca()->processarDanoOfensivo(danoBaseCalculado, personagemAtacante);
    }

    auto callbackAplicarDano = [this, turnoAtualDoCombate](SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoBruto, int perfurante) {
        this->aplicarDanoAoAlvo(atacante, alvo, danoBruto, perfurante, turnoAtualDoCombate);
    };

    personagemAtacante->obterClasse()->executarAtaqueComPassivaDaClasse(personagemAtacante, personagemDefensor, danoBaseCalculado, danoPerfurante, listaDeInimigos, callbackAplicarDano, isAtacanteJogadorOuAliado);
}

std::pair<int, int> GerenciadorCombate::calcularDanoBase(SistemaPersonagem* atacante) 
{
    double multiplicadorDeAtributos = atacante->obterMultiplicador();
    if (atacante->possuiEfeito(EfeitoID::Inviolavel)) multiplicadorDeAtributos *= 2.0;

    int danoFisicoDaArma = 1;
    int danoMagicoDaArma = 0;
    int perfuranteAtual = 0;

    if (atacante->obterArma()) 
    {
        danoFisicoDaArma = atacante->obterArma()->obterDanoFisico();
        danoMagicoDaArma = atacante->obterArma()->obterDanoMagico();

        if (atacante->obterArma()->temPropriedade(Propriedade::Magica)) {
            int bonusMagico = danoFisicoDaArma / 2;
            double bonusEscalado = bonusMagico * (1.0 + (atacante->obterSabedoria() / 100.0));
            perfuranteAtual = static_cast<int>(bonusEscalado * multiplicadorDeAtributos);
        }
    }

    int forcaEfetiva = atacante->obterForca();
    int destrezaEfetiva = atacante->obterDestreza();
    int inteligenciaEfetiva = atacante->obterInteligencia();
    int sabedoriaEfetiva = atacante->obterSabedoria();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        forcaEfetiva /= 10; destrezaEfetiva /= 10;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        inteligenciaEfetiva /= 10; sabedoriaEfetiva /= 10;
    }

    int danoFisicoCalculado = std::max(0, static_cast<int>((danoFisicoDaArma + forcaEfetiva) * (1.0 + (destrezaEfetiva / 100.0))));
    int danoMagicoCalculado = std::max(0, static_cast<int>((danoMagicoDaArma + inteligenciaEfetiva) * (1.0 + (sabedoriaEfetiva / 100.0))));
    int total = std::max(1, danoFisicoCalculado + danoMagicoCalculado);

    return { static_cast<int>(total * multiplicadorDeAtributos), perfuranteAtual };
}

void GerenciadorCombate::aplicarDanoAoAlvo(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate) 
{
    if (personagemAlvo->possuiEfeito(EfeitoID::Inviolavel))
    {
        std::string msg = "\n" + Aparencia::margemCombate() + "[COMBATE]: O alvo esta Inviolavel e desviou do ataque perfeitamente!\n";
        std::cout << msg;
        TelaCombate::adicionarMensagemFixa(msg);
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante->obterArma()) personagemAtacante->obterArma()->antesDeCausarDano(personagemAtacante, personagemAlvo);

    int danoBaseMitigado = personagemAlvo->calcularDefesaBase(quantidadeDeDanoBruto, danoPerfurante);
    int quantidadeDeDanoReduzidoPeloParry = 0;
    bool tentouParry = false;
    bool parryFoiBemSucedido = false;

    // Logica do Parry
    if (personagemAlvo->obterParryAtivado()) 
    {
        tentouParry = true;
        parryFoiBemSucedido = SistemaParry::tentarParry(personagemAtacante, danoBaseMitigado, quantidadeDeDanoReduzidoPeloParry);
    }

    bool aplicarPassivas = (isPersonagemJogadorOuAliado(personagemAlvo) || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2);

    int danoBloqueado = 0;
    bool escudoQuebrou = false;
    std::string nomeEscudoQuebrado = "";
    int danoFinalAposReducoes = personagemAlvo->receberDano(quantidadeDeDanoBruto, danoPerfurante, quantidadeDeDanoReduzidoPeloParry, personagemAtacante, aplicarPassivas, danoBloqueado, escudoQuebrou, nomeEscudoQuebrado);

    exibirResultadoDoAtaque(personagemAlvo, danoFinalAposReducoes, tentouParry, parryFoiBemSucedido, danoBloqueado, escudoQuebrou, nomeEscudoQuebrado);

    std::vector<SistemaPersonagem*> aliadosVivos;
    for (const auto& aliado : listaDeAliados) {
        if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
    }

    if (danoFinalAposReducoes > 0) 
    {
        // ANIMACAO DO DANO NO INIMIGO (Piscar Vermelho + Flicker)
        if (!isPersonagemJogadorOuAliado(personagemAlvo)) {
            TelaCombate::animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), personagemAlvo, jogadorAtual, aliadosVivos);
        }
        else {
            TelaCombate::animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAlvo, jogadorAtual, aliadosVivos, false);
        }

        // Aplicação dos efeitos no acerto
        if (personagemAtacante->obterArma()) 
        {
            personagemAtacante->obterArma()->aoCausarDano(personagemAtacante, personagemAlvo, danoFinalAposReducoes);
        }

        personagemAtacante->obterRaca()->aoCausarDano(personagemAtacante, personagemAlvo, danoFinalAposReducoes);
    }
    else if (tentouParry && parryFoiBemSucedido && isPersonagemJogadorOuAliado(personagemAlvo))
    {
        // Animaçao Ciano na HUD caso execute um bloqueio/parry perfeito e anule 100% do dano
        TelaCombate::animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAlvo, jogadorAtual, aliadosVivos, true);
    }
}

void GerenciadorCombate::exibirResultadoDoAtaque(SistemaPersonagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado)
{
    bool isJogadorOuAliado = isPersonagemJogadorOuAliado(alvo);
    std::string msg = "";

    if (danoBloqueado > 0) {
        msg += Aparencia::margemCombate() + Aparencia::cor(Cor::CIANO) + ">> [DEFESA]: O escudo bloqueou " + std::to_string(danoBloqueado) + " de dano!" + Aparencia::cor(Cor::RESET) + "\n";
        Aparencia::registrarLogBatalha(">> [DEFESA]: O escudo bloqueou " + std::to_string(danoBloqueado) + " de dano!");
        
        if (escudoQuebrou) {
            msg += Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_VERMELHO) + "[!] ALERTA: O escudo " + nomeEscudoQuebrado + " foi DESTRUIDO em pedacos!" + Aparencia::cor(Cor::RESET) + "\n";
            Aparencia::registrarLogBatalha("[!] ALERTA: O escudo " + nomeEscudoQuebrado + " foi DESTRUIDO em pedacos!");
        }
    }

    if (isJogadorOuAliado) 
    {
        if (tentouParry) 
        {
            std::string mensagemParry = parrySucesso ? (danoFinal <= 1 ? "parry efetivo! Ataque anulado!" : "parry efetivo! Mas o ataque e muito forte!") : "parry falhou!";
            msg = Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_VERMELHO) + ">> [PARRY]: " + mensagemParry + " Inimigo causou " + std::to_string(danoFinal) + " de dano em " + alvo->obterNome() + Aparencia::cor(Cor::RESET) + "\n";
            Aparencia::registrarLogBatalha("[PARRY]: " + mensagemParry + " Inimigo causou " + std::to_string(danoFinal) + " de dano em " + alvo->obterNome());
        }
        else if (danoFinal > 0) 
        {
            msg = Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_VERMELHO) + ">> " + alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano" + Aparencia::cor(Cor::RESET) + "\n";
            Aparencia::registrarLogBatalha(">> " + alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano");
        }
        else if (danoFinal == 0 && alvo->obterDefendendo()) 
        {
            msg = Aparencia::margemCombate() + ">> O dano foi totalmente absorvido pela defesa de " + alvo->obterNome() + "!\n";
            Aparencia::registrarLogBatalha(">> O dano foi totalmente absorvido pela defesa de " + alvo->obterNome() + "!");
        }
        
        if (danoFinal > 0 && alvo == jogadorAtual) totalDeDanoRecebido += danoFinal;
    }
    else if (danoFinal > 0) 
    {
        if (alvo != jogadorAtual) totalDeDanoCausado += danoFinal;
        msg = Aparencia::margemCombate() + ">> " + alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano\n";
        Aparencia::registrarLogBatalha(">> " + alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano");
    }

    if (!msg.empty()) {
        std::cout << msg;
        TelaCombate::adicionarMensagemFixa(msg);
    }
}

bool GerenciadorCombate::verificarCondicaoDeVitoriaOuDerrota() 
{
    if (listaDeInimigos.empty()) 
    { 
        jogadorAtual->limparEfeitos(); // Remove buffs e debuffs antes de voltar ao mapa
        TelaVitoria::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, jogadorAtual->obterCuraTotalRecebida(), contadorDoTurnoAtual, itensObtidos);
        jogadorAtual->finalizarBatalha();
        return true; 
    }
    if (jogadorAtual->obterVida() <= 0) 
    { 
        jogadorAtual->limparEfeitos(); // Remove buffs e debuffs na morte
        TelaDerrota::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, jogadorAtual->obterCuraTotalRecebida(), contadorDoTurnoAtual); 
        jogadorAtual->finalizarBatalha();
        return true; 
    }
    return false;
}

void GerenciadorCombate::processarMorteDeInimigo(SistemaPersonagem* inimigo)
{
    std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::AMARELO) << "[!] " << inimigo->obterNome() << " derrotado!" << Aparencia::cor(Cor::RESET) << "\n";
    Aparencia::registrarLogBatalha("[!] " + inimigo->obterNome() + " derrotado!");

    SistemaBestiario::instancia().registrarDerrota(inimigo->obterNome());

    size_t itensAntes = itensObtidos.size();
    inimigo->executarDrops(jogadorAtual, itensObtidos, quantidadeDeOuroObtido, quantidadeDeXpObtido);
    for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
        SistemaBestiario::instancia().registrarDrop(inimigo->obterNome(), itensObtidos[i]);
    }
}