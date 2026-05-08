#pragma once

#include <string>
#include <vector>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"

class InteracaoNPC {
public:
    virtual ~InteracaoNPC() = default;

    // INTERACAO PRINCIPAL
    void interagir(SistemaPersonagem* jogadorAtual);

protected:
    // INFORMACOES DO LUGAR E APARENCIA
    virtual std::string obterNomeDoLugar() const = 0;
    virtual Cor obterCorDoCabecalho() const = 0;
    virtual Cor obterCorDaArte() const = 0;
    virtual const std::vector<std::string>& obterArteASCII() const = 0;

    // INTERACAO E MENU
    virtual void exibirDialogo(SistemaPersonagem* jogador) = 0;
    virtual std::vector<std::string> obterOpcoesMenu(SistemaPersonagem* jogador, int larguraDoTerminal) = 0;
    virtual void processarOpcao(SistemaPersonagem* jogador, const std::string& opcao, int larguraDoTerminal) = 0;
};
