#include "ClasseBaseInimigo.h"

// --- INFORMACOES DA CLASSE ---
std::string ClasseBaseInimigo::obterNomeClasse() const 
{ 
    return "Monstro"; 
}

Atributos ClasseBaseInimigo::obterAtributosClasse() const 
{ 
    return { 0, 0, 0, 0, 0, 0, 0 }; 
}

const std::vector<std::string>& ClasseBaseInimigo::obterAparenciaClasseMenu() const 
{ 
    static const std::vector<std::string> aparenciaVazia = {};
    return aparenciaVazia;
}

std::vector<std::unique_ptr<Item>> ClasseBaseInimigo::obterEquipamentoClasse() const 
{ 
    return {}; 
}

// --- PASSIVA DA CLASSE ---
std::string ClasseBaseInimigo::obterNomePassivaClasse() const { return "Nenhuma"; }
std::string ClasseBaseInimigo::obterDescricaoPassivaClasse() const { return "Inimigos nao possuem passivas de classe."; }

// --- HABILIDADE DA CLASSE ---
std::string ClasseBaseInimigo::obterRecargaHabilidadeClasse() const { return ""; }
std::string ClasseBaseInimigo::obterNomeHabilidadeClasse() const { return "Nenhuma"; }
std::string ClasseBaseInimigo::obterDescricaoHabilidadeClasse() const { return "Inimigos basicos nao possuem habilidades ativas."; }
void ClasseBaseInimigo::usarHabilidadeClasse(Personagem* /*personagemUsuario*/, std::vector<Personagem*>& /*listaDeInimigos*/) {}

TipoAtaque ClasseBaseInimigo::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseBaseInimigo::habilidadeConsomeTurno() const { return true; }







