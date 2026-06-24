#pragma once

#include "../../Personagem.h"
#include <string>
#include <vector>
#include "../InteracaoNPC.h"

class NPCMorgana : public InteracaoNPC
{
public:
    void interagir(Personagem* jogador);

protected:
    // INFORMACOES DO LUGAR E APARENCIA
    std::string obterNomeDoLugar() const override;
    Cor obterCorDoCabecalho() const override;
    Cor obterCorDaArte() const override;
    const std::vector<std::string>& obterArteASCII() const override;

    // INTERACAO E MENU
    void exibirDialogo(Personagem* jogador) override;
    std::vector<std::string> obterOpcoesMenu(Personagem* jogador, int larguraDoTerminal) override;
    void processarOpcao(Personagem* jogador, const std::string& opcao, int larguraDoTerminal) override;
};
