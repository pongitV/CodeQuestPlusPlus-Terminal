#include <iostream>

#include "RacaHumano.h"

std::string RacaHumano::obterNomeRaca() const 
{
    return "Humano ";
}

std::vector<std::string> RacaHumano::obterAparenciaRaca() const 
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

Atributos RacaHumano::obterAtributosRaca() const 
{
    // Ordem: { Vida, Forca, Destreza, Resistencia, Constituicao, Inteligencia, Sabedoria }
    return { 0, 10, 10, 0, 10, 10, 10 };
}

std::string RacaHumano::obterNomeHabilidadeRaca() const { return "Espirito indomavel"; }
std::string RacaHumano::obterDescricaoHabilidadeRaca() const { return "Revive com metade da vida maxima uma vez"; }
int RacaHumano::processarDanoDefensivo(int danoFinal, Personagem* defensor) 
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