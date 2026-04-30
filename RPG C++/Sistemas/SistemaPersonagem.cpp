#include <iostream>
#include <algorithm>
#include <cctype>
#include <memory>

#include "SistemaPersonagem.h"
#include "../Racas/RacaBase.h"
#include "../Classes/ClasseBase.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "Constantes.h"

SistemaPersonagem::SistemaPersonagem(std::string nome, std::unique_ptr<RacaBase> r, std::unique_ptr<ClasseBase> c) 
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
    this->ouroRecompensa = Constantes::OURO_RECOMPENSA_INICIAL;
    
    this->nivel = 1;
    this->xpAtual = 0;
    this->xpParaSubir = Constantes::XP_BASE_PARA_SUBIR;
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

SistemaPersonagem::~SistemaPersonagem() 
{
}  

bool SistemaPersonagem::subirDeNivel(TipoAtributo atributo)
{
    if (xpAtual < xpParaSubir) return false;

    bool upou = false;
    switch (atributo)
    {
        case TipoAtributo::Vida:
            statsFinais.vida += Constantes::GANHO_VIDA_POR_NIVEL;
            vidaAtual += Constantes::GANHO_VIDA_POR_NIVEL;
            upou = true;
            break;
        case TipoAtributo::Forca: statsFinais.forca += Constantes::GANHO_ATRIBUTO_POR_NIVEL; upou = true; break;
        case TipoAtributo::Destreza: statsFinais.destreza += Constantes::GANHO_ATRIBUTO_POR_NIVEL; destrezaCacheDirty_ = true; upou = true; break;
        case TipoAtributo::Resistencia: statsFinais.resistencia += Constantes::GANHO_ATRIBUTO_POR_NIVEL; reducaoPercentualCacheDirty_ = true; upou = true; break;
        case TipoAtributo::Constituicao: statsFinais.constituicao += Constantes::GANHO_ATRIBUTO_POR_NIVEL; reducaoPercentualCacheDirty_ = true; upou = true; break;
        case TipoAtributo::Inteligencia: statsFinais.inteligencia += Constantes::GANHO_ATRIBUTO_POR_NIVEL; upou = true; break;
        case TipoAtributo::Sabedoria: statsFinais.sabedoria += Constantes::GANHO_ATRIBUTO_POR_NIVEL; upou = true; break;
    }

    if (upou)
    {
        xpAtual -= xpParaSubir;
        xpParaSubir = static_cast<int>(xpParaSubir * Constantes::MULTIPLICADOR_XP_POR_NIVEL);
        nivel++;
        return true;
    }
    return false;
}

void SistemaPersonagem::alterarAtributoEstatico(TipoAtributo atributo, int valor)
{
    switch (atributo) {
        case TipoAtributo::Forca: statsFinais.forca += valor; break;
        case TipoAtributo::Destreza: statsFinais.destreza += valor; destrezaCacheDirty_ = true; break;
        case TipoAtributo::Inteligencia: statsFinais.inteligencia += valor; break;
        case TipoAtributo::Sabedoria: statsFinais.sabedoria += valor; break;
        default: break;
    }

    if (statsFinais.forca < 0) statsFinais.forca = 0;
    if (statsFinais.destreza < 0) statsFinais.destreza = 0;
    if (statsFinais.inteligencia < 0) statsFinais.inteligencia = 0;
    if (statsFinais.sabedoria < 0) statsFinais.sabedoria = 0;
}

void SistemaPersonagem::reduzirCooldowns()
{
    if (recargaDefesa) recargaDefesa = false;
    if (recargaHabilidade) recargaHabilidade = false;
    if (cooldownsAtivos.empty()) return;
    for (auto& par : cooldownsAtivos)
    {
        if (par.second > 0) par.second--;
    }
}

void SistemaPersonagem::calcularAtributos()
{
    this->statsFinais.calcularAtributos(raca->obterAtributosRaca());
    this->statsFinais.calcularAtributos(classe->obterAtributosClasse());
    this->vidaAtual = statsFinais.vida;
    destrezaCacheDirty_ = true;
    reducaoPercentualCacheDirty_ = true;
}

int SistemaPersonagem::obterDestreza() const
{
    if (!destrezaCacheDirty_) return destrezaCache_;
    int penalidade = armadura ? (armadura->obterReducaoFixa() / 3) : 0;
    if (classe) penalidade = classe->processarPenalidadeArmaduraPassivaArqueiro(penalidade);
    int destrezaFinal = statsFinais.destreza - penalidade;
    destrezaCache_ = destrezaFinal > 0 ? destrezaFinal : 0;
    destrezaCacheDirty_ = false;
    return destrezaCache_;
}

void SistemaPersonagem::definirMultiplicador(double m) 
{ 
    if (classe) {
        multiplicadorAtual = classe->processarMultiplicadorBuffPassivaBardo(m);
    } else {
        multiplicadorAtual = m;
    }
}

void SistemaPersonagem::aplicarMultiplicadorDificuldade(double mult)
{
    if (mult <= 1.0) return;
    this->statsFinais.vida = static_cast<int>(this->statsFinais.vida * mult);
    this->statsFinais.forca = static_cast<int>(this->statsFinais.forca * mult);
    this->statsFinais.destreza = static_cast<int>(this->statsFinais.destreza * mult);
    destrezaCacheDirty_ = true;
    this->statsFinais.resistencia = static_cast<int>(this->statsFinais.resistencia * mult);
    reducaoPercentualCacheDirty_ = true;
    this->statsFinais.constituicao = static_cast<int>(this->statsFinais.constituicao * mult);
    reducaoPercentualCacheDirty_ = true;
    this->statsFinais.inteligencia = static_cast<int>(this->statsFinais.inteligencia * mult);
    this->statsFinais.sabedoria = static_cast<int>(this->statsFinais.sabedoria * mult);
    this->vidaAtual = this->statsFinais.vida;
}

void SistemaPersonagem::modificarVida(int valor) 
{
    if (valor > 0 && classe) valor = classe->processarCuraPassivaBardo(valor);

    this->vidaAtual += valor;
    if (this->vidaAtual < 0) this->vidaAtual = 0;
    if (this->vidaAtual > statsFinais.vida) this->vidaAtual = statsFinais.vida;
}

const EfeitoStatus* SistemaPersonagem::encontrarEfeito(const std::string& nome) const {
    for (const auto& ef : efeitosAtivos) {
        if (ef->obterNome() == nome) return ef.get();
    }
    return nullptr;
}

bool SistemaPersonagem::possuiEfeito(const std::string& nome) const {
    return encontrarEfeito(nome) != nullptr;
}

int SistemaPersonagem::obterTurnosEfeito(const std::string& nome) const {
    const EfeitoStatus* ef = encontrarEfeito(nome);
    return ef ? ef->obterTurnosRestantes() : 0;
}

void SistemaPersonagem::mostrarStatus() const 
{
    std::cout << "[" << nomePersonagem << "] HP: " << vidaAtual << "/" << statsFinais.vida << std::endl;
}

std::string SistemaPersonagem::obterNomeClasse() const 
{
    return this->classe->obterNomeClasse();
}

TipoClasse SistemaPersonagem::obterTipoClasse() const 
{
    if (this->classe) return this->classe->obterTipoClasse();
    return TipoClasse::Nenhum;
}

TipoRaca SistemaPersonagem::obterTipoRaca() const 
{
    if (this->raca) return this->raca->obterTipoRaca();
    return TipoRaca::Nenhum;
}

void SistemaPersonagem::equiparItem(Item* item)
{
    if (item == nullptr) return;
    if (item->obterTipo() == TipoEquipamento::ARMA) this->arma = item;
    else if (item->obterTipo() == TipoEquipamento::ESCUDO) this->escudo = item;
    else if (item->obterTipo() == TipoEquipamento::ARMADURA)
    {
        this->armadura = item;
        destrezaCacheDirty_ = true;
        reducaoPercentualCacheDirty_ = true;
    }
}

RacaBase* SistemaPersonagem::obterRaca() const 
{
    return this->raca.get();
}

ClasseBase* SistemaPersonagem::obterClasse() const 
{
    return this->classe.get();
}

TipoAtaque SistemaPersonagem::obterTipoAtaque() const 
{
    if (this->classe) return this->classe->obterTipoAtaque();
    return TipoAtaque::UNICO;
}

bool SistemaPersonagem::habilidadeDaClasseConsomeTurno() const 
{
    if (this->classe) return this->classe->habilidadeConsomeTurno();
    return true;
}

int SistemaPersonagem::calcularDefesaBase(int danoBruto, int danoPerfurante) const {
    int danoSemPerfuracao = danoBruto - danoPerfurante;
    if (danoSemPerfuracao < 0) danoSemPerfuracao = 0;

    // Cache reducao fixa (resistencia + armadura)
    if (reducaoPercentualCacheDirty_) {
        int bonusArmadura = armadura ? armadura->obterReducaoFixa() : 0;
        reducaoPercentualCache_ = statsFinais.resistencia + bonusArmadura;
        double percentualReducao = statsFinais.constituicao / 100.0;
        if (percentualReducao > 0.50) percentualReducao = 0.50;
        reducaoPercentualCache_ = static_cast<int>(reducaoPercentualCache_ * (1.0 - percentualReducao));
        reducaoPercentualCacheDirty_ = false;
    }

    int danoFinal = static_cast<int>(danoSemPerfuracao - reducaoPercentualCache_);
    if (danoFinal < 1 && danoSemPerfuracao > 0) danoFinal = 1;
    else if (danoSemPerfuracao == 0) danoFinal = 0;

    return danoFinal + danoPerfurante;
}

int SistemaPersonagem::receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, SistemaPersonagem* atacante, bool aplicarPassivas) {
    int danoFinal = calcularDefesaBase(danoBruto, danoPerfurante);

    for (auto& ef : efeitosAtivos) {
        danoFinal = ef->processarDanoRecebido(danoFinal);
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

void SistemaPersonagem::adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito) {
    efeito->aoEntrar(this);
    efeitosAtivos.push_back(std::move(efeito));
}

void SistemaPersonagem::processarEfeitosInicioTurno() {
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

bool SistemaPersonagem::podeAgir() const {
    for (auto& ef : efeitosAtivos) {
        if (ef->impedeAcao()) {
            std::cout << SimplificacoesAparencia::cor(Cor::VERDE) << "[EFEITO]: " << nomePersonagem << " esta sob efeito de " << ef->obterNome() << " e nao pode agir neste turno!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
            return false;
        }
    }
    return true;
}

std::vector<std::string> SistemaPersonagem::obterNomesEfeitosAtivos() const {
    std::vector<std::string> nomes;
    nomes.reserve(efeitosAtivos.size());
    for (auto& ef : efeitosAtivos) {
        nomes.push_back(ef->obterNome());
    }
    return nomes;
}

void SistemaPersonagem::executarDrops(SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    if (this->raca) 
    {
        this->raca->realizarDrops(this, jogadorAtual, itensObtidos, ouroTotal, xpTotal);
    }
}