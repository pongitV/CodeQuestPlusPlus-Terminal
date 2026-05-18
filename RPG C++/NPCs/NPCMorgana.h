#pragma once

#include "../Sistemas/SistemaPersonagem.h"
#include <string>
#include <vector>
#include "InteracaoNPC.h"

class NPCMorgana : public InteracaoNPC
{
public:

protected:
    // INFORMACOES DO LUGAR E APARENCIA
    std::string obterNomeDoLugar() const override;
    Cor obterCorDoCabecalho() const override;
    Cor obterCorDaArte() const override;
    const std::vector<std::string>& obterArteASCII() const override;

    // INTERACAO E MENU
    void exibirDialogo(SistemaPersonagem* jogador) override;
    std::vector<std::string> obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) override;
    void processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) override;
};
