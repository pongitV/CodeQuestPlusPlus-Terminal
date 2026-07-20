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
    Cavaleiro,
    ReinoMadeira,
    IgrejaAltar,
    IgrejaParede,
    IgrejaParedeAltar,
    IgrejaTeto,
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
    static CorRGB cache[256][16384];
    static float tabelaSin[4096];

    static void gerar(TexID id);
};

inline CorRGB GerenciadorTexturas::obterCor(TexID id, int tx, int ty) {
    if (!inicializado) inicializar();
    int res = 128;
    if (tx < 0) tx = 0; 
    if (tx >= res) tx = res - 1;
    if (ty < 0) ty = 0; 
    if (ty >= res) ty = res - 1;
    return cache[static_cast<int>(id)][ty * res + tx];
}
