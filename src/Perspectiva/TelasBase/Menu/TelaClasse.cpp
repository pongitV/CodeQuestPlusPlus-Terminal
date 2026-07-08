#include "TelaClasse.h"
#include "../../TelasBase/RegistroTelas.h"

TelaClasse::Resultado TelaClasse::exibir(const std::string& nomeJogador, const std::string& nomeRaca) {
    return RegistroTelas::telaClasse(nomeJogador, nomeRaca);
}
