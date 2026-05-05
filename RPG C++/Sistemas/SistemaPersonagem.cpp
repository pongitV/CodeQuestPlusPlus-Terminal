#include "SistemaPersonagem.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <cassert>

#include "../Classes/ClasseBase.h"
#include "../Racas/RacaBase.h"
#include "../Utilidades/Constantes.h"
#include "../Utilidades/SimplificacoesAparencia.h"

std::unordered_set<SistemaPersonagem*> SistemaPersonagem::personagensAtivos;

bool SistemaPersonagem::isValido(SistemaPersonagem* p) {
    return personagensAtivos.find(p) != personagensAtivos.end();
}

SistemaPersonagem::SistemaPersonagem(std::string nome, std::unique_ptr<RacaBase> racaEscolhida, std::unique_ptr<ClasseBase> classeEscolhida) 
{
    this->nomePersonagem = nome;
    this->raca = std::move(racaEscolhida);
    this->classe = std::move(classeEscolhida);
    this->mochila = std::make_unique<Inventario>();
    this->statsFinais = { 0, 0, 0, 0, 0, 0, 0 }; // Atributos base

    this->arma = nullptr;
    this->escudo = nullptr;
    this->armadura = nullptr;
    this->itemSelecionadoParaUso = nullptr;
    this->ouroRecompensa = Constantes::OURO_RECOMPENSA_INICIAL;
    
    this->nivel = 1;
    this->xpAtual = 0;
    this->xpParaSubir = Constantes::XP_BASE_PARA_SUBIR;
    this->xpRecompensa = 0;

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
    personagensAtivos.insert(this);
}

SistemaPersonagem::~SistemaPersonagem() 
{
    personagensAtivos.erase(this);
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
        case TipoAtributo::Destreza: statsFinais.destreza += Constantes::GANHO_ATRIBUTO_POR_NIVEL; cache_.sujo = true; upou = true; break;
        case TipoAtributo::Resistencia: statsFinais.resistencia += Constantes::GANHO_ATRIBUTO_POR_NIVEL; cache_.sujo = true; upou = true; break;
        case TipoAtributo::Constituicao: statsFinais.constituicao += Constantes::GANHO_ATRIBUTO_POR_NIVEL; cache_.sujo = true; upou = true; break;
        case TipoAtributo::Inteligencia: statsFinais.inteligencia += Constantes::GANHO_ATRIBUTO_POR_NIVEL; upou = true; break;
        case TipoAtributo::Sabedoria: statsFinais.sabedoria += Constantes::GANHO_ATRIBUTO_POR_NIVEL; upou = true; break;
    }

    if (upou)
    {
        xpAtual -= xpParaSubir;
        xpParaSubir = static_cast<int>(std::min(xpParaSubir * Constantes::MULTIPLICADOR_XP_POR_NIVEL, Constantes::MAX_XP));
        nivel++;
        return true;
    }
    return false;
}

void SistemaPersonagem::alterarAtributoEstatico(TipoAtributo atributo, int valor)
{
    switch (atributo) {
        case TipoAtributo::Forca: statsFinais.forca += valor; break;
        case TipoAtributo::Destreza: statsFinais.destreza += valor; cache_.sujo = true; break;
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
    if (combate.recargaDefesa) combate.recargaDefesa = false;
    if (combate.recargaHabilidade) combate.recargaHabilidade = false;
    if (combate.cooldownsAtivos.empty()) return;
    for (auto& par : combate.cooldownsAtivos)
    {
        if (par.second > 0) par.second--;
    }
}

void SistemaPersonagem::prepararParaNovaBatalha()
{
    combate.resetar();
    combate.vidaMaximaFixa = static_cast<int>(statsFinais.vida * sistema.dificuldadeMultiplicador);
    limparEfeitos();
}

void SistemaPersonagem::calcularAtributos()
{
    this->statsFinais.somarAtributos(raca->obterAtributosRaca());
    this->statsFinais.somarAtributos(classe->obterAtributosClasse());
    this->vidaAtual = obterVidaMaxima();
    cache_.sujo = true;
}

void SistemaPersonagem::atualizarCacheSeNecessario() const {
    if (!cache_.sujo) return;
    
    int penalidade = armadura ? (armadura->obterReducaoFixa() / 3) : 0;
    if (classe) penalidade = classe->processarPenalidadeArmaduraPassivaArqueiro(penalidade);
    
    int destrezaBase = static_cast<int>(statsFinais.destreza * sistema.dificuldadeMultiplicador);
    int destrezaFinal = destrezaBase - penalidade;
    cache_.destreza = destrezaFinal > 0 ? destrezaFinal : 0;

    int bonusArmadura = armadura ? armadura->obterReducaoFixa() : 0;
    int resistenciaBase = static_cast<int>(statsFinais.resistencia * sistema.dificuldadeMultiplicador);
    int reducao = resistenciaBase + bonusArmadura;
    
    int constBase = static_cast<int>(statsFinais.constituicao * sistema.dificuldadeMultiplicador);
    double percentualReducao = constBase / 100.0;
    if (percentualReducao > 0.50) percentualReducao = 0.50;
    cache_.reducaoPercentual = static_cast<int>(reducao * (1.0 - percentualReducao));

    cache_.sujo = false;
}

int SistemaPersonagem::obterDestreza() const
{
    atualizarCacheSeNecessario();
    return cache_.destreza;
}

void SistemaPersonagem::definirMultiplicador(double novoMultiplicador) 
{ 
    if (classe) {
        combate.multiplicadorAtual = classe->processarMultiplicadorBuffPassivaBardo(novoMultiplicador);
    } else {
        combate.multiplicadorAtual = novoMultiplicador;
    }
}

void SistemaPersonagem::aplicarMultiplicadorDificuldade(double mult)
{
    if (mult <= 1.0) return;
    sistema.dificuldadeMultiplicador = mult;
    cache_.sujo = true;
    this->vidaAtual = obterVidaMaxima();
}

void SistemaPersonagem::modificarVida(int valor) 
{
    assert(this->classe != nullptr && "Erro de Integridade: A classe do personagem nao deve ser nula ao modificar a vida!");
    if (valor > 0 && classe) valor = classe->processarCuraPassivaBardo(valor);

    int vidaAntes = this->vidaAtual;
    this->vidaAtual += valor;
    if (this->vidaAtual < 0) this->vidaAtual = 0;
    if (this->vidaAtual > obterVidaMaxima()) this->vidaAtual = obterVidaMaxima();

    if (this->vidaAtual > vidaAntes) 
    {
        combate.curaTotalRecebida += (this->vidaAtual - vidaAntes);
    }
}

const EfeitoStatus* SistemaPersonagem::encontrarEfeito(EfeitoID id) const {
    for (const auto& ef : efeitosAtivos) {
        if (ef->obterID() == id) return ef.get();
    }
    return nullptr;
}

bool SistemaPersonagem::possuiEfeito(EfeitoID id) const {
    return encontrarEfeito(id) != nullptr;
}

int SistemaPersonagem::obterTurnosEfeito(EfeitoID id) const {
    const EfeitoStatus* ef = encontrarEfeito(id);
    return ef ? ef->obterTurnosRestantes() : 0;
}

void SistemaPersonagem::mostrarStatus() const 
{
    std::cout << "[" << nomePersonagem << "] HP: " << vidaAtual << "/" << obterVidaMaxima() << std::endl;
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
    }
    cache_.sujo = true;
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

    atualizarCacheSeNecessario();

    int danoFinal = static_cast<int>(danoSemPerfuracao - cache_.reducaoPercentual);
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

    if (combate.estaDefendendo && escudo != nullptr) {
        int bloqueio = escudo->obterReducaoDanoFixaEscudo();
        std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << ">> [DEFESA]: O escudo bloqueou " << bloqueio << " de dano!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        danoFinal -= bloqueio;
        if (danoFinal < 0) danoFinal = 0;

        escudo->reduzirDurabilidade(1);
        if (escudo->obterDurabilidadeAtualEscudo() <= 0) {
            std::cout << SimplificacoesAparencia::cor(Cor::FUNDO_VERMELHO) << "[!] ALERTA: O escudo " << escudo->obterNomeItem() << " foi DESTRUIDO em pedacos!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
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

void SistemaPersonagem::limparEfeitos() {
    for (auto& ef : efeitosAtivos) {
        ef->aoSair(this); // Garante que os atributos (como Forca e Destreza) sejam restaurados
    }
    efeitosAtivos.clear();
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

void SistemaPersonagem::obterIDsEfeitosAtivos(std::vector<EfeitoID>& outIDs) const {
    outIDs.clear();
    outIDs.reserve(efeitosAtivos.size());
    for (auto& ef : efeitosAtivos) {
        outIDs.push_back(ef->obterID());
    }
}

void SistemaPersonagem::executarDrops(SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    if (this->raca) 
    {
        this->raca->realizarDrops(this, jogadorAtual, itensObtidos, ouroTotal, xpTotal);
    }
}

void SistemaPersonagem::finalizarBatalha() { 
    combate.vidaMaximaFixa = 0; 
    if (sistema.possuiRegeneracaoTroll && vidaAtual > 0 && vidaAtual < obterVidaMaxima()) {
        modificarVida(obterVidaMaxima());
        std::cout << SimplificacoesAparencia::cor(Cor::VERDE) << "\n[SISTEMA]: Seu Orgao regenerador curou completamente suas feridas apos a batalha!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        SimplificacoesAparencia::aguardarEnter();
    }
}