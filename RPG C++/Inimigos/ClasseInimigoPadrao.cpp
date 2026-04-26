#include "ClasseInimigoPadrao.h"

std::string ClasseInimigoPadrao::obterNomeClasse() const { return "Monstro"; }
Atributos ClasseInimigoPadrao::obterAtributosClasse() const { return { 0, 0, 0, 0, 0, 0, 0 }; }
std::vector<std::string> ClasseInimigoPadrao::obterAparenciaClasseMenu() const { return {}; }
std::vector<std::unique_ptr<Item>> ClasseInimigoPadrao::obterEquipamentoClasse() const { return {}; }

void ClasseInimigoPadrao::usarHabilidadeClasse(Personagem* /*usuario*/, std::vector<Personagem*>& /*inimigos*/) {}

std::string ClasseInimigoPadrao::obterNomeHabilidadeClasse() const { return "Nenhuma"; }
std::string ClasseInimigoPadrao::obterDescricaoHabilidadeClasse() const { return "Inimigos basicos nao possuem habilidades ativas."; }

std::string ClasseInimigoPadrao::obterNomePassivaClasse() const { return "Nenhuma"; }
std::string ClasseInimigoPadrao::obterDescricaoPassivaClasse() const { return "Inimigos nao possuem passivas de classe."; }
std::string ClasseInimigoPadrao::obterRecargaHabilidadeClasse() const { return ""; }

TipoAtaque ClasseInimigoPadrao::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseInimigoPadrao::habilidadeConsomeTurno() const { return true; }