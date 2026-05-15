#pragma once

#include <memory>
#include <string>
#include <vector>

class SistemaPersonagem;

class SistemaSave {
public:
    // Verifica se existe algum arquivo de progresso local
    static bool saveExiste();
    // Retorna a lista de nomes dos arquivos de save disponiveis
    static std::vector<std::string> listarSaves();
    // Serializa todos os dados e mochila do jogador em um .txt com o nome do personagem
    static void salvarJogo(SistemaPersonagem* jogador);
    // Desserializa os dados reconstruindo o jogador e os itens via Fabrica
    static std::unique_ptr<SistemaPersonagem> carregarJogo(const std::string& nomeArquivo);
    // Deleta o arquivo de save do diretorio local
    static void deletarSave(const std::string& nomeArquivo);
};