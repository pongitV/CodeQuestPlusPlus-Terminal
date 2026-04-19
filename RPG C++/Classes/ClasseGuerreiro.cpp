#include <iostream>
#include <memory>

#include "ClasseGuerreiro.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/ItemConsumivel.h"

std::string ClasseGuerreiro::obterNomeClasse() const 
{ 
    return "Guerreiro"; 
}

std::vector<std::string> ClasseGuerreiro::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                                                          ",
        "                              *###:                       ",
        "                             #+=%##%                      ",
        "                             -::+###-                     ",
        "                             +*%%+++*..                   ",
        "                             +: ==-%%.                    ",
        "                            .-%#%@@%@@%                   ",
        "                             -##@@@=:.###                 ",
        "                          =:#*##%@#:+%@@%@@=              ",
        "                        #+#*@:.#%*%@-:==-=#@              ",
        "                        *%@=:#*-: :=@*+#@#=@              ",
        "                        =++#==+=#+##@::#*=#               ",
        "                        =#*@##@%+@%@@=:+=*#               ",
        "                         =%%#%@#@%@@@===*%#               ",
        "                        =#==:==%#@@@@@+==%#               ",
        "                        =#===::+-:-####-=#*#              ",
        "                       -+#.==+=:=%###==-##-@              ",
        "                      =+#.*#@####+%#%+:=###@              ",
        "                     =:-#+@#@#%@#*@#%=+#*=                ",
        "                    :#:==-##@#%====%@#=@#*                ",
        "                    =#*+#=+#@@@@@@@@*+@@-                 ",
        "                    %#%%#+#+%##%=@@#:=@@#+                ",
        "                   ++@##=@#==%@*#*=.:=#:+#                ",
        "                   %=@@####%*=@**@:%##*@%@                ",
        "                  .==%#*=%%#@+@*#%#@@@@@@@#               ",
        "                 -==###@##=#*@%%+##@@@@@@@@               ",
        "                 @%###@%#%++@%@@%#@%@@@@%@#               ",
        "                 %%  ####=@%#@#@@@*@##@@@@@@              ",
        "                ##  ##%#+%##@@#@%@##@#@@@@%@#             ",
        "               #%-   **@@#++@%#@@%*%%%@@@@=%%             ",
        "              =#* ++#*@+=@#+@%@*@#+@@@@%%*#               ",
        "              %#      @@@@%@##*@@%+**=@@@@#=@#            ",
        "             ##       #=@@%@@@@@@+=#*@@@@%#@#%#           ",
        "            ##         =*@@@@@@@@-=*=@@@@@#@*%#*          ",
        "           ###         ::@@@@@ @@=+##@@@@@#@##%@          ",
        "          %##           %#%####  #++#%@@@@#@%*#           ",
        "         #%@            %@@#@@:  **+#+@%@@#@%@%           ",
        "         #%:            =@#%%@=  = #% %@@@@*==@#          ",
        "        ##              =##%@%         @@@@#=##           ",
        "       *#               %%#@@#         %@@@@.             ",
        "       #             *%%%%#@%@*        @@@@@+             ",
        "                  +####%@%%###       @#%%@#@@             ",
        "                                    ##%*#@@%%             ",
        "                                                          "
    };
    return aparencia;
}

Atributos ClasseGuerreiro::obterAtributosClasse() const
{
    // Ordem: { Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria }
    return { 0, 2000000, 10, 5, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> ClasseGuerreiro::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(std::make_unique<ItemConsumivel>("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }
    
    equipamentos.push_back(std::make_unique<Arma>("Espada longa de ferro", 10, 0));
    equipamentos.push_back(std::make_unique<Escudo>("Escudo medio de metal", 15, 5));
    equipamentos.push_back(std::make_unique<Armadura>("Armadura de malha e metal", 7));
    return equipamentos;
}

std::string ClasseGuerreiro::obterNomeHabilidadeClasse() const { return "Grito de guerra"; }
std::string ClasseGuerreiro::obterDescricaoHabilidadeClasse() const { return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; }
void ClasseGuerreiro::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) 
{
    if (u->obterCooldownHab1() > 0) {
        std::cout << "[SISTEMA]: Grito de guerra em recarga!\n";
        u->definirHabilidadeCancelada(true);
        return;
    }
    if (u->obterTurnosGrito() > 0) {
        std::cout << "[SISTEMA]: Grito de guerra ja esta ativo!\n";
        u->definirHabilidadeCancelada(true);
        return;
    }

    int bonusForca = u->obterForca() / 2;
    int bonusDestreza = u->obterDestreza() / 2;
    
    u->alterarAtributoEstatico("forca", bonusForca);
    u->alterarAtributoEstatico("destreza", bonusDestreza);
    
    u->definirBonusGrito(bonusForca, bonusDestreza);
    u->definirTurnosGrito(2);
    u->definirCooldownHab1(4); // 3 turnos de recarga
    
    std::cout << "[HABILIDADE]: Grito de guerra! Forca +" << bonusForca << " e Destreza +" << bonusDestreza << "!\n";
}

TipoAtaque ClasseGuerreiro::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseGuerreiro::habilidadeConsomeTurno() const { return true; }
