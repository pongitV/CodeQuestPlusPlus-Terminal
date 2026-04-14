#include <iostream>

#include "ClasseGuerreiro.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/PocaoCura.h"

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
    return { 0, 2000000, 10, 2, 10, 5, 5 };
}

std::vector<Item*> ClasseGuerreiro::obterEquipamentoClasse() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Espada longa de ferro", 10, 0), // Dano Fisico, Dano Magico
        new Escudo("Escudo medio de metal", 15, 5), // Reducao Fixa, Durabilidade
        new Armadura("Armadura de malha e metal", 7) // Reducao Fixa
    };
}

std::string ClasseGuerreiro::obterNomeHabilidadeClasse() const { return "Determinacao no combate"; }
std::string ClasseGuerreiro::obterDescricaoHabilidadeClasse() const { return "Gasta seu turno para aumentar seus atributos em 1.5x por 2 turnos."; }
void ClasseGuerreiro::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) 
{
    if (u->obterTurnosBuff() <= 0) 
    {
        u->definirTurnosBuff(3);
        u->definirMultiplicador(1.5);
        std::cout << "[HABILIDADE]: Determinacao no combate! Atributos x1.5\n";
    } 
    else 
    {
        // Se ja tiver buff, acumula o multiplicador mas NAO aumenta a duracao
        double novoMult = u->obterMultiplicador() + 1.5;
        u->definirMultiplicador(novoMult);
        std::cout << "[HABILIDADE]: Furia acumulada! Multiplicador agora e x" << novoMult << ".\n";
        std::cout << "(" << u->obterTurnosBuff() - 1 << " turnos restantes).\n";
    }
}
