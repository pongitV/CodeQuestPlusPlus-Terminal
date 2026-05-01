#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../Gerenciadores/GerenciadorEfeitosStatus.h"
#include "../Inventario/Inventario.h"

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
enum class TipoClasse;
enum class HabilidadeID;
enum class TipoRaca;

enum class DificuldadeJogo 
{
    Facil = 1,
    Normal = 2,
    Dificil = 3
};

struct ControleCombate {
    bool estaDefendendo = false;
    bool recargaDefesa = false;
    bool recargaHabilidade = false;
    bool pularTurnoInimigo = false;
    bool habilidadeCancelada = false;
    double multiplicadorAtual = 1.0;
    int curaTotalRecebida = 0;
    std::unordered_map<HabilidadeID, int> cooldownsAtivos;
    
    void resetar() {
        estaDefendendo = false;
        recargaDefesa = false;
        recargaHabilidade = false;
        pularTurnoInimigo = false;
        habilidadeCancelada = false;
        multiplicadorAtual = 1.0;
        curaTotalRecebida = 0;
        cooldownsAtivos.clear();
    }
};

struct ControleSistema {
    bool querVoltarProMenu = false;
    bool labirintoDesbloqueado = false;
    bool podeReviver = true;
    bool parryAtivado = false;
    DificuldadeJogo dificuldadeAtual = DificuldadeJogo::Normal;
};

class SistemaPersonagem 
{
protected:
    std::string nomePersonagem;
    int vidaAtual;
    std::unique_ptr<RacaBase> raca;
    std::unique_ptr<ClasseBase> classe;
    Atributos statsFinais;
    std::unique_ptr<Inventario> mochila;

    std::vector<std::unique_ptr<EfeitoStatus>> efeitosAtivos;

    ControleCombate combate;
    ControleSistema sistema;

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
    SistemaPersonagem(std::string nome, std::unique_ptr<RacaBase> r, std::unique_ptr<ClasseBase> c);
    virtual ~SistemaPersonagem();

    void calcularAtributos();
    void mostrarStatus() const;
    void modificarVida(int valor);
    void equiparItem(Item* item);

    // Getters e Setters em camelCase
    std::string obterNome() const { return nomePersonagem; }
    int obterVida() const { return vidaAtual; }
    int obterVidaMaxima() const { return statsFinais.vida; }
    int obterForca() const { return statsFinais.forca; }
    int obterDestreza() const;
    int obterResistencia() const { return statsFinais.resistencia; }
    int obterConstituicao() const { return statsFinais.constituicao; }
    int obterInteligencia() const { return statsFinais.inteligencia; }
    int obterSabedoria() const { return statsFinais.sabedoria; }
    
    int obterNivel() const { return nivel; }
    int obterXpAtual() const { return xpAtual; }
    int obterXpParaSubir() const { return xpParaSubir; }
    void definirNivel(int n) { nivel = n; }
    void definirXpAtual(int x) { xpAtual = x; }
    void definirXpParaSubir(int x) { xpParaSubir = x; }
    void definirVida(int v) { vidaAtual = v; }
    void ganharXp(int valor) { xpAtual += valor; }
    bool podeSubirDeNivel() const { return xpAtual >= xpParaSubir; }
    bool subirDeNivel(TipoAtributo atributo);
    
    int obterCuraTotalRecebida() const { return combate.curaTotalRecebida; }

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

    void definirMultiplicador(double m);
    double obterMultiplicador() const { return combate.multiplicadorAtual; }

    bool podeUsarRessurreicao() const { return sistema.podeReviver; }
    void consumirRessurreicao() { sistema.podeReviver = false; }

    int obterCooldown(HabilidadeID habilidade) const 
    {
        auto it{combate.cooldownsAtivos.find(habilidade)};
        return (it != combate.cooldownsAtivos.end()) ? it->second : 0;
    }
    void definirCooldown(HabilidadeID habilidade, int turnos) 
    {
        combate.cooldownsAtivos[habilidade] = turnos;
    }
    
    bool obterHabilidadeCancelada() const { return combate.habilidadeCancelada; }
    void definirHabilidadeCancelada(bool v) { combate.habilidadeCancelada = v; }

    void definirRecarga(bool r) { combate.recargaHabilidade = r; }
    bool obterRecarga() const { return combate.recargaHabilidade; }
    void definirPularTurnoInimigo(bool p) { combate.pularTurnoInimigo = p; }
    bool obterPularTurnoInimigo() const { return combate.pularTurnoInimigo; }
    
    void definirVoltarProMenu(bool v) { sistema.querVoltarProMenu = v; }
    bool obterVoltarProMenu() const { return sistema.querVoltarProMenu; }

    void desbloquearLabirinto() { sistema.labirintoDesbloqueado = true; }
    bool obterLabirintoDesbloqueado() const { return sistema.labirintoDesbloqueado; }

    void reduzirCooldowns();
    void prepararParaNovaBatalha();

    bool possuiEfeito(EfeitoID id) const;
    int obterTurnosEfeito(EfeitoID id) const;
    const EfeitoStatus* encontrarEfeito(EfeitoID id) const;


    void definirDefendendo(bool d) { combate.estaDefendendo = d; }
    bool obterDefendendo() const { return combate.estaDefendendo; }
    void definirRecargaDefesa(bool r) { combate.recargaDefesa = r; }
    bool obterRecargaDefesa() const { return combate.recargaDefesa; }
    void desequiparEscudo() { escudo = nullptr; }
    void desequiparArma() { arma = nullptr; }
    void desequiparArmadura() { armadura = nullptr; destrezaCacheDirty_ = true; reducaoPercentualCacheDirty_ = true; }

    void definirParryAtivado(bool p) { sistema.parryAtivado = p; }
    bool obterParryAtivado() const { return sistema.parryAtivado; }

    void definirDificuldade(DificuldadeJogo d) { sistema.dificuldadeAtual = d; }
    DificuldadeJogo obterDificuldade() const { return sistema.dificuldadeAtual; }
    void aplicarMultiplicadorDificuldade(double mult);

    TipoAtaque obterTipoAtaque() const;
    bool habilidadeDaClasseConsomeTurno() const;

    void adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito);
    void processarEfeitosInicioTurno();
    bool podeAgir() const;

    // Retorna IDs de todos os efeitos ativos (single-pass)
    std::vector<EfeitoID> obterIDsEfeitosAtivos() const;
    void limparEfeitos();

    int calcularDefesaBase(int danoBruto, int danoPerfurante) const;
    int receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, SistemaPersonagem* atacante, bool aplicarPassivas);

    virtual void executarDrops(SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal);
};