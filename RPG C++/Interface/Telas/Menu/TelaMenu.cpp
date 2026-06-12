#include "TelaMenu.h"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <chrono>
#include <sstream>

#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Entidades/Racas/RacaBase.h"
#include "TelaMenuLayout.h"
#include "../../../Core/Utilidades/GeradorAleatorio.h"

namespace {
    void exibirInfoBox(const std::string& infoBox) {
        if (!infoBox.empty()) {
            std::string cleanInfoBox = infoBox;
            size_t pos = 0;
            while ((pos = cleanInfoBox.find('|', pos)) != std::string::npos) {
                cleanInfoBox.replace(pos, 1, "║");
                pos += 3; // Avanca 3 bytes (tamanho de "║" em UTF-8)
            }
            
            int infoLength = Aparencia::obterComprimentoVisual(infoBox);
            std::string tracos = "";
            for (int i = 0; i < infoLength - 2; ++i) tracos += "═";
            
            std::string margem = Aparencia::espacosParaCentralizar(infoLength);
            std::cout << margem << "╔" << tracos << "╗\n";
            std::cout << margem << cleanInfoBox << "\n";
            std::cout << margem << "╚" << tracos << "╝\n\n";
        }
    }

    int exibirPromptGenerico(const std::string& titulo, const std::string& infoBox, const std::vector<std::string>& narracao, const std::vector<std::string>& opcoes) {
        Aparencia::limparTela();
        TelaMenu::exibirPainelLogoJogo(titulo, false);
        
        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);
        
        Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
        std::cout << "\n";
        
        return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
    }
}

bool TelaMenu::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) 
{
    std::cout << "\033[?25l";
    Aparencia::limparTela();

    Cor corArte = Cor::BRANCO;

    std::vector<std::string> infoSegura;
    for (const auto& s : informacoesParaExibir) {
        if (s.empty()) {
            infoSegura.push_back("");
            continue;
        }

        std::istringstream stream(s);
        std::string linha;
        bool isFirst = true;
        std::string prefixo = "";

        while (std::getline(stream, linha)) {
            if (!linha.empty() && linha.back() == '\r') {
                linha.pop_back();
            }

            if (isFirst) {
                infoSegura.push_back(linha);
                
                size_t pos = 0;
                while (pos < linha.length()) {
                    if (linha[pos] == ' ' || linha[pos] == '-' || linha[pos] == '>') {
                        prefixo += linha[pos];
                        pos++;
                    } else if (linha[pos] == '\033') {
                        size_t mPos = linha.find('m', pos);
                        if (mPos != std::string::npos) {
                            prefixo += linha.substr(pos, mPos - pos + 1);
                            pos = mPos + 1;
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }
                }
                isFirst = false;
            } else {
                if (linha.empty()) {
                    infoSegura.push_back("");
                } else {
                    infoSegura.push_back(prefixo + linha + "\033[0m");
                }
            }
        }
    }

    std::vector<std::string> infoTextOnly;
    for (const auto& s : infoSegura) {
        infoTextOnly.push_back(Aparencia::removerCoresANSI(s));
    }
    
    std::vector<std::string> arteTextOnly;
    for (const auto& s : arteAsciiParaExibir) {
        arteTextOnly.push_back(Aparencia::removerCoresANSI(s));
    }

    int larguraEsq = 40;
    for (const auto& s : infoTextOnly) {
        if (Aparencia::obterComprimentoVisual(s) > larguraEsq) larguraEsq = Aparencia::obterComprimentoVisual(s);
    }
    int larguraDir = 0;
    for (const auto& s : arteTextOnly) {
        if (Aparencia::obterComprimentoVisual(s) > larguraDir) larguraDir = Aparencia::obterComprimentoVisual(s);
    }
    
    int larguraConsole = Aparencia::obterLarguraTerminal();
    int recuoEsquerdo = (larguraConsole - (larguraEsq + 6 + larguraDir)) / 2;
    if (recuoEsquerdo < 0) recuoEsquerdo = 0;

    size_t maxLinhas = std::max(infoTextOnly.size(), arteTextOnly.size());

    Aparencia::animarFadeIn(17, 15, [&](int /*frame*/, int intensidade) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        
        exibirPainelLogoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
        
        std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
        
        buffer << "\n";
        for (size_t i = 0; i < maxLinhas; ++i) {
            buffer << std::string(recuoEsquerdo, ' ');
            if (i < infoTextOnly.size()) {
                std::string textoEsq = infoTextOnly[i];
                int padding = larguraEsq - Aparencia::obterComprimentoVisual(textoEsq);
                buffer << corRGB << textoEsq << std::string(padding > 0 ? padding : 0, ' ');
            } else {
                buffer << std::string(larguraEsq, ' ');
            }
            
            buffer << std::string(6, ' ');
            
            if (i < arteTextOnly.size()) {
                buffer << corRGB << arteTextOnly[i];
            }
            buffer << "\033[0m\033[K\n";
        }
        
        buffer << "\033[J";
        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
    });
    
    std::ostringstream bufferFinal;
    std::streambuf* oldCoutFinal = std::cout.rdbuf(bufferFinal.rdbuf());
    
    exibirPainelLogoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    Aparencia::imprimirLadoALado(infoSegura, arteAsciiParaExibir, 40, 6, Cor::RESET, corArte, 0);
    
    std::cout.rdbuf(oldCoutFinal);
    std::cout << "\033[H" << bufferFinal.str() << "\033[J" << std::flush;

    std::string margem = std::string(recuoEsquerdo, ' ');
    std::cout << "\n";
    std::vector<std::string> opcoes = { "VOLTAR", "CONFIRMAR" };
    int opcaoDeConfirmacao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, margem);
    
    return opcaoDeConfirmacao == 1;
}

void TelaMenu::exibirPainelLogoJogo(const std::string& tituloDaTela, bool animarFadeIn) 
{
    Aparencia::exibirPainel(tituloDaTela, Cor::BRANCO, ArtesMenu::logoTexto, 140, ArtesMenu::logoPlus, Cor::LARANJA, animarFadeIn);
}

std::vector<std::string> TelaMenu::comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    auto formatarAtributo = [](const std::string& nomeAtr, int valorAtr) { 
        std::string corVal = Aparencia::cor(Cor::BRANCO);
        
        if (nomeAtr == "Resistencia") corVal = Aparencia::cor(Cor::AZUL);
        else if (nomeAtr == "Constituicao") corVal = Aparencia::cor(Cor::CIANO);
        else if (nomeAtr == "Vida") {
            if (valorAtr > 100) corVal = Aparencia::cor(Cor::VERDE);
            else if (valorAtr < 100) corVal = Aparencia::cor(Cor::VERMELHO);
        } else {
            if (valorAtr > 10) corVal = Aparencia::cor(Cor::VERDE);
            else if (valorAtr < 10) corVal = Aparencia::cor(Cor::VERMELHO);
        }
        
        std::string sinal = (valorAtr >= 0 ? "+" : "");
        return " - " + nomeAtr + ": " + corVal + sinal + std::to_string(valorAtr) + Aparencia::cor(Cor::RESET); 
    };

    std::vector<std::string> resultado = {
        Aparencia::cor(Cor::BRANCO) + tituloSecao + Aparencia::cor(Cor::RESET),
        formatarAtributo("Vida", stats.vida),
        formatarAtributo("Forca", stats.forca),
        formatarAtributo("Destreza", stats.destreza),
        formatarAtributo("Resistencia", stats.resistencia),
        formatarAtributo("Constituicao", stats.constituicao),
        formatarAtributo("Inteligencia", stats.inteligencia),
        formatarAtributo("Sabedoria", stats.sabedoria),
        "",
        Aparencia::cor(Cor::BRANCO) + tituloHabilidade + Aparencia::cor(Cor::RESET),
        " " + Aparencia::cor(Cor::CIANO) + nomeHab + Aparencia::cor(Cor::RESET)
    };

    std::istringstream stream(descHab);
    std::string linhaDesc;
    while (std::getline(stream, linhaDesc)) {
        resultado.push_back(" - " + Aparencia::cor(Cor::CINZA) + linhaDesc + Aparencia::cor(Cor::RESET));
    }

    if (!tituloHabilidade2.empty()) {
        resultado.push_back("");
        resultado.push_back(Aparencia::cor(Cor::BRANCO) + tituloHabilidade2 + Aparencia::cor(Cor::RESET));
        resultado.push_back(" " + Aparencia::cor(Cor::CIANO) + nomeHab2 + Aparencia::cor(Cor::RESET));

        std::istringstream stream2(descHab2);
        std::string linhaDesc2;
        while (std::getline(stream2, linhaDesc2)) {
            resultado.push_back(" - " + Aparencia::cor(Cor::CINZA) + linhaDesc2 + Aparencia::cor(Cor::RESET));
        }
    }

    return resultado;
}

int TelaMenu::exibirOpcoesMenuPrincipal(bool temSave) {
    static bool animacaoAberturaConcluida = false;
    bool recemConcluida = false;

    if (!animacaoAberturaConcluida) {
        std::cout << "\033[?25l"; // Esconde o cursor durante a animacao de fade in
        Aparencia::limparTela();
        int larguraConsole = Aparencia::obterLarguraTerminal();
        int alturaConsole = Aparencia::obterAlturaTerminal();
        
        int larguraLinhaCompleta = 140; 
        int recuo = std::max(0, (larguraConsole - larguraLinhaCompleta) / 2);
        std::string margem(recuo, ' ');

        Aparencia::animarFadeIn(32, 30, [&](int /*frame*/, int intensidade) {
            std::ostringstream buffer;
            std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
            
            std::string corRGB = Aparencia::obterCorRGBFade(Cor::BRANCO, intensidade);
            int intensidadeLaranjaG = static_cast<int>(intensidade * 0.65);
            std::string corRGBPlus = "\033[38;2;" + std::to_string(intensidade) + ";" + std::to_string(intensidadeLaranjaG) + ";0m";

            buffer << "\n\n";
            for (size_t i = 0; i < ArtesMenu::logoTexto.size(); ++i) {
                buffer << margem << corRGB << ArtesMenu::logoTexto[i] << corRGBPlus << ArtesMenu::logoPlus[i] << "\033[0m\n";
            }
            
            buffer << "\n\n\n";
            std::string corOpcao = corRGB;
            std::string textoOpcao = "-> PRESSIONE QUALQUER TECLA PARA INICIAR <-";
            int espacosMenu = std::max(0, (larguraConsole - static_cast<int>(textoOpcao.length())) / 2);
            buffer << std::string(espacosMenu, ' ') << corOpcao << textoOpcao << "\033[0m\n";

            buffer << "\033[J"; // Limpa o restante da tela abaixo
            std::string versao = "Versao 0.1";
            int colVersao = std::max(1, larguraConsole - static_cast<int>(versao.length()) - 1);
            buffer << "\033[" << alturaConsole << ";" << colVersao << "H" << corOpcao << versao << "\033[0m";

            std::cout.rdbuf(oldCout);
            std::cout << "\033[H" << buffer.str() << std::flush;
        });
        
        int frame = 0;
        ControleDeInput::limparBuffer();
        while (!ControleDeInput::teclaPressionada()) {
            std::ostringstream buffer;
            std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
            
            std::string corBranco = "\033[38;2;255;255;255m";
            std::string corLaranja = "\033[38;2;255;165;0m";
            std::string corAmarelo = "\033[38;2;255;255;0m";
            
            buffer << "\n\n";
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
        recemConcluida = true;
    }

    Aparencia::limparTela();
    exibirPainelLogoJogo("MENU PRINCIPAL", recemConcluida);
    
    std::vector<std::string> opcoes = { "Novo Jogo" };
    if (temSave) opcoes.push_back("Continuar Jogo");
    opcoes.push_back("Opcoes de save");
    opcoes.push_back("Sair do jogo");

    std::cout << "\n";
    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

int TelaMenu::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    Aparencia::limparTela();
    exibirPainelLogoJogo("CARREGAR JOGO", false);
    
    std::vector<std::string> opcoes = informacoesSaves;
    opcoes.push_back("Voltar");

    std::cout << "\n";
    Aparencia::imprimirCentralizado("Selecione o save que deseja carregar:");
    std::cout << "\n";

    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

void TelaMenu::exibirPromptNome() {
    std::cout << "\033[?25l"; // Esconde o cursor durante a animacao de digitacao do texto
    Aparencia::limparTela();
    exibirPainelLogoJogo("INTRODUCAO AO RPG", false);
    std::cout << "\n";
    
    std::vector<std::string> narracao = {
        FuncoesDialogo::formatarMsgNarracao("O mundo clama por um novo destino..."),
        FuncoesDialogo::formatarMsgNarracao("E todas lendas possuem um nome.")
    };

    Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
    std::cout << "\n";
    Aparencia::exibirPrompt(" > Escolha o nome do seu personagem [0 para voltar]: ");
}

int TelaMenu::exibirPromptRaca(const std::string& nome) {
    std::vector<std::string> opcoes = {"Dwarf", "Elfo", "Humano", "Ork"};
    Aparencia::ordenarAlfabeticamente(opcoes);
    opcoes.push_back("VOLTAR (selecao de nome)");
    return exibirPromptGenerico("SELECAO DE RACA", "| JOGADOR: " + nome + " |", 
        {FuncoesDialogo::formatarMsgNarracao("Qual sua origem?")}, 
        opcoes);
}

int TelaMenu::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    std::vector<std::string> opcoes = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante"};
    Aparencia::ordenarAlfabeticamente(opcoes);
    opcoes.push_back("VOLTAR (selecao de raca)");
    return exibirPromptGenerico("SELECAO DE CLASSE", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " |", 
        {FuncoesDialogo::formatarMsgNarracao("Qual seu caminho?")}, 
        opcoes);
}

int TelaMenu::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("DIFICULDADE DO MUNDO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |", 
        {FuncoesDialogo::formatarMsgSistema("Escolha o nivel de desafio da sua jornada:")}, 
        {
            Aparencia::cor(Cor::VERDE) + "FACIL" + Aparencia::cor(Cor::RESET) + "   (Inimigos com 1x Atributos, sem habilidades adicionais)      ",
            Aparencia::cor(Cor::AMARELO) + "NORMAL" + Aparencia::cor(Cor::RESET) + "  (Inimigos com 1.5x Atributos, com habilidades de raca)       ",
            Aparencia::cor(Cor::VERMELHO) + "DIFICIL" + Aparencia::cor(Cor::RESET) + " (Inimigos com 2x Atributos, com habilidades de raca e classe)",
            "VOLTAR (selecao de classe)                                           "
        });
}

int TelaMenu::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("CONFIGURACOES DO JOGO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |", 
        {
            FuncoesDialogo::formatarMsgSistema("Deseja ativar o sistema de PARRY?"),
            "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)",
            "(O tutorial esta disponivel apenas neste momento, mas voce pode ligar/desligar depois)"
        }, 
        {"Parry LIGADO (Tutorial disponivel)", "Parry DESLIGADO (Pula o tutorial)", "VOLTAR (selecao de dificuldade)"});
}

void TelaMenu::exibirTutorialDeParry(const std::string& infoBox) {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    exibirPainelLogoJogo("TUTORIAL DE PARRY", false);
    if (infoBox.empty()) std::cout << "\n";
    else exibirInfoBox(infoBox);

    std::vector<std::string> explicacao = {
        "═══ COMO FUNCIONA O PARRY ═══",
        "Quando um inimigo atacar, voce deve estar pronto para reagir.",
        "Uma sequencia de numeros aparecera na tela com um limite de tempo.",
        "Digite os numeros rapidamente na sequencia correta e pressione ENTER.",
        "Se for rapido o suficiente e nao errar, o dano sera reduzido ou totalmente anulado!",
    };

    std::vector<std::string> opcoesTutorial = {"INICIAR TESTE", "PULAR"};
    
    Aparencia::imprimirBlocoCentralizado(explicacao);
    std::cout << "\n";
    
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
        exibirPainelLogoJogo("TUTORIAL DE PARRY - " + niveis[i].nomeInimigo);
        
        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);
        
        Aparencia::imprimirCentralizado("Inimigo: " + niveis[i].nomeInimigo);
        Aparencia::imprimirCentralizado("Sequencia: " + std::to_string(niveis[i].digitos) + " digitos | Tempo limite: " + std::to_string(niveis[i].tempoLimiteMs / 1000) + "s");
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

    std::cout << "\033[?25l";
    Aparencia::limparTela();
    exibirPainelLogoJogo("TUTORIAL CONCLUIDO");
    if (infoBox.empty()) std::cout << "\n";
    else exibirInfoBox(infoBox);
    
    Aparencia::imprimirCentralizado("Voce completou o tutorial de Parry!");
        
    ControleDeInput::aguardarEnter();
}

void TelaMenu::exibirIntroducaoJornada(const std::string& infoBox) {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    exibirPainelLogoJogo("SISTEMA DE SAVE", false);
        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);
        std::vector<std::string> avisoSave = {
            "═══ AVISO IMPORTANTE ═══",
            "O jogo NAO possui salvamento automatico (auto-save).",
            "Para salvar o seu progresso, abra a sua Ficha de Jogador",
            "durante a exploracao ou durante uma batalha,",
            "e escolha a opcao de Voltar ao Menu Principal."
        };
        
        Aparencia::imprimirBlocoCentralizado(avisoSave);

    ControleDeInput::aguardarEnter();
}
