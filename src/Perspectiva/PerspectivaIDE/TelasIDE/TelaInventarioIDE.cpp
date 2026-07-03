#include "TelaInventarioIDE.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "../TemaIDE.h"
#include "../../TelasBase/TelaBase.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
#include "../../TelasBase/Inventario/TelaInventarioLayout.h"
#include "../../../Sistemas/Inventario/Item.h"

void TelaInventarioIDE::exibirCabecalhoInventario(bool animar) {
    Aparencia::exibirPainelArte(ArtesInventario::logoInventario, 121, Cor::AMARELO, "", animar);
}

void TelaInventarioIDE::exibirCaixaEquipados(Personagem* jogadorAtual) {
    Item* armaEquipada = jogadorAtual->obterArma();
    Item* escudoEquipado = jogadorAtual->obterEscudo();
    Item* armaduraEquipada = jogadorAtual->obterArmadura();
    Item* consumivelRapido = jogadorAtual->obterConsumivelRapido();

    std::vector<std::string> linhas;
    linhas.push_back("Arma     : " + (armaEquipada ? armaEquipada->obterNomeItem() + armaEquipada->obterInfoStatus() : "Nenhuma"));
    linhas.push_back("Escudo   : " + (escudoEquipado ? escudoEquipado->obterNomeItem() + escudoEquipado->obterInfoStatus() : "Nenhum"));
    linhas.push_back("Armadura : " + (armaduraEquipada ? armaduraEquipada->obterNomeItem() + armaduraEquipada->obterInfoStatus() : "Nenhuma"));
    linhas.push_back("Cura Rap.: " + (consumivelRapido ? consumivelRapido->obterNomeItem() + " (" + std::to_string(jogadorAtual->obterInventario()->contarItem(consumivelRapido->obterNomeItem())) + "x)" : "Vazio"));

    Aparencia::imprimirBlocoCentralizado(TelaBase::criarCaixa(linhas, "EQUIPAMENTOS ATUAIS", 40, Cor::AMARELO));
}

void TelaInventarioIDE::exibirInspecaoItem(Item* item, Personagem* jogadorAtual) {
    if (!item) return;

    std::vector<std::string> linhas;
    linhas.push_back(Aparencia::cor(Cor::AMARELO) + " ═══ " + item->obterNomeItem() + " ═══" + Aparencia::cor(Cor::RESET));

    std::vector<std::string> detalhes = item->obterDetalhesInspecao(jogadorAtual);
    linhas.insert(linhas.end(), detalhes.begin(), detalhes.end());

    linhas.push_back(" > Preco de Venda: " + std::to_string(item->obterPrecoVenda()) + "G");

    std::cout << "\n";
    Aparencia::imprimirCentralizado(linhas[0]);
    std::cout << "\n";

    std::vector<std::string> resto(linhas.begin() + 1, linhas.end());
    Aparencia::imprimirBlocoCentralizado(resto);
}
