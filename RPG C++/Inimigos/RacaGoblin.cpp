#include "RacaGoblin.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/Item.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Material.h"

std::string RacaGoblin::obterNomeRaca() const { return "Goblin"; }
Atributos RacaGoblin::obterAtributosRaca() const { return { -40, 10, 15, 5, 5, 0, 0 }; }
std::string RacaGoblin::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string RacaGoblin::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::unique_ptr<Item>> RacaGoblin::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(std::make_unique<Arma>("Adaga artesanal de pedra", 8, 0));
    return equipamentos;
}

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

void RacaGoblin::realizarDrops(Personagem* inimigo, Personagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    int xpDrop = 40;
    int ouroDrop = 20;
    jogadorAtual->ganharXp(xpDrop);
    jogadorAtual->ganharOuro(ouroDrop);
    xpTotal += xpDrop;
    ouroTotal += ouroDrop;
    
    std::cout << "\033[43m+" << ouroDrop << "G\033[0m \033[44m+" << xpDrop << " XP\033[0m\n";

    if (inimigo->obterArma() && inimigo->obterArma()->obterNomeItem() == "Adaga artesanal de pedra") 
    {
        if ((std::rand() % 100) < 65) 
        {
            jogadorAtual->obterInventario()->adicionarItem(std::make_unique<Arma>("Adaga artesanal de pedra", 5, 0));
            std::cout << "\033[37m+1x Adaga artesanal de pedra\033[0m\n";
            itensObtidos.push_back("Adaga artesanal de pedra");
        }
    }
    
    int qtdDentes = (std::rand() % 5) + 4;
    for (int i = 0; i < qtdDentes; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(std::make_unique<Material>("Dente de goblin"));
        itensObtidos.push_back("Dente de goblin");
    }
    std::cout << "\033[37m+" << qtdDentes << "x Dente de goblin\033[0m\n";
}