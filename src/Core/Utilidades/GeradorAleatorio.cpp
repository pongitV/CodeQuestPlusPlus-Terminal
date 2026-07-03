#include "GeradorAleatorio.h"
#include <random>
#include <chrono>

namespace {
    std::mt19937& obterGerador() {
        static std::mt19937 gen([]() -> unsigned int {
            std::random_device rd;
            if (rd.entropy() == 0.0) {
                return static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count());
            }
            return rd();
        }());
        return gen;
    }
}

int GeradorAleatorio::obterInteiro(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(obterGerador());
}

bool GeradorAleatorio::rolarChance(int porcentagemSucesso) {
    if (porcentagemSucesso <= 0) return false;
    if (porcentagemSucesso >= 100) return true;
    
    std::uniform_int_distribution<int> dist(1, 100);
    return dist(obterGerador()) <= porcentagemSucesso;
}






