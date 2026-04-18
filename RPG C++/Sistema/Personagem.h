#include <string>
#include <iostream>
#include <vector>
#include <memory>

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

class EfeitoStatus {
protected:
    std::string nome;
    int turnosRestantes;
public:
    EfeitoStatus(const std::string& n, int t) : nome(n), turnosRestantes(t) {}
    virtual ~EfeitoStatus() = default;
    std::string obterNome() const { return nome; }
    void decrementarTurno() { turnosRestantes--; }
    bool expirou() const { return turnosRestantes <= 0; }
    virtual void aplicarInicioTurno(Personagem* alvo) {}
    virtual bool impedeAcao() const { return false; }
};

class EfeitoAtordoamento : public EfeitoStatus {
public:
    EfeitoAtordoamento(const std::string& n, int t) : EfeitoStatus(n, t) {}
    bool impedeAcao() const override { return true; }
};

class EfeitoSugaSangue : public EfeitoStatus {
private:
    Personagem* atacante;
public:
    EfeitoSugaSangue(const std::string& n, int t, Personagem* atk) : EfeitoStatus(n, t), atacante(atk) {}
    void aplicarInicioTurno(Personagem* alvo) override;
};

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
    int turnosBuff;           // Guerreiro: determinacao no combate
    bool recargaHabilidade;   // Arqueiro/Bardo: uso seguido
    bool pularTurnoInimigo;   // Bardo: flashing lights
    
    bool sofrendoSangramento; // Controle de debuff
    int turnosSangramento;
    bool sofrendoLentidao;    // Controle de debuff
    int turnosLentidao;
    bool sofrendoQuebraResistencia; // Controle de debuff
    bool sofrendoFraqueza;
    int turnosFraqueza;
    int forcaPerdidaFraqueza;

    bool estaDefendendo;      // Controle de Defesa Ativa
    bool recargaDefesa;       // Cooldown de 1 turno da Defesa
    bool podeReviver;         // Controle para "Espirito indomavel"
    bool parryAtivado;        // Controle do sistema de Parry
    int dificuldadeAtual;     // 1 = Facil, 2 = Normal, 3 = Dificil

    int cooldownHab1;
    int cooldownHab2;
    int cooldownHab3;
    bool habilidadeCancelada;
    bool recebendoMetadeDano;
    int turnosMetadeDano;
    int turnosGrito;
    int bonusForcaGrito;
    int bonusDestrezaGrito;

    double multiplicadorAtual;  // Para buffs temporarios

    Item* arma;    
    Item* escudo;   
    Item* armadura;
    Item* itemSelecionadoParaUso;
    int ouroRecompensa;
    
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
    int obterDestreza() const { 
        int penalidade = armadura ? (armadura->obterReducaoFixa() / 3) : 0;
        if (obterNomeClasse() == "Arqueiro") penalidade /= 2;
        int destrezaFinal = statsFinais.destreza - penalidade;
        return destrezaFinal > 0 ? destrezaFinal : 0; 
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
    
    void alterarAtributoEstatico(const std::string& atributo, int valor);

    RacaBase* obterRaca() const;
    ClasseBase* obterClasse() const;
    std::string obterNomeClasse() const;
    
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

    void definirMultiplicador(double m) { 
        if (m > 1.0 && obterNomeClasse() == "Bardo") {
            multiplicadorAtual = 1.0 + (m - 1.0) * 1.4;
        } else {
            multiplicadorAtual = m; 
        }
    }
    double obterMultiplicador() const { return multiplicadorAtual; }

    bool podeUsarRessurreicao() const { return podeReviver; }
    void consumirRessurreicao() { podeReviver = false; }

    void definirInviolavel(bool v) { estaInviolavel = v; }
    bool obterInviolavel() const { return estaInviolavel; }
    void definirTurnosBuff(int t) { turnosBuff = t; }
    int obterTurnosBuff() const { return turnosBuff; }
    void definirRecarga(bool r) { recargaHabilidade = r; }
    bool obterRecarga() const { return recargaHabilidade; }
    void definirPularTurnoInimigo(bool p) { pularTurnoInimigo = p; }
    bool obterPularTurnoInimigo() const { return pularTurnoInimigo; }
    
    int obterCooldownHab1() const { return cooldownHab1; }
    void definirCooldownHab1(int v) { cooldownHab1 = v; }
    int obterCooldownHab2() const { return cooldownHab2; }
    void definirCooldownHab2(int v) { cooldownHab2 = v; }
    int obterCooldownHab3() const { return cooldownHab3; }
    void definirCooldownHab3(int v) { cooldownHab3 = v; }
    
    bool obterHabilidadeCancelada() const { return habilidadeCancelada; }
    void definirHabilidadeCancelada(bool v) { habilidadeCancelada = v; }
    bool obterRecebendoMetadeDano() const { return recebendoMetadeDano; }
    void definirRecebendoMetadeDano(bool v) { recebendoMetadeDano = v; }
    int obterTurnosMetadeDano() const { return turnosMetadeDano; }
    void definirTurnosMetadeDano(int v) { turnosMetadeDano = v; }
    
    int obterTurnosGrito() const { return turnosGrito; }
    void definirTurnosGrito(int v) { turnosGrito = v; }
    int obterBonusForcaGrito() const { return bonusForcaGrito; }
    int obterBonusDestrezaGrito() const { return bonusDestrezaGrito; }
    void definirBonusGrito(int f, int d) { bonusForcaGrito = f; bonusDestrezaGrito = d; }
    void reduzirCooldowns();

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
    
    void definirFraqueza(bool f) { sofrendoFraqueza = f; }
    bool obterFraqueza() const { return sofrendoFraqueza; }
    void definirTurnosFraqueza(int t) { turnosFraqueza = t; }
    int obterTurnosFraqueza() const { return turnosFraqueza; }
    void aplicarFraquezaEstatistica();
    void removerFraquezaEstatistica();

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

    TipoAtaque obterTipoAtaque() const;
    bool habilidadeDaClasseConsomeTurno() const;

    void adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito);
    void processarEfeitosInicioTurno();
    bool podeAgir() const;

    int calcularDefesaBase(int danoBruto, int danoPerfurante) const;
    int receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, Personagem* atacante, bool aplicarPassivas);

    virtual void executarDrops(Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal);
};