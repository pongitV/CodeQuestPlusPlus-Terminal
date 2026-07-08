#include "RegistroTelas.h"
#include "../GerenciadorPerspectiva.h"
#include "Menu/TelaTutorial.h"
#include "../PerspectivaAlteradaException.h"

// Mantendo esse método caso outros lugares o utilizem, mas o RegistroTelas em si não precisa mais dele para o fluxo principal
bool RegistroTelas::modoRaycasterAtivo() {
    return GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva();
}

void RegistroTelas::abertura() {
    while (true) {
        try {
            GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->abertura();
            return;
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

void RegistroTelas::painelLogo(const std::string& tituloDaTela, bool animarFadeIn) {
    while (true) {
        try {
            GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->painelLogo(tituloDaTela, animarFadeIn);
            return;
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

bool RegistroTelas::confirmacaoEscolha(const std::string& tipoDeEscolha, const std::string& nomeDaEscolha,
    const std::vector<std::string>& informacoesParaExibir, const std::vector<std::string>& arteAsciiParaExibir) {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->confirmacaoEscolha(tipoDeEscolha, nomeDaEscolha, informacoesParaExibir, arteAsciiParaExibir);
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

std::vector<std::string> RegistroTelas::quadroAtributos(const Atributos& stats,
    const std::string& tituloSecao, const std::string& tituloHabilidade,
    const std::string& nomeHab, const std::string& descHab,
    const std::string& tituloHabilidade2, const std::string& nomeHab2, const std::string& descHab2) {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->quadroAtributos(stats, tituloSecao, tituloHabilidade, nomeHab, descHab, tituloHabilidade2, nomeHab2, descHab2);
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

int RegistroTelas::menuPrincipal() {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->menuPrincipal();
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

void RegistroTelas::tutorialParry(const std::string& infoBox) {
    while (true) {
        try {
            GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->tutorialParry(infoBox);
            return;
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

TelaNome::Resultado RegistroTelas::telaNome() {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->telaNome();
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

bool RegistroTelas::confirmarSaida() {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->confirmarSaida();
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

TelaRaca::Resultado RegistroTelas::telaRaca(const std::string& nomePersonagem) {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->telaRaca(nomePersonagem);
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

TelaClasse::Resultado RegistroTelas::telaClasse(const std::string& nomePersonagem, const std::string& raca) {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->telaClasse(nomePersonagem, raca);
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

TelaDificuldade::Resultado RegistroTelas::telaDificuldade(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->telaDificuldade(nomePersonagem, raca, classe);
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

TelaParry::Resultado RegistroTelas::telaParry(const std::string& nomePersonagem, const std::string& raca, const std::string& classe) {
    while (true) {
        try {
            return GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->telaParry(nomePersonagem, raca, classe);
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}

void RegistroTelas::telaIntroducao() {
    while (true) {
        try {
            GerenciadorPerspectiva::obterInstancia().obterGerenciadorTelas()->telaIntroducao();
            return;
        } catch (const PerspectivaAlteradaException&) {
            continue;
        }
    }
}
