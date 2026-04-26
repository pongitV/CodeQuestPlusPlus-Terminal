#include <iostream>
#include <memory>

#include "ClasseArqueiro.h"
#include "../Inventario/FabricaDeItens.h"

std::string ClasseArqueiro::obterNomeClasse() const 
{
     return "Arqueiro"; 
}

std::vector<std::string> ClasseArqueiro::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                  = --%=-                        ",
        "                    #@* @=                       ",
        "                       %@- %*                    ",
        "                         *% *                    ",
        "                         ** *                    ",
        "                          * **                   ",
        "                         +*%*#:*                 ",
        "                  ..      .%:#@%#:               ",
        "               : =-. .       ---=**.             ",
        "            :%#.:*+::::        #  %*             ",
        "           *@#+-:::*.- - #+    %*++**            ",
        "            =:+=*#+=::+#**%*:  #+*..+.           ",
        "            +#*+*%*%*%*+%*=%@#*+*#++ .           ",
        "        #@*::+*-**++**++*=#**==+*++--:           ",
        "       %@###%*#%%:#*%%#@#%@%%#%*:#*+=-+          ",
        "        =%@%@%:%#**####@@%% %@%%   #%.-#         ",
        "               #*%*##%#@%%-##        %*:-#       ",
        "                #%*#%%@%###$          **@%**     ",
        "                *#%%@%@%#*#            -=-=      ",
        "                 #+%%%*%%%#            -@-#      ",
        "                 -%:##*%#%#=%%* * %+    --=      ",
        "      +-=-===-+%==#*@=*%*@#**::##=**.#@@  ==     ",
        "    ++*=******##*%%**%*+#***+##*%# %##@@#        ",
        "      **+*****#*#%#*%#@##*:+#%---==.%%@*         ",
        "              *@#*%#*#+**:*%**                   ",
        "             %%%%##%%#+=%@%%%=                   ",
        "           *@@%%@%%:    %%%**=* *# .%            ",
        "          +*%@@%@        %%%*+* @# @+            ",
        "          #*=@@%%@@@      @%#*#* @* #            ",
        "           %#@@**@@@@#%    %## +=    @           ",
        "                  @.@*@     ##-:+=*              ",
        "                    @@#     %##%#**              ",
        "                    #*      *#@@@%%.             ",
        "                              +@@%%@             ",
        "                                @@%%             ",
        "                                 @%%             ",
        "                                  @%%            ",
        "                                  -%#%           ",
        "                                   %%%%          ",
        "                                    #:%          ",
        "                                    ##%%         ",
        "                                    *=:*         ",
        "                                     *:*         "
    };
    return aparencia;
}

Atributos ClasseArqueiro::obterAtributosClasse() const
{
    return { 0, 10, 20, 3, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> ClasseArqueiro::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    
    int quantidadePocoes = 3;
    int porcentagemCura = 30;
    for (int i = 0; i < quantidadePocoes; ++i) {
        equipamentos.push_back(FabricaDeItens::criarItem("Pocao de Cura (" + std::to_string(porcentagemCura) + "%VM)"));
    }
    
    equipamentos.push_back(FabricaDeItens::criarItem("Arco recurvo de madeira"));
    equipamentos.push_back(FabricaDeItens::criarItem("Bracedeiras de prata"));
    equipamentos.push_back(FabricaDeItens::criarItem("Armadura leve de couro com malha"));
    return equipamentos;
}

std::string ClasseArqueiro::obterNomePassivaClasse() const 
{ return "Passos leves"; }
std::string ClasseArqueiro::obterDescricaoPassivaClasse() const 
{ return "Penalidade de armaduras e debuffs de lentidao reduzidos pela metade."; }
std::string ClasseArqueiro::obterRecargaHabilidadeClasse() const 
{ return "Recarga: 1 turno."; }

std::string ClasseArqueiro::obterNomeHabilidadeClasse() const 
{ return "Retirada com pontaria"; }
std::string ClasseArqueiro::obterDescricaoHabilidadeClasse() const 
{ return "Se afasta durante um turno, no proximo turno causa 2x dano"; }
void ClasseArqueiro::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) {
    if (u->obterRecarga()) 
    {
        std::cout << "[SISTEMA]: Habilidade em recarga\n";
        u->definirHabilidadeCancelada(true);
        return;
    }
    u->adicionarEfeito(std::make_unique<EfeitoInviolavel>(1));
    u->definirRecarga(true);
    std::cout << "[HABILIDADE]: Retirada com pontaria! Voce se afasta neste turno.\n";
}

TipoAtaque ClasseArqueiro::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseArqueiro::habilidadeConsomeTurno() const { return true; }

int ClasseArqueiro::processarPenalidadeArmaduraPassivaArqueiro(int penalidadeBase) const 
{
    return penalidadeBase / 2;
}

int ClasseArqueiro::aplicarPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const 
{
    return (destrezaAtual * 3) / 4;
}

int ClasseArqueiro::reverterPenalidadeLentidaoPassivaArqueiro(int destrezaAtual) const 
{
    return (destrezaAtual * 4) / 3;
}
