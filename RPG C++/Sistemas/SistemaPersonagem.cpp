#include "SistemaPersonagem.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <cassert>

#include "../Classes/ClasseBase.h"
#include "../Racas/RacaBase.h"
#include "../Utilidades/Constantes.h"
#include "../Utilidades/Aparencia.h"

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

    auto receberEEquiparKit = [this](std::vector<std::unique_ptr<Item>> kit) {
        for (auto& itemUnique : kit) {
            Item* ptr = itemUnique.get();
            this->mochila->adicionarItem(std::move(itemUnique)); 
            this->equiparItem(ptr);            
        }
    };

    receberEEquiparKit(this->classe->obterEquipamentoClasse());
    receberEEquiparKit(this->raca->obterEquipamentoRaca());

    calcularAtributos();
    personagensAtivos.insert(this);
}

SistemaPersonagem::~SistemaPersonagem() 
{
    personagensAtivos.erase(this);
}  

int* SistemaPersonagem::obterPonteiroAtributoEstatico(TipoAtributo atributo) {
    switch (atributo) {
        case TipoAtributo::Forca: return &statsFinais.forca;
        case TipoAtributo::Destreza: return &statsFinais.destreza;
        case TipoAtributo::Resistencia: return &statsFinais.resistencia;
        case TipoAtributo::Constituicao: return &statsFinais.constituicao;
        case TipoAtributo::Inteligencia: return &statsFinais.inteligencia;
        case TipoAtributo::Sabedoria: return &statsFinais.sabedoria;
        default: return nullptr;
    }
}

bool SistemaPersonagem::subirDeNivel(TipoAtributo atributo)
{
    if (xpAtual < xpParaSubir) return false;

    if (atributo == TipoAtributo::Vida) {
        statsFinais.vida += Constantes::GANHO_VIDA_POR_NIVEL;
        vidaAtual += Constantes::GANHO_VIDA_POR_NIVEL;
    } else if (int* attr = obterPonteiroAtributoEstatico(atributo)) {
        *attr += Constantes::GANHO_ATRIBUTO_POR_NIVEL;
    } else {
        return false;
    }

    xpAtual -= xpParaSubir;
    xpParaSubir = static_cast<int>(std::min(xpParaSubir * Constantes::MULTIPLICADOR_XP_POR_NIVEL, Constantes::MAX_XP));
    nivel++;
    cache_.sujo = true;
    return true;
}

void SistemaPersonagem::alterarAtributoEstatico(TipoAtributo atributo, int valor)
{
    if (int* attr = obterPonteiroAtributoEstatico(atributo)) {
        *attr = std::max(0, *attr + valor);
        cache_.sujo = true;
    }
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
    combate.vidaMaximaFixa = obterVidaMaxima();
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
    
    double mult = sistema.dificuldadeMultiplicador;
    auto aplicarMult = [mult](int val) { return static_cast<int>(val * mult); };

    cache_.vidaMaxima = aplicarMult(statsFinais.vida);
    cache_.forca = aplicarMult(statsFinais.forca);
    cache_.resistencia = aplicarMult(statsFinais.resistencia);
    cache_.constituicao = aplicarMult(statsFinais.constituicao);
    cache_.inteligencia = aplicarMult(statsFinais.inteligencia);
    cache_.sabedoria = aplicarMult(statsFinais.sabedoria);

    int penalidade = armadura ? (armadura->obterReducaoFixa() / 3) : 0;
    if (armadura && armadura->obterNomeItem() == "Armadura de bau") penalidade = 10;
    if (classe) penalidade = classe->processarPenalidadeArmaduraPassivaArqueiro(penalidade);
    
    int destrezaBase = static_cast<int>(statsFinais.destreza * mult);
    int destrezaFinal = destrezaBase - penalidade;
    cache_.destreza = destrezaFinal > 0 ? destrezaFinal : 0;

    int bonusArmadura = armadura ? armadura->obterReducaoFixa() : 0;
    int reducao = cache_.resistencia + bonusArmadura;
    
    double percentualReducao = cache_.constituicao / 100.0;
    if (percentualReducao > 0.50) percentualReducao = 0.50;
    cache_.reducaoPercentual = static_cast<int>(reducao * (1.0 - percentualReducao));

    cache_.sujo = false;
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
    this->vidaAtual = std::clamp(this->vidaAtual + valor, 0, obterVidaMaxima());

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
    int danoSemPerfuracao = std::max(0, danoBruto - danoPerfurante);

    atualizarCacheSeNecessario();

    int danoFinal = static_cast<int>(danoSemPerfuracao - cache_.reducaoPercentual);
    if (danoFinal < 1 && danoSemPerfuracao > 0) danoFinal = 1;
    else if (danoSemPerfuracao == 0) danoFinal = 0;

    return danoFinal + danoPerfurante;
}

ResultadoDano SistemaPersonagem::receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, SistemaPersonagem* atacante, bool aplicarPassivas) {
    ResultadoDano resultado;

    int danoFinal = calcularDefesaBase(danoBruto, danoPerfurante);

    for (auto& ef : efeitosAtivos) {
        danoFinal = ef->processarDanoRecebido(danoFinal);
    }

    danoFinal = std::max(0, danoFinal - danoReduzidoParry);

    if (combate.estaDefendendo && escudo != nullptr) {
        resultado.danoBloqueado = escudo->obterReducaoDanoFixaEscudo();
        danoFinal = std::max(0, danoFinal - resultado.danoBloqueado);

        escudo->reduzirDurabilidade(1);
        if (escudo->obterDurabilidadeAtualEscudo() <= 0) {
            resultado.escudoQuebrou = true;
            resultado.nomeEscudoQuebrado = escudo->obterNomeItem();
            mochila->removerItem(escudo);
            desequiparEscudo();
        }
    }

    if (aplicarPassivas && raca) danoFinal = raca->processarDanoDefensivo(danoFinal, this);
    
    if (atacante && atacante->obterArma()) danoFinal = atacante->obterArma()->garantirDanoMinimo(danoFinal);

    if (danoFinal > 0) modificarVida(-danoFinal);

    resultado.danoFinal = danoFinal;
    return resultado;
}

void SistemaPersonagem::adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito) {
    efeito->aoEntrar(this);
    efeitosAtivos.push_back(std::move(efeito));
    cache_.sujo = true;
}

void SistemaPersonagem::processarEfeitosInicioTurno() {
    for (auto& ef : efeitosAtivos) {
        ef->aplicarInicioTurno(this);
        ef->decrementarTurno();
    }

    efeitosAtivos.erase(
        std::remove_if(efeitosAtivos.begin(), efeitosAtivos.end(),
            [this](const std::unique_ptr<EfeitoStatus>& ef) {
                if (ef->expirou()) {
                    ef->aoSair(this);
                cache_.sujo = true;
                    return true;
                }
                return false;
            }),
        efeitosAtivos.end()
    );
}

void SistemaPersonagem::limparEfeitos() {
    for (auto& ef : efeitosAtivos) {
        ef->aoSair(this); // Garante que os atributos (como Forca e Destreza) sejam restaurados
    }
    efeitosAtivos.clear();
    cache_.sujo = true;
}

bool SistemaPersonagem::podeAgir(std::string& outMotivoIncapacidade) const {
    for (auto& ef : efeitosAtivos) {
        if (ef->impedeAcao()) {
            outMotivoIncapacidade = ef->obterNome();
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
        std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE) << "[SISTEMA]: Seu Orgao regenerador curou completamente suas feridas apos a batalha!" << Aparencia::cor(Cor::RESET) << "\n";
        ControleDeInput::aguardarEnter();
    }
}