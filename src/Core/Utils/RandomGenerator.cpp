#include "Core/Utils/RandomGenerator.h"
#include <random>
#include <chrono>

namespace {
    std::mt19937& getGenerator() {
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

int RandomGenerator::getInteger(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(getGenerator());
}

bool RandomGenerator::rollChance(int successPercentage) {
    if (successPercentage <= 0) return false;
    if (successPercentage >= 100) return true;
    
    std::uniform_int_distribution<int> dist(1, 100);
    return dist(getGenerator()) <= successPercentage;
}






