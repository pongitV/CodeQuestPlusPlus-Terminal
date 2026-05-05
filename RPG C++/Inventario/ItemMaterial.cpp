#include "ItemMaterial.h"
#include <string_view>
#include <map>
#include <iostream>
#include "../Utilidades/SimplificacoesAparencia.h"

ItemMaterial::ItemMaterial(std::string nome, int preco) : Item(preco), nome(nome)
{
}

std::string ItemMaterial::obterNomeItem() const { return nome; }
TipoEquipamento ItemMaterial::obterTipo() const { return TipoEquipamento::MATERIAL; }

void ItemMaterial::exibirInspecao() const {
    std::cout << "\n" << SimplificacoesAparencia::cor(Cor::CIANO) << " === " << nome << " ===" << SimplificacoesAparencia::cor(Cor::RESET) << "\n\n";
    std::cout << " > Tipo: Material\n";
    std::cout << " > Preco de Venda: " << precoVenda << "G\n";
    
    std::string utilidade = "Pode ser util para construcoes ou rituais.";
    if (nome == "Dente de goblin") utilidade = "Pode ser usado na Cabana da Bruxa para encantar armas com Sangramento (Requer 40x).";
    else if (nome == "Nucleo pegajoso") utilidade = "Pode ser usado na Cabana da Bruxa para encantar armas com Lentidao (Requer 5x).";
    else if (nome == "Po magico") utilidade = "Pode ser usado na Cabana da Bruxa para encantar armas com Quebra de Resistencia Permanente (Requer 25x).";
    else if (nome == "Madeira enfeiticada") utilidade = "Pode ser usada na Cabana da Bruxa para encantar o Arco ou o Violao (Requer 1x).";
    else if (nome == "Coracao da floresta") utilidade = "Usado na Cabana da Bruxa para encantar o Cajado ou para desbloquear a passagem do labirinto (Requer 3x).";
    else if (nome == "Pedra magica de upgrade") utilidade = "Uma pedra extremamente rara. Pode ser usada na Forja de Bjorn para conceder +3 de Defesa (Resistencia) a uma armadura.";
    else if (nome == "Convite Real") utilidade = "Permite o acesso livre aos portoes do Castelo Real.";
    
    std::cout << " > Descricao: " << utilidade << "\n";
}
