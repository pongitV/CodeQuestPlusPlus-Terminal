#include "Progressao.h"
#include "../../Entidades/Personagem.h"
#include "ProgressaoFlags.h"

Progressao& Progressao::instancia() {
    static Progressao inst;
    return inst;
}

Progressao::Progressao() {}

void Progressao::definirFlag(const std::string& chave, bool valor) {
    std::lock_guard<std::mutex> lock(mtx);
    flags[chave] = valor;
}

bool Progressao::obterFlag(const std::string& chave) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = flags.find(chave);
    if (it != flags.end()) return it->second;
    return false;
}

int Progressao::obterProgressoVila(Personagem* jogadorAtual) const {
    if (!jogadorAtual) return 0;
    bool vilaNpcsEncontrados = jogadorAtual->obterNivel() > 1 || obterFlag(Flags::Vila_NPCs); 
    bool vilaInimigosDerrotados = jogadorAtual->obterXpAtual() > 0 || jogadorAtual->obterNivel() > 1 || obterFlag(Flags::Vila_Inimigos);
    bool vilaMissaoConcluida = obterFlag(Flags::Vila_ConviteReal) || jogadorAtual->obterInventario()->contarItem("Convite Real") > 0;
    return (vilaNpcsEncontrados ? 33 : 0) + (vilaInimigosDerrotados ? 33 : 0) + (vilaMissaoConcluida ? 34 : 0);
}

int Progressao::obterProgressoFloresta(Personagem* jogadorAtual) const {
    if (!jogadorAtual) return 0;
    bool florestaNpcsEncontrados = jogadorAtual->obterLabirintoDesbloqueado() || obterFlag(Flags::Floresta_NPCs);
    bool florestaInimigosDerrotados = obterFlag(Flags::Floresta_MahoragaDerrotado);
    bool florestaMissaoConcluida = obterFlag(Flags::Floresta_MissaoMorgana);
    return (florestaNpcsEncontrados ? 33 : 0) + (florestaInimigosDerrotados ? 33 : 0) + (florestaMissaoConcluida ? 34 : 0);
}

int Progressao::obterProgressoReino(Personagem* jogadorAtual) const {
    if (!jogadorAtual) return 0;
    bool trollDerrotado = obterFlag(Flags::Reino_TrollDerrotado);
    return (trollDerrotado ? 34 : 0) + (obterFlag(Flags::Reino_NPCs) ? 33 : 0) + (obterFlag(Flags::Reino_Inimigos) ? 33 : 0);
}

void Progressao::salvar(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    out << flags.size() << "\n";
    for (const auto& [chave, valor] : flags) out << chave << "\n" << (valor ? 1 : 0) << "\n";
}

void Progressao::carregar(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    flags.clear();
    size_t size;
    if (in >> size) {
        std::string lixo; std::getline(in, lixo); // consome a quebra de linha
        for (size_t i = 0; i < size; ++i) { std::string chave; std::getline(in, chave); int valor; in >> valor; std::getline(in, lixo); flags[chave] = (valor == 1); }
    }

    // --- RETROCOMPATIBILIDADE DE SAVES ANTIGOS ---
    // Evita que saves antigos (anteriores a atualizacao) percam o acesso a Viagem Rapida
    auto itFloresta = flags.find("Visitou_Floresta");
    auto itReino = flags.find("Visitou_Reino");
    if ((itFloresta != flags.end() && itFloresta->second) || 
        (itReino != flags.end() && itReino->second)) {
        flags["Mapas_Descobertos"] = true;
    }
}
