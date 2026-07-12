#include "FabricaClasses.h"
#include "Arqueiro.h"
#include "Bardo.h"
#include "Guerreiro.h"
#include "Mago.h"
#include "Necromante.h"

std::unique_ptr<ClasseBase> FabricaClasses::criarClasse(TipoClasse tipo) {
    switch (tipo) {
        case TipoClasse::Arqueiro: return std::make_unique<Arqueiro>();
        case TipoClasse::Bardo: return std::make_unique<Bardo>();
        case TipoClasse::Guerreiro: return std::make_unique<Guerreiro>();
        case TipoClasse::Mago: return std::make_unique<Mago>();
        case TipoClasse::NECROMANTE: return std::make_unique<Necromante>();
        default: return nullptr;
    }
}

std::vector<TipoClasse> FabricaClasses::obterClassesJogaveis() {
    return {TipoClasse::Arqueiro, TipoClasse::Bardo, TipoClasse::Guerreiro, TipoClasse::Mago, TipoClasse::NECROMANTE};
}
