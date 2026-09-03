#pragma once

#include <vector>
#include <string>
#include <tuple>

class Character;

class IDEInspector {
public:
    // [PT-BR] Gera o bloco de código C++ representando a struct/instância do Player
    static std::vector<std::string> inspectPlayer(Character* player, int posX, int posY);

    // [PT-BR] Identifica e gera a definição de classe C++ do monstro ou entidade mais próxima
    static std::vector<std::string> inspectNearestEntity(
        const std::vector<std::string>& mapMatrix,
        int playerX,
        int playerY,
        const std::string& mapTitle
    );

    // [PT-BR] Retorna o nome amigável de uma entidade pelo seu char
    static std::string getEntityTypeName(char c, const std::string& mapTitle);
};
