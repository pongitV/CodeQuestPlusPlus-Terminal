#pragma once
#include <vector>
#include <string>
#include "Personagem.h"

class Mapa 
{
private:
    std::vector<std::string> layout;
    int jogadorX;
    int jogadorY;
    Personagem* jogador;

    // Variaveis para gerenciar sub-mapas
    std::vector<std::string> mapaGeralSalvo;
    int anteriorX;
    int anteriorY;
    bool emSubMapa;
    std::vector<std::string> mapaCavernaSalvo;
    bool cavernaIniciada;
    std::vector<std::string> mapaLojaSalvo;
    bool lojaIniciada;


public:
    Mapa(Personagem* p);
    
    // Inicia o loop do mapa e gerencia a movimentação e encontros
    void iniciarExploracao();
};