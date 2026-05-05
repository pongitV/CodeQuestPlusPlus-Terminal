#include "EquipamentoArmadura.h"
#include "../Sistemas/SistemaPersonagem.h"
#include "../Utilidades/SimplificacoesAparencia.h"
#include <functional>
#include <unordered_map>

EquipamentoArmadura::EquipamentoArmadura(std::string nome, int reducaoFixa, int reqResistencia, int reqConstituicao, int preco) 
    : Item(preco), nome(nome), reducaoFixa(reducaoFixa), reqResistencia(reqResistencia), reqConstituicao(reqConstituicao)
{
}

std::string EquipamentoArmadura::obterNomeItem() const { return nome; }
TipoEquipamento EquipamentoArmadura::obterTipo() const { return TipoEquipamento::ARMADURA; }

int EquipamentoArmadura::obterReducaoFixa() const { return reducaoFixa; }
int EquipamentoArmadura::obterReqResistencia() const { return reqResistencia; }
int EquipamentoArmadura::obterReqConstituicao() const { return reqConstituicao; }

bool EquipamentoArmadura::podeSerEquipadoPor(SistemaPersonagem* personagem) const {
    if (!personagem) return false;
    return personagem->obterResistencia() >= reqResistencia &&
           personagem->obterConstituicao() >= reqConstituicao;
}

std::string EquipamentoArmadura::obterMensagemRequisito() const {
    return "\n[SISTEMA]: Atributos insuficientes para equipar " + nome + "!\n";
}

void EquipamentoArmadura::exibirInspecao() const {
    std::cout << "\n" << SimplificacoesAparencia::cor(Cor::CIANO) << " === " << nome << " ===" << SimplificacoesAparencia::cor(Cor::RESET) << "\n\n";
    std::cout << " > Tipo: Armadura\n";
    std::cout << " > Defesa Fixa: " << reducaoFixa << " (Reduz dano recebido permanentemente)\n";
    std::cout << " > Requisitos:\n";
    bool hasReq = false;
    if (reqResistencia > 0) { std::cout << "   - Resistencia: " << reqResistencia << "\n"; hasReq = true; }
    if (reqConstituicao > 0) { std::cout << "   - Constituicao: " << reqConstituicao << "\n"; hasReq = true; }
    if (!hasReq) std::cout << "   - Nenhum requisito.\n";
    
    if (reducaoFixa / 3 > 0) {
        std::cout << " > Penalidade: -" << (reducaoFixa / 3) << " Destreza\n";
    } else {
        std::cout << " > Penalidade: Nenhuma\n";
    }
    std::cout << " > Preco de Venda: " << precoVenda << "G\n";
}

std::string EquipamentoArmadura::obterInfoStatus() const {
    std::string info = " (Def: " + std::to_string(reducaoFixa);
    if (int penalidadeDestreza = reducaoFixa / 3; penalidadeDestreza > 0) {
        info += " | -" + std::to_string(penalidadeDestreza) + " Dest";
    }
    
    std::string reqs = "";
    if (reqResistencia > 0 || reqConstituicao > 0) {
        reqs += " | Req: ";
        if (reqResistencia > 0) reqs += std::to_string(reqResistencia) + " Res ";
        if (reqConstituicao > 0) reqs += std::to_string(reqConstituicao) + " Con ";
    }

    return info + reqs + ")";
}

std::unique_ptr<Item> EquipamentoArmadura::gerarCopiaMelhorada() const {
    auto novaArmadura = std::make_unique<EquipamentoArmadura>(nome + "+", static_cast<int>(reducaoFixa * 1.5), reqResistencia, reqConstituicao, precoVenda * 2);
    for (Propriedade prop : propriedades) novaArmadura->adicionarPropriedade(prop);
    novaArmadura->adicionarPropriedade(Propriedade::Melhorado);
    return novaArmadura;
}

std::unique_ptr<Item> fabricarEquipamentoArmadura(const std::string& nome) {
    static const std::unordered_map<std::string, std::function<std::unique_ptr<Item>()>> construtores = {
        {"Armadura de malha e metal", []() { return std::make_unique<EquipamentoArmadura>("Armadura de malha e metal", 7, 0, 0, 3); }},
        {"Armadura leve de couro com malha", []() { return std::make_unique<EquipamentoArmadura>("Armadura leve de couro com malha", 5, 0, 0, 3); }},
        {"Tunica", []() { return std::make_unique<EquipamentoArmadura>("Tunica", 2, 0, 0, 3); }},
        {"Traje de Couro e tecido nobre", []() { return std::make_unique<EquipamentoArmadura>("Traje de Couro e tecido nobre", 4, 0, 0, 3); }},
        {"Armadura de trapos e sucata", []() { return std::make_unique<EquipamentoArmadura>("Armadura de trapos e sucata", 3, 0, 0, 3); }}
    };
    auto it = construtores.find(nome);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
