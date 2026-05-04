#include "GerenciadorCombate.h"

#include <algorithm>
#include <iostream>
#include <limits>
#include <vector>

#include "../Classes/ClasseBase.h"
#include "../Inventario/InventarioCombate.h"
#include "../Inventario/Item.h"
#include "../Racas/RacaBase.h"
#include "../Sistemas/SistemaBestiario.h"
#include "../Sistemas/SistemaParry.h"
#include "../Telas/TelaBestiario.h"
#include "../Telas/TelaAtributos.h"
#include "../Telas/TelaCombate.h"
#include "../Telas/TelaDerrota.h"
#include "../Telas/TelaVitoria.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "GerenciadorMenu.h"

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

std::vector<SistemaPersonagem*> GerenciadorCombate::obterInimigosRaw() const
{
    std::vector<SistemaPersonagem*> raw;
    raw.reserve(listaDeInimigos.size());
    for (auto& ini : listaDeInimigos) raw.push_back(ini.get());
    return raw;
}

void GerenciadorCombate::exibirTelaDeCombate() const
{
    SimplificacoesAparencia::limparTela();
    TelaCombate::exibirLogoParaTelaDeCombate(obterTituloDoCombate());
    GerenciadorMenu::exibirHordaDeInimigosLadoALado(obterInimigosRaw());
    TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual);
}

void GerenciadorCombate::iniciarCombate() 
{
    jogadorAtual->prepararParaNovaBatalha();

    int maxDestrezaInimigos = 0;
    bool turnoExtraFirstTurn = false;

    auto determinarQuemComeca = [&]() -> bool {
        for (auto& inimigoPtr : listaDeInimigos) {
            SistemaBestiario::instancia().registrarPrimeiraVista(inimigoPtr->obterNome());
            if (inimigoPtr->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigoPtr->obterDestreza();
        }
        
        if (jogadorAtual->obterDestreza() > (maxDestrezaInimigos * 2)) {
            turnoExtraFirstTurn = true;
        }
        
        if (maxDestrezaInimigos > jogadorAtual->obterDestreza()) {
            exibirTelaDeCombate();
            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            SimplificacoesAparencia::aguardarEnter();
            executarTurnoDeTodosOsInimigos();
            return verificarCondicaoDeVitoriaOuDerrota();
        }
        return false;
    };

    auto verificarCondicaoDeFimDeTurno = [&](bool usouInventarioNoTurno, bool& pularIncremento) -> bool {
        if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) {
            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::CIANO) << "[SISTEMA]: Sua agilidade extrema (" << jogadorAtual->obterDestreza() << " VS " << maxDestrezaInimigos << ") permite que voce aja novamente!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            SimplificacoesAparencia::aguardarEnter();
            turnoExtraFirstTurn = false;
            pularIncremento = true;
            return false;
        }

        if (usouInventarioNoTurno) {
            exibirTelaDeCombate();

            std::cout << "\n" << SimplificacoesAparencia::cor(Cor::AMARELO) << "[SISTEMA]: O inimigo te pegou desprevinido enquanto voce usava o inventario!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }

        executarTurnoDeTodosOsInimigos();
        pularIncremento = false;
        return verificarCondicaoDeVitoriaOuDerrota();
    };

    auto executarLoopPrincipal = [&]() {
        while (jogadorAtual->obterVida() > 0 && !listaDeInimigos.empty()) {
            jogadorAtual->reduzirCooldowns();
            jogadorAtual->processarEfeitosInicioTurno();

            bool turnoFoiConsumido = false;
            bool usouInventarioNoTurno = false;

            while (!turnoFoiConsumido) {
                exibirTelaDeCombate();
                processarMenuDeAcoesDoJogador(turnoFoiConsumido, usouInventarioNoTurno);
                if (verificarCondicaoDeVitoriaOuDerrota()) return; 
            }
            
            bool pularIncremento = false;
            if (verificarCondicaoDeFimDeTurno(usouInventarioNoTurno, pularIncremento)) return;
            if (pularIncremento) continue;

            contadorDoTurnoAtual++;
        }
    };

    if (!determinarQuemComeca()) {
        executarLoopPrincipal();
    }
}

void GerenciadorCombate::processarMenuDeAcoesDoJogador(bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
{
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string textoDoTurno = "TURNO " + std::to_string(contadorDoTurnoAtual) + " | SUA VEZ";
    int espacosTurno = std::max(0, (larguraDoTerminal - static_cast<int>(textoDoTurno.length())) / 2);
    std::cout << "\n" << std::string(espacosTurno, ' ') << textoDoTurno << "\n";

    std::string textoAcoes = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | 6. Bestiario | Escolha: ";
    int espacosAcoes = std::max(0, (larguraDoTerminal - static_cast<int>(textoAcoes.length())) / 2);
    std::cout << std::string(espacosAcoes, ' ') << textoAcoes;
    
    int acaoEscolhida;
    while (!(std::cin >> acaoEscolhida)) 
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << std::string(espacosAcoes, ' ') << "Entrada invalida! Escolha novamente: ";
    }

    switch (static_cast<AcaoCombate>(acaoEscolhida)) 
    {
        case AcaoCombate::Atacar: processarAcaoAtacar(turnoFoiConsumido); break;
        case AcaoCombate::Defender: processarAcaoDefender(turnoFoiConsumido); break;
        case AcaoCombate::Habilidade: processarAcaoHabilidade(turnoFoiConsumido); break;
        case AcaoCombate::Inventario: processarAcaoInventario(turnoFoiConsumido, usouInventarioNoTurno); break;
        case AcaoCombate::Jogador: TelaAtributos::gerenciarFichaDoJogador(jogadorAtual); break;
        case AcaoCombate::Bestiario: TelaBestiario::exibirLista(jogadorAtual); break;
        default: 
            std::cout << "\n[ERRO] Acao invalida!\n"; 
            SimplificacoesAparencia::aguardarEnter(); 
            break;
    }
}

void GerenciadorCombate::processarAcaoAtacar(bool& turnoFoiConsumido)
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
        while (!(std::cin >> indiceDoAlvoEscolhido) || indiceDoAlvoEscolhido < 0 || indiceDoAlvoEscolhido >= static_cast<int>(listaDeInimigos.size())) 
        {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Alvo invalido! Escolha o alvo (0 a " << listaDeInimigos.size() - 1 << "): ";
        }

        realizarAtaqueFisico(jogadorAtual, listaDeInimigos[indiceDoAlvoEscolhido].get(), contadorDoTurnoAtual);
        turnoFoiConsumido = true;
    }

    limparInimigosMortos();
}

void GerenciadorCombate::processarAcaoDefender(bool& turnoFoiConsumido)
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

void GerenciadorCombate::processarAcaoHabilidade(bool& turnoFoiConsumido)
{
    std::vector<SistemaPersonagem*> alvosRaw = obterInimigosRaw();

    jogadorAtual->definirHabilidadeCancelada(false);
    jogadorAtual->obterClasse()->usarHabilidadeClasse(jogadorAtual, alvosRaw);
    
    if (jogadorAtual->obterHabilidadeCancelada()) return;

    if (jogadorAtual->habilidadeDaClasseConsomeTurno()) turnoFoiConsumido = true;
    else SimplificacoesAparencia::aguardarEnter();
}

void GerenciadorCombate::processarAcaoInventario(bool& turnoFoiConsumido, bool& usouInventarioNoTurno)
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
        std::cout << "Escolha (ou -1 para CANCELAR): ";
        
        while (!(std::cin >> indiceDoAlvoEscolhido) || indiceDoAlvoEscolhido < -1 || indiceDoAlvoEscolhido >= static_cast<int>(listaDeInimigos.size())) 
        {
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Alvo invalido! Escolha (ou -1 para CANCELAR): ";
        } 

        if (indiceDoAlvoEscolhido == -1) 
        {
            std::cout << "\n[SISTEMA] Uso do frasco cancelado. O item voltou para a mochila.\n";
            jogadorAtual->definirItemSelecionadoParaUso(nullptr);
        } 
        else 
        {
            SistemaPersonagem* alvo = listaDeInimigos[indiceDoAlvoEscolhido].get();
            
            frasco->usar(jogadorAtual, alvo);
            
            jogadorAtual->obterInventario()->removerItem(frasco);
            jogadorAtual->definirItemSelecionadoParaUso(nullptr);
            turnoFoiConsumido = true;
            usouInventarioNoTurno = true;
        }
        SimplificacoesAparencia::aguardarEnter();
    }
}

void GerenciadorCombate::limparInimigosMortos()
{
    for (auto& inimigoPtr : listaDeInimigos) 
    {
        if (inimigoPtr->obterVida() <= 0) 
        {
            processarMorteDeInimigo(inimigoPtr.get());
            SimplificacoesAparencia::aguardarEnter();
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
        std::cout << "\n[EFEITO]: Os inimigos estao atordoados e nao podem agir!\n";
        jogadorAtual->definirPularTurnoInimigo(false); 
    }
    else
    {
        std::cout << "\n--- TURNO DOS INIMIGOS ---" << std::endl;
        for (auto& inimigoAtualPtr : listaDeInimigos) 
        {
            if (jogadorAtual->obterVida() <= 0) break; // Interrompe se o jogador morrer
            
            SistemaPersonagem* inimigoAtual = inimigoAtualPtr.get();
            inimigoAtual->processarEfeitosInicioTurno();
            if (inimigoAtual->obterVida() <= 0) continue;

            if (inimigoAtual->podeAgir()) 
            {
                bool turnoConsumidoPorHabilidade = inimigoAtual->obterRaca()->tentarUsarHabilidadeAtiva(inimigoAtual, jogadorAtual, static_cast<int>(jogadorAtual->obterDificuldade()));
                
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

void GerenciadorCombate::realizarAtaqueFisico(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemDefensor, int turnoAtualDoCombate) 
{
    auto [danoBaseCalculado, danoPerfurante] = calcularDanoBase(personagemAtacante);

    if (personagemAtacante == jogadorAtual || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2) 
    {
        danoBaseCalculado = personagemAtacante->obterRaca()->processarDanoOfensivo(danoBaseCalculado, personagemAtacante);
    }

    auto callbackAplicarDano = [this, turnoAtualDoCombate](SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoBruto, int perfurante) {
        this->aplicarDanoAoAlvo(atacante, alvo, danoBruto, perfurante, turnoAtualDoCombate);
    };

    bool isAtacanteJogador = (personagemAtacante == jogadorAtual);
    personagemAtacante->obterClasse()->executarAtaqueComPassivaDaClasse(personagemAtacante, personagemDefensor, danoBaseCalculado, danoPerfurante, listaDeInimigos, callbackAplicarDano, isAtacanteJogador);
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
    int intEfetiva = atacante->obterInteligencia();
    int sabEfetiva = atacante->obterSabedoria();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        forcaEfetiva /= 10; destrezaEfetiva /= 10;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        intEfetiva /= 10; sabEfetiva /= 10;
    }

    int danoFisCalculado = std::max(0, static_cast<int>((danoFisicoDaArma + forcaEfetiva) * (1.0 + (destrezaEfetiva / 100.0))));
    int danoMagCalculado = std::max(0, static_cast<int>((danoMagicoDaArma + intEfetiva) * (1.0 + (sabEfetiva / 100.0))));
    int total = std::max(1, danoFisCalculado + danoMagCalculado);

    return { static_cast<int>(total * multiplicadorDeAtributos), perfuranteAtual };
}

void GerenciadorCombate::aplicarDanoAoAlvo(SistemaPersonagem* personagemAtacante, SistemaPersonagem* personagemAlvo, int quantidadeDeDanoBruto, int danoPerfurante, int turnoAtualDoCombate) 
{
    if (personagemAlvo->possuiEfeito(EfeitoID::Inviolavel))
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
        parryFoiBemSucedido = SistemaParry::tentarParry(personagemAtacante, danoBaseMitigado, quantidadeDeDanoReduzidoPeloParry);
    }

    bool aplicarPassivas = (personagemAlvo == jogadorAtual || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2);
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

void GerenciadorCombate::exibirResultadoDoAtaque(SistemaPersonagem* alvo, int danoFinal, bool tentouParry, bool parrySucesso)
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
    std::cout << "\n[!] " << inimigo->obterNome() << " derrotado! ";

    SistemaBestiario::instancia().registrarDerrota(inimigo->obterNome());

    size_t itensAntes = itensObtidos.size();
    inimigo->executarDrops(jogadorAtual, itensObtidos, quantidadeDeOuroObtido, quantidadeDeXpObtido);
    for (size_t i = itensAntes; i < itensObtidos.size(); ++i) {
        SistemaBestiario::instancia().registrarDrop(inimigo->obterNome(), itensObtidos[i]);
    }
}