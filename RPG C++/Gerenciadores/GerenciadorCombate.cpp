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
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Telas/TelaMenu.h"

namespace {
    int lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, int espacosIniciais = 0) {
        int valor;
        std::cout << std::string(espacosIniciais, ' ') << promptMensagem;
        while (!(std::cin >> valor) || valor < minimo || valor > maximo) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << std::string(espacosIniciais, ' ') << "Entrada invalida! " << promptMensagem;
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

std::vector<SistemaPersonagem*> GerenciadorCombate::obterInimigosRaw() const
{
    std::vector<SistemaPersonagem*> ponteirosInimigos(listaDeInimigos.size());
    std::transform(listaDeInimigos.begin(), listaDeInimigos.end(), ponteirosInimigos.begin(), [](const std::unique_ptr<SistemaPersonagem>& ptr) { return ptr.get(); });
    return ponteirosInimigos;
}

void GerenciadorCombate::exibirTelaDeCombate() const
{
    SimplificacoesAparencia::limparTela();
    TelaCombate::exibirLogoParaTelaDeCombate(obterTituloDoCombate());
    TelaCombate::exibirHordaDeInimigosLadoALado(obterInimigosRaw());
    TelaCombate::exibirBarraDeStatusDoJogador(jogadorAtual);
    for (const auto& aliado : listaDeAliados) {
        if (aliado->obterVida() > 0) {
            TelaCombate::exibirBarraDeStatusDoJogador(aliado.get());
        }
    }
}

void GerenciadorCombate::iniciarCombate() 
{
    jogadorAtual->prepararParaNovaBatalha();

    for (auto& aliado : listaDeAliados) {
        aliado->prepararParaNovaBatalha();
    }

    int maxDestrezaInimigos = 0;
    for (const auto& inimigoPtr : listaDeInimigos) {
        SistemaBestiario::instancia().registrarPrimeiraVista(inimigoPtr->obterNome());
        if (inimigoPtr->obterDestreza() > maxDestrezaInimigos) maxDestrezaInimigos = inimigoPtr->obterDestreza();
    }
    
    bool turnoExtraFirstTurn = (jogadorAtual->obterDestreza() > (maxDestrezaInimigos * 2));
    
    if (maxDestrezaInimigos > jogadorAtual->obterDestreza()) {
        exibirTelaDeCombate();
        std::cout << "\n" << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[SISTEMA]: Os inimigos sao mais ageis e atacam primeiro!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        SimplificacoesAparencia::aguardarEnter();
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
                exibirTelaDeCombate();
                processarMenuDeAcoesDoJogador(jogadorAtual, turnoFoiConsumido, usouInventarioNoTurno);
                if (verificarCondicaoDeVitoriaOuDerrota()) return; 
            }

            if (turnoExtraFirstTurn && contadorDoTurnoAtual == 1) {
                std::cout << "\n" << SimplificacoesAparencia::cor(Cor::CIANO) << "[SISTEMA]: Sua agilidade extrema (" << jogadorAtual->obterDestreza() << " VS " << maxDestrezaInimigos << ") permite que voce aja novamente!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
                SimplificacoesAparencia::aguardarEnter();
                turnoExtraFirstTurn = false;
                continue;
            }

            if (usouInventarioNoTurno) {
                exibirTelaDeCombate();
                std::cout << "\n" << SimplificacoesAparencia::cor(Cor::AMARELO) << "[SISTEMA]: O inimigo te pegou desprevinido enquanto voce usava o inventario!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
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
                std::string textoAliado = "--- TURNO DO ALIADO: " + aliado->obterNome() + " ---";
                int espacos = std::max(0, (SimplificacoesAparencia::obterLarguraTerminal() - (int)textoAliado.length()) / 2);
                std::cout << "\n" << std::string(espacos, ' ') << SimplificacoesAparencia::cor(Cor::CIANO) << textoAliado << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
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
    int larguraDoTerminal = SimplificacoesAparencia::obterLarguraTerminal();
    std::string textoDoTurno = "TURNO " + std::to_string(contadorDoTurnoAtual) + " | VEZ DE " + personagemAgindo->obterNome();
    int espacosTurno = std::max(0, (larguraDoTerminal - static_cast<int>(textoDoTurno.length())) / 2);
    std::cout << "\n" << std::string(espacosTurno, ' ') << textoDoTurno << "\n";

    std::string textoAcoes = "1. Atacar | 2. Defender | 3. Habilidade | 4. Inventario | 5. Jogador | 6. Bestiario | Escolha: ";
    int espacosAcoes = std::max(0, (larguraDoTerminal - static_cast<int>(textoAcoes.length())) / 2);
    
    int acaoEscolhida = lerInteiroComLimites(textoAcoes, 1, 6, espacosAcoes);

    switch (static_cast<AcaoCombate>(acaoEscolhida)) 
    {
        case AcaoCombate::Atacar: processarAcaoAtacar(personagemAgindo, turnoFoiConsumido); break;
        case AcaoCombate::Defender: processarAcaoDefender(personagemAgindo, turnoFoiConsumido); break;
        case AcaoCombate::Habilidade: processarAcaoHabilidade(personagemAgindo, turnoFoiConsumido); break;
        case AcaoCombate::Inventario: processarAcaoInventario(personagemAgindo, turnoFoiConsumido, usouInventarioNoTurno); break;
        case AcaoCombate::Jogador: TelaAtributos::gerenciarFichaDoJogador(personagemAgindo); break;
        case AcaoCombate::Bestiario: TelaBestiario::exibirLista(personagemAgindo); break;
        default: 
            std::cout << "\n[ERRO] Acao invalida!\n"; 
            SimplificacoesAparencia::aguardarEnter(); 
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
        int indiceDoAlvoEscolhido = lerInteiroComLimites("Escolha o alvo (0 a " + std::to_string(maxIndice) + "): ", 0, maxIndice);

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
        std::cout << "\n[!] " << personagemAgindo->obterNome() << " nao possui escudos no inventario para usar!\n";
        return nullptr;
    }

    std::cout << "=== SELECIONE UM ESCUDO PARA " << personagemAgindo->obterNome() << " ===\n";
    for (size_t indice = 0; indice < listaDeEscudos.size(); indice++) 
    {
        std::cout << " [" << indice + 1 << "] " << listaDeEscudos[indice]->obterNomeItem() << listaDeEscudos[indice]->obterInfoStatus() << "\n";
    }
    std::cout << " [0] Cancelar\n\n";
    
    int opcaoEscolhida = lerInteiroComLimites("Escolha: ", 0, static_cast<int>(listaDeEscudos.size()));
    return (opcaoEscolhida == 0) ? nullptr : listaDeEscudos[opcaoEscolhida - 1];
}

void GerenciadorCombate::processarAcaoDefender(SistemaPersonagem* personagemAgindo, bool& turnoFoiConsumido)
{
    if (personagemAgindo->obterRecargaDefesa()) 
    {
        std::cout << "\n[ERRO]: " << personagemAgindo->obterNome() << " se desequilibrou e precisa de 1 turno para poder defender novamente!\n";
        SimplificacoesAparencia::aguardarEnter();
        return; 
    }
    
    Item* escudoEscolhido = selecionarEscudo(personagemAgindo);
    if (escudoEscolhido != nullptr) 
    {
        if (!escudoEscolhido->podeSerEquipadoPor(personagemAgindo)) {
            std::cout << escudoEscolhido->obterMensagemRequisito();
            SimplificacoesAparencia::aguardarEnter();
            return;
        }

        personagemAgindo->equiparItem(escudoEscolhido);
        personagemAgindo->definirDefendendo(true);
        std::cout << "\n[SISTEMA]: " << personagemAgindo->obterNome() << " assumiu uma postura defensiva com " << escudoEscolhido->obterNomeItem() << "!\n";
        SimplificacoesAparencia::aguardarEnter();
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
    else SimplificacoesAparencia::aguardarEnter();
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
        
        std::cout << "\n--- ESCOLHA UM ALVO ---\n";
        for (size_t i = 0; i < listaDeInimigos.size(); ++i) {
            std::cout << "[" << i << "] " << listaDeInimigos[i]->obterNome() << " (HP: " << listaDeInimigos[i]->obterVida() << ")\n";
        }
        
        int maxIndice = static_cast<int>(listaDeInimigos.size()) - 1;
        int indiceDoAlvoEscolhido = lerInteiroComLimites("Escolha (ou -1 para CANCELAR): ", -1, maxIndice);

        if (indiceDoAlvoEscolhido == -1) 
        {
            std::cout << "\n[SISTEMA] Uso do frasco cancelado. O item voltou para a mochila.\n";
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
        std::cout << "\n--- TURNO DOS INIMIGOS ---\n";
        for (auto& inimigoAtualPtr : listaDeInimigos) 
        {
            if (jogadorAtual->obterVida() <= 0) break; // Interrompe se o jogador morrer
            
            SistemaPersonagem* inimigoAtual = inimigoAtualPtr.get();
            inimigoAtual->processarEfeitosInicioTurno();
            if (inimigoAtual->obterVida() <= 0) continue;

            if (inimigoAtual->podeAgir()) 
            {
                SistemaPersonagem* alvo = jogadorAtual;
                std::vector<SistemaPersonagem*> aliadosVivos;
                for (auto& aliado : listaDeAliados) {
                    if (aliado->obterVida() > 0) aliadosVivos.push_back(aliado.get());
                }
                if (!aliadosVivos.empty()) {
                    alvo = aliadosVivos[rand() % aliadosVivos.size()];
                }

                bool turnoConsumidoPorHabilidade = inimigoAtual->obterRaca()->tentarUsarHabilidadeAtiva(inimigoAtual, alvo, static_cast<int>(jogadorAtual->obterDificuldade()));
                
                if (!turnoConsumidoPorHabilidade) 
                {
                    realizarAtaqueFisico(inimigoAtual, alvo, contadorDoTurnoAtual);
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

    bool isAtacanteJogadorOuAliado = (personagemAtacante == jogadorAtual);
    if (!isAtacanteJogadorOuAliado) {
        for (const auto& aliadoAtual : listaDeAliados) { if (aliadoAtual.get() == personagemAtacante) isAtacanteJogadorOuAliado = true; }
    }

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
    if (personagemAlvo->obterParryAtivado()) 
    {
        tentouParry = true;
        parryFoiBemSucedido = SistemaParry::tentarParry(personagemAtacante, danoBaseMitigado, quantidadeDeDanoReduzidoPeloParry);
    }

    bool isAlvoAliado = false;
    for (const auto& aliadoAtual : listaDeAliados) { if (aliadoAtual.get() == personagemAlvo) isAlvoAliado = true; }
    bool aplicarPassivas = (personagemAlvo == jogadorAtual || isAlvoAliado || static_cast<int>(jogadorAtual->obterDificuldade()) >= 2);
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
    bool isJogadorOuAliado = (alvo == jogadorAtual);
    if (!isJogadorOuAliado) {
        for (const auto& aliadoAtual : listaDeAliados) { if (aliadoAtual.get() == alvo) isJogadorOuAliado = true; }
    }

    if (isJogadorOuAliado) 
    {
        if (tentouParry) 
        {
            std::string mensagemParry = parrySucesso ? (danoFinal <= 1 ? "parry efetivo! Ataque anulado!" : "parry efetivo! Mas o ataque e muito forte!") : "parry falhou!";
            std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_VERMELHO) << ">> [PARRY]: " << mensagemParry << " Inimigo causou " << danoFinal << " de dano em " << alvo->obterNome() << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }
        else if (danoFinal > 0) 
        {
            std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_VERMELHO) << ">> " << alvo->obterNome() << " recebeu " << danoFinal << " de dano" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }
        else if (danoFinal == 0 && alvo->obterDefendendo()) 
        {
            std::cout << ">> O dano foi totalmente absorvido pela defesa de " << alvo->obterNome() << "!\n";
        }
        
        if (danoFinal > 0 && alvo == jogadorAtual) totalDeDanoRecebido += danoFinal;
    }
    else if (danoFinal > 0) 
    {
        if (alvo != jogadorAtual) totalDeDanoCausado += danoFinal;
        std::cout << ">> " << alvo->obterNome() << " recebeu " << danoFinal << " de dano\n";
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