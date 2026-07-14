#include "Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <thread>
#include <chrono>

void Aparencia::animarFadeIn(int framesTotais, int tempoPorFrameMs, const std::function<void(int frame, int intensidade)>& renderFrame) {
    for (int frame = 1; frame <= framesTotais; ++frame) {
        auto inicioFrame = std::chrono::steady_clock::now();

        int intensidade = (255 * frame) / framesTotais;
        renderFrame(frame, intensidade);

        auto fimFrame = std::chrono::steady_clock::now();
        auto duracaoFrame = std::chrono::duration_cast<std::chrono::milliseconds>(fimFrame - inicioFrame).count();
        int tempoEspera = std::max(0, tempoPorFrameMs - static_cast<int>(duracaoFrame));
        for (int i = 0; i < tempoEspera; i += 10) {
            if (ControleDeInput::teclaPressionada()) {
                ControleDeInput::limparBuffer();
                int finalIntensidade = 255;
                renderFrame(framesTotais, finalIntensidade);
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(std::min(10, tempoEspera - i)));
        }
    }
}

void Aparencia::exibirTelaIntro(const std::vector<std::string>& arteLogo, const std::vector<std::string>& textoNarracao, Cor corTema) {
    int larguraConsole = obterLarguraTerminal();

    int larguraArte = 0;
    for (const auto& linha : arteLogo) {
        int comp = obterComprimentoVisual(linha);
        if (comp > larguraArte) larguraArte = comp;
    }
    int espacosArte = std::max(0, (larguraConsole - larguraArte) / 2);
    std::string margemArte(espacosArte, ' ');

    int linhasEmBrancoAntesDoTexto = 3;

    ocultarCursor();
    limparTela();

    animarFadeIn(30, 30, [&](int /*frame*/, int intensidade) {
        std::ostringstream buffer;
        std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
        
        std::string corRGB = obterCorRGBFade(corTema, intensidade);
        
        buffer << "\n\n";
        for (const auto& linha : arteLogo) {
            buffer << margemArte << corRGB << linha << "\033[0m\n";
        }
        
        buffer << "\033[J";
        
        std::cout.rdbuf(oldCout);
        std::cout << "\033[H" << buffer.str() << std::flush;
    });

    for(int i = 0; i < linhasEmBrancoAntesDoTexto; ++i) std::cout << "\n";

    imprimirBlocoCentralizadoDigitando(textoNarracao);
    
    std::cout << "\n";
    std::string pressEnter = "[ PRESSIONE ENTER PARA INICIAR ]";
    imprimirCentralizado(pressEnter, cor(Cor::CINZA));
    
    ControleDeInput::aguardarEnter();

    limparTela();
}

void Aparencia::imprimirBlocoCentralizadoDigitando(const std::vector<std::string>& linhas, int atrasoMs) {
    int tamanhoDaLinhaMaisLonga = 0;
    for (const std::string& linha : linhas) {
        size_t end = linha.find_last_not_of(' ');
        std::string trimmed = (end != std::string::npos) ? linha.substr(0, end + 1) : "";
        tamanhoDaLinhaMaisLonga = std::max(tamanhoDaLinhaMaisLonga, obterComprimentoVisual(trimmed));
    }
    std::string margem = espacosParaCentralizar(tamanhoDaLinhaMaisLonga);
    for (const std::string& linha : linhas) {
        imprimirDigitando(margem + linha, atrasoMs, true);
    }
}

void Aparencia::imprimirCentralizadoDigitando(const std::string& texto, int atrasoMs) {
    std::string margem = espacosParaCentralizar(obterComprimentoVisual(texto));
    imprimirDigitando(margem + texto, atrasoMs, true);
}

void Aparencia::imprimirDigitando(const std::string& texto, int atrasoMs, bool addNewline) {
    int linhaMsg = Aparencia::obterAlturaTerminal() - 1;
    if (linhaMsg < 1) linhaMsg = 1;
    std::cout << "\033[s\033[" << linhaMsg << ";1H" << cor(Cor::NEGRITO, Cor::CINZA) << "[Pressione ENTER para pular]" << cor(Cor::RESET) << "\033[u";

    size_t i = 0;
    while (i < texto.length() && texto[i] == ' ') {
        std::cout << texto[i];
        i++;
    }
    std::cout << std::flush;

    for (; i < texto.length(); ++i) {
        if (texto[i] == '\033') {
            size_t startAnsi = i;
            if (i + 1 < texto.length() && texto[i+1] == '[') {
                i += 2;
                while (i < texto.length() && !(texto[i] >= 0x40 && texto[i] <= 0x7E)) {
                    i++;
                }
                std::cout << texto.substr(startAnsi, i - startAnsi + 1) << std::flush;
                continue;
            }
        }

        if (ControleDeInput::teclaPressionada()) { 
            char tecla = ControleDeInput::lerTecla(); 
            if (tecla == '\r' || tecla == '\n') { 
                std::cout << "\033[s\033[" << linhaMsg << ";1H\033[K\033[u" << texto.substr(i) << std::flush; 
                if (addNewline) {
                    std::cout << std::endl;
                }
                return;
            } 
        }
        std::cout << texto[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(atrasoMs));
    }

    std::cout << "\033[s\033[" << linhaMsg << ";1H\033[K\033[u" << std::flush;

    if (addNewline) {
        std::cout << std::endl;
    }
}

void Aparencia::imprimirVetorAnimado(const std::vector<std::string>& linhas, int atrasoMs) {
    for (const std::string& linha : linhas) {
        std::cout << linha << "\n";
        std::cout << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(atrasoMs));
    }
}

void Aparencia::animarTransicaoCena3D(
    const std::vector<std::string>& logoBase,
    const std::string& corFinalLogo,
    const std::vector<std::string>& fundoLivre,
    std::function<std::vector<std::string>(float)> getFundoFinal,
    int targetY
) {
    int larguraTerminal = obterLarguraTerminal();
    int altura3D = static_cast<int>(fundoLivre.size());
    int logoHeight = static_cast<int>(logoBase.size());
    
    int logoWidth = 0;
    for(const auto& l : logoBase) {
        logoWidth = std::max(logoWidth, obterComprimentoVisual(l));
    }
    int logoX = (larguraTerminal - logoWidth) / 2;
    if (logoX < 0) logoX = 0;

    std::vector<std::vector<std::string>> decomposedLogo(logoHeight);
    for (int i = 0; i < logoHeight; i++) {
        const std::string& logoRow = logoBase[i];
        for (size_t j = 0; j < logoRow.length(); ) {
            int len = 1;
            unsigned char c = logoRow[j];
            if ((c & 0x80) == 0) len = 1;
            else if ((c & 0xE0) == 0xC0) len = 2;
            else if ((c & 0xF0) == 0xE0) len = 3;
            else if ((c & 0xF8) == 0xF0) len = 4;
            decomposedLogo[i].push_back(logoRow.substr(j, len));
            j += len;
        }
    }

    std::vector<std::string> cenaBase = getFundoFinal(1.0f);
    {
        std::ostringstream buffer;
        buffer << "\033[H";
        for (int y = 0; y < altura3D; y++) {
            buffer << cenaBase[y];
            if (y < altura3D - 1) buffer << "\n";
        }
        std::cout << buffer.str() << std::flush;
    }

    Aparencia::animarFadeIn(30, 40, [&](int frame, int /*intensidade*/) {
        float opacity = frame / 30.0f;
        
        std::string fadedCorLogo = corFinalLogo;
        if (corFinalLogo == "\033[1;31m") {
            fadedCorLogo = "\033[38;2;" + std::to_string((int)(255 * opacity)) + ";0;0m";
        } else if (corFinalLogo == "\033[1;32m") {
            fadedCorLogo = "\033[38;2;0;" + std::to_string((int)(255 * opacity)) + ";0m";
        } else if (corFinalLogo == "\033[1;37m" || corFinalLogo == "\033[37m") {
            int c = (int)(255 * opacity);
            fadedCorLogo = "\033[38;2;" + std::to_string(c) + ";" + std::to_string(c) + ";" + std::to_string(c) + "m";
        }
        
        std::ostringstream buffer;
        for (int y = targetY; y < targetY + logoHeight && y < altura3D; y++) {
            int logoRowIdx = y - targetY;
            std::string linhaLogo = sobreporLogoAnsi(cenaBase[y], decomposedLogo[logoRowIdx], logoX, fadedCorLogo, larguraTerminal);
            buffer << "\033[" << (y + 1) << ";1H" << linhaLogo << "\033[K";
        }
        std::cout << buffer.str() << std::flush;
    });

}
