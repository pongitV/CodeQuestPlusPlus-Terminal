#pragma once
#include <cstdint>

struct CorRGB {
    uint8_t r, g, b;
};

enum class TexID {
    Nenhuma = 0,
    // Paredes
    LabirintoMadeira,
    LabirintoArcoPilar,
    LabirintoArcoFundo,
    MorganaMadeira,
    IgrejaVitral,
    PonteMadeira,
    Alquimista,
    EntradaIgreja,
    ManequimAnok,
    Franchesco,
    Bjorn,
    ReinoMadeira,
    IgrejaAltar,
    IgrejaParede,
    PatioMuro,
    FlorestaEstrutura,
    PadraoEstrutura,
    ArvoreCoracao,
    ArvoreFloresta,
    PedraVila,
    PedraSpawn,
    SalaChefeParede,
    CavernaCoracaoParede,
    ParedeInvalida,
    
    // Chaos e Tetos
    ChaoLabirintoBorda,
    ChaoLabirinto,
    ChaoSalaChefeFora,
    ChaoSalaChefeDentro,
    ChaoCoracaoMusgo,
    ChaoCoracaoTerra,
    ChaoCoracaoEscuro,
    ChaoGramaFloresta,
    ChaoGramaVila,
    ChaoTerra,
    ChaoPadrao,
    TetoIndoorsCoracaoMusgo,
    TetoIndoorsCoracaoMadeira,
    TetoIndoorsCoracaoEscuro,
    TetoIndoorsPadrao
};

class GerenciadorTexturas {
public:
    static void inicializar();
    static CorRGB obterCor(TexID id, int tx, int ty);
    
    // Lookup tables para otimização de funções trigonométricas
    static float fastSin(float angle);
    static float fastCos(float angle);

private:
    static bool inicializado;
    static CorRGB cache[256][4096];
    static float tabelaSin[4096];

    static void gerar(TexID id);
};

inline CorRGB GerenciadorTexturas::obterCor(TexID id, int tx, int ty) {
    if (tx < 0) tx = 0; 
    if (tx > 63) tx = 63;
    if (ty < 0) ty = 0; 
    if (ty > 63) ty = 63;
    return cache[static_cast<int>(id)][ty * 64 + tx];
}
