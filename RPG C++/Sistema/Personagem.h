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

    double multiplicadorAtual; // Para buffs temporarios

    bool estaDefendendo;      // Controle de Defesa Ativa
    bool recargaDefesa;       // Cooldown de 1 turno da Defesa
    bool podeReviver; // Controle para "Espirito indomavel"
    bool estaInviolavel;      // Arqueiro: retirada com pontaria
    int turnosBuff;           // Guerreiro: determinacao no combate
    bool recargaHabilidade;   // Arqueiro/Bardo: uso seguido
    bool modoAtaqueArea;      // Mago: estrategia arcana
    bool pularTurnoInimigo;   // Bardo: flashing lights

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

    void definirMultiplicador(double m) { multiplicadorAtual = m; }
    double obterMultiplicador() const { return multiplicadorAtual; }

    bool podeUsarRessurreicao() const { return podeReviver; }
    void consumirRessurreicao() { podeReviver = false; }

    void definirInviolavel(bool v) { estaInviolavel = v; }
    bool obterInviolavel() const { return estaInviolavel; }
    void definirTurnosBuff(int t) { turnosBuff = t; }
    int obterTurnosBuff() const { return turnosBuff; }
    void definirRecarga(bool r) { recargaHabilidade = r; }
    bool obterRecarga() const { return recargaHabilidade; }
    void alternarModoAtaque() { modoAtaqueArea = !modoAtaqueArea; }
    bool obterModoAtaqueArea() const { return modoAtaqueArea; }
    void definirPularTurnoInimigo(bool p) { pularTurnoInimigo = p; }
    bool obterPularTurnoInimigo() const { return pularTurnoInimigo; }
    
    void definirDefendendo(bool d) { estaDefendendo = d; }
    bool obterDefendendo() const { return estaDefendendo; }
    void definirRecargaDefesa(bool r) { recargaDefesa = r; }
    bool obterRecargaDefesa() const { return recargaDefesa; }
    void desequiparEscudo() { escudo = nullptr; }
};