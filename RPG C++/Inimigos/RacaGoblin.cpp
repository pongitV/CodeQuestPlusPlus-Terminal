#include "RacaGoblin.h"
#include <iostream>
#include <cstdlib>
#include "../Sistema/Personagem.h"
#include "../Inventario/FabricaDeItens.h"
#include "../Sistema/SimplificacoesAparencia.h"
#include "ControleDeDrops.h"

std::string RacaGoblin::obterNomeRaca() const { return "Goblin"; }
Atributos RacaGoblin::obterAtributosRaca() const { return { -40, 10, 15, 5, 5, 0, 0 }; }
std::string RacaGoblin::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string RacaGoblin::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

std::vector<std::unique_ptr<Item>> RacaGoblin::obterEquipamentoRaca() const {
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaDeItens::criarItem("Adaga artesanal de pedra"));
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
    ControleDeDrops::relatarEProcessarXpOuro(jogadorAtual, xpDrop, ouroDrop, ouroTotal, xpTotal);

    if (inimigo->obterArma() && inimigo->obterArma()->obterNomeItem() == "Adaga artesanal de pedra") 
    {
        if ((std::rand() % 100) < 65) 
        {
            jogadorAtual->obterInventario()->adicionarItem(FabricaDeItens::criarItem("Adaga artesanal de pedra"));
            ControleDeDrops::relatarDropItem("Adaga artesanal de pedra", 1);
            itensObtidos.push_back("Adaga artesanal de pedra");
        }
    }
    
    int qtdDentes = (std::rand() % 5) + 4;
    for (int i = 0; i < qtdDentes; ++i) {
        jogadorAtual->obterInventario()->adicionarItem(FabricaDeItens::criarItem("Dente de goblin"));
        itensObtidos.push_back("Dente de goblin");
    }
    ControleDeDrops::relatarDropItem("Dente de goblin", qtdDentes);
}