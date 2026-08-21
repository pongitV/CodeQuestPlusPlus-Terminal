#pragma once

#include <vector>
#include <string>
#include "Core/Utils/StringConverter.h"
#include "Core/Utils/Appearance.h"

namespace ArtsRaycaster {

    inline const std::vector<std::string> textLogo =
    StringConvert::convertRawStringToVector(R"(
   █████████     ███████    ██████████   ██████████       ██████    █████  █████ ██████████  █████████  ███████████
  ███░░░░░███  ███░░░░░███ ░░███░░░░███ ░░███░░░░░█     ███░░░░███ ░░███  ░░███ ░░███░░░░░█ ███░░░░░███░█░░░███░░░█
 ███     ░░░  ███     ░░███ ░███   ░░███ ░███  █ ░     ███    ░░███ ░███   ░███  ░███  █ ░ ░███    ░░░ ░   ░███  ░
░███         ░███      ░███ ░███    ░███ ░██████      ░███     ░███ ░███   ░███  ░██████   ░░█████████     ░███
░███         ░███      ░███ ░███    ░███ ░███░░█      ░███   ██░███ ░███   ░███  ░███░░█    ░░░░░░░░███    ░███
░░███     ███░░███     ███  ░███    ███  ░███ ░   █   ░░███ ░░████  ░███   ░███  ░███ ░   █ ███    ░███    ░███
 ░░█████████  ░░░███████░   ██████████   ██████████    ░░░██████░██ ░░████████   ██████████░░█████████     █████
  ░░░░░░░░░     ░░░░░░░    ░░░░░░░░░░   ░░░░░░░░░░       ░░░░░░ ░░   ░░░░░░░░   ░░░░░░░░░░  ░░░░░░░░░     ░░░░░
)");

    inline const std::vector<std::string> plusLogo = 
    StringConvert::convertRawStringToVector(R"(
                          
     ███         ███      
    ░███        ░███      
 ███████████ ███████████  
░░░░░███░░░ ░░░░░███░░░   
    ░███        ░███      
    ░░░         ░░░       
                          
)");

    inline const std::vector<std::string> castleMenu = 
    StringConvert::convertRawStringToVector(R"(
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
    StringConvert::convertRawStringToVector(R"(
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
    StringConvert::convertRawStringToVector(R"(
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
    StringConvert::convertRawStringToVector(R"(
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
    StringConvert::convertRawStringToVector(R"(
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
    StringConvert::convertRawStringToVector(R"(
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

    inline const std::vector<std::string> artNewAdventure = 
    StringConvert::convertRawStringToVector(R"(
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


    inline const std::vector<std::string> artLeave = tvFrame1;



    inline const std::vector<std::string> archerArt = 
    StringConvert::convertRawStringToVector(R"(
     O   :\
   /&&&\  :\
  / &&& \ : \
 |  &&&  |:  )
   %%%%%  : /
   %% %%  :/
  _O_ _O_:/
)");

    inline const std::vector<std::string> bardoArte = 
    StringConvert::convertRawStringToVector(R"(
   O ♫
  /|\_
  / \
  ♪    ♫
)");

    inline const std::vector<std::string> warriorArt = 
    StringConvert::convertRawStringToVector(R"(
    _#_      /
   /#|#\    /
  / ### \  /
 |  ###  |/
   ## ##
   ## ##
  _#_ _#_
)");

    inline const std::vector<std::string> warriorAttackArt = 
    StringConvert::convertRawStringToVector(R"(
    _#_
   /#|#\
  / ### \==>
 |  ###  |
   ## ##
   ## ##
  _#_ _#_
)");

    inline const std::vector<std::string> wizardArt = 
    StringConvert::convertRawStringToVector(R"(
     O   *** 
   /&&&\ *|* 
   / &&& \ |  
 |  &&&  ||  
   %%%%%  |  
   %%%%%  |  
   %   %  |  
)");

    inline const std::vector<std::string> necromanteArte = 
    StringConvert::convertRawStringToVector(R"(
   O +
  /|\+
   / \
  (x)
)");

    inline const std::vector<std::string> goblinArt = 
    StringConvert::convertRawStringToVector(R"(
          
          
    \&/   
   /&+&\ /
   |%%%|/ 
   %% %%  
  _&_ _&_ 
)");

    inline const std::vector<std::string> orkArt = 
    StringConvert::convertRawStringToVector(R"(
    \&&&/
    &&&&&     |
  /#|&+&|#\   |
 /# |&@&| #\  |
 || |&@&|  ||/
    |%%%|     
    %% %%     
  _&_   _&_   
)");

    inline const std::vector<std::string>& classArts(int index) {
        static const std::vector<std::vector<std::string>> classes = {
            archerArt, bardoArte, warriorArt, wizardArt, necromanteArte
        };
        return classes[index % 5];
    }

    inline const std::vector<std::string> sceneIntroduction =
    StringConvert::convertRawStringToVector(R"(
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
