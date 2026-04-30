#include "SistemaSave.h"
#include "SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Inventario/Item.h"
#include "../Racas/Dwarf.h"
#include "../Racas/Elfo.h"
#include "../Racas/Humano.h"
#include "../Racas/Ork.h"
#include "../Classes/Arqueiro.h"
#include "../Classes/Bardo.h"
#include "../Classes/Guerreiro.h"
#include "../Classes/Mago.h"
#include "Utilidades/SimplificacoesAparencia.h"
#include "../Sistemas/SistemaBestiario.h"
#include <fstream>
#include <iostream>
#include <filesystem>

bool SistemaSave::saveExiste() {
    return !listarSaves().empty();
}

std::vector<std::string> SistemaSave::listarSaves() {
    std::vector<std::string> saves;
    for (const auto& entry : std::filesystem::directory_iterator(".")) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            // Verifica se começa com "save_" e termina com ".txt"
            if (filename.find("save_") == 0 && filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt") {
                saves.push_back(filename);
            }
        }
    }
    return saves;
}

void SistemaSave::salvarJogo(SistemaPersonagem* jogador) {
    if (!jogador) return;
    std::string nomeArquivo = "save_" + jogador->obterNome() + ".txt";
    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) return;

    arquivo << jogador->obterNome() << "\n";
    arquivo << jogador->obterRaca()->obterNomeRaca() << "\n";
    arquivo << jogador->obterNomeClasse() << "\n";
    
    arquivo << jogador->obterNivel() << " " << jogador->obterXpAtual() << " " << jogador->obterXpParaSubir() << "\n";
    arquivo << jogador->obterVida() << " " << jogador->obterInventario()->obterOuro() << " " << jogador->obterDificuldade() << " " << (jogador->obterLabirintoDesbloqueado() ? 1 : 0) << "\n";

    auto& attr = jogador->obterAtributosFinais();
    arquivo << attr.vida << " " << attr.forca << " " << attr.destreza << " " << attr.resistencia << " " << attr.constituicao << " " << attr.inteligencia << " " << attr.sabedoria << "\n";

    auto itens = jogador->obterInventario()->obterTodosOsItens();
    arquivo << itens.size() << "\n";
    for (Item* item : itens) {
        int equipSlot = 0;
        if (item == jogador->obterArma()) equipSlot = 1;
        else if (item == jogador->obterEscudo()) equipSlot = 2;
        else if (item == jogador->obterArmadura()) equipSlot = 3;
        arquivo << equipSlot << " " << item->obterNomeItem() << "\n";
    }

    SistemaBestiario::instancia().salvar(arquivo);
    arquivo.close();
}

std::unique_ptr<SistemaPersonagem> SistemaSave::carregarJogo(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) return nullptr;

    std::string nome, racaStr, classeStr;
    std::getline(arquivo, nome);
    std::getline(arquivo, racaStr);
    std::getline(arquivo, classeStr);

    int nivel, xpAtual, xpParaSubir, vida, ouro, dificuldade, labirinto;
    arquivo >> nivel >> xpAtual >> xpParaSubir >> vida >> ouro >> dificuldade >> labirinto;

    Atributos attr;
    arquivo >> attr.vida >> attr.forca >> attr.destreza >> attr.resistencia >> attr.constituicao >> attr.inteligencia >> attr.sabedoria;

    std::unique_ptr<RacaBase> raca;
    if (racaStr == "Dwarf") raca = std::make_unique<Dwarf>();
    else if (racaStr == "Elfo") raca = std::make_unique<Elfo>();
    else if (racaStr == "Humano") raca = std::make_unique<Humano>();
    else if (racaStr == "Ork") raca = std::make_unique<Ork>();

    std::unique_ptr<ClasseBase> classe;
    if (classeStr == "Arqueiro") classe = std::make_unique<Arqueiro>();
    else if (classeStr == "Bardo") classe = std::make_unique<Bardo>();
    else if (classeStr == "Guerreiro") classe = std::make_unique<Guerreiro>();
    else if (classeStr == "Mago") classe = std::make_unique<Mago>();

    if (!raca || !classe) return nullptr;

    auto jogador = std::make_unique<SistemaPersonagem>(nome, std::move(raca), std::move(classe));

    jogador->desequiparArma(); jogador->desequiparEscudo(); jogador->desequiparArmadura();
    auto itensPadrao = jogador->obterInventario()->obterTodosOsItens();
    for (Item* i : itensPadrao) jogador->obterInventario()->removerItem(i);

    jogador->definirNivel(nivel); jogador->definirXpAtual(xpAtual); jogador->definirXpParaSubir(xpParaSubir);
    jogador->obterAtributosFinais() = attr; jogador->definirVida(vida); jogador->definirDificuldade(dificuldade);
    if (labirinto == 1) jogador->desbloquearLabirinto();
    
    jogador->obterInventario()->adicionarOuro(-jogador->obterInventario()->obterOuro());
    jogador->obterInventario()->adicionarOuro(ouro);

    size_t qtdItens; arquivo >> qtdItens;
    std::string linhaLixo; std::getline(arquivo, linhaLixo); // Limpa o \n

    for (size_t i = 0; i < qtdItens; ++i) {
        int equipSlot; arquivo >> equipSlot;
        std::string nomeItem; std::getline(arquivo >> std::ws, nomeItem);
        auto novoItem = FabricaItens::criarItem(nomeItem);
        if (novoItem) {
            Item* ptr = novoItem.get();
            jogador->obterInventario()->adicionarItem(std::move(novoItem));
            if (equipSlot == 1) jogador->equiparItem(ptr);
            else if (equipSlot == 2) jogador->equiparItem(ptr);
            else if (equipSlot == 3) jogador->equiparItem(ptr);
        }
    }

    SistemaBestiario::instancia().carregar(arquivo);
    arquivo.close();
    return jogador;
}