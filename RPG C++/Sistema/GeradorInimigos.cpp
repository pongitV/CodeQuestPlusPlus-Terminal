#include <string>
#include <vector>

#include "GeradorInimigos.h"
#include "../Raças/RacaBase.h"
#include "../Raças/RacaOrk.h"
#include "../Classes/ClasseBase.h"
#include "../Inventario/Item.h"

class ClasseInimigoPadrao : public ClasseBase
{
public:
    std::string obterNomeClasse() const override { return "Monstro"; }
    Atributos obterAtributosClasse() const override { return { 0, 0, 0, 0, 0, 0, 0 }; }
    std::vector<std::string> obterAparenciaClasseMenu() const override { return {}; }
    std::vector<Item*> obterEquipamentoClasse() const override { return {}; }

    void usarHabilidadeClasse(Personagem* usuario, std::vector<Personagem*>& inimigos) override 
    {
    }

    std::string obterNomeHabilidadeClasse() const override 
    { 
        return "Nenhuma"; 
    }

    std::string obterDescricaoHabilidadeClasse() const override 
    { 
        return "Inimigos basicos nao possuem habilidades ativas."; 
    }
};

class RacaGoblin : public RacaBase
{
public:
    std::string obterNomeRaca() const override { return "Goblin"; }
    Atributos obterAtributosRaca() const override { return { -70, 10, 10, 3, 0, -10, -8 }; }
    
    std::string obterNomeHabilidadeRaca() const override { return "Nenhuma"; }
    std::string obterDescricaoHabilidadeRaca() const override { return "Monstros nao possuem passivas"; }

    std::vector<std::string> obterAparenciaRaca() const override
    {
        return
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
    }
};

Personagem* GeradorInimigos::criarInimigoGoblinPadrao()
{
    Personagem* goblin = new Personagem("Goblin", new RacaGoblin(), new ClasseInimigoPadrao());
    goblin->definirXpRecompensa(40);
    goblin->definirOuroRecompensa(20);
    return goblin;
}

std::vector<std::string> GeradorInimigos::obterArteAsciiDoGoblin()
{
    RacaGoblin temp;
    return temp.obterAparenciaRaca();
}

std::vector<Personagem*> GeradorInimigos::criarHordaDeGoblins(int quantidadeDeGoblins)
{
    std::vector<Personagem*> listaDeGoblinsGerados;
    for (int indiceAtual = 0; indiceAtual < quantidadeDeGoblins; indiceAtual++) listaDeGoblinsGerados.push_back(criarInimigoGoblinPadrao());
    return listaDeGoblinsGerados;
}

class RacaOrkMiniBoss : public RacaOrk
{
public:
    std::string obterNomeRaca() const override { return "Ork (Mini-Boss)"; }
    Atributos obterAtributosRaca() const override { return { 50, 25, 5, 20, 10, 0, 3 }; }
};

Personagem* GeradorInimigos::criarInimigoOrkMiniBoss()
{
    Personagem* ork = new Personagem("Ork (Mini-Boss)", new RacaOrkMiniBoss(), new ClasseInimigoPadrao());
    ork->definirXpRecompensa(120);
    ork->definirOuroRecompensa(50);
    return ork;
}