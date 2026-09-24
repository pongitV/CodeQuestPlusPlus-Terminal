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

    // [PT-BR] Inicia o laco principal em 3D, capturando inputs e renderizando quadros
    // [EN-US] Starts main 3D loop, capturing input and rendering frames
    static char start3DExploration(const std::vector<std::string>& mapMatrix, float& playerX, float& playerY, float& viewAngle, const std::string& titleMap, Character* player, int& outHitX, int& outHitY, int typeAnimationEntry = 0);
    
    // [PT-BR] Pisca a tela inteira com a cor fornecida por um curto periodo de tempo
    // [EN-US] Blinks entire screen with given color for a brief duration
    static void blinkScreenColor(Color color, int durationMs);
    
    // [PT-BR] Gera um unico quadro estatico do ambiente 3D
    // [EN-US] Renders a single static frame of the 3D environment
    static std::vector<std::string> drawFrameStatic3D(const std::vector<std::string>& mapMatrix, float playerX, float playerY, float viewAngle, const std::string& titleMap, Character* player, int heightOverride = -1);
};
