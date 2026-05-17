#include "GerenciadorCombate.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <map>
#include <chrono>

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
#include "../Utilidades/ControleDeInput.h"

namespace {
    void registrarLog(const std::string& texto, Cor cor = Cor::RESET) {
        if (cor != Cor::RESET) {
            Aparencia::registrarLogBatalha(Aparencia::cor(cor) + texto + Aparencia::cor(Cor::RESET));
        } else {
            Aparencia::registrarLogBatalha(texto);
        }
    }
}

SistemaPersonagem* g_inimigoAtacanteParry = nullptr;
int g_parryStatus = 0;

GerenciadorCombate::GerenciadorCombate(SistemaPersonagem* jogadorParaOCombate, std::vector<std::unique_ptr<SistemaPersonagem>>&& inimigosParaOCombate) 
    : jogadorAtual(jogadorParaOCombate), listaDeInimigos(std::move(inimigosParaOCombate)), contadorDoTurnoAtual(1), quantidadeDeOuroObtido(0), quantidadeDeXpObtido(0), totalDeDanoCausado(0), totalDeDanoRecebido(0)
{

    int nivelDeDificuldade = static_cast<int>(jogadorAtual->obterDificuldade());
    double multiplicadorDeDificuldadeDosInimigos = 1.0;

    if (nivelDeDificuldade == 2) {
        multiplicadorDeDificuldadeDosInimigos = 1.5;
    } else if (nivelDeDificuldade == 3) {
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
    TelaCombate::atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw(), animarEntrada);
}

std::vector<SistemaPersonagem*> GerenciadorCombate::obterAliadosVivosRaw() const {
    std::vector<SistemaPersonagem*> aliadosVivos;
    for (const auto& aliado : listaDeAliados) {
        if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
    }
    return aliadosVivos;
}

void GerenciadorCombate::prepararTurnoPersonagem(SistemaPersonagem* personagem) {
    TelaCombate::limparMensagensFixas();
    registrarLog("");
    registrarLog("═══ TURNO " + std::to_string(contadorDoTurnoAtual) + " ║ VEZ DE " + personagem->obterNome() + " ═══");
    TelaCombate::definirTurnoVisivel(contadorDoTurnoAtual, personagem->obterNome());
    personagem->reduzirCooldowns();
    personagem->processarEfeitosInicioTurno();
}

bool GerenciadorCombate::executarTurnoJogadorOuAliado(SistemaPersonagem* personagem, bool& primeiraRenderizacao) {
    prepararTurnoPersonagem(personagem);
    if (personagem->obterVida() <= 0) return false;

    bool turnoConsumido = false;
    bool usouInventario = false;

    while (!turnoConsumido && personagem->obterVida() > 0 && !listaDeInimigos.empty()) {
        exibirTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        processarMenuDeAcoesDoJogador(personagem, turnoConsumido, usouInventario);
        if (verificarCondicaoDeVitoriaOuDerrota()) return true; 
    }

    if (usouInventario) {
        exibirTelaDeCombate();
        TelaCombate::notificarDesprevencaoInventario();
    }
    return false;
}

void GerenciadorCombate::iniciarCombate() 
{
    jogadorAtual->prepararParaNovaBatalha();
    Aparencia::limparLogBatalha();
    TelaCombate::limparMensagensFixas();

    for (auto& aliado : listaDeAliados) {
        aliado->prepararParaNovaBatalha();
    }

    // Introducao ao combate
    Aparencia::limparTela();
    TelaCombate::exibirLogoParaTelaDeCombate(obterTituloDoCombate());
    TelaCombate::exibirHordaDeInimigosLadoALado(obterInimigosRaw(), nullptr, 0, false, true);
    std::cout << "\n";
    Aparencia::imprimirCentralizado("Prepare-se! O combate esta prestes a comecar...", Aparencia::cor(Cor::VERMELHO));
    std::cout << "\n";
    ControleDeInput::aguardarEnter();

    Aparencia::limparTela();

    int maxDestrezaInimigos = 0;
    for (const auto& inimigoPtr : listaDeInimigos) {
        SistemaBestiario::instancia().registrarPrimeiraVista(inimigoPtr->obterNome());
        if (inimigoPtr->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigoPtr->obterDestreza();
    }
    
    bool turnoExtraFirstTurn = (jogadorAtual->obterDestreza() > (maxDestrezaInimigos * 2));
    bool primeiraRenderizacao = false; // Modificado, pois ja animamos na intro
    
    if (maxDestrezaInimigos > jogadorAtual->obterDestreza()) {
        exibirTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        
        if (maxDestrezaInimigos > (jogadorAtual->obterDestreza() * 2)) {
            std::string msg = "[SISTEMA]: A agilidade extrema dos inimigos (" + std::to_string(maxDestrezaInimigos) + " VS " + std::to_string(jogadorAtual->obterDestreza()) + ") permite que eles ataquem duas vezes seguidas!";
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERMELHO) << msg << Aparencia::cor(Cor::RESET) << "\n";
            Aparencia::registrarLogBatalha(msg);
            ControleDeInput::aguardarEnter();

            executarTurnoDeTodosOsInimigos();
            limparInimigosMortos();
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
            executarTurnoDeTodosOsInimigos();
            limparInimigosMortos();
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
            
            contadorDoTurnoAtual++; // Jogador comeca no Turno 2
        } else {
            TelaCombate::notificarInimigosMaisAgeis();
            executarTurnoDeTodosOsInimigos();
            limparInimigosMortos();
            if (verificarCondicaoDeVitoriaOuDerrota()) return;
        }
    }

    while (jogadorAtual->obterVida() > 0 && !listaDeInimigos.empty()) {
        // Turno do Jogador
        if (jogadorAtual->obterVida() > 0) {
            if (executarTurnoJogadorOuAliado(jogadorAtual, primeiraRenderizacao)) return;

            if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) {
                TelaCombate::notificarTurnoExtra(jogadorAtual->obterDestreza(), maxDestrezaInimigos);
                turnoExtraFirstTurn = false;
                continue;
            }
        }
        
        // Turnos dos Aliados
        for (auto& aliado : listaDeAliados) {
            if (aliado->obterVida() <= 0 || listaDeInimigos.empty()) continue;
            
            bool isPrimeiraRend = false;
            if (executarTurnoJogadorOuAliado(aliado.get(), isPrimeiraRend)) return;
        }
        
        executarTurnoDeTodosOsInimigos();
        limparInimigosMortos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;

        contadorDoTurnoAtual++;
    }
}

void GerenciadorCombate::processarMenuDeAcoesDoJogador(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int acaoEscolhida = TelaCombate::obterAcaoDoJogador(contadorDoTurnoAtual, personagemAgindo, obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw());
    
    TelaCombate::personagemHUD = nullptr; // Forca reset visual ao retornar para evitar bugs de persistencia de interface

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
            TelaCombate::notificarAcaoInvalida();
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
        int indiceDoAlvoEscolhido = TelaCombate::obterAlvoAtaque(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw());
        if (indiceDoAlvoEscolhido == -1) return;

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
        TelaCombate::notificarSemEscudos(personagemAgindo->obterNome());
        return nullptr;
    }

    int opcaoEscolhida = TelaCombate::obterEscolhaDeEscudo(personagemAgindo->obterNome(), listaDeEscudos);
    return (opcaoEscolhida == 0) ? nullptr : listaDeEscudos[opcaoEscolhida - 1];
}

void GerenciadorCombate::processarAcaoDefender(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterRecargaDefesa()) 
    {
        TelaCombate::notificarDesequilibrioDefesa(personagemAgindo->obterNome());
        return; 
    }
    
    Item* escudoEscolhido = selecionarEscudo(personagemAgindo);
    if (escudoEscolhido != nullptr) 
    {
        if (!escudoEscolhido->podeSerEquipadoPor(personagemAgindo)) {
            TelaCombate::notificarRequisitoNaoAtendido(escudoEscolhido->obterMensagemRequisito());
            return;
        }

        personagemAgindo->equiparItem(escudoEscolhido);
        personagemAgindo->definirDefendendo(true);
        TelaCombate::notificarPosturaDefensiva(personagemAgindo->obterNome(), escudoEscolhido->obterNomeItem());
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
    else ControleDeInput::aguardarEnter();
}

void GerenciadorCombate::processarAcaoInventario(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int vidaAntes = personagemAgindo->obterVida();
    bool inventarioConsumiu = false;
    
    InventarioCombate::gerenciarInventario(personagemAgindo, &inventarioConsumiu);
    if (inventarioConsumiu) {
        turnoFoiConsumido = true;
        usouInventarioNoTurno = true;
    }
    
    if (personagemAgindo->obterVida() > vidaAntes) {
        TelaCombate::animarCuraNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAgindo, jogadorAtual, obterAliadosVivosRaw(), personagemAgindo->obterVida() - vidaAntes);
    }

    if (personagemAgindo->obterItemSelecionadoParaUso() != nullptr) 
    {
        Item* itemSelecionado = personagemAgindo->obterItemSelecionadoParaUso();
        
        int indiceDoAlvoEscolhido = TelaCombate::obterAlvoItem(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw());

        if (indiceDoAlvoEscolhido == -1) 
        {
            TelaCombate::notificarCancelamentoItem();
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
        exibirTelaDeCombate();
        ControleDeInput::aguardarEnter();
    }
}

void GerenciadorCombate::limparInimigosMortos()
{
    for (auto& inimigoPtr : listaDeInimigos) 
    {
        if (inimigoPtr->obterVida() <= 0) 
        {
                int xpAntes = quantidadeDeXpObtido;
                int ouroAntes = quantidadeDeOuroObtido;
                size_t itensAntes = itensObtidos.size();

                processarMorteDeInimigo(inimigoPtr.get());

                int xpDrop = quantidadeDeXpObtido - xpAntes;
                int ouroDrop = quantidadeDeOuroObtido - ouroAntes;
                
                std::vector<std::string> dropsDaMorte;
                if (xpDrop > 0) dropsDaMorte.push_back("+" + std::to_string(xpDrop) + " XP");
                if (ouroDrop > 0) dropsDaMorte.push_back("+" + std::to_string(ouroDrop) + "G");
                
                std::map<std::string, int> contagemItens;
                for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
                    contagemItens[itensObtidos[i]]++;
                }
                for (auto const& [nome, qtd] : contagemItens) {
                    dropsDaMorte.push_back("+" + std::to_string(qtd) + "x " + nome);
                }

                std::vector<SistemaPersonagem*> aliadosVivos = obterAliadosVivosRaw();
                TelaCombate::animarMorteInimigo(obterTituloDoCombate(), obterInimigosRaw(), inimigoPtr.get(), jogadorAtual, aliadosVivos, dropsDaMorte);
            ControleDeInput::aguardarEnter();
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
    TelaCombate::limparMensagensFixas();
    if (jogadorAtual->obterPularTurnoInimigo()) 
    {
        // A mensagem na UI foi removida para priorizar o combate limpo
        registrarLog("[EFEITO]: Os inimigos estao atordoados e nao podem agir!", Cor::VERDE);
        jogadorAtual->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::string textoTurnoInimigos = "═══ TURNO " + std::to_string(contadorDoTurnoAtual) + " ║ VEZ DOS INIMIGOS ═══";
        registrarLog("");
        registrarLog(textoTurnoInimigos);
            TelaCombate::definirTurnoVisivel(contadorDoTurnoAtual, "INIMIGOS");
            exibirTelaDeCombate(false); // Forca o HUD a atualizar o nome do Turno para os inimigos antes do ataque iniciar
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
                std::vector<SistemaPersonagem*> aliadosVivos = obterAliadosVivosRaw();
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
                // A mensagem na UI foi removida para priorizar o combate limpo
                registrarLog("[EFEITO]: " + inimigoAtual->obterNome() + " esta sob efeito de " + motivoIncapacidade + " e nao pode agir!", Cor::VERDE);
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
    ControleDeInput::aguardarEnter();
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

    bool aplicarPassivaClasse = isAtacanteJogadorOuAliado || static_cast<int>(jogadorAtual->obterDificuldade()) == 3;

    personagemAtacante->obterClasse()->executarAtaqueComPassivaDaClasse(personagemAtacante, personagemDefensor, danoBaseCalculado, danoPerfurante, listaDeInimigos, callbackAplicarDano, aplicarPassivaClasse);
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

    int totalFinal = static_cast<int>(total * multiplicadorDeAtributos);
    int perfuranteFinal = perfuranteAtual;

    if (atacante->obterArma() && atacante->obterArma()->temPropriedade(Propriedade::IgnoraDefesa)) {
        perfuranteFinal = totalFinal;
    }

    return { totalFinal, perfuranteFinal };
}

void GerenciadorCombate::processarPosDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso) {
    std::vector<SistemaPersonagem*> aliadosVivos = obterAliadosVivosRaw();

    g_inimigoAtacanteParry = atacante;
    g_parryStatus = 0;
    if (tentouParry) {
        if (parrySucesso) {
            if (danoFinal <= 0) g_parryStatus = 1;
            else g_parryStatus = 2;
        } else {
            g_parryStatus = 3;
        }
    }

    if (danoFinal > 0) 
    {
        // ANIMACAO DO DANO NO INIMIGO (Piscar Vermelho + Flicker)
        if (!isPersonagemJogadorOuAliado(alvo)) {
            TelaCombate::animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), alvo, atacante, jogadorAtual, aliadosVivos, danoFinal);
        }
        else {
            TelaCombate::animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), alvo, jogadorAtual, aliadosVivos, false, danoFinal);
        }

        // Aplicação dos efeitos no acerto
        int vidaAtacanteAntes = atacante->obterVida();
        
        if (atacante->obterArma()) {
            atacante->obterArma()->aoCausarDano(atacante, alvo, danoFinal);
        }
        atacante->obterRaca()->aoCausarDano(atacante, alvo, danoFinal);
        
        // Verifica se o atacante se curou (Ex: Passiva da Abominacao)
        if (atacante->obterVida() > vidaAtacanteAntes) {
            if (!isPersonagemJogadorOuAliado(atacante)) {
                TelaCombate::animarCuraNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), atacante, jogadorAtual, aliadosVivos, atacante->obterVida() - vidaAtacanteAntes);
            } else {
                TelaCombate::animarCuraNoJogador(obterTituloDoCombate(), obterInimigosRaw(), atacante, jogadorAtual, aliadosVivos, atacante->obterVida() - vidaAtacanteAntes);
            }
        }
        
        if (alvo->obterArmadura() && alvo->obterArmadura()->temPropriedade(Propriedade::ArmaduraAdaptacao)) {
            auto* ef = const_cast<EfeitoStatus*>(alvo->encontrarEfeito(EfeitoID::RodaAdaptacao));
            if (ef) {
                auto* efRoda = dynamic_cast<EfeitoRodaAdaptacao*>(ef);
                if (efRoda) efRoda->adaptar(alvo, atacante);
            }
        }
    }
    else if (tentouParry && parrySucesso && isPersonagemJogadorOuAliado(alvo)) {
        TelaCombate::animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), alvo, jogadorAtual, aliadosVivos, true, danoFinal);
    } else {
        TelaCombate::atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, aliadosVivos);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    g_inimigoAtacanteParry = nullptr;
    g_parryStatus = 0;
}

void GerenciadorCombate::aplicarDanoAoAlvo(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate) 
{
    if (personagemAlvo->possuiEfeito(EfeitoID::Inviolavel))
    {
        std::string msgSemCor = "[COMBATE]: O alvo esta Inviolavel e desviou do ataque perfeitamente!";
        registrarLog(msgSemCor, Cor::CIANO);
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante->obterArma()) personagemAtacante->obterArma()->antesDeCausarDano(personagemAtacante, personagemAlvo);

    int danoBaseMitigado = personagemAlvo->calcularDefesaBase(quantidadeDeDanoBruto, danoPerfurante);
    int quantidadeDeDanoReduzidoPeloParry = 0;
    bool tentouParry = false;
    bool parryFoiBemSucedido = false;
    
    bool ataqueImparavel = personagemAtacante && personagemAtacante->obterRaca()->ignoraParry();

    // Logica do Parry
    if (personagemAlvo->obterParryAtivado()) 
    {
        if (ataqueImparavel) {
            std::string msgImparavel = "[!] " + personagemAtacante->obterNome() + " desfere um ATAQUE IMPARAVEL! O Parry foi ignorado!";
            registrarLog(msgImparavel, Cor::FUNDO_VERMELHO);
            TelaCombate::adicionarMensagemFixa(Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_VERMELHO) + msgImparavel + Aparencia::cor(Cor::RESET) + "\n");
        } else {
            tentouParry = true;
            parryFoiBemSucedido = SistemaParry::tentarParry(personagemAtacante, danoBaseMitigado, quantidadeDeDanoReduzidoPeloParry);
        }
    }

    bool aplicarPassivas = (isPersonagemJogadorOuAliado(personagemAlvo) || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2);

    ResultadoDano res = personagemAlvo->receberDano(quantidadeDeDanoBruto, danoPerfurante, quantidadeDeDanoReduzidoPeloParry, personagemAtacante, aplicarPassivas);

    exibirResultadoDoAtaque(personagemAlvo, res.danoFinal, tentouParry, parryFoiBemSucedido, res.danoBloqueado, res.escudoQuebrou, res.nomeEscudoQuebrado);

    processarPosDano(personagemAtacante, personagemAlvo, res.danoFinal, tentouParry, parryFoiBemSucedido);

    if (tentouParry && parryFoiBemSucedido && res.danoFinal <= 0 && isPersonagemJogadorOuAliado(personagemAlvo) && personagemAtacante) {
        personagemAtacante->obterRaca()->aoSofrerParryPerfeito();

        int danoRefletido = std::max(1, (quantidadeDeDanoBruto + danoPerfurante) / 4);
        personagemAtacante->modificarVida(-danoRefletido);
        
        std::string msgReflexao = ">> [PARRY PERFEITO]: Reflexao! " + personagemAtacante->obterNome() + " sofreu " + std::to_string(danoRefletido) + " de dano de volta!";
        registrarLog(msgReflexao, Cor::AMARELO);
        TelaCombate::adicionarMensagemFixa(Aparencia::margemCombate() + Aparencia::cor(Cor::AMARELO) + msgReflexao + Aparencia::cor(Cor::RESET) + "\n");
        
        std::vector<SistemaPersonagem*> aliadosVivos = obterAliadosVivosRaw();
        if (!isPersonagemJogadorOuAliado(personagemAtacante)) {
            TelaCombate::animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), personagemAtacante, personagemAlvo, jogadorAtual, aliadosVivos, danoRefletido);
            totalDeDanoCausado += danoRefletido;
        } else {
            TelaCombate::animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAtacante, jogadorAtual, aliadosVivos, false, danoRefletido);
        }
    }
}

void GerenciadorCombate::exibirResultadoDoAtaque(SistemaPersonagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado)
{
    bool isJogadorOuAliado = isPersonagemJogadorOuAliado(alvo);
    std::string msg = "";

    if (danoBloqueado > 0) {
        msg += Aparencia::margemCombate() + Aparencia::cor(Cor::CIANO) + ">> [DEFESA]: O escudo bloqueou " + std::to_string(danoBloqueado) + " de dano!" + Aparencia::cor(Cor::RESET) + "\n";
        registrarLog(">> [DEFESA]: O escudo bloqueou " + std::to_string(danoBloqueado) + " de dano!", Cor::CIANO);
        
        if (escudoQuebrou) {
            msg += Aparencia::margemCombate() + Aparencia::cor(Cor::FUNDO_VERMELHO) + "[!] ALERTA: O escudo " + nomeEscudoQuebrado + " foi DESTRUIDO em pedacos!" + Aparencia::cor(Cor::RESET) + "\n";
            registrarLog("[!] ALERTA: O escudo " + nomeEscudoQuebrado + " foi DESTRUIDO em pedacos!", Cor::VERMELHO);
        }
    }

    if (isJogadorOuAliado) 
    {
        if (tentouParry) 
        {
            std::string mensagemParryLog;
            if (parrySucesso) {
                if (danoFinal <= 0) {
                    mensagemParryLog = "parry perfeito! Ataque anulado!";
                } else {
                    mensagemParryLog = "parry efetivo! Mas o ataque e muito forte!";
                }
            } else {
                mensagemParryLog = "parry falhou!";
            }
            
            registrarLog("[PARRY]: " + mensagemParryLog + " Inimigo causou " + std::to_string(danoFinal) + " de dano em " + alvo->obterNome(), Cor::VERMELHO_CLARO);
        }
        else if (danoFinal > 0) 
        {
            // A mensagem estatica na UI de dano ao jogador foi comentada para priorizar o Texto de Dano Flutuante
            registrarLog(">> " + alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano", Cor::VERMELHO_CLARO);
        }
        else if (danoFinal == 0 && alvo->obterDefendendo()) 
        {
            // A mensagem estatica na UI de dano ao jogador foi comentada para priorizar o Texto de Dano Flutuante
            registrarLog(">> O dano foi totalmente absorvido pela defesa de " + alvo->obterNome() + "!", Cor::CIANO);
        }
        
        if (danoFinal > 0 && alvo == jogadorAtual) totalDeDanoRecebido += danoFinal;
    }
    else if (danoFinal > 0) 
    {
        if (alvo != jogadorAtual) totalDeDanoCausado += danoFinal;
        // A mensagem estatica na UI de dano aos inimigos foi comentada para priorizar o Texto de Dano Flutuante
        registrarLog(">> " + alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano", Cor::VERMELHO);
    }

    if (!msg.empty()) {
        TelaCombate::adicionarMensagemFixa(msg);
    }
}

bool GerenciadorCombate::verificarCondicaoDeVitoriaOuDerrota() 
{
    bool isVitoria = listaDeInimigos.empty();
    bool isDerrota = jogadorAtual->obterVida() <= 0;

    if (isVitoria || isDerrota) 
    { 
        jogadorAtual->limparEfeitos(); // Remove buffs e debuffs ao final da batalha
        if (isVitoria) {
            TelaVitoria::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, jogadorAtual->obterCuraTotalRecebida(), contadorDoTurnoAtual, itensObtidos);
        } else {
            TelaDerrota::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, jogadorAtual->obterCuraTotalRecebida(), contadorDoTurnoAtual); 
        }
        jogadorAtual->finalizarBatalha();
        return true; 
    }
    return false;
}

void GerenciadorCombate::processarMorteDeInimigo(SistemaPersonagem* inimigo)
{
    registrarLog("[!] " + inimigo->obterNome() + " derrotado!", Cor::VERMELHO);

    SistemaBestiario::instancia().registrarDerrota(inimigo->obterNome());

    registrarLog("═══ DROPS ═══", Cor::AMARELO);

    size_t itensAntes = itensObtidos.size();
    inimigo->executarDrops(jogadorAtual, itensObtidos, quantidadeDeOuroObtido, quantidadeDeXpObtido);
    for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
        SistemaBestiario::instancia().registrarDrop(inimigo->obterNome(), itensObtidos[i]);
    }
}