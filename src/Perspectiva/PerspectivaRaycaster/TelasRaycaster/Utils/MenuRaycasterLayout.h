#pragma once

#include <vector>
#include <string>
#include "../../../../Core/Utilidades/ConversorString.h"
#include "../../../../Core/Utilidades/Aparencia.h"

namespace ArtesRaycaster {

    inline const std::vector<std::string> logoTexto =
    ConversorString::converterStringBrutaParaVetor(R"(
   █████████     ███████    ██████████   ██████████       ██████    █████  █████ ██████████  █████████  ███████████
  ███░░░░░███  ███░░░░░███ ░░███░░░░███ ░░███░░░░░█     ███░░░░███ ░░███  ░░███ ░░███░░░░░█ ███░░░░░███░█░░░███░░░█
 ███     ░░░  ███     ░░███ ░███   ░░███ ░███  █ ░     ███    ░░███ ░███   ░███  ░███  █ ░ ░███    ░░░ ░   ░███  ░
░███         ░███      ░███ ░███    ░███ ░██████      ░███     ░███ ░███   ░███  ░██████   ░░█████████     ░███
░███         ░███      ░███ ░███    ░███ ░███░░█      ░███   ██░███ ░███   ░███  ░███░░█    ░░░░░░░░███    ░███
░░███     ███░░███     ███  ░███    ███  ░███ ░   █   ░░███ ░░████  ░███   ░███  ░███ ░   █ ███    ░███    ░███
 ░░█████████  ░░░███████░   ██████████   ██████████    ░░░██████░██ ░░████████   ██████████░░█████████     █████
  ░░░░░░░░░     ░░░░░░░    ░░░░░░░░░░   ░░░░░░░░░░       ░░░░░░ ░░   ░░░░░░░░   ░░░░░░░░░░  ░░░░░░░░░     ░░░░░
)");

    inline const std::vector<std::string> logoPlus = 
    ConversorString::converterStringBrutaParaVetor(R"(
                          
     ███         ███      
    ░███        ░███      
 ███████████ ███████████  
░░░░░███░░░ ░░░░░███░░░   
    ░███        ░███      
    ░░░         ░░░       
                          
)");

    inline const std::vector<std::string> casteloMenu = 
    ConversorString::converterStringBrutaParaVetor(R"(
        A                             A        
       /\                            /\       
      /##\             A            /##\      
     /####\           /\           /####\     
     |####|          /##\          |####|     
    _|_--_|_        /####\        _|_--_|_    
   |########|       |####|       |########|   
   |##_##_##|      _|_--_|_      |##_##_##|   
   |#| | |#||     |########|     |#| | |#||   
   |#|_| |_||     |##_##_##|     |#|_| |_||   
   |========|     |#| || |#|     |========|   
   |###__###|     |#|_||_|#|     |###__###|   
   |##|##|##|_____|========|_____|##|##|##|   
   |##|__|##|#####|########|#####|##|__|##|   
   |########|#####|##____##|#####|########|   
   |###[]###|#####|#| || |#|#####|###[]###|   
   |########|#####|#| || |#|#####|########|   
___|________|_____|_|_||_|_|_____|________|___
)");





    inline const std::vector<std::string> tvFrame1 = 
    ConversorString::converterStringBrutaParaVetor(R"(
        \       /    
         \     /     
       ###########   
       # %%%%%%% #|  
       # %%%%%%% #O  
       # %%%%%%% #o  
       # %%%%%%% #|  
       ###########   
         ][   ][     
      -------------  
)");

    inline const std::vector<std::string> tvFrame2 = 
    ConversorString::converterStringBrutaParaVetor(R"(
        \       /    
         \     /     
       ###########   
       #         #|  
       # %%%%%%% #O  
       # %%%%%%% #o  
       #         #|  
       ###########   
         ][   ][     
      -------------  
)");

    inline const std::vector<std::string> tvFrame3 = 
    ConversorString::converterStringBrutaParaVetor(R"(
        \       /    
         \     /     
       ###########   
       #         #|  
       #         #O  
       #   %%%   #o  
       #         #|  
       ###########   
         ][   ][     
      -------------  
)");

    inline const std::vector<std::string> tvFrame4 = 
    ConversorString::converterStringBrutaParaVetor(R"(
        \       /    
         \     /     
       ###########   
       #         #|  
       #         #O  
       #    %    #o  
       #         #|  
       ###########   
         ][   ][     
      -------------  
)");

    inline const std::vector<std::string> tvFrame5 = 
    ConversorString::converterStringBrutaParaVetor(R"(
        \       /    
         \     /     
       ###########   
       #         #|  
       #         #O  
       #         #o  
       #         #|  
       ###########   
         ][   ][     
      -------------  
)");

    inline const std::vector<std::string> arteNovaAventura = 
    ConversorString::converterStringBrutaParaVetor(R"(
           *           
          @*@          
         @*@*@         
         @@*@@         
        @@*@*@@        
       @@*@*@*@@       
      @@@*@*@*@@@      
      @@*@*@*@*@@      
      ##\#####/##      
    OOOOO#\#/#OOOOO    
   OOOOOOOOOOOOOOOOO   
)");


    inline const std::vector<std::string> arteSair = tvFrame1;



    inline const std::vector<std::string> arqueiroArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
     O   :\
   /&&&\  :\
  / &&& \ : \
 |  &&&  |:  )
   %%%%%  : /
   %% %%  :/
  _O_ _O_:/
)");

    inline const std::vector<std::string> bardoArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
   O ♫  
  /|\_  
  / \  
  ♪    ♫
)");

    inline const std::vector<std::string> guerreiroArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
    _#_      /
   /#|#\    / 
  / ### \  /  
 |  ###  |/   
   ## ##      
   ## ##      
  _#_ _#_     
)");

    inline const std::vector<std::string> guerreiroAtaqueArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
    _#_       
   /#|#\      
  / ### \==>  
 |  ###  |    
   ## ##      
   ## ##      
  _#_ _#_     
)");

    inline const std::vector<std::string> magoArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
     O   *** 
   /&&&\ *|* 
  / &&& \ |  
 |  &&&  ||  
   %%%%%  |  
   %%%%%  |  
   %   %  |  
)");

    inline const std::vector<std::string> necromanteArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
   O +  
  /|\+  
   / \  
  (x)   
)");

    inline const std::vector<std::string> goblinArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
          
          
    \&/   
   /&+&\ /
   |%%%|/ 
   %% %%  
  _&_ _&_ 
)");

    inline const std::vector<std::string> orkArte = 
    ConversorString::converterStringBrutaParaVetor(R"(
    \&&&/
    &&&&&     |
  /#|&+&|#\   |
 /# |&@&| #\  |
 || |&@&|  ||/
    |%%%|     
    %% %%     
  _&_   _&_   
)");

    inline const std::vector<std::string>& artesClasse(int indice) {
        static const std::vector<std::vector<std::string>> classes = {
            arqueiroArte, bardoArte, guerreiroArte, magoArte, necromanteArte
        };
        return classes[indice % 5];
    }

    inline const std::vector<std::string> cenaIntroducao =
    ConversorString::converterStringBrutaParaVetor(R"(
 █████ ██████   █████ █████   █████████  █████    ███████   
░░███ ░░██████ ░░███ ░░███   ███░░░░░███░░███   ███░░░░░███ 
 ░███  ░███░███ ░███  ░███  ███     ░░░  ░███  ███     ░░███
 ░███  ░███░░███░███  ░███ ░███          ░███ ░███      ░███
 ░███  ░███ ░░██████  ░███ ░███          ░███ ░███      ░███
 ░███  ░███  ░░█████  ░███ ░░███     ███ ░███ ░░███     ███ 
 █████ █████  ░░█████ █████ ░░█████████  █████ ░░░███████░  
░░░░░ ░░░░░    ░░░░░ ░░░░░   ░░░░░░░░░  ░░░░░    ░░░░░░░    
)");

}
