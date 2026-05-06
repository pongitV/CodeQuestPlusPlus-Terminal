#include "ItemMaterial.h"
#include <string_view>
#include <map>
#include <iostream>
#include "../Utilidades/SimplificacoesAparencia.h"
#include <vector>
#include <functional>
#include <unordered_map>

ItemMaterial::ItemMaterial(std::string nome, int preco) : Item(preco), nome(nome)
{
}

std::string ItemMaterial::obterNomeItem() const { return nome; }
TipoEquipamento ItemMaterial::obterTipo() const { return TipoEquipamento::MATERIAL; }

void ItemMaterial::exibirInspecao() const {
    std::vector<std::string> linhas;
    linhas.push_back(SimplificacoesAparencia::cor(Cor::CIANO) + " === " + nome + " ===" + SimplificacoesAparencia::cor(Cor::RESET));
    linhas.push_back(" > Tipo: Material");
    linhas.push_back(" > Preco de Venda: " + std::to_string(precoVenda) + "G");
    
    std::string utilidade = "Pode ser util para construcoes ou rituais.";
    if (nome == "Dente de goblin") utilidade = "Pode ser usado na Cabana da Bruxa para encantar armas com Sangramento (Requer 40x).";
    else if (nome == "Nucleo pegajoso") utilidade = "Pode ser usado na Cabana da Bruxa para encantar armas com Lentidao (Requer 5x).";
    else if (nome == "Po magico") utilidade = "Pode ser usado na Cabana da Bruxa para encantar armas com Quebra de Resistencia Permanente (Requer 25x).";
    else if (nome == "Madeira enfeiticada") utilidade = "Pode ser usada na Cabana da Bruxa para encantar o Arco ou o Violao (Requer 1x).";
    else if (nome == "Coracao da floresta") utilidade = "Usado na Cabana da Bruxa para encantar o Cajado ou para desbloquear a passagem do labirinto (Requer 3x).";
    else if (nome == "Pedra magica de upgrade") utilidade = "Uma pedra extremamente rara. Pode ser usada na Forja de Bjorn para conceder +3 de Defesa (Resistencia) a uma armadura.";
    else if (nome == "Convite Real") utilidade = "Permite o acesso livre aos portoes do Castelo Real.";
    
    linhas.push_back(" > Descricao: " + utilidade);

    std::cout << "\n";
    SimplificacoesAparencia::imprimirCentralizado(linhas[0]);
    std::cout << "\n";
    
    std::vector<std::string> resto(linhas.begin() + 1, linhas.end());
    SimplificacoesAparencia::imprimirBlocoCentralizado(resto);
}

std::unique_ptr<Item> fabricarItemMaterial(const std::string& nome) {
    static const std::unordered_map<std::string, std::function<std::unique_ptr<Item>()>> construtores = {
        {"Gosma acida", []() { return std::make_unique<ItemMaterial>("Gosma acida", 5); }},
        {"Dente de goblin", []() { return std::make_unique<ItemMaterial>("Dente de goblin", 1); }},
        {"Nucleo pegajoso", []() { return std::make_unique<ItemMaterial>("Nucleo pegajoso", 30); }},
        {"Po magico", []() { return std::make_unique<ItemMaterial>("Po magico", 15); }},
        {"Madeira enfeiticada", []() { return std::make_unique<ItemMaterial>("Madeira enfeiticada", 3); }},
        {"Coracao da floresta", []() { return std::make_unique<ItemMaterial>("Coracao da floresta", 3); }},
        {"Pedra magica de upgrade", []() { return std::make_unique<ItemMaterial>("Pedra magica de upgrade", 3); }}
    };
    auto it = construtores.find(nome);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
