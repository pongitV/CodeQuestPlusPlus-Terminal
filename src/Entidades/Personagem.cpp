#include "Personagem.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <cassert>

#include "Classes/ClasseBase.h"
#include "Racas/RacaBase.h"
#include "Classes/CloneNecro.h"
#include "../Core/Utilidades/Constantes.h"
#include "../Perspectiva/TelasBase/Combate/TelaCombate.h"
#include "../Core/Utilidades/Aparencia.h"

std::unordered_set<Personagem*> Personagem::personagensAtivos;

bool Personagem::isValido(Personagem* p) {
    return personagensAtivos.find(p) != personagensAtivos.end();
}

Personagem::Personagem(const Personagem& other)
    : nomePersonagem(other.nomePersonagem),
      vidaAtual(other.vidaAtual),
      raca(std::make_unique<RacaClone>(other.raca ? other.raca->obterNomeRaca() : "Desconhecido", other.raca ? other.raca->obterAparenciaRaca() : std::vector<std::string>())),
      classe(std::make_unique<ClasseClone>()),
      statsFinais(other.statsFinais),
      mochila(std::make_unique<Inventario>()),
      itemSelecionadoParaUso(nullptr),
      sistemaDeNivel(std::make_unique<SistemaDeNivel>(other.sistemaDeNivel->obterNivel(), other.sistemaDeNivel->obterXpAtual(), other.sistemaDeNivel->obterXpParaSubir()))
{
    sistema = other.sistema;
    
    combate.estaDefendendo = other.combate.estaDefendendo;
    // almasColetadas nao sao copiadas
    combate.recargaDefesa = other.combate.recargaDefesa;
    combate.recargaHabilidade = other.combate.recargaHabilidade;
    combate.pularTurnoInimigo = other.combate.pularTurnoInimigo;
    combate.habilidadeCancelada = other.combate.habilidadeCancelada;
    combate.morteAnimada = other.combate.morteAnimada;
    combate.multiplicadorAtual = other.combate.multiplicadorAtual;
    combate.curaTotalRecebida = other.combate.curaTotalRecebida;
    combate.vidaMaximaFixa = other.combate.vidaMaximaFixa;
    combate.cooldownsAtivos = other.combate.cooldownsAtivos;

    cache_ = other.cache_;
    personagensAtivos.insert(this);

    // Copia dos Itens (Conforme regra: "mas possui os mesmos items")
    for (const auto& par : other.equipamentos) {
        if (par.second) {
            auto copiaItem = FabricaItens::criarItem(Aparencia::removerCoresANSI(par.second->obterNomeItem()));
            if (copiaItem) { 
                this->equipamentos[par.first] = copiaItem.get(); 
                this->mochila->adicionarItem(std::move(copiaItem)); 
            }
        }
    }
    atualizarCacheSeNecessario();
}

Personagem::Personagem(const std::string& nome, std::unique_ptr<RacaBase> racaEscolhida, std::unique_ptr<ClasseBase> classeEscolhida)
    : nomePersonagem(nome),
      vidaAtual(0),
      raca(std::move(racaEscolhida)),
      classe(std::move(classeEscolhida)),
      statsFinais{ 0, 0, 0, 0, 0, 0, 0 },
      mochila(std::make_unique<Inventario>()),
      itemSelecionadoParaUso(nullptr),
      sistemaDeNivel(std::make_unique<SistemaDeNivel>(1, 0, Constantes::XP_BASE_PARA_SUBIR))
{
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

Personagem::~Personagem() 
{
    personagensAtivos.erase(this);
}  

std::unique_ptr<Personagem> Personagem::clone() const {
    return std::make_unique<Personagem>(*this);
}

void Personagem::escalarAtributos(double fator) {
    statsFinais.vida = std::max(1, static_cast<int>(statsFinais.vida * fator));
    statsFinais.forca = static_cast<int>(statsFinais.forca * fator);
    statsFinais.destreza = static_cast<int>(statsFinais.destreza * fator);
    statsFinais.resistencia = static_cast<int>(statsFinais.resistencia * fator);
    statsFinais.constituicao = static_cast<int>(statsFinais.constituicao * fator);
    statsFinais.inteligencia = static_cast<int>(statsFinais.inteligencia * fator);
    statsFinais.sabedoria = static_cast<int>(statsFinais.sabedoria * fator);
    combate.vidaMaximaFixa = statsFinais.vida; 
    forcarRecalculoCache();
    vidaAtual = obterVidaMaxima();
}

void Personagem::adicionarAlma(std::unique_ptr<Personagem> alma) { combate.almasColetadas.push_back(std::move(alma)); }

std::vector<std::unique_ptr<Personagem>>& Personagem::obterAlmas() { return combate.almasColetadas; }

size_t Personagem::obterNumeroDeAlmas() const { return combate.almasColetadas.size(); }

std::unique_ptr<Personagem> Personagem::removerAlma(int index) {
    if (index < 0 || index >= static_cast<int>(combate.almasColetadas.size())) return nullptr;
    auto alma = std::move(combate.almasColetadas[index]);
    combate.almasColetadas.erase(combate.almasColetadas.begin() + index);
    return alma;
}

int* Personagem::obterPonteiroAtributoEstatico(TipoAtributo atributo) {
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

bool Personagem::subirDeNivel(TipoAtributo atributo)
{
    if (sistemaDeNivel->obterXpAtual() < sistemaDeNivel->obterXpParaSubir()) return false;

    if (atributo == TipoAtributo::Vida) {
        statsFinais.vida += Constantes::GANHO_VIDA_POR_NIVEL;
        vidaAtual += Constantes::GANHO_VIDA_POR_NIVEL;
    } else if (int* attr = obterPonteiroAtributoEstatico(atributo)) {
        *attr += Constantes::GANHO_ATRIBUTO_POR_NIVEL;
    } else {
        return false;
    }

    sistemaDeNivel->definirXpAtual(sistemaDeNivel->obterXpAtual() - sistemaDeNivel->obterXpParaSubir());
    sistemaDeNivel->definirXpParaSubir(static_cast<int>(std::min(sistemaDeNivel->obterXpParaSubir() * Constantes::MULTIPLICADOR_XP_POR_NIVEL, Constantes::MAX_XP)));
    sistemaDeNivel->definirNivel(sistemaDeNivel->obterNivel() + 1);
    cache_.sujo = true;
    return true;
}

void Personagem::alterarAtributoEstatico(TipoAtributo atributo, int valor)
{
    if (int* attr = obterPonteiroAtributoEstatico(atributo)) {
        *attr = std::max(0, *attr + valor);
        cache_.sujo = true;
    }
}

void Personagem::reduzirCooldowns()
{
    if (combate.recargaDefesa) combate.recargaDefesa = false;
    if (combate.recargaHabilidade) combate.recargaHabilidade = false;
    if (combate.cooldownsAtivos.empty()) return;
    for (auto& par : combate.cooldownsAtivos)
    {
        if (par.second > 0) par.second--;
    }
}

void Personagem::prepararParaNovaBatalha()
{
    combate.resetar();
    combate.vidaMaximaFixa = obterVidaMaxima();
    limparEfeitos();
    
    if (obterArmadura() && obterArmadura()->temPropriedade(Propriedade::ArmaduraAdaptacao)) {
        adicionarEfeito(std::make_unique<EfeitoRodaAdaptacao>());
    }
}

void Personagem::calcularAtributos()
{
    this->statsFinais.somarAtributos(raca->obterAtributosRaca());
    this->statsFinais.somarAtributos(classe->obterAtributosClasse());
    this->vidaAtual = obterVidaMaxima();
    cache_.sujo = true;
}

void Personagem::atualizarCacheSeNecessario() const {
    if (!cache_.sujo) return;
    
    double mult = sistema.dificuldadeMultiplicador;
    auto aplicarMult = [mult](int val) { return static_cast<int>(val * mult); };

    cache_.vidaMaxima = aplicarMult(statsFinais.vida);
    cache_.forca = aplicarMult(statsFinais.forca);
    cache_.resistencia = aplicarMult(statsFinais.resistencia);
    cache_.constituicao = aplicarMult(statsFinais.constituicao);
    cache_.inteligencia = aplicarMult(statsFinais.inteligencia);
    cache_.sabedoria = aplicarMult(statsFinais.sabedoria);

    int penalidade = obterArmadura() ? (obterArmadura()->obterReducaoFixa() / 3) : 0;
    if (obterArmadura() && obterArmadura()->obterNomeItem() == "Armadura de bau") penalidade = 10;
    if (classe) penalidade = classe->processarPenalidadeArmaduraPassivaArqueiro(penalidade);
    
    int destrezaBase = static_cast<int>(statsFinais.destreza * mult);
    int destrezaFinal = destrezaBase - penalidade;
    cache_.destreza = destrezaFinal > 0 ? destrezaFinal : 0;

    int bonusArmadura = obterArmadura() ? obterArmadura()->obterReducaoFixa() : 0;
    int reducao = cache_.resistencia + bonusArmadura;
    
    double percentualReducao = cache_.constituicao / 100.0;
    if (percentualReducao > 0.50) percentualReducao = 0.50;
    cache_.reducaoPercentual = static_cast<int>(reducao * (1.0 - percentualReducao));

    cache_.sujo = false;
}

void Personagem::definirMultiplicador(double novoMultiplicador) 
{ 
    if (classe) {
        combate.multiplicadorAtual = classe->processarMultiplicadorBuffPassivaBardo(novoMultiplicador);
    } else {
        combate.multiplicadorAtual = novoMultiplicador;
    }
}

void Personagem::aplicarMultiplicadorDificuldade(double mult)
{
    if (mult <= 1.0) return;
    sistema.dificuldadeMultiplicador = mult;
    cache_.sujo = true;
    this->vidaAtual = obterVidaMaxima();
}

void Personagem::modificarVida(int valor) 
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

const EfeitoStatus* Personagem::encontrarEfeito(EfeitoID id) const {
    auto it = std::find_if(efeitosAtivos.begin(), efeitosAtivos.end(), [id](const auto& ef) {
        return ef->obterID() == id;
    });
    return it != efeitosAtivos.end() ? it->get() : nullptr;
}

bool Personagem::possuiEfeito(EfeitoID id) const {
    return encontrarEfeito(id) != nullptr;
}

int Personagem::obterTurnosEfeito(EfeitoID id) const {
    const EfeitoStatus* ef = encontrarEfeito(id);
    return ef ? ef->obterTurnosRestantes() : 0;
}

void Personagem::mostrarStatus() const 
{
    std::cout << "[" << nomePersonagem << "] HP: " << vidaAtual << "/" << obterVidaMaxima() << std::endl;
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
    if (item->obterTipo() == TipoEquipamento::ARMA) this->equipamentos[SlotEquipamento::MAO_PRINCIPAL] = item;
    else if (item->obterTipo() == TipoEquipamento::ESCUDO) this->equipamentos[SlotEquipamento::MAO_SECUNDARIA] = item;
    else if (item->obterTipo() == TipoEquipamento::ARMADURA)
    {
        this->equipamentos[SlotEquipamento::ARMADURA] = item;
        if (combate.vidaMaximaFixa > 0 && item->temPropriedade(Propriedade::ArmaduraAdaptacao)) {
            if (!possuiEfeito(EfeitoID::RodaAdaptacao)) {
                adicionarEfeito(std::make_unique<EfeitoRodaAdaptacao>());
            }
        }
    }
    else if (item->obterTipo() == TipoEquipamento::CONSUMIVEL) this->equipamentos[SlotEquipamento::CONSUMIVEL] = item;
    cache_.sujo = true;
}

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

int Personagem::calcularDefesaBase(int danoBruto, int danoPerfurante) {
    int danoSemPerfuracao = std::max(0, danoBruto - danoPerfurante);

    atualizarCacheSeNecessario();

    int danoFinal = static_cast<int>(danoSemPerfuracao - cache_.reducaoPercentual);
    if (danoFinal < 1 && danoSemPerfuracao > 0) danoFinal = 1;
    else if (danoSemPerfuracao == 0) danoFinal = 0;

    return danoFinal + danoPerfurante;
}

ResultadoDano Personagem::receberDano(int danoBruto, int danoPerfurante, int danoReduzidoParry, IAttacker* atacante, bool aplicarPassivas) {
    ResultadoDano resultado;

    int danoFinal = calcularDefesaBase(danoBruto, danoPerfurante);

    for (auto& ef : efeitosAtivos) {
        danoFinal = ef->processarDanoRecebido(danoFinal);
    }

    danoFinal = std::max(0, danoFinal - danoReduzidoParry);

    if (combate.estaDefendendo && obterEscudo() != nullptr) {
        Item* esc = obterEscudo();
        resultado.danoBloqueado = esc->obterReducaoDanoFixaEscudo();
        danoFinal = std::max(0, danoFinal - resultado.danoBloqueado);

        esc->reduzirDurabilidade(1);
        if (esc->obterDurabilidadeAtualEscudo() <= 0) {
            resultado.escudoQuebrou = true;
            resultado.nomeEscudoQuebrado = esc->obterNomeItem();
            mochila->removerItem(esc);
            desequiparEscudo();
        }
    }

    if (aplicarPassivas && raca) danoFinal = raca->processarDanoDefensivo(danoFinal, this);
    
    if (atacante) danoFinal = atacante->garantirDanoMinimo(danoFinal);

    if (danoFinal > 0) modificarVida(-danoFinal);

    resultado.danoFinal = danoFinal;
    return resultado;
}

void Personagem::adicionarEfeito(std::unique_ptr<EfeitoStatus> efeito) {
    efeito->aoEntrar(this);
    if (processandoEfeitos) {
        efeitosFilaAdicao.push_back(std::move(efeito));
    } else {
        efeitosAtivos.push_back(std::move(efeito));
    }
    cache_.sujo = true;
}

void Personagem::processarEfeitosInicioTurno() {
    processandoEfeitos = true;
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
    processandoEfeitos = false;

    for (EfeitoID id : efeitosFilaRemocao) {
        removerEfeito(id);
    }
    efeitosFilaRemocao.clear();

    for (auto& ef : efeitosFilaAdicao) {
        efeitosAtivos.push_back(std::move(ef));
    }
    efeitosFilaAdicao.clear();
}

void Personagem::limparEfeitos() {
    for (auto& ef : efeitosAtivos) {
        ef->aoSair(this); // Garante que os atributos (como Forca e Destreza) sejam restaurados
    }
    efeitosAtivos.clear();
    efeitosFilaAdicao.clear();
    efeitosFilaRemocao.clear();
    cache_.sujo = true;
}

void Personagem::removerEfeito(EfeitoID id) {
    if (processandoEfeitos) {
        efeitosFilaRemocao.push_back(id);
        return;
    }
    auto it = std::find_if(efeitosAtivos.begin(), efeitosAtivos.end(),
        [id](const std::unique_ptr<EfeitoStatus>& ef) {
            return ef->obterID() == id;
        });
    if (it != efeitosAtivos.end()) {
        (*it)->aoSair(this);
        efeitosAtivos.erase(it);
        cache_.sujo = true;
    }
}

bool Personagem::podeAgir(std::string& outMotivoIncapacidade) const {
    auto it = std::find_if(efeitosAtivos.begin(), efeitosAtivos.end(), [](const auto& ef) {
        return ef->impedeAcao();
    });
    if (it != efeitosAtivos.end()) {
        outMotivoIncapacidade = (*it)->obterNome();
        return false;
    }
    return true;
}

void Personagem::obterIDsEfeitosAtivos(std::vector<EfeitoID>& outIDs) const {
    outIDs.clear();
    outIDs.reserve(efeitosAtivos.size());
    std::transform(efeitosAtivos.begin(), efeitosAtivos.end(), std::back_inserter(outIDs), [](const auto& ef) {
        return ef->obterID();
    });
}

void Personagem::executarDrops(Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal) {
    if (raca) {
        raca->realizarDrops(this, jogadorAtual, itensObtidos, ouroTotal, xpTotal);
    }
}

int Personagem::garantirDanoMinimo(int danoAtual) {
    if (obterArma()) {
        return obterArma()->garantirDanoMinimo(danoAtual);
    }
    return danoAtual;
}

std::pair<int, int> Personagem::calcularDanoOfensivoBase() {
    double multiplicadorDeAtributos = obterMultiplicador();

    int danoFisicoDaArma = 1;
    int danoMagicoDaArma = 0;
    int perfuranteAtual = 0;

    if (obterArma()) 
    {
        danoFisicoDaArma = obterArma()->obterDanoFisico();
        danoMagicoDaArma = obterArma()->obterDanoMagico();

        if (obterArma()->temPropriedade(Propriedade::Magica)) {
            int bonusMagico = danoFisicoDaArma / 2;
            double bonusEscalado = bonusMagico * (1.0 + (obterSabedoria() / 100.0));
            perfuranteAtual = static_cast<int>(bonusEscalado * multiplicadorDeAtributos);
        }
    }

    int forcaEfetiva = obterForca();
    int destrezaEfetiva = obterDestreza();
    int inteligenciaEfetiva = obterInteligencia();
    int sabedoriaEfetiva = obterSabedoria();

    if (danoFisicoDaArma == 0 && danoMagicoDaArma > 0) {
        forcaEfetiva /= 10; destrezaEfetiva /= 10;
    } else if (danoFisicoDaArma > 0 && danoMagicoDaArma == 0) {
        inteligenciaEfetiva /= 10; sabedoriaEfetiva /= 10;
    }

    int danoFisicoCalculado = std::max(0, static_cast<int>((danoFisicoDaArma + forcaEfetiva) * (1.0 + (destrezaEfetiva / 100.0))));
    int danoMagicoCalculado = std::max(0, static_cast<int>((danoMagicoDaArma + inteligenciaEfetiva) * (1.0 + (sabedoriaEfetiva / 100.0))));
    
    int total = std::max(1, danoFisicoCalculado + danoMagicoCalculado);
    int totalFinal = static_cast<int>(total * multiplicadorDeAtributos);
    int perfuranteFinal = perfuranteAtual;

    if (obterArma() && obterArma()->temPropriedade(Propriedade::IgnoraDefesa)) {
        perfuranteFinal = totalFinal;
    }

    if (raca && raca->ignoraEscudo()) {
        perfuranteFinal = totalFinal;
    }

    if (possuiEfeito(EfeitoID::MiraCerteira)) {
        totalFinal *= 2;
        perfuranteFinal *= 2;
        removerEfeito(EfeitoID::MiraCerteira);
    }

    return { totalFinal, perfuranteFinal };
}

void Personagem::finalizarBatalha() { 
    combate.vidaMaximaFixa = 0; 
    if (sistema.possuiRegeneracaoTroll && vidaAtual > 0 && vidaAtual < obterVidaMaxima()) {
        modificarVida(obterVidaMaxima());
        std::cout << "\n" << TelaCombate::margemCombate() << Aparencia::cor(Cor::VERDE) << "[SISTEMA]: Seu Orgao regenerador curou completamente suas feridas apos a batalha!" << Aparencia::cor(Cor::RESET) << "\n";
        ControleDeInput::aguardarEnter();
    }
}

bool Personagem::isBoss() const {
    TipoRaca t = obterTipoRaca();
    return (t == TipoRaca::Mahoraga || 
            t == TipoRaca::OrkExilado || 
            t == TipoRaca::Troll || 
            t == TipoRaca::AbominacaoFloresta);
}
