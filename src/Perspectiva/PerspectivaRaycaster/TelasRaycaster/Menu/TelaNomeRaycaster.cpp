#include "TelaNomeRaycaster.h"
#include "../Utils/MenuRaycasterLayout.h"
#include "../Utils/MenuRaycasterUtils.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#include "../../../TelasBase/Menu/TelaBaseMenu.h"
#include "../../../../Core/Utilidades/FuncoesDialogo.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"

TelaNome::Resultado TelaNomeRaycaster::exibir() {
    std::cout << "\033[?25l";
    MenuRaycasterUtils::cachearFundo3D("Vila", nullptr);

    int largura = Aparencia::obterLarguraTerminal();
    std::string corTitulo = "\033[38;2;255;215;0m";
    std::string corNarrativa = "\033[38;2;180;180;255m";
    std::string nome;
    std::string mensagemErro = "";

    while (true) {
        MenuRaycasterUtils::incrementarCicloDia();
        std::ostringstream buffer;
        MenuRaycasterUtils::exibirFundo3D(buffer);

        int y = 3;
        int espTitulo = std::max(0, (largura - 17) / 2);
        int espNarrativa = std::max(0, (largura - 50) / 2);
        
        std::string promptStr = "> Digite o nome do seu personagem [0 para voltar]: ";
        int boxW = std::max(54, (int)promptStr.length() + (int)nome.length() + 5);
        int boxX = TelaBaseMenu::calcularOffsetCentral(boxW, largura);
        TelaBaseMenu::desenharCaixaPreta(buffer, 2, boxX, boxW, 8);

        MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, corTitulo + "O NOME DO DESTINO\033[0m", y++, espTitulo); y++;
        MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, corNarrativa + "O mundo clama por um novo destino...\033[0m", y++, espNarrativa);
        MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, corNarrativa + "E todas as lendas possuem um nome.\033[0m", y++, espNarrativa); y++;

        int yPrompt = y;
        int espPrompt = TelaBaseMenu::calcularOffsetCentral(promptStr + nome + "_", largura);
        MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, promptStr + "\033[38;2;100;255;100m" + nome + "\033[38;2;150;255;150m_\033[0m", yPrompt, espPrompt);
        
        if (!mensagemErro.empty()) {
            int espErr = TelaBaseMenu::calcularOffsetCentral(mensagemErro, largura);
            MenuRaycasterUtils::sobreporTextoAbsoluto(buffer, "\033[38;2;255;100;100m" + mensagemErro + "\033[0m", yPrompt + 2, espErr);
        }

        MenuRaycasterUtils::flushFrameParaConsole(buffer.str());

        if (!ControleDeInput::teclaPressionada()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }

        unsigned char tecla = static_cast<unsigned char>(ControleDeInput::lerTecla());
        
        if (tecla != 0) {
            mensagemErro = ""; 
        }

        if (tecla == '\r' || tecla == '\n') {
            if (nome == "0") {
                TelaNome::Resultado r;
                r.voltou = true;
                return r;
            }
            if (nome.empty() || nome.length() > 20) {
                mensagemErro = nome.empty() ? "Nome invalido! Nao pode ser vazio." : "Nome muito longo! Maximo 20 caracteres.";
                continue;
            }
            TelaNome::Resultado r;
            r.nome = nome;
            return r;
        }
        if ((tecla == 8 || tecla == 127) && !nome.empty()) nome.pop_back();
        if (tecla >= 32 && tecla <= 126 && nome.length() < 20) nome += static_cast<char>(tecla);
    }
}
