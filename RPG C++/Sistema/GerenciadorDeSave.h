#pragma once
#include <memory>

class Personagem;

class GerenciadorDeSave {
public:
    // Verifica se o arquivo de progresso local existe
    static bool saveExiste();
    // Serializa todos os dados e mochila do jogador em um .txt
    static void salvarJogo(Personagem* jogador);
    // Desserializa os dados reconstruindo o jogador e os itens via Fabrica
    static std::unique_ptr<Personagem> carregarJogo();
};