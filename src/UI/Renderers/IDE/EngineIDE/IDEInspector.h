#pragma once

#include <vector>
#include <string>
#include <tuple>

class Character;

class IDEInspector {
public:
    // Gera o bloco de codigo C++ representando a struct/instancia do Player
    static std::vector<std::string> inspectPlayer(Character* player, int posX, int posY);

    // Identifica e gera a definicao de classe C++ do monstro ou entidade mais proxima
    static std::vector<std::string> inspectNearestEntity(
        const std::vector<std::string>& mapMatrix,
        int playerX,
        int playerY,
        const std::string& mapTitle
    );

    // Retorna o nome amigavel de uma entidade pelo seu char
    static std::string getEntityTypeName(char c, const std::string& mapTitle);
};
