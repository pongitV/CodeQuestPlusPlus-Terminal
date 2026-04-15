#include <iostream>
#include <memory>

#include "ClasseArqueiro.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/ItemConsumivel.h"

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
    // Ordem: { Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria }
    return { 0, 10, 20, 2, 10, 5, 5 };
}

std::vector<std::unique_ptr<Item>> ClasseArqueiro::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(std::make_unique<ItemConsumivel>("Pocao de Cura (30%)"));
    equipamentos.push_back(std::make_unique<ItemConsumivel>("Pocao de Cura (30%)"));
    equipamentos.push_back(std::make_unique<ItemConsumivel>("Pocao de Cura (30%)"));
    equipamentos.push_back(std::make_unique<Arma>("Arco recurvo de madeira", 10, 0));
    equipamentos.push_back(std::make_unique<Escudo>("Bracedeiras de prata", 5, 3));
    equipamentos.push_back(std::make_unique<Armadura>("Armadura leve de couro com malha", 5));
    return equipamentos;
}

std::string ClasseArqueiro::obterNomeHabilidadeClasse() const { return "Retirada com pontaria"; }
std::string ClasseArqueiro::obterDescricaoHabilidadeClasse() const { return "Se afasta durante um turno, no proximo turno causa 2x dano"; }
void ClasseArqueiro::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& /*inimigos*/) {
    if (u->obterRecarga()) 
    {
        std::cout << "[SISTEMA]: Habilidade em recarga\n";
        return;
    }
    u->definirInviolavel(true);
    u->definirRecarga(true);
    std::cout << "[HABILIDADE]: Retirada com pontaria! Voce se afasta neste turno.\n";
}

TipoAtaque ClasseArqueiro::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseArqueiro::habilidadeConsomeTurno() const { return true; }
