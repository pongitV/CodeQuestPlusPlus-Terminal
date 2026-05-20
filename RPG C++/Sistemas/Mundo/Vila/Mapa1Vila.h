#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

#include "../../../Entidades/Personagem.h"
#include "../MapaInteracao.h"

class Mapa1Vila final : public IMapa 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Personagem* jogadorAtual;
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;

    std::unordered_map<char, std::unique_ptr<InteracaoVila>> interacoes;

    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;

    std::vector<std::string> matrizDoMapaDaCavernaSalva;
    std::vector<std::string> matrizDoMapaDaLojaSalva;
    std::vector<std::string> matrizDoMapaDaForjaSalva;
    std::vector<std::string> matrizDoMapaDoSpawnSalva;
    std::vector<std::string> mapaBaseDaVila;

    // State moved from loop function
    bool bjornResgatado;
    bool forjaJaFoiVisitada;
    bool lojaJaFoiVisitada;
    bool cavernaJaFoiVisitada;
    bool spawnJaFoiVisitado;

    ProximaTransicaoMapa proximoMapa;
    bool veioDaFloresta;

public:
    explicit Mapa1Vila(Personagem* personagemJogador);
    ~Mapa1Vila() override;

    std::string obterTitulo() const override { return tituloDoMapaAtual; }
    ProximaTransicaoMapa iniciarLoopDeExploracao() override;

private:
    void inicializarInteracoes();
};
