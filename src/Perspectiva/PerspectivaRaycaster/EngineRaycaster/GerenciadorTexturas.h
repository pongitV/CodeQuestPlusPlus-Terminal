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
    static CorRGB cache[256][16384];
    static float tabelaSin[4096];

    static int obterResolucao(TexID id) {
        switch (id) {
            case TexID::ChaoGramaFloresta:
            case TexID::ChaoGramaVila:
            case TexID::PedraSpawn:
            case TexID::CavernaCoracaoParede:
                return 128;
            case TexID::ArvoreFloresta:
            case TexID::ArvoreCoracao:
            case TexID::PedraVila:
            case TexID::PadraoEstrutura:
                return 32;
            default:
                return 64;
        }
    }

    static void gerar(TexID id);
};

inline CorRGB GerenciadorTexturas::obterCor(TexID id, int tx, int ty) {
    if (!inicializado) inicializar();
    int res = obterResolucao(id);
    if (res == 64) {
        tx /= 2;
        ty /= 2;
    } else if (res == 32) {
        tx /= 4;
        ty /= 4;
    }
    if (tx < 0) tx = 0; 
    if (tx >= res) tx = res - 1;
    if (ty < 0) ty = 0; 
    if (ty >= res) ty = res - 1;
    return cache[static_cast<int>(id)][ty * res + tx];
}
