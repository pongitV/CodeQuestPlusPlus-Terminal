#include <iostream>
#include <memory>

#include "ClasseBardo.h"
#include "../Inventario/Arma.h"
#include "../Inventario/Escudo.h"
#include "../Inventario/Armadura.h"
#include "../Inventario/ItemConsumivel.h"

std::string ClasseBardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

std::vector<std::string> ClasseBardo::obterAparenciaClasseMenu() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                       ..:: :                    ",
        "                      =-.-:-:=:                  ",
        "                      =:--+-:--                  ",
        "                      ::-::::*=                  ",
        "                      .. ::-+-:                  ",
        "                    :-*-:-**=:.                  ",
        "                -**+******=+=**:                 ",
        "              :*+++*++++***#*#*#-                ",
        "             +*+==++===**=*+**+*-:               ",
        "             .+==+=*+====++.-==*==               ",
        "             :=*++***+=+*+*:====++               ",
        "            +:*****#*+++===++**+**               ",
        "            :=**+*##***+=+=+=#****               ",
        "           -*****#%****++++*##****.              ",
        "           ##*#***%#*****+*%#****#:              ",
        "           ##%+**#%******%#***#***+              ",
        "          *#%###***%#+*%%#**#+*###*:             ",
        "          ##@##*:-*#*%%%%%%%%+###***:            ",
        "         =##%%**#**%%%**%%%%*%******+            ",
        "         *%#@@#@@%##%#******%********#*.         ",
        "        -#%#%@@@@%%%%%####*===****** =#=#+       ",
        "        *%%%%%@@@@%@%==---==++#*****  %**        ",
        "       *%%%%%%@@@@%*=====++*+=#****   **=        ",
        "      +*%%%#%%%@@@%*++===+*#+=####*  -**         ",
        "     **%%%%%%%%###%*+===*+%#*====-   #**         ",
        "      *%#%%%%#%###%*+==+=%%%*++==    ***         ",
        "       #%%%%%#####%*+==+#-@%**++*   :**=         ",
        "       #@%%%######%*+++*% @%**+*.  :***:         ",
        "      +%%%%%#####%#*++*#* @%*++* -#****=         ",
        "      #%%%%#####*%*****%*:%#****#*******+        ",
        "    .#*%%%##%**##%*****@*@%****%*********+       ",
        "    %=%%%%###**#@%#***#**%%####@******#**+       ",
        "     %*%%%%     @#%%#%%@%@@%%#%@***#******=      ",
        "    #%%%@@%:   :%@%%%@@+@@@@@%%@***********      ",
        "      %%@%%%   @@@%@@@@. @@%%%%#***********      ",
        "          ** %@@@@%@: -@@@@   ***#********:      ",
        "                %@@@@@   :@@@# ***********       ",
        "                @@@@@:   -@@%%  *********        ",
        "                @%%@#::= #@@@@%%##               ",
        "                #%%@+      **%@%%@@              ",
        "               #%#%@=                            ",
        "              =@#%@@-                            ",
        "              %*#@@:                             ",
        "               :=:                               "
    };
    return aparencia;
}

Atributos ClasseBardo::obterAtributosClasse() const
{
    // Ordem: { Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria }
    return { 0, 10, 10, 2, 10, 10, 10 };
}

std::vector<std::unique_ptr<Item>> ClasseBardo::obterEquipamentoClasse() const 
{
    std::vector<std::unique_ptr<Item>> equipamentos;
    for (int i = 0; i < 3; ++i) {
        auto pocao = std::make_unique<ItemConsumivel>("Pocao de Cura (30%)", 6);
        pocao->adicionarPropriedade(Propriedade::ConsumivelCura);
        equipamentos.push_back(std::move(pocao));
    }
    equipamentos.push_back(std::make_unique<Arma>("Violao encantado", 0, 10));
    equipamentos.push_back(std::make_unique<Escudo>("Capa magica", 6, 10, 9));
    equipamentos.push_back(std::make_unique<Armadura>("Traje de Couro e tecido nobre", 4));
    return equipamentos;
}

std::string ClasseBardo::obterNomeHabilidadeClasse() const { return "Flashing lights"; }
std::string ClasseBardo::obterDescricaoHabilidadeClasse() const { return "Cura 30% da vida perdida e pula o turno do inimigo. Recarga: 1 turno."; }
void ClasseBardo::usarHabilidadeClasse(Personagem& u, std::vector<Personagem*>& /*inimigos*/) 
{
    if (u.obterRecarga()) 
    {
        std::cout << "[SISTEMA]: Habilidade em recarga\n";
        return; 
    }

    // Cura 30% da vida perdida
    int vidaPerdida = u.obterVidaMaxima() - u.obterVida();
    int cura = static_cast<int>(vidaPerdida * 0.3);
    u.modificarVida(cura);

    u.definirPularTurnoInimigo(true);
    u.definirRecarga(true); // Ativa recarga para o proximo turno

    std::cout << "[HABILIDADE]: !Flashing lights! Voce recuperou " << cura << " HP e encantou os inimigos, pulando o proximo turno.\n";
}

TipoAtaque ClasseBardo::obterTipoAtaque() const { return TipoAtaque::UNICO; }
bool ClasseBardo::habilidadeConsomeTurno() const { return true; }
