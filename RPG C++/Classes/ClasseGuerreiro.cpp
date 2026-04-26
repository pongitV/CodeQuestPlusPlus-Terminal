#include <iostream>
#include <memory>

#include "ClasseGuerreiro.h"
#include "../Inventario/FabricaDeItens.h"
#include "../Sistema/Tipos.h"
#include "../Sistema/SimplificacoesAparencia.h"

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
    return { 0, 2000000, 10, 5, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> ClasseGuerreiro::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaDeItens::criarItem("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }

    equipamentos.push_back(FabricaDeItens::criarItem("Espada longa de ferro"));
    equipamentos.push_back(FabricaDeItens::criarItem("Escudo medio de metal"));
    equipamentos.push_back(FabricaDeItens::criarItem("Armadura de malha e metal"));
    return equipamentos;
}

std::string ClasseGuerreiro::obterNomePassivaClasse() const 
{ return "Golpe decisivo"; }
std::string ClasseGuerreiro::obterDescricaoPassivaClasse() const 
{ return "Causa +10%/+20%/+30% de dano em inimigos com menos de 30%/20%/10% de HP."; }
std::string ClasseGuerreiro::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 3 turnos."; }

std::string ClasseGuerreiro::obterNomeHabilidadeClasse() const 
{ return "Grito de guerra"; }
std::string ClasseGuerreiro::obterDescricaoHabilidadeClasse() const 
{ return "Gasta seu turno para aumentar Forca e Destreza em 1.5x por 2 turnos."; }
void ClasseGuerreiro::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) 
{
    if (u->obterCooldown("Determinacao") > 0) {
        std::cout << "[SISTEMA]: Grito de guerra em recarga!\n";
        u->definirHabilidadeCancelada(true);
        return;
    }
    if (u->possuiEfeito(EfeitoNomes::GRITO_DE_GUERRA)) {
        std::cout << "[SISTEMA]: O grito de guerra ja esta ativo!\n";
        u->definirHabilidadeCancelada(true);
        return;
    }

    int bonusForca = u->obterForca() / 2;
    int bonusDestreza = u->obterDestreza() / 2;
    
    u->adicionarEfeito(std::make_unique<EfeitoGritoGuerra>(2, bonusForca, bonusDestreza));
    u->definirCooldown("Determinacao", 4); // 3 turnos de recarga
    
    std::cout << "[HABILIDADE]: Grito de guerra! Forca +" << bonusForca << " e Destreza +" << bonusDestreza << "!\n";
}

void ClasseGuerreiro::executarAtaqueComPassivaDaClasse(Personagem* atacante, Personagem* defensor, int danoBase, int danoPerfurante, std::vector<std::unique_ptr<Personagem>>& inimigos, std::function<void(Personagem*, Personagem*, int, int)> aplicarDano) 
{
    int danoFinal = danoBase;
    
    if (defensor != nullptr) {
        double percVida = (double)defensor->obterVida() / defensor->obterVidaMaxima();
        if (percVida < 0.10) {
            danoFinal = static_cast<int>(danoFinal * 1.30);
            std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[Golpe Decisivo]: O inimigo esta nas ultimas! Dano aumentado em 30%!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        } else if (percVida < 0.20) {
            danoFinal = static_cast<int>(danoFinal * 1.20);
            std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[Golpe Decisivo]: O inimigo esta gravemente ferido! Dano aumentado em 20%!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        } else if (percVida < 0.30) {
            danoFinal = static_cast<int>(danoFinal * 1.10);
            std::cout << SimplificacoesAparencia::cor(Cor::VERMELHO) << "[Golpe Decisivo]: O inimigo esta ferido! Dano aumentado em 10%!" << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
        }
    }
    
    ClasseBase::executarAtaqueComPassivaDaClasse(atacante, defensor, danoFinal, danoPerfurante, inimigos, aplicarDano);
}

TipoAtaque ClasseGuerreiro::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseGuerreiro::habilidadeConsomeTurno() const { return true; }
