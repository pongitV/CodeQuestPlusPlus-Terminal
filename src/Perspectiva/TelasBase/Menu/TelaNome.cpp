#include "TelaNome.h"
#include "../../TelasBase/RegistroTelas.h"

TelaNome::Resultado TelaNome::exibir() {
    return RegistroTelas::telaNome();
}
