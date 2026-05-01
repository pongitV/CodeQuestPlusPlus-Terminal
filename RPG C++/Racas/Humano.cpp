#include "Humano.h"

#include <iostream>

std::string Humano::obterNomeRaca() const 
{
    return "Humano";
}

std::vector<std::string> Humano::obterAparenciaRaca() const 
{
    return 
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
}

Atributos Humano::obterAtributosRaca() const
{
    return { 0, 10, 10, 0, 10, 10, 10 };
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
        std::cout << "[PASSIVA]: Espirito indomavel! O humano reviveu com metade de sua vida maxima!\n";
        return 0; // O dano atual e anulado pois a vida foi resetada
    }
    return danoFinal;
}