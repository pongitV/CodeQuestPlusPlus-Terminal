#include "CloneNecro.h"

RacaClone::RacaClone(const std::string& n, const std::vector<std::string>& a) : nomeOriginal(n), aparenciaOriginal(a) {}
std::string RacaClone::obterNomeRaca() const { return nomeOriginal; }
TipoRaca RacaClone::obterTipoRaca() const { return TipoRaca::Nenhum; }
const std::vector<std::string>& RacaClone::obterAparenciaRaca() const { return aparenciaOriginal; }
Atributos RacaClone::obterAtributosRaca() const { return {0,0,0,0,0,0,0}; }
std::string RacaClone::obterNomeHabilidadeRaca() const { return "Ataque Basico"; }
std::string RacaClone::obterDescricaoHabilidadeRaca() const { return "Ataca o inimigo."; }

std::string ClasseClone::obterNomeClasse() const { return "Morto-Vivo"; }
TipoClasse ClasseClone::obterTipoClasse() const { return TipoClasse::Nenhum; }
const std::vector<std::string>& ClasseClone::obterAparenciaClasseMenu() const { static std::vector<std::string> empty; return empty; }
Atributos ClasseClone::obterAtributosClasse() const { return {0,0,0,0,0,0,0}; }
std::vector<std::unique_ptr<Item>> ClasseClone::obterEquipamentoClasse() const { return {}; }
std::string ClasseClone::obterNomePassivaClasse() const { return "Decomposicao"; }
std::string ClasseClone::obterDescricaoPassivaClasse() const { return "O corpo reanimado se decompoe continuamente, perdendo 15% da Vida Maxima a cada turno do invocador."; }
std::string ClasseClone::obterRecargaHabilidadeClasse() const { return ""; }
std::string ClasseClone::obterNomeHabilidadeClasse() const { return ""; }
std::string ClasseClone::obterDescricaoHabilidadeClasse() const { return ""; }
void ClasseClone::usarHabilidadeClasse(Combate*, Personagem*, std::vector<Personagem*>&) {}