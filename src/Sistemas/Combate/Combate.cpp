#include "Combate.h"
#include "CombateUIImpl.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <map>
#include <chrono>

#include "../../Entidades/Classes/ClasseBase.h"
#include "../Inventario/InventarioCombate.h"
#include "../Inventario/Item.h"
#include "../Inventario/Equipamentos/EquipamentoEscudo.h"
#include "../../Entidades/Inimigos/Mahoraga.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../Progresso/Bestiario.h"
#include "../Progresso/Diario.h"
#include "../Progresso/Progressao.h"
#include "../Progresso/ProgressaoFlags.h"
#include "Parry.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/GeradorAleatorio.h"
#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "Mecanicas/CalculadoraDano.h"
#include "Mecanicas/MecanicasInimigo.h"
#include "Mecanicas/GerenciadorTurnos.h"

namespace {
    void registrarLog(const std::string& texto, Cor cor = Cor::RESET) {
        if (cor != Cor::RESET) {
            Aparencia::registrarLogBatalha(Aparencia::cor(cor) + texto + Aparencia::cor(Cor::RESET));
        } else {
            Aparencia::registrarLogBatalha(texto);
        }
    }
}

Personagem* g_inimigoAtacanteParry = nullptr;
int g_parryStatus = 0;

int Combate::stats_parriesTentados = 0;
int Combate::stats_parriesEfetivos = 0;
int Combate::stats_parriesPerfeitos = 0;
int Combate::stats_maiorDanoCausado = 0;
int Combate::stats_itensConsumidos = 0;
std::vector<std::string> Combate::stats_novasDescobertas;

void Combate::resetarEstatisticasAvancadas() {
    stats_parriesTentados = 0;
    stats_parriesEfetivos = 0;
    stats_parriesPerfeitos = 0;
    stats_maiorDanoCausado = 0;
    stats_itensConsumidos = 0;
    stats_novasDescobertas.clear();
}

Combate::Combate(Personagem* jogadorParaOCombate, std::vector<std::unique_ptr<Personagem>>&& inimigosParaOCombate, std::unique_ptr<ICombateUI> interfaceVisual) 
    : jogadorAtual(jogadorParaOCombate), listaDeInimigos(std::move(inimigosParaOCombate)), quantidadeDeOuroObtido(0), quantidadeDeXpObtido(0), totalDeDanoCausado(0), totalDeDanoRecebido(0), contadorDoTurnoAtual(1),
      isModo3D(false), jogadorPosX(0.0f), jogadorPosY(0.0f), jogadorAngulo(0.0f), tituloMapaAtual(""),
      ui(interfaceVisual ? std::move(interfaceVisual) : std::make_unique<CombateUIImpl>())
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

void Combate::setContexto3D(bool modo3D, const std::vector<std::string>& matriz, float posX, float posY, float angulo, const std::string& titulo) {
    this->isModo3D = modo3D;
    this->matrizDoMapaAtual = matriz;
    this->jogadorPosX = posX;
    this->jogadorPosY = posY;
    this->jogadorAngulo = angulo;
    this->tituloMapaAtual = titulo;
}

void Combate::adicionarAliados(std::vector<std::unique_ptr<Personagem>> aliados)
{
    listaDeAliados = std::move(aliados);
}

void Combate::adicionarAliadoEmCombate(std::unique_ptr<Personagem> aliado) {
    listaDeAliados.push_back(std::move(aliado));
}

Combate::~Combate()
{
    Parry::onUpdateScreen = nullptr;
    ControleDeInput::onAguardarEnterUpdate = nullptr;
}

std::string Combate::obterTituloDoCombate() const
{
    std::string titulo = "EM COMBATE (";
    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
        titulo += listaDeInimigos[i]->obterNome();
        if (i < listaDeInimigos.size() - 1) titulo += ", ";
    }
    titulo += ")";
    return titulo;
}

bool Combate::isPersonagemJogadorOuAliado(Personagem* personagem) const {
    if (personagem == jogadorAtual) return true;
    for (const auto& aliadoAtual : listaDeAliados) {
        if (aliadoAtual.get() == personagem) return true;
    }
    return false;
}

std::vector<Personagem*> Combate::obterInimigosRaw() const
{
    std::vector<Personagem*> ponteirosInimigos(listaDeInimigos.size());
    std::transform(listaDeInimigos.begin(), listaDeInimigos.end(), ponteirosInimigos.begin(), [](const std::unique_ptr<Personagem>& ptr) { return ptr.get(); });
    return ponteirosInimigos;
}

void Combate::exibirTelaDeCombate(bool animarEntrada) const
{
    ui->configurarContexto3D(isModo3D, matrizDoMapaAtual, jogadorPosX, jogadorPosY, jogadorAngulo, tituloMapaAtual);
    ui->atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw(), animarEntrada);
}

std::vector<Personagem*> Combate::obterAliadosVivosRaw() const {
    std::vector<Personagem*> aliadosVivos;
    for (const auto& aliado : listaDeAliados) {
        if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
    }
    return aliadosVivos;
}

void Combate::prepararTurnoPersonagem(Personagem* personagem) {
    ui->limparMensagensFixas();
    registrarLog("");
    registrarLog("═══ TURNO " + std::to_string(contadorDoTurnoAtual) + " ║ VEZ DE " + personagem->obterNome() + " ═══");
    ui->definirTurnoVisivel(contadorDoTurnoAtual, personagem->obterNome());
    personagem->reduzirCooldowns();
    personagem->processarEfeitosInicioTurno();
}

bool Combate::executarTurnoJogadorOuAliado(Personagem* personagem, bool& primeiraRenderizacao, bool processarEfeitosInicio) {
    if (processarEfeitosInicio) {
        prepararTurnoPersonagem(personagem);
    }
    if (personagem->obterVida() <= 0) return false;

    if (personagem == jogadorAtual) {
        bool limpouAliado = false;
        for (auto& aliado : listaDeAliados) {
            if (aliado->isMinion() && aliado->obterVida() > 0) {
                int dano = std::max(1, static_cast<int>(aliado->obterVidaMaxima() * 0.15));
                aliado->modificarVida(-dano);
                registrarLog(FuncoesDialogo::formatarMsgStatus(aliado->obterNome() + " perdeu " + std::to_string(dano) + " HP (decomposicao).", Cor::MAGENTA));
                
                if (aliado->obterVida() <= 0) {
                    registrarLog(FuncoesDialogo::formatarMsgStatus(aliado->obterNome() + " se decompos durante o combate", Cor::VERMELHO));
                    limpouAliado = true;
                }
            }
        }
        // Remove definitivamente da memória os aliados que morreram pelo dreno
        if (limpouAliado) {
            std::erase_if(listaDeAliados, [](const auto& a) { return a->obterVida() <= 0; });
        }
    }

    bool turnoConsumido = false;
    bool usouInventario = false;

    while (!turnoConsumido && personagem->obterVida() > 0 && !listaDeInimigos.empty()) {
        exibirTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        processarMenuDeAcoesDoJogador(personagem, turnoConsumido, usouInventario);
        
        limparInimigosMortos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return true; 
    }

    if (usouInventario) {
        exibirTelaDeCombate();
        ui->notificarDesprevencaoInventario();
    }
    return false;
}

void Combate::iniciarCombate() 
{
    Parry::onUpdateScreen = [this]() {
        this->exibirTelaDeCombate(false);
    };
    ControleDeInput::onAguardarEnterUpdate = [this]() {
        this->exibirTelaDeCombate(false);
    };
    resetarEstatisticasAvancadas();
    jogadorAtual->prepararParaNovaBatalha();
    Aparencia::limparLogBatalha();
    ui->limparMensagensFixas();

    for (auto& aliado : listaDeAliados) {
        aliado->prepararParaNovaBatalha();
    }

    ui->configurarContexto3D(isModo3D, matrizDoMapaAtual, jogadorPosX, jogadorPosY, jogadorAngulo, tituloMapaAtual);
    ui->animarIntroducaoCombate(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual);

    ui->limparTela();

    int maxDestrezaInimigos = GerenciadorTurnos::calcularMaxDestrezaInimigos(listaDeInimigos);
    for (const auto& inimigoPtr : listaDeInimigos) {
        Bestiario::instancia().registrarPrimeiraVista(inimigoPtr->obterRaca()->obterNomeRaca());
        Diario::instancia().registrarRaca(inimigoPtr->obterRaca()->obterNomeRaca());
        if (inimigoPtr->obterNomeClasse() != "Monstro") {
            Diario::instancia().registrarClasse(inimigoPtr->obterNomeClasse());
        }
    }
    
    bool turnoExtraFirstTurn = GerenciadorTurnos::jogadorTemTurnoExtraNoInicio(jogadorAtual, maxDestrezaInimigos);
    bool primeiraRenderizacao = false; // Modificado, pois ja animamos na intro
    
    if (GerenciadorTurnos::inimigosSaoMaisAgeis(jogadorAtual, maxDestrezaInimigos)) {
        exibirTelaDeCombate(primeiraRenderizacao);
        primeiraRenderizacao = false;
        
        if (GerenciadorTurnos::inimigosTemDobroDeAgilidade(jogadorAtual, maxDestrezaInimigos)) {
            std::string msg = FuncoesDialogo::formatarMsgSistema("A agilidade extrema dos inimigos (" + std::to_string(maxDestrezaInimigos) + " VS " + std::to_string(jogadorAtual->obterDestreza()) + ") permite que eles ataquem duas vezes seguidas!", Cor::VERMELHO);
            std::cout << "\n" << ui->margemCombate() << msg << "\n";
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
            ui->notificarInimigosMaisAgeis();
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
                ui->notificarTurnoExtra(jogadorAtual->obterDestreza(), maxDestrezaInimigos);
                turnoExtraFirstTurn = false;
                if (executarTurnoJogadorOuAliado(jogadorAtual, primeiraRenderizacao, false)) return;
            }
        }
        
        // Turnos dos Aliados
        for (size_t i = 0; i < listaDeAliados.size(); ++i) {
            Personagem* aliado = listaDeAliados[i].get();
            if (aliado->obterVida() <= 0 || listaDeInimigos.empty()) continue;
            
            bool isPrimeiraRend = false;
            if (executarTurnoJogadorOuAliado(aliado, isPrimeiraRend)) return;
        }
        
        executarTurnoDeTodosOsInimigos();
        limparInimigosMortos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;

        contadorDoTurnoAtual++;
    }
}

void Combate::processarMenuDeAcoesDoJogador(Personagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int acaoEscolhida = ui->obterAcaoDoJogador(contadorDoTurnoAtual, personagemAgindo, obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw());
    
    ui->limparContextoPersonagemHUD(); // Forca reset visual ao retornar para evitar bugs de persistencia de interface

    switch (acaoEscolhida) 
    {
        case 1: processarAcaoAtacar(personagemAgindo, turnoFoiConsumido); break;
        case 2: processarAcaoDefender(personagemAgindo, turnoFoiConsumido); break;
        case 3: processarAcaoHabilidade(personagemAgindo, turnoFoiConsumido); break;
        case 4: processarAcaoInventario(personagemAgindo, turnoFoiConsumido, usouInventarioNoTurno); break;
        case 5: ui->exibirTelaAtributos(personagemAgindo); break;
        case 6: ui->exibirTelaDiario(personagemAgindo); break;
        case 7: Aparencia::exibirHistoricoCompleto(); break;
        default: 
            ui->notificarAcaoInvalida();
            break;
    }
}

void Combate::processarAcaoAtacar(Personagem* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterTipoAtaque() == TipoAtaque::AREA) 
    {
        realizarAtaqueFisico(personagemAgindo, nullptr, contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }
    else 
    {
        int indiceDoAlvoEscolhido = ui->obterAlvoAtaque(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw());
        if (indiceDoAlvoEscolhido == -1) return;

        realizarAtaqueFisico(personagemAgindo, listaDeInimigos[indiceDoAlvoEscolhido].get(), contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }
}

Item* Combate::selecionarEscudo(Personagem* personagemAgindo) 
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
        ui->notificarSemEscudos(personagemAgindo->obterNome());
        return nullptr;
    }

    int opcaoEscolhida = ui->obterEscolhaDeEscudo(personagemAgindo->obterNome(), listaDeEscudos);
    return (opcaoEscolhida == 0) ? nullptr : listaDeEscudos[opcaoEscolhida - 1];
}

void Combate::processarAcaoDefender(Personagem* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterRecargaDefesa()) 
    {
        ui->notificarDesequilibrioDefesa(personagemAgindo->obterNome());
        return; 
    }
    
    Item* escudoEscolhido = selecionarEscudo(personagemAgindo);
    if (escudoEscolhido != nullptr) 
    {
        if (escudoEscolhido->obterDurabilidadeAtualEscudo() <= 0) {
            std::string msg = FuncoesDialogo::formatarMsgSistema("O escudo [" + escudoEscolhido->obterNomeItem() + "] esta quebrado e nao pode ser usado!", Cor::VERMELHO);
            std::cout << "\n" << ui->margemCombate() << msg << "\n";
            ControleDeInput::aguardarEnter();
            return; // Nao consome o turno
        }

        if (!escudoEscolhido->podeSerEquipadoPor(personagemAgindo)) {
            ui->notificarRequisitoNaoAtendido(escudoEscolhido->obterMensagemRequisito());
            return;
        }

        personagemAgindo->equiparItem(escudoEscolhido);
        personagemAgindo->definirDefendendo(true);
        ui->notificarPosturaDefensiva(personagemAgindo->obterNome(), escudoEscolhido->obterNomeItem());
        turnoFoiConsumido = true;
    }
}

void Combate::processarAcaoHabilidade(Personagem* personagemAgindo, bool& turnoFoiConsumido)
{
    std::vector<Personagem*> alvosRaw = obterInimigosRaw();
    
    personagemAgindo->definirHabilidadeCancelada(false);
    personagemAgindo->obterClasse()->usarHabilidadeClasse(this, personagemAgindo, alvosRaw);
    
    if (personagemAgindo->obterHabilidadeCancelada()) return;

    if (personagemAgindo->habilidadeDaClasseConsomeTurno()) turnoFoiConsumido = true;
    else ControleDeInput::aguardarEnter();
}

void Combate::processarAcaoInventario(Personagem* personagemAgindo, bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int vidaAntes = personagemAgindo->obterVida();
    bool inventarioConsumiu = false;
    
    InventarioCombate::gerenciarInventario(personagemAgindo, &inventarioConsumiu);
    if (inventarioConsumiu) {
        turnoFoiConsumido = true;
        usouInventarioNoTurno = true;
    }
    
    if (personagemAgindo->obterVida() > vidaAntes) {
        ui->animarCuraNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAgindo, jogadorAtual, obterAliadosVivosRaw(), personagemAgindo->obterVida() - vidaAntes);
    }

    if (personagemAgindo->obterItemSelecionadoParaUso() != nullptr) 
    {
        Item* itemSelecionado = personagemAgindo->obterItemSelecionadoParaUso();
        
        int indiceDoAlvoEscolhido = ui->obterAlvoItem(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, obterAliadosVivosRaw());

        if (indiceDoAlvoEscolhido == -1) 
        {
            ui->notificarCancelamentoItem();
            personagemAgindo->definirItemSelecionadoParaUso(nullptr);
        } 
        else 
        {
            Personagem* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
            
            itemSelecionado->usar(personagemAgindo, alvo);
            
            if (personagemAgindo->obterConsumivelRapido() == itemSelecionado) {
                personagemAgindo->desequiparConsumivel();
                std::string nomeDesteItem = itemSelecionado->obterNomeItem();
                for (auto* outroItem : personagemAgindo->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != itemSelecionado && outroItem->obterNomeItem() == nomeDesteItem) {
                        personagemAgindo->equiparItem(outroItem);
                        break;
                    }
                }
            }
            
            personagemAgindo->obterInventario()->removerItem(itemSelecionado);
            personagemAgindo->definirItemSelecionadoParaUso(nullptr);
            turnoFoiConsumido = true;
            usouInventarioNoTurno = true;
            stats_itensConsumidos++;
        }
    }
}

void Combate::limparInimigosMortos()
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

                std::vector<Personagem*> aliadosVivos = obterAliadosVivosRaw();
                ui->animarMorteInimigo(obterTituloDoCombate(), obterInimigosRaw(), inimigoPtr.get(), jogadorAtual, aliadosVivos, dropsDaMorte);
                inimigoPtr->definirMorteAnimada(true);
                if (listaDeInimigos.size() > 1) {
                    ControleDeInput::aguardarEnter();
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
                ui->limparContextoInimigoMortoEDrops();
        }
    }

    std::erase_if(listaDeInimigos, [](const auto& inimigo) { return inimigo->obterVida() <= 0; });
}

void Combate::executarTurnoDeTodosOsInimigos() 
{
    ui->limparMensagensFixas();
    if (jogadorAtual->obterPularTurnoInimigo()) 
    {
        // A mensagem na UI foi removida para priorizar o combate limpo
        registrarLog(FuncoesDialogo::formatarMsgStatus("Os inimigos estao atordoados e nao podem agir!", Cor::VERDE));
        jogadorAtual->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::string textoTurnoInimigos = "═══ TURNO " + std::to_string(contadorDoTurnoAtual) + " ║ VEZ DOS INIMIGOS ═══";
        registrarLog("");
        registrarLog(textoTurnoInimigos);
            ui->definirTurnoVisivel(contadorDoTurnoAtual, "INIMIGOS");
            exibirTelaDeCombate(false); // Forca o HUD a atualizar o nome do Turno para os inimigos antes do ataque iniciar
        for (size_t i = 0; i < listaDeInimigos.size(); ++i) 
        {
            auto& inimigoAtualPtr = listaDeInimigos[i];
            if (jogadorAtual->obterVida() <= 0) break; // Interrompe se o jogador morrer
            
            Personagem* inimigoAtual = inimigoAtualPtr.get();
            inimigoAtual->processarEfeitosInicioTurno();
            if (inimigoAtual->obterVida() <= 0) continue;

            bool agiu = false;
            std::string motivoIncapacidade;
            if (inimigoAtual->podeAgir(motivoIncapacidade)) 
            {
                agiu = true;

                // Logica de escolha de alvo do inimigo
                Personagem* alvo = MecanicasInimigo::escolherAlvo(obterAliadosVivosRaw(), jogadorAtual);

                bool turnoConsumidoPorHabilidade = inimigoAtual->obterRaca()->tentarUsarHabilidadeAtiva(inimigoAtual, alvo, static_cast<int>(jogadorAtual->obterDificuldade()));
                
                if (!turnoConsumidoPorHabilidade) {
                    realizarAtaqueFisico(inimigoAtual, alvo, contadorDoTurnoAtual);
                }
            }
            else
            {
                // A mensagem na UI foi removida para priorizar o combate limpo
                registrarLog(FuncoesDialogo::formatarMsgStatus(inimigoAtual->obterNome() + " esta sob efeito de " + motivoIncapacidade + " e nao pode agir!", Cor::VERDE));
            }

            if (agiu && i < listaDeInimigos.size() - 1 && jogadorAtual->obterVida() > 0) {
                ControleDeInput::aguardarEnter("... o ataque continua ...");
                std::this_thread::sleep_for(std::chrono::milliseconds(200));
                ControleDeInput::limparBuffer();
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

void Combate::realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate) 
{
    auto [danoBaseCalculado, danoPerfurante] = CalculadoraDano::calcularDanoOfensivoBase(personagemAtacante);

    bool isAtacanteJogadorOuAliado = isPersonagemJogadorOuAliado(personagemAtacante);

    if (isAtacanteJogadorOuAliado || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2) 
    {
        danoBaseCalculado = personagemAtacante->obterRaca()->processarDanoOfensivo(danoBaseCalculado, personagemAtacante);
    }

    auto callbackAplicarDano = [this, turnoAtualDoCombate](Personagem* atacante, Personagem* alvo, int danoBruto, int perfurante) {
        this->aplicarDanoAoAlvo(atacante, alvo, danoBruto, perfurante, turnoAtualDoCombate);
    };

    bool aplicarPassivaClasse = isAtacanteJogadorOuAliado || static_cast<int>(jogadorAtual->obterDificuldade()) == 3;

    personagemAtacante->obterClasse()->executarAtaqueComPassivaDaClasse(personagemAtacante, personagemDefensor, danoBaseCalculado, danoPerfurante, listaDeInimigos, callbackAplicarDano, aplicarPassivaClasse);
}



void Combate::processarPosDano(Personagem* atacante, Personagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso) {
    std::vector<Personagem*> aliadosVivos = obterAliadosVivosRaw();

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
            ui->animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), alvo, atacante, jogadorAtual, aliadosVivos, danoFinal);
        }
        else {
            ui->animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), alvo, jogadorAtual, aliadosVivos, false, danoFinal);
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
                ui->animarCuraNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), atacante, jogadorAtual, aliadosVivos, atacante->obterVida() - vidaAtacanteAntes);
            } else {
                ui->animarCuraNoJogador(obterTituloDoCombate(), obterInimigosRaw(), atacante, jogadorAtual, aliadosVivos, atacante->obterVida() - vidaAtacanteAntes);
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
        ui->animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), alvo, jogadorAtual, aliadosVivos, true, danoFinal);
    } else {
        ui->atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, aliadosVivos);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    g_inimigoAtacanteParry = nullptr;
    g_parryStatus = 0;
}

void Combate::aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int /*turnoAtualDoCombate*/) 
{
    if (personagemAlvo->possuiEfeito(EfeitoID::Inviolavel))
    {
        std::string msgEsquiva = personagemAlvo->obterNome() + " evitou o ataque de " + personagemAtacante->obterNome();
        registrarLog(FuncoesDialogo::formatarMsgCombate(msgEsquiva, Cor::CIANO));
        
        std::vector<Personagem*> aliadosVivos = obterAliadosVivosRaw();
        ui->atualizarTelaEstatica(obterTituloDoCombate(), obterInimigosRaw(), jogadorAtual, aliadosVivos);
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante->obterArma()) personagemAtacante->obterArma()->antesDeCausarDano(personagemAtacante, personagemAlvo);

    int danoBaseMitigado = CalculadoraDano::calcularMitigacaoDefensiva(personagemAlvo, quantidadeDeDanoBruto, danoPerfurante);
    int quantidadeDeDanoReduzidoPeloParry = 0;
    bool tentouParry = false;
    bool parryFoiBemSucedido = false;
    
    bool ataqueImparavel = personagemAtacante && personagemAtacante->obterRaca()->ignoraParry();

    // Logica do Parry
    if (personagemAlvo->obterParryAtivado() && !personagemAlvo->obterDefendendo()) 
    {
        if (ataqueImparavel) {
            std::string msgImparavel = FuncoesDialogo::formatarMsgCombate(personagemAtacante->obterNome() + " desfere um ATAQUE IMPARAVEL! O Parry foi ignorado!", Cor::FUNDO_VERMELHO);
            registrarLog(msgImparavel);
            ui->adicionarMensagemFixa(ui->margemCombate() + msgImparavel + "\n");
        } else {
            tentouParry = true;
            parryFoiBemSucedido = Parry::tentarParry(personagemAtacante, personagemAlvo, danoBaseMitigado, quantidadeDeDanoReduzidoPeloParry);
            stats_parriesTentados++;
            if (parryFoiBemSucedido) stats_parriesEfetivos++;
        }
    }

    bool aplicarPassivas = (isPersonagemJogadorOuAliado(personagemAlvo) || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2);

    ResultadoDano res = personagemAlvo->receberDano(quantidadeDeDanoBruto, danoPerfurante, quantidadeDeDanoReduzidoPeloParry, personagemAtacante, aplicarPassivas);

    // Logica de adaptacao do Mahoraga ao ter seu ataque bloqueado por escudo
    if (res.danoBloqueado > 0 && personagemAtacante->obterTipoRaca() == TipoRaca::Mahoraga) {
        // Precisamos de um cast para chamar o metodo especifico da raca Mahoraga
        auto* mahoraga = dynamic_cast<Mahoraga*>(personagemAtacante->obterRaca());
        if (mahoraga) {
            mahoraga->aoTerAtaqueBloqueadoPorEscudo();
        }
    }

    // Burlar o limite de "minimo de 1 de dano" do sistema base caso o Parry absorva todo o impacto
    if (tentouParry && parryFoiBemSucedido && quantidadeDeDanoReduzidoPeloParry >= danoBaseMitigado) 
    {
        if (personagemAlvo == jogadorAtual) stats_parriesPerfeitos++;
        if (res.danoFinal > 0) 
        {
            personagemAlvo->modificarVida(res.danoFinal); // Restaura o HP retirado pela trava de minimo de dano
            res.danoFinal = 0; // Anula o dano para ativar a Reflexao de Parry Perfeito
        }
    }

    exibirResultadoDoAtaque(personagemAlvo, res.danoFinal, tentouParry, parryFoiBemSucedido, res.danoBloqueado, res.escudoQuebrou, res.nomeEscudoQuebrado);

    processarPosDano(personagemAtacante, personagemAlvo, res.danoFinal, tentouParry, parryFoiBemSucedido);

    if (tentouParry && parryFoiBemSucedido && res.danoFinal <= 0 && isPersonagemJogadorOuAliado(personagemAlvo) && personagemAtacante) {
        personagemAtacante->obterRaca()->aoSofrerParryPerfeito();

        int danoRefletido = std::max(1, (quantidadeDeDanoBruto + danoPerfurante) / 2);
        personagemAtacante->modificarVida(-danoRefletido);
        std::string atacanteReflexao = personagemAtacante->obterNome();
        auto inimigosRaw = obterInimigosRaw();
        int inimigoIdx = -1;
        for (size_t k = 0; k < inimigosRaw.size(); ++k) {
            if (inimigosRaw[k] == personagemAtacante) {
                inimigoIdx = static_cast<int>(k) + 1;
                break;
            }
        }
        if (inimigoIdx != -1) {
            atacanteReflexao += "(" + std::to_string(inimigoIdx) + ")";
        }
        
        std::string msgReflexao = FuncoesDialogo::formatarMsgCombate("Reflexao! Inimigo tomou " + std::to_string(danoRefletido) + " de dano", Cor::AMARELO);
        registrarLog(msgReflexao);
        
        std::vector<Personagem*> aliadosVivos = obterAliadosVivosRaw();
        if (!isPersonagemJogadorOuAliado(personagemAtacante)) {
            ui->animarDanoNoInimigo(obterTituloDoCombate(), obterInimigosRaw(), personagemAtacante, personagemAlvo, jogadorAtual, aliadosVivos, danoRefletido);
            totalDeDanoCausado += danoRefletido;
        } else {
            ui->animarDanoNoJogador(obterTituloDoCombate(), obterInimigosRaw(), personagemAtacante, jogadorAtual, aliadosVivos, false, danoRefletido);
        }
    }
}

void Combate::exibirResultadoDoAtaque(Personagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso, int danoBloqueado, bool escudoQuebrou, const std::string& nomeEscudoQuebrado)
{
    bool isJogadorOuAliado = isPersonagemJogadorOuAliado(alvo);

    if (danoBloqueado > 0) {
        std::string msgDefesa = FuncoesDialogo::formatarMsgCombate("O escudo bloqueou " + std::to_string(danoBloqueado) + " de dano!", Cor::AZUL);
        registrarLog(msgDefesa);
        
        if (escudoQuebrou) {
            std::string msgQuebra = FuncoesDialogo::formatarMsgCombate("ALERTA: O escudo " + nomeEscudoQuebrado + " foi DESTRUIDO em pedacos e desequipado!", Cor::AZUL);
            registrarLog(msgQuebra);
            alvo->desequiparEscudo();
        }
    }

    if (isJogadorOuAliado) 
    {
        if (tentouParry) {
            std::string mensagemParryLog = Parry::obterMensagemFeedback(parrySucesso, danoFinal);
            registrarLog(FuncoesDialogo::formatarMsgCombate(mensagemParryLog, Cor::LARANJA));
        }
        else if (danoFinal > 0) 
        {
            // A mensagem estatica na UI de dano ao jogador foi comentada para priorizar o Texto de Dano Flutuante
            registrarLog(FuncoesDialogo::formatarMsgCombate(alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano", Cor::LARANJA));
        }
        else if (danoFinal == 0 && alvo->obterDefendendo()) 
        {
            // A mensagem estatica na UI de dano ao jogador foi comentada para priorizar o Texto de Dano Flutuante
            registrarLog(FuncoesDialogo::formatarMsgCombate("O dano foi totalmente absorvido pela defesa de " + alvo->obterNome() + "!", Cor::AZUL));
        }
        
        if (danoFinal > 0 && alvo == jogadorAtual) totalDeDanoRecebido += danoFinal;
    }
    else if (danoFinal > 0) 
    {
        if (danoFinal > stats_maiorDanoCausado) stats_maiorDanoCausado = danoFinal;
        if (alvo != jogadorAtual) totalDeDanoCausado += danoFinal;
        // A mensagem estatica na UI de dano aos inimigos foi comentada para priorizar o Texto de Dano Flutuante
        registrarLog(FuncoesDialogo::formatarMsgCombate(alvo->obterNome() + " recebeu " + std::to_string(danoFinal) + " de dano", Cor::VERMELHO));
    }
}

bool Combate::verificarCondicaoDeVitoriaOuDerrota() 
{
    bool isVitoria = listaDeInimigos.empty();
    bool isDerrota = jogadorAtual->obterVida() <= 0;

    if (isVitoria || isDerrota) 
    { 
        jogadorAtual->limparEfeitos(); // Remove buffs e debuffs ao final da batalha
        ControleDeInput::onAguardarEnterUpdate = nullptr; // Impede que o aguardarEnter da tela de vitoria/derrota redesenhe o combate
        if (isVitoria) {
            ui->exibirTelaVitoria(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, 
                                totalDeDanoRecebido, jogadorAtual->obterCuraTotalRecebida(), contadorDoTurnoAtual, 
                                itensObtidos, inimigosDerrotados, stats_parriesPerfeitos, stats_maiorDanoCausado);
        } else {
            ui->exibirTelaDerrota(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, jogadorAtual->obterCuraTotalRecebida(), contadorDoTurnoAtual); 
        }
        jogadorAtual->finalizarBatalha();
        return true; 
    }
    return false;
}

void Combate::processarMorteDeInimigo(Personagem* inimigo)
{
    registrarLog(FuncoesDialogo::formatarMsgCombate(inimigo->obterNome() + " derrotado!", Cor::VERMELHO));
    inimigosDerrotados.push_back(inimigo->obterNome());

    std::string nomeRaca = inimigo->obterRaca()->obterNomeRaca();
    if (!Bestiario::instancia().jaDerrotado(nomeRaca)) {
        stats_novasDescobertas.push_back("Novo monstro catalogado: " + nomeRaca);
    }

    Bestiario::instancia().registrarDerrota(inimigo->obterRaca()->obterNomeRaca());

    if (inimigo->obterNome() == "Mahoraga") {
        Progressao::instancia().definirFlag(Flags::Floresta_MahoragaDerrotado, true);
    }

    // Passiva do Necromante: Coletar alma
    if (jogadorAtual->obterTipoClasse() == TipoClasse::NECROMANTE) {
        jogadorAtual->adicionarAlma(inimigo->clone());
        std::string msg = FuncoesDialogo::formatarMsgHabilidade("Voce coletou a alma de " + inimigo->obterNome() + "!", Cor::MAGENTA);
        registrarLog(msg);
    }

    registrarLog("═══ DROPS ═══", Cor::AMARELO);

    size_t itensAntes = itensObtidos.size();
    inimigo->executarDrops(jogadorAtual, itensObtidos, quantidadeDeOuroObtido, quantidadeDeXpObtido);
    for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
        if (!Bestiario::instancia().jaColetouDrop(nomeRaca, itensObtidos[i])) {
            stats_novasDescobertas.push_back("Novo drop descoberto: " + itensObtidos[i]);
        }
        Bestiario::instancia().registrarDrop(inimigo->obterRaca()->obterNomeRaca(), itensObtidos[i]);
    }
}
