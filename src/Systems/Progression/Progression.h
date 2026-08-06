/*
 * Arquivo: Progression.h
 * Proposito: Singleton gerenciador de flags globais de progresso da historia e salvamento.
 */

#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <mutex>

class Character;

/*
 * Gerenciador thread-safe do progresso das missoes e eventos do jogo.
 */
class Progression {
private:
    std::unordered_map<std::string, bool> flags;
    mutable std::mutex mtx;

    Progression();
public:
    static Progression& instance();
    
    void setFlag(const std::string& key, bool value);
    bool getFlag(const std::string& key) const;

    // Calculos dinamicos de progresso combinando dados do jogador com as flags salvas
    int getVillageProgress(Character* currentPlayer) const;
    int getForestProgress(Character* currentPlayer) const;
    int getKingdomBridgeProgress(Character* currentPlayer) const;
    int getKingdomProgress(Character* currentPlayer) const;

    // Funcoes para ligar ao sistema de salvamento
    void save(std::ofstream& out) const;
    void load(std::ifstream& in);
};
