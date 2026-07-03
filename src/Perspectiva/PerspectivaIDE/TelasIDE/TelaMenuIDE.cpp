#include "TelaMenuIDE.h"

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
#include "../../TelasBase/Menu/TelaMenuLayout.h"

namespace {
    void exibirInfoBox(const std::string& infoBox) {
        if (!infoBox.empty()) {
            std::string cleanInfoBox = infoBox;
            size_t pos = 0;
            while ((pos = cleanInfoBox.find('|', pos)) != std::string::npos) {
                cleanInfoBox.replace(pos, 1, "║");
                pos += 3;
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
        TelaMenuIDE::exibirPainelLogoJogo(titulo, false);

        if (infoBox.empty()) std::cout << "\n";
        else exibirInfoBox(infoBox);

        Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
        std::cout << "\n";

        return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
    }

}

bool TelaMenuIDE::exibirConfirmacaoDeEscolhaComArteLadoALado(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha, const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir)
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

        TelaMenuIDE::exibirPainelLogoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);

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

    TelaMenuIDE::exibirPainelLogoJogo("PREVIA DA " + tipoDeEscolha + ": " + nomeDaEscolha);
    Aparencia::imprimirLadoALado(infoSegura, arteAsciiParaExibir, 40, 6, Cor::RESET, corArte, 0);

    std::cout.rdbuf(oldCoutFinal);
    std::cout << "\033[H" << bufferFinal.str() << "\033[J" << std::flush;

    std::string margem = std::string(recuoEsquerdo, ' ');
    std::cout << "\n";
    std::vector<std::string> opcoes = { "VOLTAR", "CONFIRMAR" };
    int opcaoDeConfirmacao = ControleDeInput::lerSelecaoMenuComSetas(opcoes, false, margem);

    return opcaoDeConfirmacao == 1;
}

void TelaMenuIDE::exibirPainelLogoJogo(const std::string& tituloDaTela, bool animarFadeIn)
{
    Aparencia::exibirPainel(tituloDaTela, Cor::BRANCO, ArtesMenu::logoTexto, 140, ArtesMenu::logoPlus, Cor::LARANJA, animarFadeIn);
}

std::vector<std::string> TelaMenuIDE::comporQuadroDeAtributos(const Atributos& stats, const std::string& tituloSecao, const std::string& tituloHabilidade, const std::string& nomeHab, const std::string& descHab, const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
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

int TelaMenuIDE::exibirOpcoesMenuPrincipal(bool temSave, const std::vector<std::vector<std::string>>& /*artesClasses*/, const std::vector<std::vector<std::string>>& /*artesRacas*/, const std::vector<std::vector<std::string>>& /*artesInimigos*/) {
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("MENU PRINCIPAL", false);

    std::vector<std::string> opcoes = { "Novo Jogo" };
    if (temSave) opcoes.push_back("Continuar Jogo");
    opcoes.push_back("Debug");
    opcoes.push_back("Opcoes de save");
    opcoes.push_back("Sair do jogo");

    std::cout << "\n";
    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

int TelaMenuIDE::exibirMenuCarregarJogo(const std::vector<std::string>& informacoesSaves) {
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("CARREGAR JOGO", false);

    std::vector<std::string> opcoes = informacoesSaves;
    opcoes.push_back("Voltar");

    std::cout << "\n";
    Aparencia::imprimirCentralizado("Selecione o save que deseja carregar:");
    std::cout << "\n";

    return ControleDeInput::lerSelecaoMenuComSetas(opcoes, true);
}

void TelaMenuIDE::exibirPromptNome() {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("INTRODUCAO AO RPG", false);
    std::cout << "\n";

    std::vector<std::string> narracao = {
        FuncoesDialogo::formatarMsgNarracao("O mundo clama por um novo destino..."),
        FuncoesDialogo::formatarMsgNarracao("E todas lendas possuem um nome.")
    };

    Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
    std::cout << "\n";
    Aparencia::exibirPrompt(" > Escolha o nome do seu personagem [0 para voltar]: ");
}

int TelaMenuIDE::exibirPromptRaca(const std::string& nome) {
    std::vector<std::string> opcoes = {"Dwarf", "Elfo", "Humano", "Ork"};
    Aparencia::ordenarAlfabeticamente(opcoes);
    opcoes.push_back("VOLTAR (selecao de nome)");
    return exibirPromptGenerico("SELECAO DE RACA", "| JOGADOR: " + nome + " |",
        {FuncoesDialogo::formatarMsgNarracao("Qual sua origem?")},
        opcoes);
}

int TelaMenuIDE::exibirPromptClasse(const std::string& nome, const std::string& nomeRaca) {
    std::vector<std::string> opcoes = {"Arqueiro", "Bardo", "Guerreiro", "Mago", "Necromante"};
    Aparencia::ordenarAlfabeticamente(opcoes);
    opcoes.push_back("VOLTAR (selecao de raca)");
    return exibirPromptGenerico("SELECAO DE CLASSE", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " |",
        {FuncoesDialogo::formatarMsgNarracao("Qual seu caminho?")},
        opcoes);
}

int TelaMenuIDE::exibirPromptDificuldade(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    int selecaoAtual = 0;
    int totalOpcoes = 4;
    std::string infoBox = "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |";
    std::vector<std::string> narracao = {FuncoesDialogo::formatarMsgSistema("Escolha o nivel de desafio da sua jornada:")};

    std::vector<std::string> opcoes = {
        Aparencia::cor(Cor::VERDE) + "FACIL" + Aparencia::cor(Cor::RESET) + "   (Inimigos com 1x Atributos, sem habilidades adicionais)      ",
        Aparencia::cor(Cor::AMARELO) + "NORMAL" + Aparencia::cor(Cor::RESET) + "  (Inimigos com 1.5x Atributos, com habilidades de raca)       ",
        Aparencia::cor(Cor::VERMELHO) + "DIFICIL" + Aparencia::cor(Cor::RESET) + " (Inimigos com 2x Atributos, com habilidades de raca e classe)",
        "VOLTAR (selecao de classe)                                           "
    };

    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("DIFICULDADE DO MUNDO", false);
    if (!infoBox.empty()) exibirInfoBox(infoBox);
    else std::cout << "\n";

    Aparencia::imprimirBlocoCentralizadoDigitando(narracao);
    std::cout << "\n";

    ControleDeInput::limparBuffer();

    while (true) {
        auto agora = std::chrono::steady_clock::now();
        int tempoMs = static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(agora.time_since_epoch()).count());
        std::string cursorIcon = ((tempoMs / 400) % 2 == 0) ? ">  " : " > ";

        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());

        buffer << "\033[H";
        TelaMenuIDE::exibirPainelLogoJogo("DIFICULDADE DO MUNDO", false);

        if (!infoBox.empty()) exibirInfoBox(infoBox);
        else buffer << "\n";

        Aparencia::imprimirBlocoCentralizado(narracao);
        buffer << "\n";

        int larguraTerminal = Aparencia::obterLarguraTerminal();

        std::vector<std::string> linhasOpcoes;
        for (int i = 0; i < totalOpcoes; ++i) {
            if (i == selecaoAtual) {
                linhasOpcoes.push_back(Aparencia::cor(Cor::VERDE) + cursorIcon + Aparencia::cor(Cor::RESET) + opcoes[i]);
            } else {
                linhasOpcoes.push_back("   " + opcoes[i]);
            }
        }

        std::vector<std::string> arte;
        if (selecaoAtual == 0) {
            for (const auto& linha : ArtesMenu::arteDificuldadeFacil) arte.push_back(Aparencia::cor(Cor::VERDE) + linha + Aparencia::cor(Cor::RESET));
        } else if (selecaoAtual == 1) {
            for (const auto& linha : ArtesMenu::arteDificuldadeMedia) arte.push_back(Aparencia::cor(Cor::AMARELO) + linha + Aparencia::cor(Cor::RESET));
        } else if (selecaoAtual == 2) {
            for (const auto& linha : ArtesMenu::arteDificuldadeDificil) arte.push_back(Aparencia::cor(Cor::VERMELHO) + linha + Aparencia::cor(Cor::RESET));
        }

        for (const auto& linha : linhasOpcoes) {
            int comp = Aparencia::obterComprimentoVisual(linha);
            int espacos = std::max(0, (larguraTerminal - comp) / 2);
            buffer << std::string(espacos, ' ') << linha << "\033[K\n";
        }

        buffer << "\033[K\n";

        for (const auto& linha : arte) {
            int comp = Aparencia::obterComprimentoVisual(linha);
            int espacos = std::max(0, (larguraTerminal - comp) / 2);
            buffer << std::string(espacos, ' ') << linha << "\033[K\n";
        }

        buffer << "\033[J";

        std::cout.rdbuf(oldCout);
        std::cout << buffer.str() << std::flush;

        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());

        if (tecla == 224 || tecla == 0 || tecla == '\033') {
            unsigned char proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == '[') proxTecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
            if (proxTecla == 72 || proxTecla == 'A') tecla = 'w';
            else if (proxTecla == 80 || proxTecla == 'B') tecla = 's';
            else if (proxTecla == 27) return totalOpcoes - 1;
        }

        if (tecla == 'w' || tecla == 'W') {
            selecaoAtual--;
            if (selecaoAtual < 0) selecaoAtual = totalOpcoes - 1;
        } else if (tecla == 's' || tecla == 'S') {
            selecaoAtual++;
            if (selecaoAtual >= totalOpcoes) selecaoAtual = 0;
        } else if (tecla == '\r' || tecla == '\n') {
            return selecaoAtual;
        }
    }
}

int TelaMenuIDE::exibirPromptParry(const std::string& nome, const std::string& nomeRaca, const std::string& nomeClasse) {
    return exibirPromptGenerico("CONFIGURACOES DO JOGO", "| JOGADOR: " + nome + " | RACA: " + nomeRaca + " | CLASSE: " + nomeClasse + " |",
        {
            FuncoesDialogo::formatarMsgSistema("Deseja ativar o sistema de PARRY?"),
            "(Permite reduzir danos ao digitar uma sequencia de numeros num tempo limite)",
            "(O tutorial esta disponivel apenas neste momento, mas voce pode ligar/desligar depois)"
        },
        {"Parry LIGADO (Tutorial disponivel)", "Parry DESLIGADO (Pula o tutorial)", "VOLTAR (selecao de dificuldade)"});
}

void TelaMenuIDE::exibirIntroducaoJornada(const std::string& infoBox) {
    std::cout << "\033[?25l";
    Aparencia::limparTela();
    TelaMenuIDE::exibirPainelLogoJogo("SISTEMA DE SAVE", false);
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
