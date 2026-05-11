#pragma once
#include <map>
#include <string>
#include <functional>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/Item.h"
#include "../Utilidades/Aparencia.h"

struct ProdutoLoja {
    ItemID idItem;
    int preco;
    int quantidade; // -1 para infinito
};

class GerenciadorLoja {
public:
    static void processarCompra(SistemaPersonagem* jogadorAtual, const std::string& tituloLoja, Cor corLoja, 
                                std::map<int, ProdutoLoja>& estoqueAtual, 
                                const std::function<void(const std::string&)>& exibirDialogoNPC, 
                                const std::function<std::string(ItemID)>& formatadorNomeExtra = nullptr);
};
