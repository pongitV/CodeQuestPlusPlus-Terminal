#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <mutex>

#include "../Core/Controladores/Status.h"
#include "../Sistemas/Inventario/Inventario.h"
#include "../Core/Utilidades/Aparencia.h"

struct Atributos 
{
    int vida;         // Pontos de vida (HP) maximos do personagem
    int forca;        // Influencia o dano de ataques fisicos frontais e armas pesadas
    int destreza;     // Define a ordem do turno, dano de armas ageis e acerto critico/esquiva
    int resistencia;  // Reduz dano fisico recebido e atua como requisito para escudos pesados
    int constituicao; // Vitalidade geral, reduz efetividade de debuffs e atua como requisito para armaduras
    int inteligencia; // Multiplicador base do dano magico e requisito para usar cajados/varinhas
    int sabedoria;    // Aumenta atributos magicos secundarios, forca das curas e defesa magica

    void somarAtributos(const Atributos& outro) 
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

struct ResultadoDano {
    int danoFinal = 0;
    int danoBloqueado = 0;
    bool escudoQuebrou = false;
    std::string nomeEscudoQuebrado = "";
};

enum class DificuldadeJogo 
{
    Facil = 1,
    Normal = 2,
    Dificil = 3
};

class Personagem 
{
private:
    struct ControleCombate {
        bool estaDefendendo = false;
        std::vector<std::unique_ptr<Personagem>> almasColetadas;
        bool recargaDefesa = false;
        bool recargaHabilidade = false;
        bool pularTurnoInimigo = false;
        bool habilidadeCancelada = false;
        bool morteAnimada = false;
        double multiplicadorAtual = 1.0;
        int curaTotalRecebida = 0;
        int vidaMaximaFixa = 0;
        std::unordered_map<HabilidadeID, int> cooldownsAtivos;
        
        void resetar() {
            estaDefendendo = false;
            recargaDefesa = false;
            recargaHabilidade = false;
            pularTurnoInimigo = false;
            habilidadeCancelada = false;
            morteAnimada = false;
            multiplicadorAtual = 1.0;
            curaTotalRecebida = 0;
            vidaMaximaFixa = 0;
            cooldownsAtivos.clear();
        }
    };

    struct ControleSistema {
        bool querVoltarProMenu = false;
        bool labirintoDesbloqueado = false;
        bool podeReviver = true;
        bool parryAtivado = false;
        bool parryModerno = true;
        bool possuiRegeneracaoTroll = false;
        bool godModeAtivo = false;
        bool noclipAtivo = false;
        bool isMinion = false;
        DificuldadeJogo dificuldadeAtual = DificuldadeJogo::Normal;
        double dificuldadeMultiplicador = 1.0;
        char iconeJogador = '@';
        Cor corJogador = Cor::VERDE;
        Cor corFundoTerminal = Cor::RESET;
    };

    static std::unordered_set<Personagem*> personagensAtivos;

    ControleCombate combate;
    ControleSistema sistema;

protected:
    std::string nomePersonagem;
    int vidaAtual;
    std::unique_ptr<RacaBase> raca;
    std::unique_ptr<ClasseBase> classe;
    Atributos statsFinais;
    std::unique_ptr<Inventario> mochila;

    std::vector<std::unique_ptr<EfeitoStatus>> efeitosAtivos;
    std::vector<std::unique_ptr<EfeitoStatus>> efeitosFilaAdicao;
    std::vector<EfeitoID> efeitosFilaRemocao;
    bool processandoEfeitos = false;

    Item* arma;
    Item* escudo;
    Item* armadura;
    Item* consumivelRapido;
    Item* itemSelecionadoParaUso;

    // Cache de getters calculados
    // ATENCAO: Esta estrutura usando 'mutable' nao e thread-safe.
    // Caso o jogo passe a utilizar multi-threading (ex: IA rodando em background), e necessario proteger com std::mutex ou std::atomic.
    struct CacheAtributos {
        int vidaMaxima = 0;
        int forca = 0;
        int destreza = 0;
        int resistencia = 0;
        int constituicao = 0;
        int inteligencia = 0;
        int sabedoria = 0;
        int reducaoPercentual = 0;
        bool sujo = true;
    };
    mutable CacheAtributos cache_;
    mutable std::mutex mutexCache_; // Protege o acesso ao cache em ambientes multithread
    void atualizarCacheSeNecessario() const;
    
    int nivel;
    int xpAtual;
    int xpParaSubir;

    int* obterPonteiroAtributoEstatico(TipoAtributo atributo);

public:
    Personagem(const Personagem& other);
    Personagem(const std::string& nome, std::unique_ptr<RacaBase> r, std::unique_ptr<ClasseBase> c);
    virtual ~Personagem();

    static bool isValido(Personagem* p);

    void calcularAtributos();
    std::unique_ptr<Personagem> clone() const;
    void mostrarStatus() const;
    void modificarVida(int valor);
    void alterarNome(const std::string& novoNome) { nomePersonagem = novoNome; }
    void equiparItem(Item* item);

    // Getters e Setters em camelCase
    std::string obterNome() const { return nomePersonagem; }
    int obterVida() const { return vidaAtual; }
    int obterVidaMaxima() const {
        if (combate.vidaMaximaFixa > 0) return combate.vidaMaximaFixa;
        atualizarCacheSeNecessario(); return cache_.vidaMaxima;
    }
    int obterForca() const { atualizarCacheSeNecessario(); return cache_.forca; }
    int obterDestreza() const { atualizarCacheSeNecessario(); return cache_.destreza; }
    int obterResistencia() const { atualizarCacheSeNecessario(); return cache_.resistencia; }
    int obterConstituicao() const { atualizarCacheSeNecessario(); return cache_.constituicao; }
    int obterInteligencia() const { atualizarCacheSeNecessario(); return cache_.inteligencia; }
    int obterSabedoria() const { atualizarCacheSeNecessario(); return cache_.sabedoria; }
    
    int obterNivel() const { return nivel; }
    int obterXpAtual() const { return xpAtual; }
    int obterXpParaSubir() const { return xpParaSubir; }
    void definirNivel(int novoNivel) { nivel = novoNivel; }
    void definirXpAtual(int novoXp) { xpAtual = novoXp; }
    void definirXpParaSubir(int novoXpParaSubir) { xpParaSubir = novoXpParaSubir; }
    void definirVida(int novaVida) { vidaAtual = novaVida; }
    void ganharXp(int valor) { xpAtual += valor; }
    bool podeSubirDeNivel() const { return xpAtual >= xpParaSubir; }
    bool subirDeNivel(TipoAtributo atributo);
    void escalarAtributos(double fator);
    void adicionarAlma(std::unique_ptr<Personagem> alma);
    std::vector<std::unique_ptr<Personagem>>& obterAlmas();
    size_t obterNumeroDeAlmas() const;
    std::unique_ptr<Personagem> removerAlma(int index);

    void forcarRecalculoCache() { cache_.sujo = true; }
    
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
    Item* obterConsumivelRapido() const { return consumivelRapido; }
    void desequiparConsumivel() { consumivelRapido = nullptr; cache_.sujo = true; }
    Inventario* obterInventario() const { return mochila.get(); }
    Item* obterItemSelecionadoParaUso() const { return itemSelecionadoParaUso; }
    bool isItemEquipado(Item* item) const { return item != nullptr && (item == arma || item == escudo || item == armadura || item == consumivelRapido); }
    void definirItemSelecionadoParaUso(Item* item) { itemSelecionadoParaUso = item; }

    void ganharOuro(int valor) { mochila->adicionarOuro(valor); }

    void definirMultiplicador(double novoMultiplicador);
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
    void definirHabilidadeCancelada(bool foiCancelada) { combate.habilidadeCancelada = foiCancelada; }

    void definirRecarga(bool emRecarga) { combate.recargaHabilidade = emRecarga; }
    bool obterRecarga() const { return combate.recargaHabilidade; }
    void definirPularTurnoInimigo(bool pularTurno) { combate.pularTurnoInimigo = pularTurno; }
    bool obterPularTurnoInimigo() const { return combate.pularTurnoInimigo; }
    
    void definirVoltarProMenu(bool voltar) { sistema.querVoltarProMenu = voltar; }
    bool obterVoltarProMenu() const { return sistema.querVoltarProMenu; }

    void desbloquearLabirinto() { sistema.labirintoDesbloqueado = true; }
    bool obterLabirintoDesbloqueado() const { return sistema.labirintoDesbloqueado; }

    void desbloquearRegeneracaoTroll() { sistema.possuiRegeneracaoTroll = true; }
    bool possuiRegeneracaoTroll() const { return sistema.possuiRegeneracaoTroll; }

    void alternarGodMode() { sistema.godModeAtivo = !sistema.godModeAtivo; }
    bool isGodMode() const { return sistema.godModeAtivo; }

    void alternarNoclip() { sistema.noclipAtivo = !sistema.noclipAtivo; }
    bool isNoclip() const { return sistema.noclipAtivo; }

    void reduzirCooldowns();
    void prepararParaNovaBatalha();
    void finalizarBatalha();

    bool possuiEfeito(EfeitoID id) const;
    int obterTurnosEfeito(EfeitoID id) const;
    const EfeitoStatus* encontrarEfeito(EfeitoID id) const;


    void definirDefendendo(bool d) { combate.estaDefendendo = d; }
    bool obterDefendendo() const { return combate.estaDefendendo; }
    void definirRecargaDefesa(bool r) { combate.recargaDefesa = r; }
    bool obterRecargaDefesa() const { return combate.recargaDefesa; }
    void desequiparEscudo() { escudo = nullptr; cache_.sujo = true; }
    void desequiparArma() { arma = nullptr; cache_.sujo = true; }
    void desequiparArmadura() { armadura = nullptr; cache_.sujo = true; }

    void definirMorteAnimada(bool m) { combate.morteAnimada = m; }
    bool obterMorteAnimada() const { return combate.morteAnimada; }

    void definirParryAtivado(bool p) { sistema.parryAtivado = p; }
    bool obterParryAtivado() const { return sistema.parryAtivado; }
    void definirParryModerno(bool m) { sistema.parryModerno = m; }
    bool obterParryModerno() const { return sistema.parryModerno; }

    void setAsMinion(bool minion) { sistema.isMinion = minion; }
    bool isMinion() const { return sistema.isMinion; }

    void definirDificuldade(DificuldadeJogo d) { sistema.dificuldadeAtual = d; }
    DificuldadeJogo obterDificuldade() const { return sistema.dificuldadeAtual; }
    void aplicarMultiplicadorDificuldade(double mult);

    void definirIconeJogador(char icone) { sistema.iconeJogador = icone; }
    char obterIconeJogador() const { return sistema.iconeJogador; }

    void definirCorJogador(Cor cor) { sistema.corJogador = cor; }
    Cor obterCorJogador() const { return sistema.corJogador; }

    void definirCorFundoTerminal(Cor cor) { sistema.corFundoTerminal = cor; Aparencia::corFundoAtiva = cor; }
    Cor obterCorFundoTerminal() const { return sistema.corFundoTerminal; }

    TipoAtaque obterTipoAtaque() const;
    bool habilidadeDaClasseConsomeTurno() const;

    void adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito);
    void processarEfeitosInicioTurno();
    bool podeAgir(std::string& outMotivoIncapacidade) const;

    // Preenche o vetor com os IDs de todos os efeitos ativos (evita alocações indesejadas)
    void obterIDsEfeitosAtivos(std::vector<EfeitoID>& outIDs) const;
    void limparEfeitos();
    void removerEfeito(EfeitoID id);

    int calcularDefesaBase(int danoBruto, int danoPerfurante) const;
    ResultadoDano receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, Personagem* atacante, bool aplicarPassivas);

    virtual void executarDrops(Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal);
};
