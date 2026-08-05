#pragma once

#include <vector>
#include <string>
#include <tuple>
#include "Domain/Characters/Character.h"

class RaycasterRendererCombat {
public:
    // Renderiza o fundo 3D estatico e sobrepoe a arte do inimigo
    // Retorna o quadro renderizado na resolucao da tela para ser exibido acima do HUD do combate.
    static std::vector<std::string> renderFrame(
        const std::string& titleMap, 
        Character* player, 
        const std::vector<Character*>& enemies,
        Character* targetAnimation = nullptr,
        int frame = 0,
        int framesDeDamagePlayer = 0,
        int damageAmount = -1,
        bool isCure = false,
        int timeMs = 0,
        bool isDeath = false,
        const std::vector<std::string>& dropsAnimation = {},
        float spriteOpacity = 1.0f
    );

    // Gera uma mini-arena baseada no titulo do mapa (bioma)
    static std::vector<std::string> getArenaByTitle(const std::string& title);

    // Retorna o fundo 3D que foi gerado na ultima renderizacao do combate
    static const std::vector<std::string>& getLastBackgroundRendered();

    // Retorna a cor RGB base para texturizar o sprite do inimigo no 3D
    static std::tuple<int,int,int> getColorSpriteEnemy(Character* enemy);

    // Pinta uma string de texto sobre o buffer 1D da tela 3D (overlay)
    static void paintTextNoBuffer(std::vector<std::string>& screen, int widthScreen, int heightMax, int postX, int postY, const std::string& text, const std::string& colorFg, const std::string& colorBgOverride = "");

private:
    // Pega as linhas de arte ASCII do monstro e pinta por cima das strings renderizadas do motor 3D
    static void superimposeSprite(
        std::vector<std::string>& screen, 
        Character* enemy, 
        int enemyIdx,
        int totalEnemies,
        int widthScreen, 
        int heightScreen, 
        int flashDamageEnemy, 
        int damageAmount, 
        bool isCure, 
        int timeMs, 
        bool isDeath = false, 
        int frameDeath = 0, 
        const std::vector<std::string>& dropsAnimation = {}, 
        bool isSelected = false,
        float spriteOpacity = 1.0f
    );
};
