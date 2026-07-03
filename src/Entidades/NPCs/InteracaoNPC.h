#pragma once

#include <string>
#include <vector>
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Sistemas/Inventario/Item.h"

class Personagem;

class InteracaoNPC {
public:
    virtual ~InteracaoNPC() = default;

    virtual std::string obterNomeDoLugar() const = 0;
    virtual Cor obterCorDoCabecalho() const = 0;
    virtual void exibirDialogo(Personagem* jogadorAtual) = 0;
    virtual std::vector<std::string> obterOpcoesMenu(Personagem* jogadorAtual, int larguraDoTerminal) = 0;
    virtual void processarOpcao(Personagem* jogadorAtual, const std::string& opcao, int larguraDoTerminal) = 0;
    virtual Cor obterCorDaArte() const = 0;
    virtual const std::vector<std::string>& obterArteASCII() const = 0;

    void interagir(Personagem* jogadorAtual);

    static void processarMenuMissoesVazio(Personagem* jogadorAtual, const std::string& tituloMenu, Cor corCabecalho, const std::string& nomeNPC, const std::string& falaVazia);
    static bool verificarMaterialNoInventario(Personagem* jogadorAtual, const std::string& nomeMaterial, int quantidadeNecessaria, const std::string& nomeNPC, Cor corNPC, const std::string& mensagemPersonalizada = "");
    static bool verificarItemNaoEquipado(Personagem* jogadorAtual, Item* itemAvaliado, const std::string& nomeNPC, Cor corNPC, const std::string& msgErro);
    static Item* lerItemDoInventario(Personagem* jogadorAtual, const std::string& mensagemDialogo, const std::string& nomeNPC, Cor corNPC, std::string& codigoSaida, bool exibirPrecos = false);
    static void exibirTelaDeSucesso(const std::string& tituloCabecalho, Cor corCabecalho, const std::string& equacao, const std::vector<std::string>& arteAscii, const std::string& nomeNPC, const std::string& falaNPC);
    static std::string obterFormatadorStatusItem(ItemID id);
};
