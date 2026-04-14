#include <iostream>

#include "ClasseArqueiro.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/PocaoCura.h"

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

std::vector<Item*> ClasseArqueiro::obterEquipamentoClasse() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Arco recurvo de madeira", 10, 0), // Dano Fisico, Dano Magico
        new Escudo("Bracedeiras de prata", 5, 3), // Reducao Fixa, Durabilidade
        new Armadura("Armadura leve de couro com malha", 5) // Reducao Fixa
    };
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
