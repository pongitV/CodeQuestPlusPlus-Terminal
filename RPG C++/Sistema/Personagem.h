#include <string>
#include <iostream>
#include <vector>
#include <memory>
#include <unordered_map>

#include "../Inventario/Inventario.h"
#include "Tipos.h"

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

enum class TipoAtributo 
{
    Vida = 1,
    Forca,
    Destreza,
    Resistencia,
    Constituicao,
    Inteligencia,
    Sabedoria
};

enum class TipoAtaque 
{
    UNICO,
    AREA
};

class RacaBase;   
class ClasseBase; 

#include "EfeitosStatus.h"

class Personagem 
{
protected:
    std::string nomePersonagem;
    int vidaAtual;
    std::unique_ptr<RacaBase> raca;
    std::unique_ptr<ClasseBase> classe;
    Atributos statsFinais;
    std::unique_ptr<Inventario> mochila;

    std::vector<std::unique_ptr<EfeitoStatus>> efeitosAtivos;

    bool estaInviolavel;      // Arqueiro: retirada com pontaria
    bool recargaHabilidade;   // Arqueiro/Bardo: uso seguido
    bool pularTurnoInimigo;
    bool querVoltarProMenu;

    bool estaDefendendo;      // Controle de Defesa Ativa
    bool recargaDefesa;       // Cooldown de 1 turno da Defesa
    bool podeReviver;         // Controle para "Espirito indomavel"
    bool parryAtivado;        // Controle do sistema de Parry
    int dificuldadeAtual;     // 1 = Facil, 2 = Normal, 3 = Dificil

    std::unordered_map<std::string, int> cooldownsAtivos;
    bool habilidadeCancelada;

    double multiplicadorAtual;  // Para buffs temporarios

    Item* arma;
    Item* escudo;
    Item* armadura;
    Item* itemSelecionadoParaUso;
    int ouroRecompensa;

    // Cache de getters calculados
    mutable int destrezaCache_ = 0;
    mutable bool destrezaCacheDirty_ = true;
    mutable int reducaoPercentualCache_ = 0;
    mutable bool reducaoPercentualCacheDirty_ = true;
    
    int nivel;
    int xpAtual;
    int xpParaSubir;
    int xpRecompensa;

public:
    Personagem(std::string nome, std::unique_ptr<RacaBase> r, std::unique_ptr<ClasseBase> c);
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
    int obterDestreza() const
    {
        if (!destrezaCacheDirty_) return destrezaCache_;
        int penalidade = armadura ? (armadura->obterReducaoFixa() / 3) : 0;
        if (obterTipoClasse() == TipoClasse::Arqueiro) penalidade /= 2;
        int destrezaFinal = statsFinais.destreza - penalidade;
        destrezaCache_ = destrezaFinal > 0 ? destrezaFinal : 0;
        destrezaCacheDirty_ = false;
        return destrezaCache_;
    }
    int obterResistencia() const { return statsFinais.resistencia; }
    int obterConstituicao() const { return statsFinais.constituicao; }
    int obterInteligencia() const { return statsFinais.inteligencia; }
    int obterSabedoria() const { return statsFinais.sabedoria; }
    
    int obterNivel() const { return nivel; }
    int obterXpAtual() const { return xpAtual; }
    int obterXpParaSubir() const { return xpParaSubir; }
    void ganharXp(int valor) { xpAtual += valor; }
    bool podeSubirDeNivel() const { return xpAtual >= xpParaSubir; }
    bool subirDeNivel(TipoAtributo atributo);
    
    void alterarAtributoEstatico(TipoAtributo atributo, int valor);
    Atributos& obterAtributosFinais() { return statsFinais; }

    RacaBase* obterRaca() const;
    ClasseBase* obterClasse() const;
    std::string obterNomeClasse() const;
    TipoClasse obterTipoClasse() const;
    TipoRaca obterTipoRaca() const;
    
    Item* obterArma() const { return arma; }
    Item* obterEscudo() const { return escudo; }
    Item* obterArmadura() const { return armadura; }
    Inventario* obterInventario() const { return mochila.get(); }
    Item* obterItemSelecionadoParaUso() const { return itemSelecionadoParaUso; }
    void definirItemSelecionadoParaUso(Item* item) { itemSelecionadoParaUso = item; }

    void definirOuroRecompensa(int valor) { ouroRecompensa = valor; }
    int obterOuroRecompensa() const { return ouroRecompensa; }
    void ganharOuro(int valor) { mochila->adicionarOuro(valor); }
    void definirXpRecompensa(int valor) { xpRecompensa = valor; }
    int obterXpRecompensa() const { return xpRecompensa; }

    void definirMultiplicador(double m) 
    { 
        if (m > 1.0 && obterTipoClasse() == TipoClasse::Bardo) 
        {
            multiplicadorAtual = 1.0 + (m - 1.0) * 1.4;
        } 
        else 
        {
            multiplicadorAtual = m; 
        }
    }
    double obterMultiplicador() const { return multiplicadorAtual; }

    bool podeUsarRessurreicao() const { return podeReviver; }
    void consumirRessurreicao() { podeReviver = false; }

    int obterCooldown(const std::string& habilidade) const 
    {
        auto it{cooldownsAtivos.find(habilidade)};
        return (it != cooldownsAtivos.end()) ? it->second : 0;
    }
    void definirCooldown(const std::string& habilidade, int turnos) 
    {
        cooldownsAtivos[habilidade] = turnos;
    }
    
    bool obterHabilidadeCancelada() const { return habilidadeCancelada; }
    void definirHabilidadeCancelada(bool v) { habilidadeCancelada = v; }

    void definirRecarga(bool r) { recargaHabilidade = r; }
    bool obterRecarga() const { return recargaHabilidade; }
    void definirPularTurnoInimigo(bool p) { pularTurnoInimigo = p; }
    bool obterPularTurnoInimigo() const { return pularTurnoInimigo; }
    
    void definirVoltarProMenu(bool v) { querVoltarProMenu = v; }
    bool obterVoltarProMenu() const { return querVoltarProMenu; }

    void reduzirCooldowns();
    bool possuiEfeito(const std::string& nome) const;
    int obterTurnosEfeito(const std::string& nome) const;
    const EfeitoStatus* encontrarEfeito(const std::string& nome) const;


    void definirDefendendo(bool d) { estaDefendendo = d; }
    bool obterDefendendo() const { return estaDefendendo; }
    void definirRecargaDefesa(bool r) { recargaDefesa = r; }
    bool obterRecargaDefesa() const { return recargaDefesa; }
    void desequiparEscudo() { escudo = nullptr; }
    void desequiparArma() { arma = nullptr; }
    void desequiparArmadura() { armadura = nullptr; destrezaCacheDirty_ = true; reducaoPercentualCacheDirty_ = true; }

    void definirParryAtivado(bool p) { parryAtivado = p; }
    bool obterParryAtivado() const { return parryAtivado; }

    void definirDificuldade(int d) { dificuldadeAtual = d; }
    int obterDificuldade() const { return dificuldadeAtual; }
    void aplicarMultiplicadorDificuldade(double mult);

    TipoAtaque obterTipoAtaque() const;
    bool habilidadeDaClasseConsomeTurno() const;

    void adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito);
    void processarEfeitosInicioTurno();
    bool podeAgir() const;

    // Retorna set com nomes de todos os efeitos ativos (single-pass)
    std::vector<std::string> obterNomesEfeitosAtivos() const;

    int calcularDefesaBase(int danoBruto, int danoPerfurante) const;
    int receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, Personagem* atacante, bool aplicarPassivas);

    virtual void executarDrops(Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal);
};