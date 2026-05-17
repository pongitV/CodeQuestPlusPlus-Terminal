#include "TelaMenu.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

#include "../Utilidades/Aparencia.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/ControleDeInput.h"
#include "../Racas/RacaBase.h"
#include "TelaMenuLayouts.h"
#include "../Utilidades/GeradorAleatorio.h"

namespace {
    int exibirPromptGenerico(const std::string& titulo, const std::string& infoBox, const std::vector<std::string>& narracao, const std::vector<std::string>& opcoes) {
        Aparencia::limparTela();
        TelaMenu::exibirLogoDoJogo(titulo);
        
        if (!infoBox.empty()) {
            std::string cleanInfoBox = infoBox;
            size_t pos = 0;
            while ((pos = cleanInfoBox.find('|', pos)) != std::string::npos) {
                cleanInfoBox.replace(pos, 1, "║");
                pos += 3; // Avança 3 bytes (tamanho de "║" em UTF-8)
            }
            
            int infoLength = Aparencia::obterComprimentoVisual(infoBox);
            std::string tracos = "";
            for (int i = 0; i < infoLength - 2; ++i) tracos += "═";
            
            std::string margem = Aparencia::espacosParaCentralizar(infoLength);
            std::cout << margem << "╔" << tracos << "╗\n";
            std::cout << margem << cleanInfoBox << "\n";
            std::cout << margem << "╚" << tracos << "╝\n\n";
        }
        
        Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
        std::cout << "\n";
        
        return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
    }
}

bool TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    Aparencia::limparTela();
    exibirLogoDoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    int larguraTerminal = Aparencia::obterLarguraTerminal();

    int recuoEsquerdo = Aparencia::imprimirLadoALado(informacoesParaExibir, arteAsciiParaExibir, 40, 6, Cor::RESET, Cor::RESET);

    std::string margem = std::string(recuoEsquerdo, ' ');
    std::cout << "\n";
    std::vector<std::string> opcoes = { "VOLTAR", "CONFIRMAR" };
    int opcaoDeConfirmacao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, margem);
    
    return opcaoDeConfirmacao == 1;
}

void TelaMenu::exibirLogoDoJogo(const std::string& tituloDaTela) 
{
    int larguraConsole = Aparencia::obterLarguraTerminal();
    
    std::string linhaDupla = "";
    for(int i = 0; i < larguraConsole; ++i) linhaDupla += "═";
    std::cout << "\n" << linhaDupla << "\n\n";

    int larguraLinhaCompleta = 140; 

    for (size_t i = 0; i < ArtesMenu::logoTexto.size(); ++i) 
    {
        int recuo = (larguraConsole - larguraLinhaCompleta) / 2;
        if (recuo < 0) recuo = 0;
        std::cout << std::string(recuo, ' ');

        std::cout << ArtesMenu::logoTexto[i];
        std::cout << Aparencia::cor(Cor::LARANJA) << ArtesMenu::logoPlus[i] << Aparencia::cor(Cor::RESET);
        std::cout << "\n";
    }

    std::cout << "\n";
    
    if (tituloDaTela.empty()) 
    {
        Aparencia::imprimirLinhaDivisoria();
    } 
    else 
    {
        Aparencia::imprimirLinhaDivisoria();
        Aparencia::imprimirCentralizado(tituloDaTela);
        Aparencia::imprimirLinhaDivisoria();
    }
    std::cout << "\n";
}

std::vector<std::string> TelaMenu::comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab) {
    auto formatarAtributo = [](const std::string& nomeAtr, int valorAtr) { 
        return " - " + nomeAtr + ": " + (valorAtr >= 0 ? "+" : "") + std::to_string(valorAtr); 
    };
    return {
        tituloSecao,
        formatarAtributo("Vida", stats.vida),
        formatarAtributo("Forca", stats.forca),
        formatarAtributo("Destreza", stats.destreza),
        formatarAtributo("Resistencia", stats.resistencia),
        formatarAtributo("Constituicao", stats.constituicao),
        formatarAtributo("Inteligencia", stats.inteligencia),
        formatarAtributo("Sabedoria", stats.sabedoria),
        "",
        tituloHabilidade,
        " " + nomeHab,
        " - " + descHab
    };
}

int TelaMenu::exibirOpcoesMenuPrincipal(bool temSave) {
    static bool animacaoAberturaConcluida = false;

    if (!animacaoAberturaConcluida) {
        std::cout << "\033[?25l"; // Esconde o cursor durante a animacao de fade in
        Aparencia::limparTela();
        int larguraConsole = Aparencia::obterLarguraTerminal();
        int alturaConsole = Aparencia::obterAlturaTerminal();
        
        int larguraLinhaCompleta = 140; 
        int recuo = std::max(0, (larguraConsole - larguraLinhaCompleta) / 2);
        std::string margem(recuo, ' ');

        for (int intensidade = 0; intensidade <= 255; intensidade += 8) {
            std::ostringstream buffer;
            std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
            
            std::string corRGB = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + "m";
            int intensidadeLaranjaG = static_cast<int>(intensidade * 0.65);
            std::string corRGBPlus = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidadeLaranjaG) + ";0m";

            buffer << "\n\n\n\n\n\n";
            for (size_t i = 0; i < ArtesMenu::logoTexto.size(); ++i) {
                buffer << margem << corRGB << ArtesMenu::logoTexto[i] << corRGBPlus << ArtesMenu::logoPlus[i] << "\033[0m\n";
            }
            
            buffer << "\n\n\n";
            std::string corOpcao = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + ";" + std::to_string(intensidade) + "m";
            std::string textoOpcao = "-> PRESSIONE QUALQUER TECLA PARA INICIAR <-";
            int espacosMenu = std::max(0, (larguraConsole - static_cast<int>(textoOpcao.length())) / 2);
            buffer << std::string(espacosMenu, ' ') << corOpcao << textoOpcao << "\033[0m\n";

            buffer << "\033[J"; // Limpa o restante da tela abaixo
            std::string versao = "Versao 0.1";
            int colVersao = std::max(1, larguraConsole - static_cast<int>(versao.length()) - 1);
            buffer << "\033[" << alturaConsole << ";" << colVersao << "H" << corOpcao << versao << "\033[0m";

            std::cout.rdbuf(oldCout);
            std::cout << "\033[H" << buffer.str() << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
        }
        
        int frame = 0;
        ControleDeInput::limparBuffer();
        while (!ControleDeInput::teclaPressionada()) {
            std::ostringstream buffer;
            std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
            
            std::string corBranco = "\033[38;2;255;255;255m";
            std::string corLaranja = "\033[38;2;255;165;0m";
            std::string corAmarelo = "\033[38;2;255;255;0m";
            
            buffer << "\n\n\n\n\n\n";
            for (size_t i = 0; i < ArtesMenu::logoTexto.size(); ++i) {
                buffer << margem << corBranco << ArtesMenu::logoTexto[i] << corLaranja << ArtesMenu::logoPlus[i] << "\033[0m\n";
            }
            
            buffer << "\n\n\n";
            // Alterna a cor entre branco e cinza escuro para fazer o texto piscar
            std::string corOpcao = (frame % 4 < 2) ? "\033[38;2;255;255;255m" : "\033[38;2;120;120;120m";
            std::string textoOpcao = "-> PRESSIONE QUALQUER TECLA PARA INICIAR <-";
            int espacosMenu = std::max(0, (larguraConsole - static_cast<int>(textoOpcao.length())) / 2);
            buffer << std::string(espacosMenu, ' ') << corOpcao << textoOpcao << "\033[0m\n\n\n";

            // Desenha a animacao de RPG lado a lado centralizada
            int step = frame % 4; // 4 frames de animacao por classe
            int larguraAnimacaoTotal = 18 * 4; // 4 classes com 18 caracteres de largura cada
            int espacosAnim = std::max(0, (larguraConsole - larguraAnimacaoTotal) / 2);

            for (size_t i = 0; i < 5; ++i) { // Cada arte tem 5 linhas de altura
                std::string corBranca = "\033[38;2;255;255;255m";
                
                buffer << std::string(espacosAnim, ' ') 
                       << corBranca << ArtesMenu::animacaoRpg[0 + step][i] 
                       << ArtesMenu::animacaoRpg[4 + step][i] 
                       << ArtesMenu::animacaoRpg[8 + step][i] 
                       << ArtesMenu::animacaoRpg[12 + step][i] << "\033[0m\n";
            }

            buffer << "\033[J"; // Limpa o restante da tela abaixo
            std::string versao = "Versao 0.1";
            int colVersao = std::max(1, larguraConsole - static_cast<int>(versao.length()) - 1);
            buffer << "\033[" << alturaConsole << ";" << colVersao << "H\033[38;2;100;100;100m" << versao << "\033[0m";

            std::cout.rdbuf(oldCout);
            std::cout << "\033[H" << buffer.str() << std::flush;
            
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            frame++;
        }
        
        ControleDeInput::lerTecla();      // Consome a tecla que o usuário apertou
        ControleDeInput::limparBuffer(); // Garante que a entrada não vazou para o próximo menu
        
        animacaoAberturaConcluida = true;
    }

    Aparencia::limparTela();
    exibirLogoDoJogo("MENU PRINCIPAL");
    
    std::vector<std::string> opcoes = { "Novo Jogo" };
    if (temSave) opcoes.push_back("Continuar Jogo");
    opcoes.push_back("Opcoes");
    opcoes.push_back("Sair");

    std::cout << "\n";
    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

int TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    Aparencia::limparTela();
    exibirLogoDoJogo("CARREGAR JOGO");
    
    std::cout << "\n";
    Aparencia::imprimirCentralizado("Selecione o save que deseja carregar:");
    std::cout << "\n";

    std::vector<std::string> opcoes = informacoesSaves;
    opcoes.push_back("Voltar");

    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

void TelaMenu::exibirPromptNome() {
    std::cout << "\033[?25l"; // Esconde o cursor durante a animacao de digitacao do texto
    Aparencia::limparTela();
    exibirLogoDoJogo("INTRODUCAO AO RPG");
    
    Aparencia::imprimirBlocoCentralizadoDigitando({
        "[NARRACAO]: O mundo clama por um novo destino...",
        "[NARRACAO]: E todas lendas possuem um nome."
    });
    std::cout << "\n";
    Aparencia::exibirPrompt(" > Escolha o nome do seu personagem (ou '0' para sair): ");
}

int TelaMenu::exibirPromptRaca(const std::string& nome) {
    return exibirPromptGenerico("SELECAO DE RACA", "| JOGADOR: " + nome + " |", 
        {"[NARRACAO]: Qual sua origem?"}, 
        {"Dwarf", "Elfo", "Humano", "Ork", "VOLTAR (selecao de nome)"});
}

int TelaMenu::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    return exibirPromptGenerico("SELECAO DE CLASSE", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " |", 
        {"[NARRACAO]: Qual seu caminho?"}, 
        {"Arqueiro", "Bardo", "Guerreiro", "Mago", "VOLTAR (selecao de raca)"});
}

int TelaMenu::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("DIFICULDADE DO MUNDO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |", 
        {"[SISTEMA]: Escolha o nivel de desafio da sua jornada:"}, 
        {
            Aparencia::cor(Cor::VERDE) + "FACIL   (Inimigos com 1x Atributos, sem habilidades adicionais)" + Aparencia::cor(Cor::RESET),
            Aparencia::cor(Cor::AMARELO) + "NORMAL  (Inimigos com 1.5x Atributos, com habilidades de raca)" + Aparencia::cor(Cor::RESET),
            Aparencia::cor(Cor::VERMELHO) + "DIFICIL (Inimigos com 2x Atributos, com habilidades de raca e classe)" + Aparencia::cor(Cor::RESET),
            "VOLTAR (selecao de classe)"
        });
}

int TelaMenu::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("CONFIGURACOES DO JOGO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |", 
        {
            "[SISTEMA]: Deseja ativar o sistema de PARRY?",
            "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)",
            "(O tutorial esta disponivel apenas neste momento, mas voce pode ligar/desligar depois)"
        }, 
        {"Parry LIGADO (Inicia o Tutorial)", "Parry DESLIGADO (Pula o tutorial)", "VOLTAR (selecao de dificuldade)"});
}

void TelaMenu::exibirTutorialDeParry() {
    Aparencia::limparTela();
    exibirLogoDoJogo("TUTORIAL DE PARRY");

    std::vector<std::string> explicacao = {
        "═══ COMO FUNCIONA O PARRY ═══",
        "Quando um inimigo atacar, voce deve estar pronto para reagir.",
        "Uma sequencia de numeros aparecera na tela com um limite de tempo.",
        "Digite os numeros rapidamente na sequencia correta e pressione ENTER.",
        "Se for rapido o suficiente e nao errar, o dano sera reduzido ou totalmente anulado!",
    };
    Aparencia::imprimirBlocoCentralizado(explicacao);
    std::cout << "\n";
    std::vector<std::string> opcoesTutorial = {"INICIAR TESTE", "PULAR"};
    int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoesTutorial, true);
    if (escolha == 1) return;

    struct NivelTutorial {
        std::string nomeInimigo;
        int digitos;
        int tempoLimiteMs;
    };

    std::vector<NivelTutorial> niveis = {
        {"(Nivel 1)", 3, 3000},
        {"(Nivel 2)", 4, 2500},
        {"(Nivel 3)", 5, 2000},
        {"(Nivel Extra)", 6, 1500}
    };

    for (size_t i = 0; i < niveis.size(); ++i) {
        Aparencia::limparTela();
        exibirLogoDoJogo("TUTORIAL DE PARRY - " + niveis[i].nomeInimigo);
        
        std::cout << "\n";
        Aparencia::imprimirCentralizado("Inimigo: " + niveis[i].nomeInimigo);
        Aparencia::imprimirCentralizado("Sequencia: " + std::to_string(niveis[i].digitos) + " digitos | Tempo limite: " + std::to_string(niveis[i].tempoLimiteMs / 1000.0) + "s");
        ControleDeInput::aguardarEnter();

        int acertos = 0;
        for (int teste = 1; teste <= 5; ++teste) {
            std::cout << "\n";
            Aparencia::imprimirCentralizado("[ Teste " + std::to_string(teste) + "/5 ] Prepare-se...");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            std::string sequencia = "";
            for(int d = 0; d < niveis[i].digitos; d++){
                sequencia += std::to_string(GeradorAleatorio::obterInteiro(0, 9));
            }

            Aparencia::imprimirCentralizado("O " + niveis[i].nomeInimigo + " ataca! DIGITE: " + Aparencia::cor(Cor::CIANO) + sequencia + Aparencia::cor(Cor::RESET));
            
            auto inicio = std::chrono::steady_clock::now();
            std::string entrada = ControleDeInput::lerEntradaProtegida("> ");
            auto fim = std::chrono::steady_clock::now();
            
            int tempoGastoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio).count());

            if (entrada == sequencia) {
                if (tempoGastoMs <= niveis[i].tempoLimiteMs) {
                    Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERDE) + "Parry Perfeito! (" + std::to_string(tempoGastoMs) + "ms)" + Aparencia::cor(Cor::RESET));
                    acertos++;
                } else {
                    Aparencia::imprimirCentralizado(Aparencia::cor(Cor::AMARELO) + "Muito lento! (" + std::to_string(tempoGastoMs) + "ms / " + std::to_string(niveis[i].tempoLimiteMs) + "ms)" + Aparencia::cor(Cor::RESET));
                }
            } else {
                Aparencia::imprimirCentralizado(Aparencia::cor(Cor::VERMELHO) + "Errou a sequencia! (Voce digitou: " + entrada + ")" + Aparencia::cor(Cor::RESET));
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(800));
        }

        std::cout << "\n";
        Aparencia::imprimirCentralizado("Resultado do nivel: " + std::to_string(acertos) + "/5 acertos.");
        ControleDeInput::aguardarEnter();
    }

    Aparencia::limparTela();
    exibirLogoDoJogo("TUTORIAL CONCLUIDO");
    Aparencia::imprimirCentralizado("Voce completou o tutorial de Parry!");
    ControleDeInput::aguardarEnter();
}

void TelaMenu::exibirIntroducaoJornada() {
    std::cout << "\n";
    std::string textoFinal = "[SISTEMA]: Personagem criado com sucesso!";
    Aparencia::imprimirCentralizadoDigitando(textoFinal);
    ControleDeInput::aguardarEnter();

    Aparencia::limparTela();
    exibirLogoDoJogo("SISTEMA DE SAVE");
    std::vector<std::string> avisoSave = {
        "═══ AVISO IMPORTANTE ═══",
        "O jogo NAO possui salvamento automatico (auto-save).",
        "Para salvar o seu progresso, abra a sua Ficha de Jogador",
        "durante a exploracao ou durante uma batalha,",
        "e escolha a opcao de Voltar ao Menu Principal."
    };
    Aparencia::imprimirBlocoCentralizado(avisoSave);
    ControleDeInput::aguardarEnter();

    Aparencia::limparTela();
    exibirLogoDoJogo("INICIO DA JORNADA");
    std::vector<std::string> dialogoInicio = {
        "[NARRACAO]: Voce desperta nos arredores de um lugar desconhecido...",
        "[NARRACAO]: Na sua vista, uma pequena vila sendo atacada por monstros.",
        "[NARRACAO]: Empunhando seu equipamento, voce sente que seu destino o aguarda.",
        "[NARRACAO]: Um novo capitulo se inicia agora."
    };
    Aparencia::imprimirBlocoCentralizadoDigitando(dialogoInicio);
    ControleDeInput::aguardarEnter("Pressione ENTER para iniciar...");
}