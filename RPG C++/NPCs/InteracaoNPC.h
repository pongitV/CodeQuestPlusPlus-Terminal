#pragma once

#include <string>
#include <vector>
#include "../Utilidades/Aparencia.h"
#include "../Inventario/Item.h"

class SistemaPersonagem;

class InteracaoNPC {
public:
    virtual ~InteracaoNPC() = default;

    virtual std::string obterNomeDoLugar() const = 0;
    virtual Cor obterCorDoCabecalho() const = 0;
    virtual void exibirDialogo(SistemaPersonagem* jogadorAtual) = 0;
    virtual std::vector<std::string> obterOpcoesMenu(SistemaPersonagem* jogadorAtual, int larguraDoTerminal) = 0;
    virtual void processarOpcao(SistemaPersonagem* jogadorAtual, const std::string& opcao, int larguraDoTerminal) = 0;
    virtual Cor obterCorDaArte() const = 0;
    virtual const std::vector<std::string>& obterArteASCII() const = 0;

    void interagir(SistemaPersonagem* jogadorAtual);

    static void processarMenuMissoesVazio(SistemaPersonagem* jogadorAtual, const std::string& tituloMenu, Cor corCabecalho, const std::string& nomeNPC, const std::string& falaVazia);
    static bool verificarMaterialNoInventario(SistemaPersonagem* jogadorAtual, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Cor corNPC, const std::string& mensagemPersonalizada = "");
    static bool verificarItemNaoEquipado(SistemaPersonagem* jogadorAtual, Item* itemAvaliado, const std::string& nomeNPC, Cor corNPC, const std::string& msgErro);
    static Item* lerItemDoInventario(SistemaPersonagem* jogadorAtual, const std::string& mensagemDialogo, const std::string& nomeNPC, Cor corNPC, std::string& codigoSaida, bool exibirPrecos = false);
    static void exibirTelaDeSucesso(const std::string& tituloCabecalho, Cor corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC);
    static std::string obterFormatadorStatusItem(ItemID id);
};
