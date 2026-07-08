#include "TelaParry.h"
#include "../../TelasBase/RegistroTelas.h"

TelaParry::Resultado TelaParry::exibir(const std::string& nomeJogador, const std::string& nomeRaca, const std::string& nomeClasse) {
    return RegistroTelas::telaParry(nomeJogador, nomeRaca, nomeClasse);
}
