#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>

#include "Personagem.h"
#include "../Raças/RacaBase.h"
#include "../Classes/ClasseBase.h"

Personagem::Personagem(std::string nome, std::unique_ptr<RacaBase> r, std::unique_ptr<ClasseBase> c) 
{
    this->nomePersonagem = nome;
    this->raca = std::move(r);
    this->classe = std::move(c);
    this->mochila = std::make_unique<Inventario>();
    this->statsFinais = { 100, 0, 0, 0, 0, 0, 0 }; // Atributos base

    this->arma = nullptr;
    this->escudo = nullptr;
    this->armadura = nullptr;
    this->itemSelecionadoParaUso = nullptr;
    this->ouroRecompensa = 15;
    
    this->nivel = 1;
    this->xpAtual = 0;
    this->xpParaSubir = 100;
    this->xpRecompensa = 0;

    this->multiplicadorAtual = 1.0;
    this->estaDefendendo = false;
    this->recargaDefesa = false;
    this->estaInviolavel = false;
    this->recargaHabilidade = false;
    this->pularTurnoInimigo = false;
    this->parryAtivado = false;
    this->dificuldadeAtual = 2; // Padrao: Normal
    this->habilidadeCancelada = false;
    this->querVoltarProMenu = false;


    auto kit = this->classe->obterEquipamentoClasse();
    for (auto& itemUnique : kit) 
    {
        Item* ptr = itemUnique.get();
        this->mochila->adicionarItem(std::move(itemUnique)); 
        this->equiparItem(ptr);            
    }

    auto kitRaca = this->raca->obterEquipamentoRaca();
    for (auto& itemUnique : kitRaca) 
    {
        Item* ptr = itemUnique.get();
        this->mochila->adicionarItem(std::move(itemUnique)); 
        this->equiparItem(ptr);            
    }

    calcularAtributos();

    this->podeReviver = true; // Habilidade de raça "Espirito indomavel" ativa por padrão
}

Personagem::~Personagem() 
{
}  

bool Personagem::subirDeNivel(TipoAtributo atributo) 
{
    if (xpAtual < xpParaSubir) return false;

    bool upou = false;
    switch (atributo) 
    {
        case TipoAtributo::Vida: 
            statsFinais.vida += 20;
            vidaAtual += 20;
            upou = true;
            break;
        case TipoAtributo::Forca: statsFinais.forca += 1; upou = true; break;
        case TipoAtributo::Destreza: statsFinais.destreza += 1; upou = true; break;
        case TipoAtributo::Resistencia: statsFinais.resistencia += 1; upou = true; break;
        case TipoAtributo::Constituicao: statsFinais.constituicao += 1; upou = true; break;
        case TipoAtributo::Inteligencia: statsFinais.inteligencia += 1; upou = true; break;
        case TipoAtributo::Sabedoria: statsFinais.sabedoria += 1; upou = true; break;
    }

    if (upou) 
    {
        xpAtual -= xpParaSubir;
        xpParaSubir = static_cast<int>(xpParaSubir * 1.5);
        nivel++;
        return true;
    }
    return false;
}

void Personagem::alterarAtributoEstatico(const std::string& atributo, int valor)
{
    if (atributo == "forca") statsFinais.forca += valor;
    else if (atributo == "destreza") statsFinais.destreza += valor;
    else if (atributo == "inteligencia") statsFinais.inteligencia += valor;
    else if (atributo == "sabedoria") statsFinais.sabedoria += valor;
    
    if (statsFinais.forca < 0) statsFinais.forca = 0;
    if (statsFinais.destreza < 0) statsFinais.destreza = 0;
    if (statsFinais.inteligencia < 0) statsFinais.inteligencia = 0;
    if (statsFinais.sabedoria < 0) statsFinais.sabedoria = 0;
}

void Personagem::reduzirCooldowns() 
{
    if (recargaDefesa) recargaDefesa = false;
    if (recargaHabilidade) recargaHabilidade = false;
    for (auto& par : cooldownsAtivos) 
    {
        if (par.second > 0) par.second--;
    }
}

void Personagem::calcularAtributos() 
{
    this->statsFinais.calcularAtributos(raca->obterAtributosRaca());
    this->statsFinais.calcularAtributos(classe->obterAtributosClasse());
    this->vidaAtual = statsFinais.vida;
}

void Personagem::aplicarMultiplicadorDificuldade(double mult) 
{
    if (mult <= 1.0) return; // Facil nao sofre alteracao (1x)
    this->statsFinais.vida = static_cast<int>(this->statsFinais.vida * mult);
    this->statsFinais.forca = static_cast<int>(this->statsFinais.forca * mult);
    this->statsFinais.destreza = static_cast<int>(this->statsFinais.destreza * mult);
    this->statsFinais.resistencia = static_cast<int>(this->statsFinais.resistencia * mult);
    this->statsFinais.constituicao = static_cast<int>(this->statsFinais.constituicao * mult);
    this->statsFinais.inteligencia = static_cast<int>(this->statsFinais.inteligencia * mult);
    this->statsFinais.sabedoria = static_cast<int>(this->statsFinais.sabedoria * mult);
    this->vidaAtual = this->statsFinais.vida;
}

void Personagem::modificarVida(int valor) 
{
    if (valor > 0 && obterTipoClasse() == TipoClasse::Bardo) 
    {
        valor = static_cast<int>(valor * 1.4);
    }

    this->vidaAtual += valor;
    if (this->vidaAtual < 0) this->vidaAtual = 0;
    if (this->vidaAtual > statsFinais.vida) this->vidaAtual = statsFinais.vida;
}

bool Personagem::possuiEfeito(const std::string& nome) const {
    for (const auto& ef : efeitosAtivos) {
        if (ef->obterNome() == nome) return true;
    }
    return false;
}

int Personagem::obterTurnosEfeito(const std::string& nome) const {
    for (const auto& ef : efeitosAtivos) {
        if (ef->obterNome() == nome) return ef->obterTurnosRestantes();
    }
    return 0;
}

void Personagem::mostrarStatus() const 
{
    std::cout << "[" << nomePersonagem << "] HP: " << vidaAtual << "/" << statsFinais.vida << std::endl;
}

std::string Personagem::obterNomeClasse() const 
{
    return this->classe->obterNomeClasse();
}

TipoClasse Personagem::obterTipoClasse() const 
{
    if (this->classe) return this->classe->obterTipoClasse();
    return TipoClasse::Nenhum;
}

TipoRaca Personagem::obterTipoRaca() const 
{
    if (this->raca) return this->raca->obterTipoRaca();
    return TipoRaca::Nenhum;
}

void Personagem::equiparItem(Item* item) 
{
    if (item == nullptr) return;
    if (item->obterTipo() == TipoEquipamento::ARMA) this->arma = item;
    else if (item->obterTipo() == TipoEquipamento::ESCUDO) this->escudo = item;
    else if (item->obterTipo() == TipoEquipamento::ARMADURA) this->armadura = item;
}

// CORREÇÕES: Adicionando as definições dos métodos que causavam o erro de linkagem

RacaBase* Personagem::obterRaca() const 
{
    return this->raca.get();
}

ClasseBase* Personagem::obterClasse() const 
{
    return this->classe.get();
}

TipoAtaque Personagem::obterTipoAtaque() const 
{
    if (this->classe) return this->classe->obterTipoAtaque();
    return TipoAtaque::UNICO;
}

bool Personagem::habilidadeDaClasseConsomeTurno() const 
{
    if (this->classe) return this->classe->habilidadeConsomeTurno();
    return true;
}

int Personagem::calcularDefesaBase(int danoBruto, int danoPerfurante) const {
    int danoSemPerfuracao = danoBruto - danoPerfurante;
    if (danoSemPerfuracao < 0) danoSemPerfuracao = 0;

    int bonusArmadura = armadura ? armadura->obterReducaoFixa() : 0;
    int reducaoFixa = statsFinais.resistencia + bonusArmadura;

    double percentualReducao = statsFinais.constituicao / 100.0;
    if (percentualReducao > 0.50) percentualReducao = 0.50; // Hardcap de 50%

    int danoFinal = static_cast<int>((danoSemPerfuracao - reducaoFixa) * (1.0 - percentualReducao));
    if (danoFinal < 1 && danoSemPerfuracao > 0) danoFinal = 1;
    else if (danoSemPerfuracao == 0) danoFinal = 0;

    return danoFinal + danoPerfurante;
}

int Personagem::receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, Personagem* atacante, bool aplicarPassivas) {
    int danoFinal = calcularDefesaBase(danoBruto, danoPerfurante);

    if (possuiEfeito("MetadeDano")) {
        danoFinal /= 2;
        std::cout << "\033[36m>> [EFEITO]: O dano foi reduzido pela metade! (Through the wire)\033[0m\n";
    }

    danoFinal -= danoReduzidoParry;
    if (danoFinal < 0) danoFinal = 0;

    if (estaDefendendo && escudo != nullptr) {
        int bloqueio = escudo->obterReducaoDanoFixaEscudo();
        std::cout << ">> [DEFESA]: O escudo bloqueou " << bloqueio << " de dano!\n";
        danoFinal -= bloqueio;
        if (danoFinal < 0) danoFinal = 0;

        escudo->reduzirDurabilidade(1);
        if (escudo->obterDurabilidadeAtualEscudo() <= 0) {
            std::cout << "[!] ALERTA: O escudo " << escudo->obterNomeItem() << " foi DESTRUIDO em pedacos!\n";
            mochila->removerItem(escudo->obterNomeItem());
            desequiparEscudo();
        }
    }

    if (aplicarPassivas && raca) danoFinal = raca->processarDanoDefensivo(danoFinal, this);
    
    if (atacante && atacante->obterArma()) danoFinal = atacante->obterArma()->garantirDanoMinimo(danoFinal);

    if (danoFinal > 0) modificarVida(-danoFinal);

    return danoFinal;
}

void Personagem::adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito) {
    efeito->aoEntrar(this);
    efeitosAtivos.push_back(std::move(efeito));
}

void Personagem::processarEfeitosInicioTurno() {
    for (auto it = efeitosAtivos.begin(); it != efeitosAtivos.end(); ) {
        (*it)->aplicarInicioTurno(this);
        (*it)->decrementarTurno();
        if ((*it)->expirou()) {
            (*it)->aoSair(this);
            it = efeitosAtivos.erase(it);
        } else {
            ++it;
        }
    }
}

bool Personagem::podeAgir() const {
    for (auto& ef : efeitosAtivos) {
        if (ef->impedeAcao()) {
            std::cout << "\033[32m[EFEITO]: " << nomePersonagem << " esta sob efeito de " << ef->obterNome() << " e nao pode agir neste turno!\033[0m\n";
            return false;
        }
    }
    return true;
}

void Personagem::executarDrops(Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    if (this->raca) 
    {
        this->raca->realizarDrops(this, jogadorAtual, itensObtidos, ouroTotal, xpTotal);
    }
}