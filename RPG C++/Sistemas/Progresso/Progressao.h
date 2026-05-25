#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>

class Personagem;

class Progressao {
private:
    std::unordered_map<std::string, bool> flags;
    mutable std::mutex mtx;

    Progressao();
public:
    static Progressao& instancia();
    
    void definirFlag(const std::string& chave, bool valor);
    bool obterFlag(const std::string& chave) const;

    // Cálculos dinâmicos de progresso combinando dados do jogador com as flags salvas
    int obterProgressoVila(Personagem* jogadorAtual) const;
    int obterProgressoFloresta(Personagem* jogadorAtual) const;
    int obterProgressoReino(Personagem* jogadorAtual) const;

    // Funções para ligar ao sistema de salvamento
    void salvar(std::ofstream& out) const;
    void carregar(std::ifstream& in);
};
