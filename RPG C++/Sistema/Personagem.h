#include <string>
#include <iostream>
#include <vector>

#include "../Inventario/Inventario.h"

#pragma once

struct Atributos 
{
    int vida;
    int forca;
    int destreza;
    int resistencia;
    int constituicao;
    int inteligencia;
    int sabedoria;

    void calcularAtributos(const Atributos& outro) 
    {
        this->vida += outro.vida;
        this->forca += outro.forca;
        this->destreza += outro.destreza;
        this->resistencia += outro.resistencia;
        this->constituicao += outro.constituicao;
        this->inteligencia += outro.inteligencia;
        this->sabedoria += outro.sabedoria;
    }
};

class RacaBase;   
class ClasseBase; 

class Personagem 
{
protected:
    std::string nomePersonagem;
    int vidaAtual;
    Atributos statsFinais;
    RacaBase* raca;
    ClasseBase* classe;
    Inventario* mochila;

    Item* arma;    
    Item* escudo;   
    Item* armadura;
    int ouroRecompensa;

public:
    Personagem(std::string nome, RacaBase* r, ClasseBase* c);
    virtual ~Personagem();

    void calcularAtributos();
    void mostrarStatus() const;
    void modificarVida(int valor);
    void equiparItem(Item* item);

    // Getters e Setters em camelCase
    std::string obterNome() const { return nomePersonagem; }
    int obterVida() const { return vidaAtual; }
    int obterVidaMaxima() const { return statsFinais.vida; }
    int obterForca() const { return statsFinais.forca; }
    int obterResistencia() const { return statsFinais.resistencia; }
    int obterConstituicao() const { return statsFinais.constituicao; }
    int obterInteligencia() const { return statsFinais.inteligencia; }
    int obterSabedoria() const { return statsFinais.sabedoria; }

    RacaBase* obterRaca() const;
    ClasseBase* obterClasse() const;
    std::string obterNomeClasse() const;
    
    Item* obterArma() const { return arma; }
    Item* obterEscudo() const { return escudo; }
    Item* obterArmadura() const { return armadura; }
    Inventario* obterInventario() const { return mochila; }

    void definirOuroRecompensa(int valor) { ouroRecompensa = valor; }
    int obterOuroRecompensa() const { return ouroRecompensa; }
    void ganharOuro(int valor) { mochila->adicionarOuro(valor); }

    void usarHabilidadeDeClasse(Personagem* alvo);
    void usarHabilidadeDeRaca(Personagem* alvo);
};