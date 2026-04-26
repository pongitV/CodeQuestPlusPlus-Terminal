#include "Arma.h"
#include "../Sistema/Personagem.h"
#include "../Sistema/Tipos.h"
#include <iostream>
#include <string_view>
#include <map>
#include "../Sistema/SimplificacoesAparencia.h"

Arma::Arma(std::string nome, int danoFisico, int danoMagico, int preco)
    : Item(preco), nome(nome), danoFisico(danoFisico), danoMagico(danoMagico),
      efeitoSangramento(false), efeitoLentidao(false)
{
}

std::string Arma::obterNomeItem() const { return nome; }
void Arma::alterarNome(const std::string& n) { nome = n; }
TipoEquipamento Arma::obterTipo() const { return TipoEquipamento::ARMA; }

int Arma::obterDanoFisico() const { return danoFisico; }
int Arma::obterDanoMagico() const { return danoMagico; }

bool Arma::possuiEfeitoSangramento() const { return efeitoSangramento; }
bool Arma::possuiEfeitoLentidao() const { return efeitoLentidao; }

std::string Arma::obterInfoStatus() const {
    std::string ef = "";
    if (possuiEfeitoSangramento()) ef += " | +Sangramento";
    if (possuiEfeitoLentidao()) ef += " | +Lentidao";
    if (temPropriedade(Propriedade::Penetrante)) ef += " | +Penetracao";
    return " (Dano: " + std::to_string(danoFisico) + "F/" + std::to_string(danoMagico) + "M" + ef + ")";
}

void Arma::aplicarEfeitoSangramento() { efeitoSangramento = true; }
void Arma::aplicarEfeitoLentidao() { efeitoLentidao = true; }

void Arma::antesDeCausarDano(Personagem* atacante, Personagem* alvo) {
    if (temPropriedade(Propriedade::Penetrante) && !alvo->possuiEfeito(EfeitoNomes::QUEBRA_RESISTENCIA)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoQuebraResistencia>(3));
        std::cout << SimplificacoesAparencia::cor(Cor::CIANO) << ">> A arma de " << atacante->obterNome() << " perfurou a armadura, reduzindo a resistencia de " << alvo->obterNome() << " pela metade e a constituicao em um terco!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }
}

void Arma::aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) {
    if (danoCausado <= 0) return;

    if (temPropriedade(Propriedade::ViolaoMagico) && !alvo->possuiEfeito(EfeitoNomes::SUGA_SANGUE)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoSugaSangue>(2, atacante));
    }

    if (temPropriedade(Propriedade::CipoPrisao) && (std::rand() % 100) < 30 && !alvo->possuiEfeito(EfeitoNomes::ATORDOAMENTO)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoAtordoamento>(1));
    }

    if (possuiEfeitoSangramento() && !alvo->possuiEfeito(EfeitoNomes::SANGRAMENTO)) {
        int danoSangramento = std::max(1, alvo->obterVidaMaxima() / 10);
        alvo->adicionarEfeito(std::make_unique<EfeitoSangramento>(3, danoSangramento));
        std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << ">> " << alvo->obterNome() << " comecou a sangrar profundamente! (3 turnos)" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }

    if (possuiEfeitoLentidao() && !alvo->possuiEfeito(EfeitoNomes::LENTIDAO)) {
        alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
        std::cout << SimplificacoesAparencia::cor(Cor::MAGENTA) << ">> " << alvo->obterNome() << " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }
}

int Arma::garantirDanoMinimo(int danoFinal) {
    if (temPropriedade(Propriedade::ViolaoBase)) {
        return std::max(danoFinal, danoMagico);
    }
    return danoFinal;
}

std::unique_ptr<Item> Arma::gerarCopiaMelhorada() const {
    auto novaArma = std::make_unique<Arma>(nome + "+", static_cast<int>(danoFisico * 1.5), static_cast<int>(danoMagico * 1.5), precoVenda * 2);
    
    for (Propriedade prop : propriedades) novaArma->adicionarPropriedade(prop);
    novaArma->adicionarPropriedade(Propriedade::Melhorado);
    
    if (efeitoSangramento) novaArma->aplicarEfeitoSangramento();
    if (efeitoLentidao) novaArma->aplicarEfeitoLentidao();
    
    return novaArma;
}
