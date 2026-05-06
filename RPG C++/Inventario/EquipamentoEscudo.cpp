#include "EquipamentoEscudo.h"
#include <string_view>
#include <map>
#include <memory>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include <vector>
#include <functional>
#include <unordered_map>

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
    std::vector<std::string> linhas;
    linhas.push_back(SimplificacoesAparencia::cor(Cor::CIANO) + " === " + nome + " ===" + SimplificacoesAparencia::cor(Cor::RESET));
    linhas.push_back(" > Tipo: Escudo");
    linhas.push_back(" > Poder de Bloqueio: " + std::to_string(reducaoFixa) + " (Dano bloqueado na acao 'Defender')");
    linhas.push_back(" > Durabilidade Maxima: " + std::to_string(durabilidade) + " usos");
    linhas.push_back(" > Requisitos:");
    bool hasReq = false;
    if (reqResistencia > 0) { linhas.push_back("   - Resistencia Base: " + std::to_string(reqResistencia)); hasReq = true; }
    if (reqSecundario > 0) {
        std::string atrSec = "";
        if (tipoSecundario == TipoAtributo::Forca) atrSec = "Forca";
        else if (tipoSecundario == TipoAtributo::Destreza) atrSec = "Destreza";
        else if (tipoSecundario == TipoAtributo::Inteligencia) atrSec = "Inteligencia";
        else if (tipoSecundario == TipoAtributo::Sabedoria) atrSec = "Sabedoria";
        linhas.push_back("   - Atributo Secundario (" + atrSec + "): " + std::to_string(reqSecundario));
        hasReq = true;
    }
    if (!hasReq) linhas.push_back("   - Nenhum requisito.");
    linhas.push_back(" > Preco de Venda: " + std::to_string(precoVenda) + "G");

    std::cout << "\n";
    SimplificacoesAparencia::imprimirCentralizado(linhas[0]);
    std::cout << "\n";
    
    std::vector<std::string> resto(linhas.begin() + 1, linhas.end());
    int maxLen = 0;
    for (const auto& l : resto) {
        int len = SimplificacoesAparencia::removerCoresANSI(l).length();
        if (len > maxLen) maxLen = len;
    }
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(resto, maxLen);
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

std::unique_ptr<Item> fabricarEquipamentoEscudo(const std::string& nome) {
    static const std::unordered_map<std::string, std::function<std::unique_ptr<Item>()>> construtores = {
        {"Escudo medio de metal", []() { return std::make_unique<EquipamentoEscudo>("Escudo medio de metal", 15, 5, 0, 0, TipoAtributo::Forca, 9); }},
        {"Barreira magica", []() { return std::make_unique<EquipamentoEscudo>("Barreira magica", 50, 2, 0, 0, TipoAtributo::Inteligencia, 3); }},
        {"Capa magica", []() { return std::make_unique<EquipamentoEscudo>("Capa magica", 6, 10, 0, 0, TipoAtributo::Sabedoria, 9); }},
        {"Bracedeiras de prata", []() { return std::make_unique<EquipamentoEscudo>("Bracedeiras de prata", 5, 3, 0, 0, TipoAtributo::Destreza, 3); }}
    };
    auto it = construtores.find(nome);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
