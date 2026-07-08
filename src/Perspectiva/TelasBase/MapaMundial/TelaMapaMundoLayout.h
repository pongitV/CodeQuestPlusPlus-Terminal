#pragma once
#include <string>
#include <vector>

namespace ArtesMapaMundo {

const std::vector<std::string> planetaTerra = {
    R"(            _-o#&&*''''?d:>b\_``\                         )",
    R"(          _o/###########dMF9MMMMMHo_                      )",
    R"(       .o&#'########`"MbHMMMMMMMMMMMHo.                   )",
    R"(     .o""###########vodM*$&&HMMMMMMMMMM?.                 )",
    R"(    ,'##############$M&ood,~'`(&##MMMMMMH\                )",
    R"(   /###############,MMMMMMM#b?#bobMMMMHMMML               )",
    R"(  &##############?MMMMMMMMMMMMMMMMM7MMM$R*Hk              )",
    R"( ?$.############:MMMMMMMMMMMMMMMMMMM/HMMM|##L             )",
    R"(|###############|MMMMMMMMMMMMMMMMMMMMbMH####T,            )",
    R"($H#:############`*MMMMMMMMMMMMMMMMMMMMb#}####?            )",
    R"(]MMH##############""*""""*#MMMMMMMMMMMMM'####-            )",
    R"(MMMMMb_###################|MMMMMMMMMMMP'#####:            )",
    R"(HMMMMMMMHo#################`MMMMMMMMMT#######.            )",
    R"(?MMMMMMMMP##################9MMMMMMMM}#######-            )",
    R"(-?MMMMMMM##################|MMMMMMMMM?######'             )",
    R"( :|MMMMMM-#################`MMMMMMMT########:             )",
    R"(  .9MMM[####################&MMMMM*########.              )",
    R"(   :9MMk####################`MMM#"########-               )",
    R"(     &M}#################################-                )",
    R"(      `&.#############################.                   )",
    R"(        `~,#########################./                    )",
    R"(           \.#######################/                     )",
    R"(              '`--._,dd###pp=""'´´                        )",
};

struct LocalInfo {
    const char* flag;
    const char* nomeExibicao;
};

inline std::vector<LocalInfo> obterLocais() {
    return {
        {nullptr, "Vila Inicial"},
        {"Visitou_Floresta", "Floresta Sombria"},
        {"Visitou_PonteReino", "Ponte do Reino"},
        {"Visitou_Reino", "Reino Distante"},
    };
}

}
