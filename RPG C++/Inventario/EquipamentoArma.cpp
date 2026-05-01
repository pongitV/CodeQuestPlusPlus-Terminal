#include "EquipamentoArma.h"

#include <iostream>
#include <map>
#include <string_view>

#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include "../Utilidades/GeradorAleatorio.h"

EquipamentoArma::EquipamentoArma(std::string nome, int danoFisico, int danoMagico, int preco)
    : Item(preco), nome(nome), danoFisico(danoFisico), danoMagico(danoMagico),
      efeitoSangramento(false), efeitoLentidao(false)
{
}

std::string EquipamentoArma::obterNomeItem() const { return nome; }
void EquipamentoArma::alterarNome(const std::string& n) { nome = n; }
TipoEquipamento EquipamentoArma::obterTipo() const { return TipoEquipamento::ARMA; }

int EquipamentoArma::obterDanoFisico() const { return danoFisico; }
int EquipamentoArma::obterDanoMagico() const { return danoMagico; }

bool EquipamentoArma::possuiEfeitoSangramento() const { return efeitoSangramento; }
bool EquipamentoArma::possuiEfeitoLentidao() const { return efeitoLentidao; }

std::string EquipamentoArma::obterInfoStatus() const {
    std::string ef = "";
    if (possuiEfeitoSangramento()) ef += " | +Sangramento";
    if (possuiEfeitoLentidao()) ef += " | +Lentidao";
    if (temPropriedade(Propriedade::Penetrante)) ef += " | +Penetracao";
    return " (Dano: " + std::to_string(danoFisico) + "F/" + std::to_string(danoMagico) + "M" + ef + ")";
}

void EquipamentoArma::aplicarEfeitoSangramento() { efeitoSangramento = true; }
void EquipamentoArma::aplicarEfeitoLentidao() { efeitoLentidao = true; }

void EquipamentoArma::antesDeCausarDano(SistemaPersonagem* atacante, SistemaPersonagem* alvo) {
    if (temPropriedade(Propriedade::Penetrante) && !alvo->possuiEfeito(EfeitoID::QuebraResistencia)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoQuebraResistencia>(3));
        std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << ">> A arma de " << atacante->obterNome() << " perfurou a armadura, reduzindo a resistencia de " << alvo->obterNome() << " pela metade e a constituicao em um terco!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
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
        std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << ">> " << alvo->obterNome() << " comecou a sangrar profundamente! (3 turnos)" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }

    if (possuiEfeitoLentidao() && !alvo->possuiEfeito(EfeitoID::Lentidao)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
        std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << ">> " << alvo->obterNome() << " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }
}

int EquipamentoArma::garantirDanoMinimo(int danoFinal) {
    if (temPropriedade(Propriedade::ViolaoBase)) {
        return std::max(danoFinal, danoMagico);
    }
    return danoFinal;
}

std::unique_ptr<Item> EquipamentoArma::gerarCopiaMelhorada() const {
    auto novaArma = std::make_unique<EquipamentoArma>(nome + "+", static_cast<int>(danoFisico * 1.5), static_cast<int>(danoMagico * 1.5), precoVenda * 2);
    
    for (Propriedade prop : propriedades) novaArma->adicionarPropriedade(prop);
    novaArma->adicionarPropriedade(Propriedade::Melhorado);
    
    if (efeitoSangramento) novaArma->aplicarEfeitoSangramento();
    if (efeitoLentidao) novaArma->aplicarEfeitoLentidao();
    
    return novaArma;
}
