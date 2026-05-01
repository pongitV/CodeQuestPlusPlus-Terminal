#include "GeradorAleatorio.h"
#include <random>

namespace {
    std::mt19937& obterGerador() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
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
