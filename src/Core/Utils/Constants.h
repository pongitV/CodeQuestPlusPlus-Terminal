#pragma once

namespace Constants 
{
    // [PT-BR] Evolucao e Experiencia
    // [EN-US] Leveling and Experience
    constexpr int BASE_XP_TO_LEVEL_UP = 100;
    constexpr double XP_MULTIPLIER_PER_LEVEL = 1.5;
    // [PT-BR] Evita integer overflow em niveis altos
    // [EN-US] Prevents integer overflow at high levels
    constexpr double MAX_XP = 1000000000.0;
    
    // [PT-BR] Atributos de Nivel
    // [EN-US] Level-up Attributes
    constexpr int HEALTH_GAIN_PER_LEVEL = 20;
    constexpr int ATTRIBUTE_GAIN_PER_LEVEL = 1;

    // [PT-BR] Recompensas Padrao e Multiplicadores
    // [EN-US] Standard Rewards and Multipliers
    constexpr int INITIAL_REWARD_GOLD = 15;
    constexpr double BARD_HEALING_MULTIPLIER = 1.4;
}
