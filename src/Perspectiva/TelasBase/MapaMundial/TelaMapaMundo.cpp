#include "TelaMapaMundo.h"
#include "TelaMapaMundoLayout.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"

ProximaTransicaoMapa TelaMapaMundo::exibir(Personagem* jogadorAtual, LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino) {
    (void)jogadorAtual;
    (void)progressoVila;
    (void)progressoFloresta;
    (void)progressoPonteReino;
    (void)progressoReino;

    auto locais = ArtesMapaMundo::obterLocais();
    std::vector<std::string> lugares;

    for (const auto& l : locais) {
        if (l.flag == nullptr) {
            lugares.push_back(l.nomeExibicao);
        } else if (Progressao::instancia().obterFlag(l.flag)) {
            lugares.push_back(l.nomeExibicao);
        }
    }

    std::string nomeLocalAtual;
    switch (localAtual) {
        case LocalizacaoMapa::VilaInicial: nomeLocalAtual = "Vila Inicial"; break;
        case LocalizacaoMapa::Floresta:   nomeLocalAtual = "Floresta Sombria"; break;
        case LocalizacaoMapa::PonteReino: nomeLocalAtual = "Ponte do Reino"; break;
        case LocalizacaoMapa::Reino:      nomeLocalAtual = "Reino Distante"; break;
    }

    int sel = 0;
    bool executando = true;
    bool redesenhoCompleto = true;

    while (executando) {
        GerenciadorPerspectiva::obterMapaMundoUI().renderizarPopup(ArtesMapaMundo::planetaTerra, lugares, sel, redesenhoCompleto);

        redesenhoCompleto = false;
        char tecla = ControleDeInput::lerTecla();
        int totalOp = 1 + (int)lugares.size();

        if (tecla == 'w' || tecla == 'W') {
            sel--;
            if (sel < 0) sel = totalOp - 1;
        } else if (tecla == 's' || tecla == 'S') {
            sel++;
            if (sel >= totalOp) sel = 0;
        } else if (tecla == '\n' || tecla == '\r') {
            redesenhoCompleto = true;
            if (sel == 0) {
                executando = false;
            } else {
                std::string& nomeSel = lugares[sel - 1];
                if (nomeSel == nomeLocalAtual)
                    executando = false;
                else if (nomeSel.find("Vila") != std::string::npos)
                    { executando = false; return ProximaTransicaoMapa::Vila; }
                else if (nomeSel.find("Floresta") != std::string::npos)
                    { executando = false; return ProximaTransicaoMapa::Floresta; }
                else if (nomeSel.find("Ponte") != std::string::npos)
                    { executando = false; return ProximaTransicaoMapa::PonteReino; }
                else if (nomeSel.find("Reino") != std::string::npos)
                    { executando = false; return ProximaTransicaoMapa::Reino; }
            }
        }
    }
    return ProximaTransicaoMapa::Nenhuma;
}
