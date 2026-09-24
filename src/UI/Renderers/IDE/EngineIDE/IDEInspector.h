#pragma once

#include <vector>
#include <string>
#include <tuple>

class Character;

class IDEInspector {
public:
    // [PT-BR] Gera o bloco de código C++ representando a struct/instância do Player
    // [EN-US] Generates C++ code block representing Player struct/instance
    static std::vector<std::string> inspectPlayer(Character* player, int posX, int posY);

    // [PT-BR] Identifica e gera a definição de classe C++ do monstro ou entidade mais próxima
    // [EN-US] Identifies and generates C++ class definition for nearest monster or entity
    static std::vector<std::string> inspectNearestEntity(
        const std::vector<std::string>& mapMatrix,
        int playerX,
        int playerY,
        const std::string& mapTitle
    );

    // [PT-BR] Retorna o nome amigável de uma entidade pelo seu char
    // [EN-US] Returns friendly entity name from its map character
    static std::string getEntityTypeName(char c, const std::string& mapTitle);
};
