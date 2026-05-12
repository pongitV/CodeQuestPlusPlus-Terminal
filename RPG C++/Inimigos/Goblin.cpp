#include "Goblin.h"
#include <iostream>
#include "../Sistemas/SistemaPersonagem.h"
#include "../Inventario/FabricaItens.h"
#include "../Utilidades/Aparencia.h"
#include "../Gerenciadores/GerenciadorDrops.h"
#include "../Utilidades/GeradorAleatorio.h"

// --- INFORMACOES DA RACA ---
std::string Goblin::obterNomeRaca() const 
{ 
    return "Goblin"; 
}

Atributos Goblin::obterAtributosRaca() const 
{ 
    return { 60, 10, 15, 5, 5, 0, 0 }; 
}

std::vector<std::unique_ptr<Item>> Goblin::obterEquipamentoRaca() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    equipamentos.push_back(FabricaItens::criarItem(ItemID::AdagaPedra));
    return equipamentos;
}

// --- HABILIDADE DA RACA ---
std::string Goblin::obterNomeHabilidadeRaca() const { return "Nenhuma"; }
std::string Goblin::obterDescricaoHabilidadeRaca() const { return "Monstros nao possuem passivas"; }

// --- APARENCIA ---
const std::vector<std::string>& Goblin::obterAparenciaRaca() const
{
    static const std::vector<std::string> aparencia =
    {
        "                =*.         *+--=:                          ",
        "                 --###+: +**=-:  :-=                        ",
        "                   +:=++=+:=-:::: ..=     =.#+-             ",
        "                   =+##%+%%-:.: =-::-+=%***-:               ",
        "                        :+###%#%*#%*%%*+=-+                 ",
        "                     +**#+%=*=*:+**%=++-=                   ",
        "                    =-=*+%+=#+*:-#*%=:-                     ",
        "                    +=*+=*#%==-.*##%+++.-=                  ",
        "                    **+=*=*#%#+%+%#=-*:=+:                  ",
        "                   =--+-##---*-=-:=-*-=-:-..                ",
        "                  =+=.  =-=::-+=-==+##  =.---               ",
        "                 *:..   *#*+===+:-*+%    --=-:              ",
        "                =. : =   #%%#*=:-=##-      -=-              ",
        "               ++=.   -  #%##*--##+#=      +=               ",
        "               *-.     #@+%##*##+-*=*     :=-.              ",
        "               +-      #%+#+*##=+==+=     *=::              ",
        "               =.     +%%++**##+-#-#*      +                ",
        "             -+.-    #%#%**#**+##.+#=.    :--               ",
        "            +==+*   ###+*#==*##%:=%=#+    *-.               ",
        "            +#+ *.  +-=*%#-=*=**###=++   *-..               ",
        "            .*=- + +*.*-  ===+*=++**:: .*=+-:               ",
        "              +*-  #+*=.   +=::-  **+%.% #%-+               ",
        "                -   +*=-   +=:     **+=  #*==               ",
        "                    *:-:    :       ==-   =.                ",
        "                     +:              --  *                  ",
        "                     =                .-                    ",
        "                    +.                -+=:                  ",
        "                   +=:                 +--+                 ",
        "              ..+-*=*=.-:            =+=##+.-:              ",
        "             ==--::+...:            =:::-=*++::             "
    };
    return aparencia;
}

// --- BESTIARIO E DROPS ---
InfoBestiario Goblin::obterInfoBestiario() const {
    return {
        "Vila Inicial", 
        "Arredores",
        "Pequenas criaturas verdes e astutas que costumam viver em bandos perto de vilarejos.",
        "Goblins adoram itens brilhantes e guardam dentes como trofeus.",
        {FabricaItens::obterNomeDeID(ItemID::DenteGoblin), FabricaItens::obterNomeDeID(ItemID::AdagaPedra), "Ouro"},
        1
    };
}

void Goblin::realizarDrops(SistemaPersonagem* inimigo, SistemaPersonagem* jogadorAtual, std::vector<std::string>& itensObtidos, int& ouroTotal, int& xpTotal)
{
    GerenciadorDrops::relatarEProcessarXpOuro(jogadorAtual, 40, 20, ouroTotal, xpTotal);

    if (inimigo->obterArma() && inimigo->obterArma()->obterNomeItem() == "Adaga artesanal de pedra") {
        GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::AdagaPedra, 1, itensObtidos, 65);
    }
    
    GerenciadorDrops::darEProcessarItem(jogadorAtual, ItemID::DenteGoblin, GeradorAleatorio::obterInteiro(4, 8), itensObtidos);
}