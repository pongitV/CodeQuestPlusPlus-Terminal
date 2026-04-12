#include <iostream>

#include "ClasseBardo.h"

std::string ClasseBardo::obterNomeClasse() const 
{
     return "Bardo"; 
}

std::vector<std::string> ClasseBardo::obterAparenciaClasseMenu() const 
{
    return 
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
}

Atributos ClasseBardo::obterAtributosClasse() const
{
    return { 0, 10, 10, 2, 10, 10, 10 };
} // Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria

std::vector<Item*> ClasseBardo::obterEquipamentoClasse() const 
{
    return 
    {
        new PocaoCura(), new PocaoCura(), new PocaoCura(),
        new Arma("Violao encantado", 0, 7), // dano fisico, dano magico
        new Escudo("Capa magica", 10, 8), // durabilidade, reducao fixa
        new Armadura("Traje de Couro e tecido nobre", 4) // reducao fixa
    };
}

std::string ClasseBardo::obterNomeHabilidadeClasse() const { return "Flashing lights"; }
std::string ClasseBardo::obterDescricaoHabilidadeClasse() const { return "Cura 30% da vida perdida e pula o turno do inimigo. Recarga: 1 turno."; }
void ClasseBardo::usarHabilidadeClasse(Personagem* u, std::vector<Personagem*>& inimigos) 
{
    if (u->obterRecarga()) 
    {
        std::cout << "[SISTEMA]: Habilidade em recarga\n";
        return; 
    }

    // Cura 30% da vida perdida
    int vidaPerdida = u->obterVidaMaxima() - u->obterVida();
    int cura = static_cast<int>(vidaPerdida * 0.3);
    u->modificarVida(cura);

    u->definirPularTurnoInimigo(true);
    u->definirRecarga(true); // Ativa recarga para o proximo turno

    std::cout << "[HABILIDADE]: !Flashing lights! Voce recuperou " << cura << " HP e encantou os inimigos, pulando o proximo turno.\n";
}
