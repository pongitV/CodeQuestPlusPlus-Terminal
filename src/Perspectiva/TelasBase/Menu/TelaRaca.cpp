#include "TelaRaca.h"
#include "../../TelasBase/RegistroTelas.h"

TelaRaca::Resultado TelaRaca::exibir(const std::string& nomeJogador) {
    return RegistroTelas::telaRaca(nomeJogador);
}
