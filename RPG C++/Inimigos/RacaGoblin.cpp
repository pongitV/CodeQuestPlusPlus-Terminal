#include "RacaGoblin.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Material.h"

std::string RacaGoblin::obterNomeRaca() const { return "Goblin"; }
Atributos RacaGoblin::obterAtributosRaca() const { return { -60, 7, 12, 3, 3, -15, -10 }; }
std::string RacaGoblin::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string RacaGoblin::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::string> RacaGoblin::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {                    
        "                  --=++++++++*.        ..          ",
        "      #=:      .::+++++*++++++++*.. =#*+=..        ",
        "       #-*:. .+=+++++++=:====+*+**%=%=*=#==+.+     ",
        "       .=--+=:=:=+++*:==*::-***++++-=#+##*### .    ",
        "        ++-=+..-=++#*=#..@#:**++*+#+++++==+=*..    ",
        "        ===+#..@%*-+++..%%+*%%++++-%+=+====+* =    ",
        "        :=+-++%*+:=+++++*:=====++*+=+.==#+=* =     ",
        "    *..  #==#===+:===*++==+*==++++*...:+**# +      ",
        "  -*+#*+%..+#:==+%=+*+++#=*+++***...  ..=+..       ",
        " #%*=*:=*. ..%++#:::*%+=*%+++****+===...*=* .      ",
        " -=***+:=+.= -..++#+*=**%+*+**+++*+-===+==+.-      ",
        " .*.=.=+=+.+.  .+.+++-=:=*++====+*#.*+++==+..      ",
        " .=+. ...=+*.   .#====+++=====+=+*# = ...*# =      ",
        "       .. %=++##==*.:+=========++*@ .   .=-.       ",
        "           .:=+**# ..-=+=====++**%# .              ",
        "            .... +.  #*+++++*%#%##%%..             ",
        "             . .   .########%#%%%%%%%..            ",
        "                 .############%%%%%%%%..           ",
        "               .+*%#####%%%%%%%%%%%%%%%.=          ",
        "               .-=+%#@=%%%%%+%+%*.*@*=+*..         ",
        "               +=+* *.. :--=-=--+-..:+=*.+         ",
        "               .=**=.              .:=++.:         ",
        "               ..=+* .             .:=+..          ",
        "                .#=* -              *=*.=          ",
        "                 .#+ -               =*.:          ",
        "                  .=# .              ++.           ",
        "              .#===+*#              ++===%.        ",
        "         %===+=+=++#..:            ..#-======*=..  ",
        "         #*=+=#==*# =                ..%===--+++%. ",
        "          ....##...                   :.#+++#*.=   "
    };
    return aparencia;
}

void RacaGoblin::realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos)
{
    if (inimigo->obterArma() && inimigo->obterArma()->obterNomeItem() == "Adaga artesanal de pedra") 
    {
        if ((std::rand() % 100) < 65) 
        {
            jogadorAtual->obterInventario()->adicionarItem(new Arma("Adaga artesanal de pedra", 5, 0));
            std::cout << "\033[37m+1x Adaga artesanal de pedra\033[0m\n";
            itensObtidos.push_back("Adaga artesanal de pedra");
        }
    }
    
    int qtdDentes = (std::rand() % 5) + 4;
    for (int i = 0; i < qtdDentes; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(new Material("Dente de goblin"));
        itensObtidos.push_back("Dente de goblin");
    }
    std::cout << "\033[37m+" << qtdDentes << "x Dente de goblin\033[0m\n";
}