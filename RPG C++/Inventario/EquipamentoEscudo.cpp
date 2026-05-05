#include "EquipamentoEscudo.h"
#include <string_view>
#include <map>
#include <memory>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"

EquipamentoEscudo::EquipamentoEscudo(std::string nome, int reducaoFixa, int durabilidade, int reqResistencia, int reqSecundario, TipoAtributo tipoSecundario, int preco)
    : Item(preco), nome(nome), reducaoFixa(reducaoFixa), durabilidade(durabilidade), reqResistencia(reqResistencia), reqSecundario(reqSecundario), tipoSecundario(tipoSecundario)
{
}

std::string EquipamentoEscudo::obterNomeItem() const { return nome; }
TipoEquipamento EquipamentoEscudo::obterTipo() const { return TipoEquipamento::ESCUDO; }

int EquipamentoEscudo::obterDurabilidadeAtualEscudo() const { return durabilidade; }
int EquipamentoEscudo::obterReducaoDanoFixaEscudo() const { return reducaoFixa; }

int EquipamentoEscudo::obterReqResistencia() const { return reqResistencia; }
int EquipamentoEscudo::obterReqSecundario() const { return reqSecundario; }
TipoAtributo EquipamentoEscudo::obterTipoSecundario() const { return tipoSecundario; }

void EquipamentoEscudo::reduzirDurabilidade(int qtd) { durabilidade -= qtd; }
void EquipamentoEscudo::aumentarDurabilidade(int qtd) { durabilidade += qtd; }

bool EquipamentoEscudo::podeSerEquipadoPor(SistemaPersonagem* personagem) const {
    if (!personagem) return false;
    if (personagem->obterResistencia() < reqResistencia) return false;
    int secVal = 0;
    switch(tipoSecundario) {
        case TipoAtributo::Forca: secVal = personagem->obterForca(); break;
        case TipoAtributo::Destreza: secVal = personagem->obterDestreza(); break;
        case TipoAtributo::Inteligencia: secVal = personagem->obterInteligencia(); break;
        case TipoAtributo::Sabedoria: secVal = personagem->obterSabedoria(); break;
        default: secVal = 9999;
    }
    return secVal >= reqSecundario;
}

std::string EquipamentoEscudo::obterMensagemRequisito() const {
    return "\n[SISTEMA]: Atributos insuficientes para equipar " + nome + "!\n";
}

void EquipamentoEscudo::exibirInspecao() const {
    std::cout << "\n" << SimplificacoesAparencia::cor(Cor::CIANO) << " === " << nome << " ===" << SimplificacoesAparencia::cor(Cor::RESET) << "\n\n";
    std::cout << " > Tipo: Escudo\n";
    std::cout << " > Poder de Bloqueio: " << reducaoFixa << " (Dano bloqueado na acao 'Defender')\n";
    std::cout << " > Durabilidade Maxima: " << durabilidade << " usos\n";
    std::cout << " > Requisitos:\n";
    bool hasReq = false;
    if (reqResistencia > 0) { std::cout << "   - Resistencia Base: " << reqResistencia << "\n"; hasReq = true; }
    if (reqSecundario > 0) {
        std::string atrSec = "";
        if (tipoSecundario == TipoAtributo::Forca) atrSec = "Forca";
        else if (tipoSecundario == TipoAtributo::Destreza) atrSec = "Destreza";
        else if (tipoSecundario == TipoAtributo::Inteligencia) atrSec = "Inteligencia";
        else if (tipoSecundario == TipoAtributo::Sabedoria) atrSec = "Sabedoria";
        std::cout << "   - Atributo Secundario (" << atrSec << "): " << reqSecundario << "\n";
        hasReq = true;
    }
    if (!hasReq) std::cout << "   - Nenhum requisito.\n";
    std::cout << " > Preco de Venda: " << precoVenda << "G\n";
}

std::string EquipamentoEscudo::obterInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reducaoFixa) + " | Dur: " + std::to_string(durabilidade);
    std::string reqs = "";
    if (reqResistencia > 0 || reqSecundario > 0) {
        reqs += " | Req: ";
        if (reqResistencia > 0) reqs += std::to_string(reqResistencia) + " Res ";
        if (reqSecundario > 0) {
            reqs += std::to_string(reqSecundario) + " ";
            if (tipoSecundario == TipoAtributo::Forca) reqs += "For ";
            else if (tipoSecundario == TipoAtributo::Destreza) reqs += "Des ";
            else if (tipoSecundario == TipoAtributo::Inteligencia) reqs += "Int ";
            else if (tipoSecundario == TipoAtributo::Sabedoria) reqs += "Sab ";
        }
    }
    return info + reqs + ")";
}

std::unique_ptr<Item> EquipamentoEscudo::gerarCopiaMelhorada() const {
    auto novoEscudo = std::make_unique<EquipamentoEscudo>(nome + "+", static_cast<int>(reducaoFixa * 1.5), static_cast<int>(durabilidade * 1.5), reqResistencia, reqSecundario, tipoSecundario, precoVenda * 2);
    for (Propriedade prop : propriedades) novoEscudo->adicionarPropriedade(prop);
    novoEscudo->adicionarPropriedade(Propriedade::Melhorado);
    return novoEscudo;
}
