#include "NPCPadre.h"
#include "NPCPadreLayout.h"
#include "../../../Core/Controladores/MenuJogo.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include <iostream>

void NPCPadre::interagir(Personagem* jogador) {
    InteracaoNPC::interagir(jogador);
}

std::string NPCPadre::obterNomeDoLugar() const {
    return "ALTAR DA IGREJA";
}

Cor NPCPadre::obterCorDoCabecalho() const {
    return Cor::CIANO;
}

Cor NPCPadre::obterCorDaArte() const {
    return Cor::CIANO;
}

const std::vector<std::string>& NPCPadre::obterArteASCII() const {
    return NPCPadreLayouts::artePadre;
}

void NPCPadre::exibirDialogo(Personagem* jogador) {
    std::vector<std::string> linhas = {
        "Que a paz e a luz dos deuses guiem seus passos, meu filho.",
        "Nesta sagrada igreja do reino, oferecemos refúgio e cura para as almas fatigadas.",
        "Sinto uma aura extremamente sombria emanando do palácio real no norte...",
        "Prepare-se bem antes de desafiar o que quer que resida lá."
    };
    Aparencia::exibirPopup("PADRE BENEDITO", linhas, Cor::CIANO, obterArteASCII());
}

std::vector<std::string> NPCPadre::obterOpcoesMenu(Personagem* jogador, int /*larguraDoTerminal*/) {
    return {
        "Pedir Bencao (Restaurar HP)",
        "Conversar sobre o Palacio",
        "Voltar"
    };
}

void NPCPadre::processarOpcao(Personagem* jogador, const std::string& opcao, int /*larguraDoTerminal*/) {
    if (opcao == "Pedir Bencao (Restaurar HP)") {
        if (jogador->obterVida() >= jogador->obterVidaMaxima()) {
            Aparencia::exibirPopup("PADRE BENEDITO", {"Sua saúde já está plena! Guarde a bênção para quando precisar."}, Cor::CIANO, obterArteASCII());
        } else {
            jogador->modificarVida(jogador->obterVidaMaxima());
            Aparencia::exibirPopup("BENCAO CONCEDIDA", {"O Padre recita preces sagradas. Uma luz quente envolve seu corpo!", "Seu HP foi totalmente restaurado!"}, Cor::VERDE_CLARO, obterArteASCII());
        }
    }
    else if (opcao == "Conversar sobre o Palacio") {
        std::vector<std::string> lore = {
            "O palácio real costumava ser o farol de esperança do Reino.",
            "Contudo, há algumas semanas, o rei trancou-se em seus aposentos",
            "e ordenou que guardas mágicos selassem a entrada.",
            "Ninguém entra ou sai. Barulhos aterrorizantes são ouvidos à noite.",
            "Temo que o pior tenha acontecido com a realeza..."
        };
        Aparencia::exibirPopup("LORE DO PALACIO", lore, Cor::CIANO, obterArteASCII());
    }
}
