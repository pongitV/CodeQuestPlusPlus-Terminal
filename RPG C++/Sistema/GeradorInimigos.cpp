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
    std::vector<Item*> gerarKitInicial() const override { return {}; }

    // --- NOVAS IMPLEMENTACOES OBRIGATORIAS ---

    void usarHabilidadeClasseAtiva(Personagem* usuario, std::vector<Personagem*>& inimigos) override 
    {
        // Inimigos padrao nao usam habilidades complexas por enquanto
    }

    std::string obterNomeHabilidadeClasseAtiva() const override 
    { 
        return "Nenhuma"; 
    }

    std::string obterDescricaoHabilidadeClasseAtiva() const override 
    { 
        return "Inimigos basicos nao possuem habilidades ativas."; 
    }
};

class RacaGoblin : public RacaBase
{
public:
    std::string obterNomeRaca() const override { return "Goblin"; }
    Atributos obterAtributosRaca() const override { return { -80, 10, 10, 3, 0, -10, -8 }; }
    
    std::string obterNomeHabilidadeRaca() const override { return "Nenhuma"; }
    std::string obterDescricaoHabilidadeRaca() const override { return "Monstros nao possuem passivas"; }

    std::vector<std::string> obterAparenciaRaca() const override 
    {
        return 
        {
            "                                                 ",
            "                 +-:=++++++++*.                  ",
            "      *=+     .=:+++++*+++++++**  :-=++.         ",
            "       =:*-. :+++++++=-==++++*+**=+=*=+=**+      ",
            "       .+-=*+%*==+**-=+-=::***+*===#%*****= :    ",
            "        +=*=*..#+#+*#:::@%*++++*==-=+===*++%     ",
            "        :=*=*.%%*=+++*#+*=:-:*+*:==*=+===+#      ",
            "        ==+-====-===#++=======**==*  =+++#       ",
            "   :+++- :=*:==+#-=#%*++*%%+++**      -==.:      ",
            " .=*++**#   *==+=+*+=*%=#*+++**#      .=+ .      ",
            " =+*#**==*    *++=*+=+**+++**++=+====*+===       ",
            " =*=+====+       #*+#=*=+#+====+*:++=====        ",
            "   :: ***=*-   -=====+**========*.:              ",
            "        ====+*=-==*===========+**.:              ",
            "           *==++*   =========+*#%..              ",
            "                     %%*+=+*%%#%%##+             ",
            "                   %%#####%##%%%%%%% .           ",
            "                 ############%%%%%%%% .          ",
            "               :%%%######%%%%%%%%%%%%#.          ",
            "               ===#:             *++=*=          ",
            "               ==# -              +==+=.         ",
            "               :++#-              *=+* :         ",
            "                +=* =             .==*           ",
            "                 =+.+              +*:-          ",
            "                 .+=                = :          ",
            "               .===*%              +==*.         ",
            "         .:+=====*** =             *-=====*-.    ",
            "        =*+===-=+# :.              : :==+=+=+*#  ",
            "          =-:+*#* =                  ..++=***=   ",
            "                                                 "
        };
    }
};

Personagem* GeradorInimigos::gerarGoblin()
{
    return new Personagem("Goblin", new RacaGoblin(), new ClasseInimigoPadrao());
}

std::vector<std::string> GeradorInimigos::obterGoblinASCII()
{
    RacaGoblin temp;
    return temp.obterAparenciaRaca();
}

std::vector<Personagem*> GeradorInimigos::gerarHordaGoblins(int quantidade)
{
    std::vector<Personagem*> horda;
    for (int i = 0; i < quantidade; i++) horda.push_back(gerarGoblin());
    return horda;
}

class RacaOrkMiniBoss : public RacaOrk
{
public:
    std::string obterNomeRaca() const override { return "Ork (Mini-Boss)"; }
    Atributos obterAtributosRaca() const override { return { 50, 25, 5, 20, 10, 0, 3 }; }
};

Personagem* GeradorInimigos::gerarOrk()
{
    Personagem* p = new Personagem("Ork", new RacaOrkMiniBoss(), new ClasseInimigoPadrao());
    p->definirXpRecompensa(120);
    p->definirOuroRecompensa(50);
    return p;
}