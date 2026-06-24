#include "TelaBase.h"
#include "../../Core/Utilidades/ControleDeInput.h"
#include "../../Entidades/Personagem.h"
#include "../../Entidades/Racas/RacaBase.h"
#include "../../Entidades/Classes/ClasseBase.h"
#include "../../Sistemas/Inventario/Inventario.h"
#include <iostream>

void TelaBase::executarLoop(
    const std::function<void(bool)>& renderCabecalho,
    const std::function<void()>& renderConteudo,
    const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
    const std::function<bool(int)>& processarEscolha,
    bool centralizarMenu,
    const std::string& margemMenu)
{
    bool primeiraVez = true;
    while (true) {
        Aparencia::limparTela();
        
        if (renderCabecalho) {
            renderCabecalho(primeiraVez);
            primeiraVez = false;
        }

        if (renderConteudo) {
            renderConteudo();
        }

        std::vector<std::string> opcoes = construtorOpcoesMenu();
        std::cout << "\n";
        int escolha = ControleDeInput::lerSelecaoMenuComSetas(opcoes, centralizarMenu, margemMenu);
        
        if (!processarEscolha(escolha)) {
            break;
        }
    }
}

void TelaBase::executarLoopPadrao(
    const std::string& titulo,
    Cor corTema,
    const std::function<void()>& renderConteudo,
    const std::function<std::vector<std::string>()>& construtorOpcoesMenu,
    const std::function<bool(int)>& processarEscolha)
{
    executarLoop(
        [titulo, corTema](bool animar) { Aparencia::exibirPainelTexto(titulo, corTema, animar); },
        renderConteudo,
        construtorOpcoesMenu,
        processarEscolha
    );
}

bool TelaBase::deveAnimarEntradaDaTela(std::chrono::steady_clock::time_point& ultimoAcesso, int delayMilissegundos) {
    auto agora = std::chrono::steady_clock::now();
    bool animar = std::chrono::duration_cast<std::chrono::milliseconds>(agora - ultimoAcesso).count() > delayMilissegundos;
    ultimoAcesso = agora;
    return animar;
}