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
    RacaBase* raca;
    ClasseBase* classe;
    Atributos statsFinais;
    Inventario* mochila;

    bool estaInviolavel;      // Arqueiro: retirada com pontaria
    int turnosBuff;           // Guerreiro: determinacao no combate
    bool recargaHabilidade;   // Arqueiro/Bardo: uso seguido
    bool modoAtaqueArea;      // Mago: estrategia arcana
    bool pularTurnoInimigo;   // Bardo: flashing lights
    
    bool sofrendoSangramento; // Controle de debuff
    int turnosSangramento;
    bool sofrendoLentidao;    // Controle de debuff
    int turnosLentidao;
    bool sofrendoQuebraResistencia; // Controle de debuff

    bool estaDefendendo;      // Controle de Defesa Ativa
    bool recargaDefesa;       // Cooldown de 1 turno da Defesa
    bool podeReviver;         // Controle para "Espirito indomavel"
    bool parryAtivado;        // Controle do sistema de Parry
    int dificuldadeAtual;     // 1 = Facil, 2 = Normal, 3 = Dificil

    double multiplicadorAtual;  // Para buffs temporarios

    Item* arma;    
    Item* escudo;   
    Item* armadura;
    int ouroRecompensa;
    
    int nivel;
    int xpAtual;
    int xpParaSubir;
    int xpRecompensa;

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
    int obterDestreza() const { return statsFinais.destreza; }
    int obterResistencia() const { return statsFinais.resistencia; }
    int obterConstituicao() const { return statsFinais.constituicao; }
    int obterInteligencia() const { return statsFinais.inteligencia; }
    int obterSabedoria() const { return statsFinais.sabedoria; }
    
    int obterNivel() const { return nivel; }
    int obterXpAtual() const { return xpAtual; }
    int obterXpParaSubir() const { return xpParaSubir; }
    void ganharXp(int valor) { xpAtual += valor; }
    bool podeSubirDeNivel() const { return xpAtual >= xpParaSubir; }
    bool subirDeNivel(std::string atributo);

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
    void definirXpRecompensa(int valor) { xpRecompensa = valor; }
    int obterXpRecompensa() const { return xpRecompensa; }

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
    
    void definirSangramento(bool s) { sofrendoSangramento = s; }
    bool obterSangramento() const { return sofrendoSangramento; }
    void definirTurnosSangramento(int t) { turnosSangramento = t; }
    int obterTurnosSangramento() const { return turnosSangramento; }
    void definirLentidao(bool l) { sofrendoLentidao = l; }
    bool obterLentidao() const { return sofrendoLentidao; }
    void definirTurnosLentidao(int t) { turnosLentidao = t; }
    int obterTurnosLentidao() const { return turnosLentidao; }
    void aplicarLentidaoEstatistica();
    void removerLentidaoEstatistica();
    
    bool obterQuebraResistencia() const { return sofrendoQuebraResistencia; }
    void aplicarQuebraResistenciaEstatistica();

    void definirDefendendo(bool d) { estaDefendendo = d; }
    bool obterDefendendo() const { return estaDefendendo; }
    void definirRecargaDefesa(bool r) { recargaDefesa = r; }
    bool obterRecargaDefesa() const { return recargaDefesa; }
    void desequiparEscudo() { escudo = nullptr; }
    void desequiparArma() { arma = nullptr; }
    void desequiparArmadura() { armadura = nullptr; }

    void definirParryAtivado(bool p) { parryAtivado = p; }
    bool obterParryAtivado() const { return parryAtivado; }

    void definirDificuldade(int d) { dificuldadeAtual = d; }
    int obterDificuldade() const { return dificuldadeAtual; }
    void aplicarMultiplicadorDificuldade(double mult);
};