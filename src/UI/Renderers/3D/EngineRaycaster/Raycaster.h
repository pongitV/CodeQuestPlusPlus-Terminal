/*
 * Arquivo: Raycaster.h
 * Proposito: Motor grafico 3D e renderizador do laco principal de exploracao.
 */

#pragma once

#include <vector>
#include <string>
#include "UI/Renderers/3D/EngineRaycaster/RaycasterFrame.h"
#include "Domain/Characters/Character.h"
#include "Core/Utils/Appearance.h"

/*
 * Responsavel pela visao em primeira pessoa (Raycasting) e animacoes de transicao.
 */
class Raycaster : public RaycasterFrame {
public:
    static float sensitivityX;
    static float sensitivityY;

    // Inicia o laco principal em 3D, capturando inputs e renderizando quadros
    static char start3DExploration(const std::vector<std::string>& mapMatrix, float& playerX, float& playerY, float& angleVisa, const std::string& titleMap, Character* player, int& outHitX, int& outHitY, int typeAnimationEntry = 0);
    
    // Pisca a tela inteira com a cor fornecida por um curto periodo de tempo
    static void blinkScreenColor(Color color, int durationMs);
    
    // Gera um unico quadro estatico do ambiente 3D
    static std::vector<std::string> drawFrameStatic3D(const std::vector<std::string>& mapMatrix, float playerX, float playerY, float angleVisa, const std::string& titleMap, Character* player, int heightOverride = -1);
};
