#include "Ork.h"

#include <iostream>
#include "../Utilidades/Aparencia.h"
#include "../Telas/TelaCombate.h"

std::string Ork::obterNomeRaca() const 
{
    return "Ork";
}

const std::vector<std::string>& Ork::obterAparenciaRaca() const 
{
    static const std::vector<std::string> aparencia = 
    {
        "                      #%=::@$$@#                          ",
        "                  +@#$)%%%#@@@*#+:                        ",
        "              % .##*%%%#%%%@=#***$#                       ",
        "             %#=#*###%*@@@%%%+%*#%@=                      ",
        "       #%=#%##*%+%*#%=*%%@%%%%%@*@@*:-=                   ",
        "        #%#%##%%%#%%%-#+#%@@%%@@*=@%#*==*++*              ",
        "        :%%%#%%#*#%##-****%%@@%%@%*%+====-:=**            ",
        "       @#%%***%%#%%%#+#%*+=##*++%%%=+-=====%%%@           ",
        "        %#%#*##*%*###%%-*****+=-=--====****#%#@           ",
        "        @%%%@%*#**@***%%#%%*%%+--**+*%*#%#%*%%%%          ",
        "         :@%%%@@%@%@%%##*%**%@@****#%%@%%%%%%%%*%         ",
        "           :%%%%%%@%*==+=%@%%%@@***%%%@%********%         ",
        "           %%%%*#%%@%%##%%%+%%*%%%#%@%%%*+*##*+*%:        ",
        "           #%%#**#%@%**%*%+%%%@@%@%%%%%%***##%#*%%        ",
        "        =##%%%##%%%%%#%%%%%%%%=@%%**%%%%%%#%%%**%%%       ",
        "       %%%+==%%%%%%  #%%%@*#**@@%@%%@%* =%%%%**%%#%       ",
        "      %%%*+=+**=%%   *%@%%**%#+*%#%@%%%   @%%#**%@%%      ",
        "      %%*%=+*+**%=  %%%*%#**%%%@@%@@@@@  =@@%@%***#@.     ",
        "      #%%@@%%%%%+   %%%#%%*%@**%%%@%%%@  :@%%%@#@%%%      ",
        "      %####=*=@%.  :@%%%*@%%%@#%#@%%#%%@-@%%%@*%@%@%@     ",
        "     =%*****++%.  @%@%@%#%%%#%@###@%#%%%%:%%@%%%%#%@:     ",
        "     @%*+*%%%** .%%%%%@@@*##%%@%%%%@%%%%%#%%#*%#%@%       ",
        "     -#*%###%%% %@@#%%%@%%%%@@%@@%%%*=*%***%%%%%%%%+      ",
        "      %%*###%% %%%#%%%%@%%##@%%@%%%*+=**%%@%*#%%@%%       ",
        "      .*-*%%@  %#%*%%@%@%%#%@%%%%%@#+*%%%*%#%###          ",
        "        @#:%% #%%*#**%%@%%%*@@%@%%@#%**%%@%%%%#           ",
        "          *@% @%%***#@@%%@#%@@@%%@%%##%#%%*%@.            ",
        "           % .%%*#*%%@%%%%%%%#@@@%%%#***=**%*             ",
        "            # %**#%#%%%%%@@%@%@@@.%%%#*%#%%%              ",
        "             %@@%%%%%%%%@@@%@%@@@  %%%#*+=%@              ",
        "            #%%%##**##%#%%@%@%-@    %%@%%%%%%%            ",
        "             *%#*%#%#%*#%% +*%==  :%%%%%%%%%%@=           ",
        "             *%@*%*%*@*%%#%  %     %%@**%%%##%@           ",
        "                %%%%@%%%%%%% #     %%%#*#%*%%%%           ",
        "                %%%%%%%%*%%%        %%%@@%**+%%           ",
        "              .%%%%%%%@@*#%@         %@%#*%*@#%           ",
        "            #%%#%+#%*%%#%%%%         @%%%###*#%.          ",
        "          %%%%%###%%%#%%%%%%%.       @#%%#%@**%           ",
        "        :+%%%%*%*#*#%%%%@==.=       @+%@%#**%%%@          ",
        "       =.:@@@@%%@@%%%+*:%%..        @#%@@%%%#*#*%:        ",
        "                                     :%#%@@%*%%*%%%@      ",
        "                                     -:=::%@%@#%%%%#.     ",
        "                                      ::::-#%%%%=%%       "
    };
    return aparencia;
}

Atributos Ork::obterAtributosRaca() const
{
    return { 120, 20, 10, 0, 10, 5, 5 };
}

std::string Ork::obterNomeHabilidadeRaca() const { return "Furia cega"; }
std::string Ork::obterDescricaoHabilidadeRaca() const { return "Dano extra baseado na porcentagem de vida perdida"; }
int Ork::processarDanoOfensivo(int danoBase, SistemaPersonagem* atacante) 
{
    double percVidaPerdida = 1.0 - (static_cast<double>(atacante->obterVida()) / atacante->obterVidaMaxima());
    int danoExtra = static_cast<int>(danoBase * percVidaPerdida);
    if (danoExtra > 0) 
    {
        std::string msg = Aparencia::margemCombate() + "[PASSIVA]: Furia cega aumentou o dano em " + std::to_string(danoExtra) + "!\n";
        std::cout << msg;
        TelaCombate::adicionarMensagemFixa(msg);
        return danoBase + danoExtra;
    }
    return danoBase;
}
