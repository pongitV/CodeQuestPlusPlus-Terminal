#include "Elfo.h"

#include <iostream>

#include "../Utilidades/GeradorAleatorio.h"
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"

// --- INFORMACOES DA RACA ---
std::string Elfo::obterNomeRaca() const 
{
    return "Elfo";
}

Atributos Elfo::obterAtributosRaca() const
{
    return { 90, 5, 15, 0, 10, 15, 5 };
}

// --- APARENCIA ---
const std::vector<std::string>& Elfo::obterAparenciaRaca() const 
{
    static const std::vector<std::string> aparencia = 
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
    return aparencia;
}

// --- HABILIDADE DA RACA ---
std::string Elfo::obterNomeHabilidadeRaca() const 
{ 
    return "Agil e preciso"; 
}

std::string Elfo::obterDescricaoHabilidadeRaca() const 
{ 
    return "Possui 33% chance de causar 1.5x de dano em cada ataque"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Elfo::processarDanoOfensivo(int danoBase, SistemaPersonagem* atacante) 
{
    if (GeradorAleatorio::rolarChance(33)) 
    {
        std::string msg = Aparencia::margemCombate() + "[PASSIVA]: Agil e preciso! Golpe critico.\n";
        TelaCombate::adicionarMensagemFixa(msg);
        return static_cast<int>(danoBase * 1.5);
    }
    return danoBase;
}