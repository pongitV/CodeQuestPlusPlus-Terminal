#include "TelaDificuldade.h"
#include "../../TelasBase/RegistroTelas.h"

TelaDificuldade::Resultado TelaDificuldade::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    return RegistroTelas::telaDificuldade(nomeJogador, nomeRaca, nomeClasse);
}
