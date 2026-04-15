#include "Arma.h"
#include "../Sistema/Personagem.h"
#include <iostream>

Arma::Arma(std::string nome, int danoFisico, int danoMagico) 
    : nome(nome), danoFisico(danoFisico), danoMagico(danoMagico), 
      efeitoSangramento(false), efeitoLentidao(false) 
{
}

std::string Arma::obterNomeItem() const { return nome; }
void Arma::alterarNome(const std::string& n) { nome = n; }
Raridade Arma::obterRaridade() const { return Raridade::COMUM; }
TipoEquipamento Arma::obterTipo() const { return TipoEquipamento::ARMA; }

int Arma::obterDanoFisico() const { return danoFisico; }
int Arma::obterDanoMagico() const { return danoMagico; }

bool Arma::possuiEfeitoSangramento() const { return efeitoSangramento; }
bool Arma::possuiEfeitoLentidao() const { return efeitoLentidao; }

int Arma::obterPrecoVenda() const {
    if (nome == "Adaga artesanal de pedra") return 5;
    return 3;
}

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
    if (temPropriedade(Propriedade::Penetrante) && !alvo->obterQuebraResistencia()) {
        alvo->aplicarQuebraResistenciaEstatistica();
        std::cout << "\033[36m>> A arma de " << atacante->obterNome() << " perfurou a armadura, reduzindo a resistencia de " << alvo->obterNome() << " pela metade e a constituicao em um terco!\033[0m\n";
    }
}

void Arma::aoCausarDano(Personagem* atacante, Personagem* alvo, int danoCausado) {
    if (temPropriedade(Propriedade::ViolaoMagico) && danoCausado > 0) {
        alvo->adicionarEfeito(std::make_unique<EfeitoSugaSangue>("Raizes Sangue Suga", 1, atacante));
    }
    if (temPropriedade(Propriedade::CipoPrisao) && danoCausado > 0) {
        if ((std::rand() % 100) < 30) {
            alvo->adicionarEfeito(std::make_unique<EfeitoAtordoamento>("Cipos", 1));
        }
    }
    if (possuiEfeitoSangramento() && !alvo->obterSangramento() && danoCausado > 0) {
        alvo->definirSangramento(true);
        alvo->definirTurnosSangramento(3);
        std::cout << "\033[31m>> " << alvo->obterNome() << " comecou a sangrar profundamente! (3 turnos)\033[0m\n";
    }
    if (possuiEfeitoLentidao() && !alvo->obterLentidao() && danoCausado > 0) {
        alvo->aplicarLentidaoEstatistica();
        alvo->definirTurnosLentidao(3);
        std::cout << "\033[35m>> " << alvo->obterNome() << " foi coberto por gosma e sua destreza caiu pela metade! (3 turnos)\033[0m\n";
    }
}

int Arma::garantirDanoMinimo(int danoFinal) {
    if (temPropriedade(Propriedade::ViolaoBase)) {
        return std::max(danoFinal, danoMagico);
    }
    return danoFinal;
}
