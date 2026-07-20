#include "Mapa4Reino.h"
#include "../../Core/Controladores/Debug.h"
#include "Mapa4ReinoLayout.h"
#include "../../Core/Controladores/MenuJogo.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../ControleMapa.h"
#include "../Sistemas/AnimadorMapa.h"
#include "../Sistemas/CarregadorMapa.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Entidades/NPCs/Mercador/NPCMercador.h"
#include "../../Entidades/NPCs/Ferreiro/NPCFerreiro.h"
#include "../../Entidades/NPCs/Aparencia/NPCAparencia.h"
#include "../../Entidades/NPCs/CavaleiroGenerico/NPCCavaleiroGenerico.h"
#include "../../Entidades/NPCs/Alquimista/NPCAlquimista.h"
#include "../../Entidades/NPCs/Padre/NPCPadre.h"
#include "../../Sistemas/Progresso/Diario.h"
#include "../../Sistemas/Progresso/Progressao.h"
#include "../../Sistemas/Progresso/ProgressaoFlags.h"
#include "../../Sistemas/Combate/Combate.h"

#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>

Mapa4Reino::Mapa4Reino(Personagem* personagemJogador) :
    posicaoXDoJogador(41), 
    posicaoYDoJogador(41),
    jogadorAtual(personagemJogador), 
    exploracaoEstaAtiva(true), 
    tituloDoMapaAtual("REINO"),
    proximoMapa(ProximaTransicaoMapa::Nenhuma),
    jogadorEstaDentroDeUmSubMapa(false),
    igrejaJaFoiVisitada(false)
{
    matrizDoMapaAtual = Mapa4ReinoLayouts::obterLayoutReino();
    CarregadorMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);
    matrizDoMapaPrincipalSalva = matrizDoMapaAtual; // Caso necessário
}

Mapa4Reino::~Mapa4Reino() = default;

ProximaTransicaoMapa Mapa4Reino::iniciarLoopDeExploracao()
{
    CarregadorMapa::padronizarTamanhoDoMapa(matrizDoMapaAtual);
    Aparencia::ocultarCursor();

    auto formatador = [&](char celula, int x, int y) -> std::string {
        if (x == posicaoXDoJogador && y == posicaoYDoJogador) {
            char ic = Aparencia::iconeJogadorPersonalizado;
            if (ic <= 32 || ic > 126) ic = '@'; // Garante caractere visivel
            return Aparencia::cor(Aparencia::corJogadorPersonalizada) + std::string(1, ic) + Aparencia::cor(Cor::RESET);
        }
        return ControleMapa::formatarCelula(celula, x, y, tituloDoMapaAtual, matrizDoMapaAtual, false);
    };

    int linhaInicialParaDesenharOMapa = 0;

    auto restaurarTela = [&]() {
        linhaInicialParaDesenharOMapa = AnimadorMapa::animarIntroducaoMapa(tituloDoMapaAtual, {}, 0, {}, 0, Cor::ROXO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, false, true, nullptr);
    };

    auto animarTela = [&]() {
        std::vector<std::string> arteTitulo;
        int larguraArte = 0;
        
        // Ensure title is strictly REINO for the banner
        if (tituloDoMapaAtual == "REINO" || tituloDoMapaAtual.find("Reino") != std::string::npos || tituloDoMapaAtual.find("REINO") != std::string::npos) {
            tituloDoMapaAtual = "REINO";
            arteTitulo = Mapa4ReinoLayouts::obterLogoReino();
            larguraArte = 77;
        }
        linhaInicialParaDesenharOMapa = AnimadorMapa::animarIntroducaoMapa(tituloDoMapaAtual, arteTitulo, larguraArte, {}, 0, Cor::ROXO, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador, formatador, true, true, nullptr);
    };

    animarTela();

    std::unordered_map<char, std::function<void(int, int, int)>> interacoes;

    // Triggers e Teleportes
    interacoes['^'] = [&](int px, int py, int larg) {
        if (jogadorEstaDentroDeUmSubMapa) {
            // Saindo da Igreja (submapa) de volta para o patio do Reino
            if (px == 18 && py == 3) {
                matrizDoMapaAtual = matrizDoMapaPrincipalSalva;
                posicaoXDoJogador = posicaoXSalvaAntesDeEntrarNoSubMapa;
                posicaoYDoJogador = posicaoYSalvaAntesDeEntrarNoSubMapa;
                jogadorEstaDentroDeUmSubMapa = false;
                tituloDoMapaAtual = "REINO";
                restaurarTela();
            }
        } else {
            // Retornar para o Reino (Ponte)
            if (py > 30) {
                exploracaoEstaAtiva = false;
                proximoMapa = ProximaTransicaoMapa::Reino;
            }
            // Entrada do Palacio (agora no X=43, Y=1)
            else if (py == 1 && (px >= 40 && px <= 45)) {
                Aparencia::iniciarInteracaoPopup();
                std::vector<std::string> msg = {
                    "Os grandes portoes do Palacio Real estao selados por runas magicas.",
                    "Uma barreira intransponivel impede sua passagem por enquanto.",
                    "A aventura continuara em breve..."
                };
                Aparencia::exibirPopup("PORTAO DO PALACIO", msg, Cor::AMARELO);
                posicaoXDoJogador = px;
                posicaoYDoJogador = py + 1; // Recua um passo
                restaurarTela();
            }
        }
    };

    // Entrada da Igreja
    interacoes['I'] = [&](int px, int py, int larg) {
        if (!jogadorEstaDentroDeUmSubMapa) {
            CarregadorMapa::entrarSubMapa(
                matrizDoMapaAtual, matrizDoMapaPrincipalSalva,
                posicaoXSalvaAntesDeEntrarNoSubMapa, posicaoYSalvaAntesDeEntrarNoSubMapa,
                posicaoXDoJogador, posicaoYDoJogador, jogadorEstaDentroDeUmSubMapa,
                tituloDoMapaAtual, matrizDoMapaDaIgrejaSalva, igrejaJaFoiVisitada,
                Mapa4ReinoLayouts::obterLayoutIgreja(), 17, 3, "IGREJA DO REINO", restaurarTela
            );
        }
    };

    // Padre da Igreja (só funciona dentro do submapa da igreja)
    interacoes['P'] = [&](int px, int py, int larg) {
        if (jogadorEstaDentroDeUmSubMapa) {
            NPCPadre padre;
            padre.interagir(jogadorAtual);
            Diario::instancia().registrarNPC("Padre Benedito");
            if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
        }
    };

    // Loja do Franchesco
    interacoes['F'] = [&](int px, int py, int larg) {
        NPCMercador franchesco;
        franchesco.interagir(jogadorAtual);
        Diario::instancia().registrarNPC("Franchesco (Mercador)");
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    // Forja do Bjorn
    interacoes['B'] = [&](int px, int py, int larg) {
        NPCFerreiro bjorn;
        bjorn.interagir(jogadorAtual);
        Diario::instancia().registrarNPC("Bjorn (Ferreiro)");
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    // Cavaleiro Generico (Treino)
    interacoes['C'] = [&](int px, int py, int larg) {
        Aparencia::iniciarInteracaoPopup();
        std::vector<std::string> falas = {
            "Saudacoes, guerreiro!",
            "Deseja treinar suas habilidades em um combate amistoso?",
            "Esta luta nao concede experiencia (XP) ou recompensas permanentes,",
            "mas serve como um otimo teste de suas taticas."
        };
        int escolha = ControleDeInput::lerSelecaoMenuEmPopup("TREINO DE COMBATE", falas, {"Aceitar Treino", "Recusar"}, Cor::CINZA);
        if (escolha == 0) {
            std::vector<std::unique_ptr<Personagem>> inimigos;
            inimigos.push_back(NPCCavaleiroGenerico::criarCavaleiro("Cavaleiro de Treino"));

            int xpAntes = jogadorAtual->obterXpAtual();
            int ouroAntes = jogadorAtual->obterInventario()->obterOuro();

            Combate combate(jogadorAtual, std::move(inimigos));
            if (ControleMapa::isExploracao3DAtiva()) {
                combate.setContexto3D(
                    true, 
                    matrizDoMapaAtual, 
                    ControleMapa::obterPosCamera3DX(), 
                    ControleMapa::obterPosCamera3DY(), 
                    ControleMapa::obterAnguloCamera3D(), 
                    ControleMapa::obterTituloMapaAtual()
                );
            }
            combate.iniciarCombate();

            // Restaura o progresso de XP e Ouro para garantir que nao ganhe nada permanente
            jogadorAtual->definirXpAtual(xpAntes);
            int ouroDepois = jogadorAtual->obterInventario()->obterOuro();
            jogadorAtual->obterInventario()->adicionarOuro(ouroAntes - ouroDepois);
        }
        
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    // Loja de aparencia (Anok)
    interacoes['N'] = [&](int px, int py, int larg) {
        NPCAparencia aparencia;
        aparencia.interagir(jogadorAtual);
        Diario::instancia().registrarNPC("Anok (Estilista)");
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    // Alquimista
    interacoes['Q'] = [&](int px, int py, int larg) {
        NPCAlquimista alquimista;
        alquimista.interagir(jogadorAtual);
        Diario::instancia().registrarNPC("Alquimista Real");
        if (exploracaoEstaAtiva && !ControleMapa::isExploracao3DAtiva()) restaurarTela();
    };

    auto processarInteracao = [&](int px, int py, int larg) {
        char celulaDestino = matrizDoMapaAtual[py][px];
        auto it = interacoes.find(celulaDestino);
        if (it != interacoes.end()) {
            it->second(px, py, larg);
        } else if ((celulaDestino != '*' && celulaDestino != '#' && celulaDestino != '=' && celulaDestino != '|' && celulaDestino != '[' && celulaDestino != ']' && celulaDestino != ' ') || Debug::isNoclipAtivo) {
            posicaoXDoJogador = px;
            posicaoYDoJogador = py;
            
            // Transicao para a Ponte do Reino (saida sul do mapa)
            if (!jogadorEstaDentroDeUmSubMapa && py >= matrizDoMapaAtual.size() - 3) {
                proximoMapa = ProximaTransicaoMapa::PonteReino;
                exploracaoEstaAtiva = false;
            }
        }
    };

    bool precisaRenderizar = true;
    ProximaTransicaoMapa destinoViagemRapida = ControleMapa::executarLoopDeExploracao(
        jogadorAtual, matrizDoMapaAtual, posicaoXDoJogador, posicaoYDoJogador,
        exploracaoEstaAtiva, tituloDoMapaAtual, []() { return ""; },
        [this]() -> std::vector<std::string> { 
            if (jogadorEstaDentroDeUmSubMapa) return Mapa4ReinoLayouts::obterLayoutIgreja();
            return Mapa4ReinoLayouts::obterLayoutReino();
        },
        processarInteracao, formatador, restaurarTela,
        linhaInicialParaDesenharOMapa, precisaRenderizar
    );

    if (destinoViagemRapida != ProximaTransicaoMapa::Nenhuma) {
        return destinoViagemRapida;
    }

    if (jogadorAtual->obterVida() <= 0 || jogadorAtual->obterVoltarProMenu()) {
        return ProximaTransicaoMapa::VoltarMenu;
    }
    return proximoMapa;
}
