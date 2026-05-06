#include "Humano.h"

#include <iostream>
#include "../Utilidades/SimplificacoesAparencia.h"

std::string Humano::obterNomeRaca() const 
{
    return "Humano";
}

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

Atributos Humano::obterAtributosRaca() const
{
    return { 100, 10, 10, 0, 10, 10, 10 };
}

std::string Humano::obterNomeHabilidadeRaca() const { return "Espirito indomavel"; }
std::string Humano::obterDescricaoHabilidadeRaca() const { return "Revive com metade da vida maxima uma vez"; }
int Humano::processarDanoDefensivo(int danoFinal, SistemaPersonagem* defensor) 
{
    // Verifica se o golpe seria fatal
    if ((defensor->obterVida() - danoFinal) <= 0 && defensor->podeUsarRessurreicao()) 
    {
        defensor->consumirRessurreicao();
        int curaReviver = defensor->obterVidaMaxima() / 2;
        defensor->modificarVida(curaReviver);
        std::cout << SimplificacoesAparencia::margemCombate() << "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!\n";
        return 0; // O dano atual e anulado pois a vida foi resetada
    }
    return danoFinal;
}