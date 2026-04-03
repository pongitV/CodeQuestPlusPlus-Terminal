#include <vector>

#include "../Sistema/Personagem.h"

#pragma once

class Personagem;
class Item;

class ClasseBase {
public:
    virtual ~ClasseBase() {}
    virtual std::string obterNomeClasse() const = 0;
    virtual std::vector<std::string> obterAparenciaClasseMenu() const = 0;
    virtual Atributos obterAtributosClasse() const = 0;
    virtual void usarHabilidadeClasse(Personagem* usuario, Personagem* alvo) = 0;
    virtual std::vector<Item*> gerarKitInicial() const = 0;
};
