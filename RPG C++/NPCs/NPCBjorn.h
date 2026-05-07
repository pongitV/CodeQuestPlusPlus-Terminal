#pragma once

#include "../Sistemas/SistemaPersonagem.h"
#include "InteracaoNPC.h"
#include <string>
#include <vector>

class NPCBjorn : public InteracaoNPC
{
public:

protected:
    std::string obterNomeDoLugar() const override;
    Cor obterCorDoCabecalho() const override;
    Cor obterCorDaArte() const override;
    const std::vector<std::string>& obterArteASCII() const override;
    void exibirDialogo(SistemaPersonagem* jogador) override;
    std::vector<std::string> obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) override;
    void processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) override;
};
