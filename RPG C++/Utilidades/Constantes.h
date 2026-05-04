#pragma once

namespace Constantes 
{
    // Evolucao e Experiencia
    constexpr int XP_BASE_PARA_SUBIR = 100;
    constexpr double MULTIPLICADOR_XP_POR_NIVEL = 1.5;
    constexpr double MAX_XP = 1000000000.0; // Evita integer overflow em niveis altos
    
    // Atributos de Nivel
    constexpr int GANHO_VIDA_POR_NIVEL = 20;
    constexpr int GANHO_ATRIBUTO_POR_NIVEL = 1;

    // Recompensas Padrao e Multiplicadores
    constexpr int OURO_RECOMPENSA_INICIAL = 15;
    constexpr double MULTIPLICADOR_CURA_BARDO = 1.4;
}