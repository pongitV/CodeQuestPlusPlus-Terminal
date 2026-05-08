#include "Humano.h"

#include <iostream>
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"

// --- INFORMACOES DA RACA ---
std::string Humano::obterNomeRaca() const 
{
    return "Humano";
}

Atributos Humano::obterAtributosRaca() const
{
    return { 100, 10, 10, 0, 10, 10, 10 };
}

// --- APARENCIA ---
const std::vector<std::string>& Humano::obterAparenciaRaca() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                        @%%#                     ",
        "                       ##@%@#=                   ",
        "                      :=:::-=#                   ",
        "                      ::=+:+*=                   ",
        "                       ::::*=                    ",
        "                       :::-++                    ",
        "                       :::=+=                    ",
        "                   .##.=::=:--==                 ",
        "                 #####. ==...####=               ",
        "               #**#### .*...=#######             ",
        "              . +++**#-. ...####*##::            ",
        "              .. +++***....*######.-:            ",
        "              .:.+++**+:..:**#*###.:::           ",
        "              ...++****+=:#**#####.::.           ",
        "              . *++++**#-:##**%*##.::.           ",
        "             .. ++=+++**:.%*%#*%%#.::::          ",
        "             .:.:++++++*++%#+###%#.::::          ",
        "             .:::=+=+++*+.@######:.::::          ",
        "            ..::==.*#**=+*##%#*%% .::::          ",
        "             .:+=***+++#+.##%*+%#@..:::          ",
        "           ..=.:.+*+*+*++:*#**+##%:..-:.         ",
        "              .= +***++**:*@#*+##%:::.-:         ",
        "            -: =.***+**++:*%@###%%--==-          ",
        "             :==+*+*+++*#*#%%*##%%-:*+           ",
        "                :::=+++*+*+=%*##*=.              ",
        "                :::--==##=--#*+*+=               ",
        "                :::-=++##---#=====               ",
        "               ::---==*# =-=#=====               ",
        "               ::--==++#  -=#=====               ",
        "               :-:===++   =-=====.               ",
        "               ---=+++    =-=====                ",
        "              +##@*#:      %##%%%                ",
        "             -##=@#%@     =***%#%                ",
        "              #%#%%#       %%@@%                 ",
        "              ###%%:       #*%##                 ",
        "              ###%#        %#%%                  ",
        "              ##%#         ###*                  ",
        "              #%%%         ###:                  ",
        "             :#%%-        :###                   ",
        "            :@#%%         +#%#                   ",
        "           @@%###-        =##*+                  ",
        "         %#@@@:.           ##@@                  "
    };
    return aparencia;
}

// --- HABILIDADE DA RACA ---
std::string Humano::obterNomeHabilidadeRaca() const 
{ 
    return "Espirito indomavel"; 
}

std::string Humano::obterDescricaoHabilidadeRaca() const 
{ 
    return "Revive com metade da vida maxima uma vez"; 
}

// --- PROCESSAMENTO DE DANO  ---
int Humano::processarDanoDefensivo(int danoFinal, SistemaPersonagem* defensor) 
{
    // Verifica se o golpe seria fatal
    if ((defensor->obterVida() - danoFinal) <= 0 && defensor->podeUsarRessurreicao()) 
    {
        defensor->consumirRessurreicao();
        int curaReviver = defensor->obterVidaMaxima() / 2;
        defensor->modificarVida(curaReviver);
        std::string msg = Aparencia::margemCombate() + "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!\n";
        std::cout << msg;
        TelaCombate::adicionarMensagemFixa(msg);
        return 0; // O dano atual e anulado pois a vida foi resetada
    }
    return danoFinal;
}