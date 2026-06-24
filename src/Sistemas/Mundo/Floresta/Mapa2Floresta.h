#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../../../Entidades/Personagem.h"
#include "../MapaInteracao.h"

class Mapa2Floresta final : public IMapa 
{
public:
    std::vector<std::string> matrizDoMapaAtual;
    int posicaoXDoJogador;
    int posicaoYDoJogador;
    Personagem* jogadorAtual;
    
    std::vector<std::string> matrizDoMapaPrincipalSalva;
    int posicaoXSalvaAntesDeEntrarNoSubMapa;
    int posicaoYSalvaAntesDeEntrarNoSubMapa;
    bool jogadorEstaDentroDeUmSubMapa;
 
    std::vector<std::string> matrizDoMapaDoCoracaoDaArvoreSalva;
    std::vector<std::string> matrizDoMapaDoLabirintoSalva;
    std::vector<std::string> matrizDoMapaSalaDoChefeSalva;
    bool coracaoDaArvoreJaFoiVisitado;
    bool labirintoJaFoiVisitado;
    bool salaDoChefeJaFoiVisitada;
    bool exploracaoEstaAtiva;
    std::string tituloDoMapaAtual;
    ProximaTransicaoMapa proximoMapa;

    std::unordered_map<char, std::unique_ptr<InteracaoFloresta>> interacoes;

public:
    explicit Mapa2Floresta(Personagem* personagemJogador);
    ~Mapa2Floresta() override;
    
    std::string obterTitulo() const override { return tituloDoMapaAtual; }
    ProximaTransicaoMapa iniciarLoopDeExploracao() override;

private:
    void inicializarInteracoes();
};
