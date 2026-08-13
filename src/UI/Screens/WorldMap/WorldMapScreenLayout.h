#pragma once
#include <string>
#include <vector>

namespace ArtsMapWorld {

const std::vector<std::string> planetEarth = {
    R"(            _-o#&&*''''?d:>b\_``\                         )",
    R"(          _o/###########dMF9MMMMMHo_                      )",
    R"(       .o&#'########`"MbHMMMMMMMMMMHo.                   )",
    R"(     .o""###########vodM*$&&HMMMMMMMMMM?.                 )",
    R"(    ,'##############$M&ood,~'`(&##MMMMMMH\                )",
    R"(   /###############,MMMMMM#b?#bobMMMMHMMML               )",
    R"(  &##############?MMMMMMMMMMMMMMMM7MMM$R*Hk              )",
    R"( ?$.############:MMMMMMMMMMMMMMMMMM/HMMM|##L             )",
    R"(|###############|MMMMMMMMMMMMMMMMMMMbMH####T,            )",
    R"($H#:############`*MMMMMMMMMMMMMMMMMMMb#}####?            )",
    R"(]MMH##############""*""""*#MMMMMMMMMMMM'####-            )",
    R"(MMMMMb_###################|MMMMMMMMMMMP'#####:            )",
    R"(HMMMMMMHo#################`MMMMMMMMMT########.            )",
    R"(?MMMMMMMP##################9MMMMMMMM}########-            )",
    R"(-?MMMMMM##################|MMMMMMMM?#########'             )",
    R"( :|MMMMMM-#################`MMMMMMMT########:             )",
    R"(  .9MMM[####################&MMMMM*########.              )",
    R"(   :9MMk####################`MMM#"########-               )",
    R"(     &M}#################################-                )",
    R"(      `&.#############################.                   )",
    R"(        `~,#########################./                    )",
    R"(           \.#######################/                     )",
    R"(              '`--._,dd###pp=""'´´                        )",
};

struct LocationInfo {
    const char* flag;
    const char* nameDisplay;
};

inline std::vector<LocationInfo> getLocations() {
    return {
        {nullptr, "Village Inicial"},
        {"Visited_Forest", "Forest Sombria"},
        {"Visited_KingdomBridge", "Ponte do Kingdom"},
        {"Visited_Kingdom", "Kingdom Distante"},
    };
}

}
