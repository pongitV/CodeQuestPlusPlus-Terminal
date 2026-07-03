#include "TelaMapaMundial.h"
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaMapaIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaMapaRaycaster.h"
#include "../../../Sistemas/Progresso/Progressao.h"
#include "../../../Sistemas/Progresso/ProgressaoFlags.h"

ProximaTransicaoMapa TelaMapaMundial::exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino) {
    int idAtual = (localAtual == LocalizacaoMapa::VilaInicial) ? 0 :
                  (localAtual == LocalizacaoMapa::Floresta) ? 1 :
                  (localAtual == LocalizacaoMapa::PonteReino) ? 2 : 3;

    bool mapasDescobertos = Progressao::instancia().obterFlag(Flags::Mapas_Descobertos);

    while (true) {
        int idSelecionado;
        if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
            idSelecionado = TelaMapaRaycaster::exibir(localAtual, progressoVila, progressoFloresta, progressoPonteReino, progressoReino);
        } else {
            idSelecionado = TelaMapaIDE::exibir(localAtual, progressoVila, progressoFloresta, progressoPonteReino, progressoReino);
        }

        if (idSelecionado < 0) return ProximaTransicaoMapa::Nenhuma;

        if (idSelecionado > 0 && !mapasDescobertos) continue;

        if (idSelecionado == idAtual) continue;

        return (idSelecionado == 0) ? ProximaTransicaoMapa::Vila :
               (idSelecionado == 1) ? ProximaTransicaoMapa::Floresta :
               (idSelecionado == 2) ? ProximaTransicaoMapa::PonteReino :
                                      ProximaTransicaoMapa::Reino;
    }
}
