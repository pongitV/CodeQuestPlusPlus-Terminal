#include "Elfo.h"

#include <iostream>

#include "../Utilidades/GeradorAleatorio.h"

std::string Elfo::obterNomeRaca() const 
{
    return "Elfo";
}

std::vector<std::string> Elfo::obterAparenciaRaca() const 
{
    return 
    {
        "                        ::::                          ",
        "                      =-:-..::                        ",
        "                     =-:::-.:-*  ___-                 ",
        "                     =*%:*:*.=:@###.                  ",
        "                     .**=::#:@%@#:.                   ",
        "                   *+++==#%==::+-:                    ",
        "                   .:=%##:=#*:::=:.                   ",
        "                 .  @-=*+-:=*:=:.+-                   ",
        "                   %%%%#%#-*+.:: -+=                  ",
        "                  #%%%#%#*%+*=++=:=*                  ",
        "                 +@*%%+*=#:.****.-==-                 ",
        "                -%%%%%*%=+#-=%:=%:++=.                ",
        "                *@*%@%*--=***+%#+%+===                ",
        "                .%%#@%%%#*##%**-+%*+*=                ",
        "                 .:@@@@@%%@@%@@*%%%#**                ",
        "                   *#*%%%*%%%%@@%%%#*+                ",
        "                  :%%%#@%%%%#+#%%%%*+                 ",
        "                  %%%%#**#%%#-+*@@%**                 ",
        "                  =%%*-#-*%%%%#@%#%+=                 ",
        "                  %*=*%=*%*#:=*#%%%*+.                ",
        "                  *+=:-*%#%*:.=%%#%##+                ",
        "                .%===*=@%%*-*+-:%%%%**                ",
        "                +%#+=:%%#@=--:#:=%%.%%                ",
        "                #*+*+***@@#=-::#:*#* .                ",
        "               =+*=#%%%@@@@*--=*=-#-                  ",
        "               %*+++%@%@%@@+=--=*--#=                 ",
        "              :%#**#@%@@%@@*=-==*--:*=                ",
        "              :%%*##@%@%%%@@*===*==:-=                ",
        "              .%%##%%@%%%%@@*+=++#+--..               ",
        "               %*%#%*%%##%@@*+++*%=---+               ",
        "               %*###%%%##%@@*+****+===*               ",
        "              .%%###%%###%@@******#===:               ",
        "              .%%##%@@*%%%%@******#+++=               ",
        "              :%%###@@@@@@@@*****%%=+++               ",
        "              +%####@%@@@@@@***##@@#++*:              ",
        "             .%%#@@#@@@@@@@@##*@%@@%++#*              ",
        "             :%%#@@@@@@#.:%@%*=@%@@@**#               ",
        "               +%@%%@:         @@@##.                 ",
        "             .%@%*%@@=         @%%@+                  ",
        "            @@@@%.            :%%**                   ",
        "                               @@%@                   "
    };
}

Atributos Elfo::obterAtributosRaca() const
{
    return { 90, 5, 15, 0, 10, 15, 5 };
}

std::string Elfo::obterNomeHabilidadeRaca() const { return "Agil e preciso"; }
std::string Elfo::obterDescricaoHabilidadeRaca() const { return "Possui 33% chance de causar 1.5x de dano em cada ataque"; }
int Elfo::processarDanoOfensivo(int danoBase, SistemaPersonagem* atacante) 
{
    if (GeradorAleatorio::rolarChance(33)) 
    {
        std::cout << "[PASSIVA]: Agil e preciso! Golpe critico.\n";
        return static_cast<int>(danoBase * 1.5);
    }
    return danoBase;
}