#include "Arma.h"

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
    if (nome.find("(Penetrante)") != std::string::npos) ef += " | +Penetracao";
    return " (Dano: " + std::to_string(danoFisico) + "F/" + std::to_string(danoMagico) + "M" + ef + ")";
}

void Arma::aplicarEfeitoSangramento() { efeitoSangramento = true; }
void Arma::aplicarEfeitoLentidao() { efeitoLentidao = true; }