#include "Systems/Progression/Progression.h"
#include "Domain/Characters/Character.h"
#include "Systems/Progression/ProgressionFlags.h"
#include "Systems/Progression/Diary.h"

/*
 * Instancia global do gerenciador de progresso (Singleton).
 * Centraliza as regras de negocios e as bandeiras de enredo.
 */
Progression& Progression::instance() {
    static Progression inst;
    return inst;
}

Progression::Progression() {}

/*
 * Modificacao segura de Thread (Thread-Safe):
 * Utilizamos std::mutex para garantir que o renderizador assincrono 3D 
 * nao acesse o mapa de flags enquanto ele eh modificado pelo jogo principal.
 */
void Progression::setFlag(const std::string& key, bool value) {
    std::lock_guard<std::mutex> lock(mtx);
    flags[key] = value;
}

bool Progression::getFlag(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = flags.find(key);
    if (it != flags.end()) return it->second;
    return false;
}

int Progression::getVillageProgress(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool villageNpcsFound = currentPlayer->getLevel() > 1 || getFlag(Flags::Village_NPCs); 
    bool villageEnemiesDefeated = currentPlayer->getCurrentXp() > 0 || currentPlayer->getLevel() > 1 || getFlag(Flags::Village_Enemies);
    bool villageMissionCompleted = getFlag(Flags::Village_RoyalInvitation) || currentPlayer->getInventory()->countItem("Convite Real") > 0;
    return (villageNpcsFound ? 33 : 0) + (villageEnemiesDefeated ? 33 : 0) + (villageMissionCompleted ? 34 : 0);
}

int Progression::getForestProgress(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool forestNpcsFound = currentPlayer->getUnlockedMaze() || getFlag(Flags::Forest_NPCs);
    bool forestEnemiesDefeated = getFlag(Flags::Forest_MahoragaDefeated);
    bool forestMissionCompleted = getFlag(Flags::Forest_MorganaQuest);
    return (forestNpcsFound ? 33 : 0) + (forestEnemiesDefeated ? 33 : 0) + (forestMissionCompleted ? 34 : 0);
}

int Progression::getKingdomBridgeProgress(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool trollDefeated = getFlag(Flags::BridgeKingdom_TrollDefeated);
    return (trollDefeated ? 34 : 0) + (getFlag(Flags::BridgeKingdom_NPCs) ? 33 : 0) + (getFlag(Flags::BridgeKingdom_Enemies) ? 33 : 0);
}

int Progression::getKingdomProgress(Character* currentPlayer) const {
    if (!currentPlayer) return 0;
    bool visited = getFlag(Flags::Visited_Kingdom);
    bool talkedPriest = Diary::instance().npcDiscovered("Padre Benedito");
    return (visited ? 50 : 0) + (talkedPriest ? 50 : 0);
}

/*
 * Serializacao (Save Game):
 * Exporta as chaves do mapa local para a persistencia em disco, garantindo
 * a consistencia dos estados e missoes abertas em um formato iteravel seguro.
 */
void Progression::save(std::ofstream& out) const {
    std::lock_guard<std::mutex> lock(mtx);
    out << flags.size() << "\n";
    for (const auto& [key, value] : flags) out << key << "\n" << (value ? 1 : 0) << "\n";
}

/*
 * Deserializacao (Load Game):
 * Realimenta o mapa em memoria (RAM) a partir do estado do save anterior.
 */
void Progression::load(std::ifstream& in) {
    std::lock_guard<std::mutex> lock(mtx);
    flags.clear();
    size_t size;
    if (in >> size) {
        // [PT-BR] Consome a quebra de linha inicial do stream
    // [EN-US] Consumes the initial newline from stream
    std::string garbage; std::getline(in, garbage);
        for (size_t i = 0; i < size; ++i) { std::string key; std::getline(in, key); int value; in >> value; std::getline(in, garbage); flags[key] = (value == 1); }
    }

    /*
     * --- RETROCOMPATIBILIDADE DE SAVES ANTIGOS ---
     * Evita que saves antigos (anteriores a atualizacao) percam o acesso a Viagem Rapida
     */
    auto itForest = flags.find("Visitou_Floresta");
    auto itBridgeKingdom = flags.find("Visitou_PonteReino");
    if ((itForest != flags.end() && itForest->second) || 
        (itBridgeKingdom != flags.end() && itBridgeKingdom->second)) {
        flags["Mapas_Descobertos"] = true;
    }
}
