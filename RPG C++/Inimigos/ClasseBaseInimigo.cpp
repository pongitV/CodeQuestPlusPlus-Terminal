#include "ClasseBaseInimigo.h"

std::string ClasseBaseInimigo::obterNomeClasse() const { return "Monstro"; }
Atributos ClasseBaseInimigo::obterAtributosClasse() const { return { 0, 0, 0, 0, 0, 0, 0 }; }

const std::vector<std::string>& ClasseBaseInimigo::obterAparenciaClasseMenu() const { 
    static const std::vector<std::string> aparenciaVazia = {};
    return aparenciaVazia;
}

std::vector<std::unique_ptr<Item>> ClasseBaseInimigo::obterEquipamentoClasse() const { return {}; }

void ClasseBaseInimigo::usarHabilidadeClasse(SistemaPersonagem* /*personagemUsuario*/, std::vector<SistemaPersonagem*>& /*listaDeInimigos*/) {}

std::string ClasseBaseInimigo::obterNomeHabilidadeClasse() const { return "Nenhuma"; }
std::string ClasseBaseInimigo::obterDescricaoHabilidadeClasse() const { return "Inimigos basicos nao possuem habilidades ativas."; }

std::string ClasseBaseInimigo::obterNomePassivaClasse() const { return "Nenhuma"; }
std::string ClasseBaseInimigo::obterDescricaoPassivaClasse() const { return "Inimigos nao possuem passivas de classe."; }
std::string ClasseBaseInimigo::obterRecargaHabilidadeClasse() const { return ""; }

TipoAtaque ClasseBaseInimigo::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseBaseInimigo::habilidadeConsomeTurno() const { return true; }