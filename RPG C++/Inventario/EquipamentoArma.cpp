#include "EquipamentoArma.h"

#include <iostream>
#include <vector>

#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/Aparencia.h"
#include "../Utilidades/GeradorAleatorio.h"
#include "../Telas/TelaCombate.h"
#include <functional>
#include <unordered_map>
#include "FabricaItens.h"

EquipamentoArma::EquipamentoArma(std::string nome, int danoFisico, int danoMagico, int reqForca, int reqDestreza, int reqInteligencia, int reqSabedoria, int preco)
    : Item(preco), nome(nome), danoFisico(danoFisico), danoMagico(danoMagico), reqForca(reqForca), reqDestreza(reqDestreza), reqInteligencia(reqInteligencia), reqSabedoria(reqSabedoria), efeitoSangramento(false), efeitoLentidao(false)
{
}

std::string EquipamentoArma::obterNomeItem() const { return nome; }
void EquipamentoArma::alterarNome(const std::string& n) { nome = n; }
TipoEquipamento EquipamentoArma::obterTipo() const { return TipoEquipamento::ARMA; }

int EquipamentoArma::obterDanoFisico() const { return danoFisico; }
int EquipamentoArma::obterDanoMagico() const { return danoMagico; }

int EquipamentoArma::obterReqForca() const { return reqForca; }
int EquipamentoArma::obterReqDestreza() const { return reqDestreza; }
int EquipamentoArma::obterReqInteligencia() const { return reqInteligencia; }
int EquipamentoArma::obterReqSabedoria() const { return reqSabedoria; }

bool EquipamentoArma::possuiEfeitoSangramento() const { return efeitoSangramento; }
bool EquipamentoArma::possuiEfeitoLentidao() const { return efeitoLentidao; }

bool EquipamentoArma::podeSerEquipadoPor(SistemaPersonagem* personagem) const {
    if (!personagem) return false;
    return personagem->obterForca() >= reqForca &&
           personagem->obterDestreza() >= reqDestreza &&
           personagem->obterInteligencia() >= reqInteligencia &&
           personagem->obterSabedoria() >= reqSabedoria;
}

std::vector<std::string> EquipamentoArma::obterDetalhesInspecao() const {
    std::vector<std::string> linhas;
    linhas.push_back(" > Tipo: Arma");
    linhas.push_back(" > Dano Fisico: " + std::to_string(danoFisico));
    linhas.push_back(" > Dano Magico: " + std::to_string(danoMagico));
    linhas.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqForca > 0) { linhas.push_back("   - Forca: " + std::to_string(reqForca)); hasReq = true; }
    if (reqDestreza > 0) { linhas.push_back("   - Destreza: " + std::to_string(reqDestreza)); hasReq = true; }
    if (reqInteligencia > 0) { linhas.push_back("   - Inteligencia: " + std::to_string(reqInteligencia)); hasReq = true; }
    if (reqSabedoria > 0) { linhas.push_back("   - Sabedoria: " + std::to_string(reqSabedoria)); hasReq = true; }
    if (!hasReq) linhas.push_back("   - Nenhum requisito.");
    
    linhas.push_back(" > Efeitos e Propriedades:");
    bool hasEfeito = false;
    if (efeitoSangramento) { linhas.push_back("   - Sangramento (Dano continuo no alvo)"); hasEfeito = true; }
    if (efeitoLentidao) { linhas.push_back("   - Lentidao (Reduz destreza do alvo)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::Penetrante)) { linhas.push_back("   - Penetrante (Reduz resistencia do alvo)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::Magica)) { linhas.push_back("   - Magica (Parte do dano ignora defesa)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::ViolaoMagico)) { linhas.push_back("   - Raizes Drenantes (Causa dano e cura o usuario)"); hasEfeito = true; }
    if (temPropriedade(Propriedade::CipoPrisao)) { linhas.push_back("   - Prisao de Cipos (Chance de atordoar alvo)"); hasEfeito = true; }
    if (!hasEfeito) linhas.push_back("   - Nenhuma propriedade extra.");
    return linhas;
}

std::string EquipamentoArma::obterInfoStatus() const {
    std::string ef = "";
    if (possuiEfeitoSangramento()) ef += " | +Sangramento";
    if (possuiEfeitoLentidao()) ef += " | +Lentidao";
    if (temPropriedade(Propriedade::Penetrante)) ef += " | +Penetracao";
    
    std::string reqs = "";
    bool hasReq = false;
    if (reqForca > 0) { reqs += std::to_string(reqForca) + " For "; hasReq = true; }
    if (reqDestreza > 0) { reqs += std::to_string(reqDestreza) + " Des "; hasReq = true; }
    if (reqInteligencia > 0) { reqs += std::to_string(reqInteligencia) + " Int "; hasReq = true; }
    if (reqSabedoria > 0) { reqs += std::to_string(reqSabedoria) + " Sab "; hasReq = true; }
    if (hasReq) reqs = " | Req: " + reqs;

    return " (Dano: " + std::to_string(danoFisico) + "F/" + std::to_string(danoMagico) + "M" + ef + reqs + ")";
}

void EquipamentoArma::aplicarEfeitoSangramento() { efeitoSangramento = true; }
void EquipamentoArma::aplicarEfeitoLentidao() { efeitoLentidao = true; }

void EquipamentoArma::antesDeCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo) {
    if (temPropriedade(Propriedade::Penetrante) && !alvo->possuiEfeito(EfeitoID::QuebraResistencia)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoQuebraResistencia>());
        TelaCombate::adicionarMensagemFixa(Aparencia::margemCombate() + Aparencia::cor(Cor::CIANO) + ">> A arma de " + atacante->obterNome() + " ativou o po magico! O ataque enfraqueceu " + alvo->obterNome() + " ate o fim do combate!" + Aparencia::cor(Cor::RESET) + "\n");
        Aparencia::registrarLogBatalha(Aparencia::cor(Cor::CIANO) + ">> A arma de " + atacante->obterNome() + " ativou o po magico! O ataque enfraqueceu " + alvo->obterNome() + " ate o fim do combate!" + Aparencia::cor(Cor::RESET));
    }
}

void EquipamentoArma::aoCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo, int danoCausado) {
    if (danoCausado <= 0) return;

    if (temPropriedade(Propriedade::ViolaoMagico) && !alvo->possuiEfeito(EfeitoID::SugaSangue)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoSugaSangue>(2, atacante));
    }

    if (temPropriedade(Propriedade::CipoPrisao) && GeradorAleatorio::rolarChance(30) && !alvo->possuiEfeito(EfeitoID::Atordoamento)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoAtordoamento>(1));
    }

    if (possuiEfeitoSangramento() && !alvo->possuiEfeito(EfeitoID::Sangramento)) {
        int danoSangramento = std::max(1, alvo->obterVidaMaxima() / 10);
        alvo->adicionarEfeito(std::make_unique<EfeitoSangramento>(3, danoSangramento));
        Cor corSangramento = (alvo->obterNomeClasse() != "Monstro") ? Cor::VERMELHO_CLARO : Cor::VERMELHO;
        TelaCombate::adicionarMensagemFixa(Aparencia::margemCombate() + Aparencia::cor(corSangramento) + ">> " + alvo->obterNome() + " comecou a sangrar profundamente! (3 turnos)" + Aparencia::cor(Cor::RESET) + "\n");
        Aparencia::registrarLogBatalha(Aparencia::cor(corSangramento) + ">> " + alvo->obterNome() + " comecou a sangrar profundamente! (3 turnos)" + Aparencia::cor(Cor::RESET));
    }

    if (possuiEfeitoLentidao() && !alvo->possuiEfeito(EfeitoID::Lentidao)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
        TelaCombate::adicionarMensagemFixa(Aparencia::margemCombate() + Aparencia::cor(Cor::MAGENTA) + ">> " + alvo->obterNome() + " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)" + Aparencia::cor(Cor::RESET) + "\n");
        Aparencia::registrarLogBatalha(Aparencia::cor(Cor::MAGENTA) + ">> " + alvo->obterNome() + " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)" + Aparencia::cor(Cor::RESET));
    }
}

int EquipamentoArma::garantirDanoMinimo(int danoFinal) {
    int minimo = 1;
    if (temPropriedade(Propriedade::ViolaoBase)) {
        minimo = std::max(minimo, danoMagico);
    }
    return std::max(danoFinal, minimo);
}

std::unique_ptr<Item> EquipamentoArma::gerarCopiaMelhorada() const {
    auto novaArma = std::make_unique<EquipamentoArma>(nome + "+", static_cast<int>(danoFisico * 1.5), static_cast<int>(danoMagico * 1.5), reqForca, reqDestreza, reqInteligencia, reqSabedoria, precoVenda * 2);
    
    for (Propriedade prop : propriedades) novaArma->adicionarPropriedade(prop);
    novaArma->adicionarPropriedade(Propriedade::Melhorado);
    
    if (efeitoSangramento) novaArma->aplicarEfeitoSangramento();
    if (efeitoLentidao) novaArma->aplicarEfeitoLentidao();
    
    return novaArma;
}

std::unique_ptr<Item> fabricarEquipamentoArma(ItemID id) {
    auto criarArma = [](ItemID id, int dFis, int dMag, int rFor, int rDes, int rInt, int rSab, int preco) {
        return std::make_unique<EquipamentoArma>(FabricaItens::obterNomeDeID(id), dFis, dMag, rFor, rDes, rInt, rSab, preco);
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::AdagaPedra, [criarArma]() { return criarArma(ItemID::AdagaPedra, 5, 0, 0, 0, 0, 0, 3); }},
        {ItemID::ArcoMadeira, [criarArma]() { return criarArma(ItemID::ArcoMadeira, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::CajadoCristal, [criarArma]() { return criarArma(ItemID::CajadoCristal, 0, 30, 0, 0, 0, 0, 3); }},
        {ItemID::VarinhaCorroida, [criarArma]() { return criarArma(ItemID::VarinhaCorroida, 0, 25, 0, 0, 0, 0, 3); }},
        {ItemID::ViolaoEncantado, [criarArma]() { 
            auto violao = criarArma(ItemID::ViolaoEncantado, 0, 10, 0, 0, 0, 0, 3); 
            violao->adicionarPropriedade(Propriedade::ViolaoBase);
            return violao; 
        }},
        {ItemID::EspadaFerro, [criarArma]() { return criarArma(ItemID::EspadaFerro, 10, 0, 0, 0, 0, 0, 3); }},
        {ItemID::MachadoGuerra, [criarArma]() { return criarArma(ItemID::MachadoGuerra, 15, 0, 10, 0, 0, 0, 3); }},
        {ItemID::GosmaAcidaArma, [criarArma]() { return criarArma(ItemID::GosmaAcidaArma, 2, 7, 0, 0, 0, 0, 3); }},
        {ItemID::TroncoAmarrotado, [criarArma]() { return criarArma(ItemID::TroncoAmarrotado, 40, 0, 25, 0, 0, 0, 30); }},
        {ItemID::EspadaCavaleiro, [criarArma]() { return criarArma(ItemID::EspadaCavaleiro, 12, 0, 0, 0, 0, 0, 0); }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
