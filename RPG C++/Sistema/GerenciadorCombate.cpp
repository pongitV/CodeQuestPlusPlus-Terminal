#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

#include "GerenciadorCombate.h"
#include "FuncionalidadeMenu.h"
#include "SimplificacoesAparencia.h"
#include "SistemaDeParry.h"

#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

#include "../Interfaces/TelaVitoria.h"
#include "../Interfaces/TelaDerrota.h"
#include "../Interfaces/TelaCombate.h"
#include "../Inventario/InventarioCombate.h"
#include "../Interfaces/TelaAtributos.h"
#include "../Interfaces/TelaBestiario.h"
#include "../Inimigos/Bestiario.h"

SistemaRPG::SistemaRPG(Personagem* jogadorParaOCombate, std::vector<std::unique_ptr<Personagem>>&& inimigosParaOCombate) 
    : jogadorAtual(jogadorParaOCombate), listaDeInimigos(std::move(inimigosParaOCombate)), contadorDoTurnoAtual(1), quantidadeDeOuroObtido(0), quantidadeDeXpObtido(0), totalDeDanoCausado(0), totalDeDanoRecebido(0)
{

    int nivelDeDificuldade = jogadorAtual->obterDificuldade();
    double multiplicadorDeDificuldadeDosInimigos = 1.0;

    if (nivelDeDificuldade == 3) {
        multiplicadorDeDificuldadeDosInimigos = 2.0;
    }

    for (auto& inimigoAtualPtr : this->listaDeInimigos) 
    {
        inimigoAtualPtr->aplicarMultiplicadorDificuldade(multiplicadorDeDificuldadeDosInimigos);
    }
}

SistemaRPG::~SistemaRPG()
{
}

std::string SistemaRPG::obterTituloDoCombate() const
{
    std::string titulo = "EM COMBATE (";
    for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
        titulo += listaDeInimigos[i]->obterNome();
        if (i < listaDeInimigos.size() - 1) titulo += ", ";
    }
    titulo += ")";
    return titulo;
}

std::vector<Personagem*> SistemaRPG::obterInimigosRaw() const
{
    std::vector<Personagem*> raw;
    raw.reserve(listaDeInimigos.size());
    for (auto& ini : listaDeInimigos) raw.push_back(ini.get());
    return raw;
}

void SistemaRPG::exibirTelaDeCombate() const
{
    SimplificacoesAparencia::limparTela();
    TelaCombate::exibirLogoParaTelaDeCombate(obterTituloDoCombate());
    Menu::exibirHordaDeInimigosLadoALado(obterInimigosRaw());
    TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual);
}

void SistemaRPG::iniciarCombate() 
{
    int maxDestrezaInimigos = 0;
    for (auto& inimigoPtr : listaDeInimigos)
    {
        Bestiario::instancia().registrarPrimeiraVista(inimigoPtr->obterNome());
        if (inimigoPtr->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigoPtr->obterDestreza();
    }
    
    bool turnoExtraFirstTurn = false;
    if (jogadorAtual->obterDestreza() > (maxDestrezaInimigos * 2)) 
    {
        turnoExtraFirstTurn = true;
    }
    
    // Destreza decide quem ataca primeiro no combate
    if (maxDestrezaInimigos > jogadorAtual->obterDestreza())
    {
        exibirTelaDeCombate();

        std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        SimplificacoesAparencia::aguardarEnter();
        executarTurnoDeTodosOsInimigos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;
    }

    while (jogadorAtual->obterVida() > 0 && !listaDeInimigos.empty())
    {
        jogadorAtual->reduzirCooldowns();
        jogadorAtual->processarEfeitosInicioTurno();

        bool turnoFoiConsumido = false;
        bool usouInventarioNoTurno = false;

        while (!turnoFoiConsumido)
        {
            exibirTelaDeCombate();
            processarMenuDeAcoesDoJogador(turnoFoiConsumido, usouInventarioNoTurno);
            
            if (verificarCondicaoDeVitoriaOuDerrota()) return; 
        }
        
        if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) 
        {
            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::CIANO) << "[SISTEMA]: Sua agilidade extrema (" << jogadorAtual->obterDestreza() << " VS " << maxDestrezaInimigos << ") permite que voce aja novamente!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            SimplificacoesAparencia::aguardarEnter();
            turnoExtraFirstTurn = false;
            continue; // Pula o turno inimigo e permite que o jogador jogue o turno novamente
        }

        if (usouInventarioNoTurno)
        {
            exibirTelaDeCombate();

            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::AMARELO) << "[SISTEMA]: O inimigo te pegou desprevinido enquanto voce usava o inventario!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }

        executarTurnoDeTodosOsInimigos();
        if (verificarCondicaoDeVitoriaOuDerrota()) return;
        contadorDoTurnoAtual++;
    }
}

void SistemaRPG::processarMenuDeAcoesDoJogador(bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string textoDoTurno = "TURNO " + std::to_string(contadorDoTurnoAtual) + " | SUA VEZ";
    int espacosTurno = std::max(0, (larguraDoTerminal - static_cast<int>(textoDoTurno.length())) / 2);
    std::cout << "\n" << std::string(espacosTurno, ' ') << textoDoTurno << "\n";

    std::string textoAcoes = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | 6. Bestiario | Escolha: ";
    int espacosAcoes = std::max(0, (larguraDoTerminal - static_cast<int>(textoAcoes.length())) / 2);
    std::cout << std::string(espacosAcoes, ' ') << textoAcoes;
    
    int acaoEscolhida;
    if (!(std::cin >> acaoEscolhida)) 
    {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }

    switch (acaoEscolhida) 
    {
        case 1: processarAcaoAtacar(turnoFoiConsumido); break;
        case 2: processarAcaoDefender(turnoFoiConsumido); break;
        case 3: processarAcaoHabilidade(turnoFoiConsumido); break;
        case 4: processarAcaoInventario(turnoFoiConsumido, usouInventarioNoTurno); break;
        case 5: TelaAtributos::gerenciarFichaDoJogador(jogadorAtual); break;
        case 6: TelaBestiario::exibirLista(jogadorAtual); break;
    }
}

void SistemaRPG::processarAcaoAtacar(bool& turnoFoiConsumido)
{
    if (jogadorAtual->obterTipoAtaque() == TipoAtaque::AREA) 
    {
        realizarAtaqueFisico(jogadorAtual, nullptr, contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }
    else 
    {
        int indiceDoAlvoEscolhido;
        std::cout << "Escolha o alvo (0 a " << listaDeInimigos.size() - 1 << "): ";
        if (!(std::cin >> indiceDoAlvoEscolhido) || indiceDoAlvoEscolhido < 0 || indiceDoAlvoEscolhido >= static_cast<int>(listaDeInimigos.size())) 
        {
            std::cin.clear(); 
            std::cin.ignore(1000, '\n');
            std::cout << "\n[ERRO] Alvo invalido!\n";
            SimplificacoesAparencia::aguardarEnter();
            return;
        }

        realizarAtaqueFisico(jogadorAtual, listaDeInimigos[indiceDoAlvoEscolhido].get(), contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }

    limparInimigosMortos();
}

void SistemaRPG::processarAcaoDefender(bool& turnoFoiConsumido)
{
    if (jogadorAtual->obterRecargaDefesa()) 
    {
        std::cout << "\n[ERRO]: Voce se desequilibrou e precisa de 1 turno para poder defender novamente!\n";
        SimplificacoesAparencia::aguardarEnter();
        return; 
    }
    
    Item* escudoEscolhido = jogadorAtual->obterInventario()->selecionarEscudo();
    if (escudoEscolhido != nullptr) 
    {
        jogadorAtual->equiparItem(escudoEscolhido);
        jogadorAtual->definirDefendendo(true);
        std::cout << "\n[SISTEMA]: " << jogadorAtual->obterNome() << " assumiu uma postura defensiva com " << escudoEscolhido->obterNomeItem() << "!\n";
        SimplificacoesAparencia::aguardarEnter();
        turnoFoiConsumido = true;
    }
}

void SistemaRPG::processarAcaoHabilidade(bool& turnoFoiConsumido)
{
    std::vector<Personagem*> alvosRaw = obterInimigosRaw();

    jogadorAtual->definirHabilidadeCancelada(false);
    jogadorAtual->obterClasse()->usarHabilidadeClasse(jogadorAtual, alvosRaw);
    
    if (jogadorAtual->obterHabilidadeCancelada()) return;

    if (jogadorAtual->habilidadeDaClasseConsomeTurno()) turnoFoiConsumido = true;
    else SimplificacoesAparencia::aguardarEnter();
}

void SistemaRPG::processarAcaoInventario(bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    bool inventarioConsumiu = false;
    InventarioCombate::gerenciarInventario(jogadorAtual, &inventarioConsumiu);
    if (inventarioConsumiu) {
        turnoFoiConsumido = true;
        usouInventarioNoTurno = true;
    }
    if (jogadorAtual->obterItemSelecionadoParaUso() != nullptr) 
    {
        Item* frasco = jogadorAtual->obterItemSelecionadoParaUso();
        int indiceDoAlvoEscolhido = -1;
        
        std::cout << "\n--- ESCOLHA UM ALVO PARA O FRASCO ---\n";
        for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
            std::cout << "[" << i << "] " << listaDeInimigos[i]->obterNome() << " (HP: " << listaDeInimigos[i]->obterVida() << ")\n";
        }
        std::cout << "Escolha (ou tecla invalida para CANCELAR): ";
        
        if (!(std::cin >> indiceDoAlvoEscolhido) || indiceDoAlvoEscolhido < 0 || indiceDoAlvoEscolhido >= static_cast<int>(listaDeInimigos.size())) 
        {
            std::cin.clear(); 
            std::cin.ignore(1000, '\n');
            std::cout << "\n[SISTEMA] Uso do frasco cancelado. O item voltou para a mochila.\n";
            jogadorAtual->definirItemSelecionadoParaUso(nullptr);
        } 
        else 
        {
            Personagem* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
            std::string nomeFrasco = frasco->obterNomeItem();
            
            if (frasco->temPropriedade(Propriedade::ConsumivelDebuffLentidao)) {
                alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
                std::cout << "\n" << SimplificacoesAparencia::cor(Cor::MAGENTA) << ">> Voce jogou o frasco! " << alvo->obterNome() << " esta com lentidao por 3 turnos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            } else if (frasco->temPropriedade(Propriedade::ConsumivelDebuffFraqueza)) {
                alvo->adicionarEfeito(std::make_unique<EfeitoFraqueza>(3));
                std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << ">> Voce jogou o frasco! " << alvo->obterNome() << " teve sua forca reduzida em 25% por 3 turnos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            }
            
            jogadorAtual->obterInventario()->removerItem(frasco);
            jogadorAtual->definirItemSelecionadoParaUso(nullptr);
            turnoFoiConsumido = true;
            usouInventarioNoTurno = true;
        }
        SimplificacoesAparencia::aguardarEnter();
    }
}

void SistemaRPG::limparInimigosMortos()
{
    for (auto iteradorInimigos = listaDeInimigos.begin(); iteradorInimigos != listaDeInimigos.end(); ) 
    {
        if ((*iteradorInimigos)->obterVida() <= 0) 
        {
            processarMorteDeInimigo(iteradorInimigos->get());
            iteradorInimigos = listaDeInimigos.erase(iteradorInimigos);
            SimplificacoesAparencia::aguardarEnter();
        } else { ++iteradorInimigos; }
    }
}

void SistemaRPG::executarTurnoDeTodosOsInimigos() 
{
    if (jogadorAtual->obterPularTurnoInimigo()) 
    {
        std::cout << "\n[EFEITO]: Os inimigos estao atordoados e nao podem agir!\n";
        jogadorAtual->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::cout << "\n--- TURNO DOS INIMIGOS ---" << std::endl;
        for (auto& inimigoAtualPtr : listaDeInimigos) 
        {
            if (jogadorAtual->obterVida() <= 0) break; // Interrompe se o jogador morrer
            
            Personagem* inimigoAtual = inimigoAtualPtr.get();
            inimigoAtual->processarEfeitosInicioTurno();
            if (inimigoAtual->obterVida() <= 0) continue;

            if (inimigoAtual->podeAgir()) 
            {
                bool turnoConsumidoPorHabilidade = inimigoAtual->obterRaca()->tentarUsarHabilidadeAtiva(inimigoAtual, jogadorAtual, jogadorAtual->obterDificuldade());
                
                if (!turnoConsumidoPorHabilidade) 
                {
                    realizarAtaqueFisico(inimigoAtual, jogadorAtual, contadorDoTurnoAtual);
                }
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
    SimplificacoesAparencia::aguardarEnter();
}

void SistemaRPG::realizarAtaqueFisico(Personagem* personagemAtacante, Personagem* personagemDefensor, int turnoAtualDoCombate) 
{
    auto [danoBaseCalculado, danoPerfurante] = calcularDanoBase(personagemAtacante);

    if (personagemAtacante == jogadorAtual || jogadorAtual->obterDificuldade() >= 2) 
    {
        danoBaseCalculado = personagemAtacante->obterRaca()->processarDanoOfensivo(danoBaseCalculado, personagemAtacante);
    }

    auto callbackAplicarDano = [this, turnoAtualDoCombate](Personagem* atacante, Personagem* alvo, int danoBruto, int perfurante) {
        this->aplicarDanoAoAlvo(atacante, alvo, danoBruto, perfurante, turnoAtualDoCombate);
    };

    personagemAtacante->obterClasse()->executarAtaqueComPassivaDaClasse(personagemAtacante, personagemDefensor, danoBaseCalculado, danoPerfurante, listaDeInimigos, callbackAplicarDano);
}

std::pair<int, int> SistemaRPG::calcularDanoBase(Personagem* atacante) 
{
    double multiplicadorDeAtributos = atacante->obterMultiplicador();
    if (atacante->possuiEfeito(EfeitoNomes::INVOIVEL)) multiplicadorDeAtributos *= 2.0;

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

    double forcaEfetiva = atacante->obterForca();
    double destrezaEfetiva = atacante->obterDestreza();
    double intEfetiva = atacante->obterInteligencia();
    double sabEfetiva = atacante->obterSabedoria();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        forcaEfetiva *= 0.1; destrezaEfetiva *= 0.1;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        intEfetiva *= 0.1; sabEfetiva *= 0.1;
    }

    double danoFisCalculado = std::max(0.0, (danoFisicoDaArma + forcaEfetiva) * (1.0 + (destrezaEfetiva / 100.0)));
    double danoMagCalculado = std::max(0.0, (danoMagicoDaArma + intEfetiva) * (1.0 + (sabEfetiva / 100.0)));
    double total = std::max(1.0, danoFisCalculado + danoMagCalculado);

    return { static_cast<int>(total * multiplicadorDeAtributos), perfuranteAtual };
}

void SistemaRPG::aplicarDanoAoAlvo(Personagem* personagemAtacante, Personagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate) 
{
    if (personagemAlvo->possuiEfeito(EfeitoNomes::INVOIVEL))
    {
        std::cout << "[COMBATE]: O alvo esta Inviolavel e desviou do ataque perfeitamente!\n";
        return;
    }

    // Logica da Quebra de Resistencia (Pó Mágico)
    if (personagemAtacante->obterArma()) personagemAtacante->obterArma()->antesDeCausarDano(personagemAtacante, personagemAlvo);

    int danoBaseMitigado = personagemAlvo->calcularDefesaBase(quantidadeDeDanoBruto, danoPerfurante);
    int quantidadeDeDanoReduzidoPeloParry = 0;
    bool tentouParry = false;
    bool parryFoiBemSucedido = false;

    // Logica do Parry
    if (personagemAlvo == jogadorAtual && jogadorAtual->obterParryAtivado()) 
    {
        tentouParry = true;
        parryFoiBemSucedido = SistemaDeParry::tentarParry(personagemAtacante, danoBaseMitigado, quantidadeDeDanoReduzidoPeloParry);
    }

    bool aplicarPassivas = (personagemAlvo == jogadorAtual || jogadorAtual->obterDificuldade() >= 2);
    int danoFinalAposReducoes = personagemAlvo->receberDano(quantidadeDeDanoBruto, danoPerfurante, quantidadeDeDanoReduzidoPeloParry, personagemAtacante, aplicarPassivas);

    exibirResultadoDoAtaque(personagemAlvo, danoFinalAposReducoes, tentouParry, parryFoiBemSucedido);

    if (danoFinalAposReducoes > 0) 
    {
        // Aplicação dos efeitos no acerto
        if (personagemAtacante->obterArma()) 
        {
            personagemAtacante->obterArma()->aoCausarDano(personagemAtacante, personagemAlvo, danoFinalAposReducoes);
        }

        personagemAtacante->obterRaca()->aoCausarDano(personagemAtacante, personagemAlvo, danoFinalAposReducoes);
    }
}

void SistemaRPG::exibirResultadoDoAtaque(Personagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso)
{
    if (alvo == jogadorAtual) 
    {
        if (tentouParry) 
        {
            std::string mensagemParry = parrySucesso ? (danoFinal <= 1 ? "parry efetivo! Ataque anulado!" : "parry efetivo! Mas o ataque e muito forte!") : "parry falhou!";
            std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_VERMELHO) << ">> [PARRY]: " << mensagemParry << " Inimigo causou " << danoFinal << " de dano" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }
        else if (danoFinal > 0) 
        {
            std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_VERMELHO) << ">> " << alvo->obterNome() << " recebeu " << danoFinal << " de dano" << SimplificacoesAparencia::cor(Cor::RESET) << std::endl;
        }
        else if (danoFinal == 0 && alvo->obterDefendendo()) 
        {
            std::cout << ">> O dano foi totalmente absorvido pela sua defesa!" << std::endl;
        }
        
        if (danoFinal > 0) totalDeDanoRecebido += danoFinal;
    }
    else if (danoFinal > 0) 
    {
        totalDeDanoCausado += danoFinal;
        std::cout << ">> " << alvo->obterNome() << " recebeu " << danoFinal << " de dano" << std::endl;
    }
}

bool SistemaRPG::verificarCondicaoDeVitoriaOuDerrota() 
{
    if (listaDeInimigos.empty()) 
    { 
        TelaVitoria::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, itensObtidos);
        return true; 
    }
    if (jogadorAtual->obterVida() <= 0) 
    { 
        TelaDerrota::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido); 
        return true; 
    }
    return false;
}

void SistemaRPG::processarMorteDeInimigo(Personagem* inimigo)
{
    std::cout << "\n[!] " << inimigo->obterNome() << " derrotado! ";

    Bestiario::instancia().registrarDerrota(inimigo->obterNome());

    size_t itensAntes = itensObtidos.size();
    inimigo->executarDrops(jogadorAtual, itensObtidos, quantidadeDeOuroObtido, quantidadeDeXpObtido);
    for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
        Bestiario::instancia().registrarDrop(inimigo->obterNome(), itensObtidos[i]);
    }
}