#include "UI/Renderers/IDE/EngineIDE/IDEInspector.h"
#include "UI/Renderers/IDE/IDETheme.h"
#include "Domain/Characters/Character.h"
#include "Domain/Characters/Races/BaseRace.h"
#include "Systems/Inventory/Inventory.h"
#include "Domain/Items/Item.h"
#include <cmath>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace {
    enum class EntityKind {
        NPC,
        MONSTER,
        OBJECT,
        TELEPORT
    };

    struct DetectedEntity {
        EntityKind kind;
        std::string typeName;
        std::string baseClass;
        std::string roleOrArchetype;
        std::string interactionMethod;
        int hp = 0;
        int maxHp = 0;
        int atk = 0;
        int def = 0;
        std::string state;
    };

    DetectedEntity classifyEntity(char c, const std::string& mapTitle) {
        std::string upper = mapTitle;
        for (char& ch : upper) ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));

        bool isVillage = (upper.find("VILA") != std::string::npos || upper.find("INICIO") != std::string::npos || upper.find("VILLAGE") != std::string::npos);
        bool isKingdom = (upper.find("REINO") != std::string::npos || upper.find("KINGDOM") != std::string::npos || upper.find("CASTELO") != std::string::npos);
        bool isForest  = (upper.find("FLORESTA") != std::string::npos || upper.find("FOREST") != std::string::npos);
        bool isCave    = (upper.find("CAVERNA") != std::string::npos || upper.find("CAVE") != std::string::npos);

        DetectedEntity e;

        // 1. Verificações de NPCs Amigáveis:
        if ((isVillage || isKingdom) && c == 'B') {
            e.kind = EntityKind::NPC;
            e.typeName = "Bjorn";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Ferreiro Real";
            e.interactionMethod = "bjorn.interact(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if ((isVillage || isKingdom) && c == 'F') {
            e.kind = EntityKind::NPC;
            e.typeName = "Franchesco";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Mercador Ambulante";
            e.interactionMethod = "franchesco.interact(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if (isKingdom && c == 'N') {
            e.kind = EntityKind::NPC;
            e.typeName = "Anok";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Estilista Real";
            e.interactionMethod = "anok.interact(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if (isKingdom && c == 'Q') {
            e.kind = EntityKind::NPC;
            e.typeName = "AlquimistaReal";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Alquimista";
            e.interactionMethod = "alchemist.interact(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if (isKingdom && c == 'P') {
            e.kind = EntityKind::NPC;
            e.typeName = "PadreBenedito";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Sacerdote";
            e.interactionMethod = "priest.interact(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if (isKingdom && c == 'I') {
            e.kind = EntityKind::NPC;
            e.typeName = "ReiDeCodeQuest";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Monarca";
            e.interactionMethod = "king.interact(hero);";
            e.state = "NPCDialog::ROYAL_AUDIENCE";
            return e;
        }
        if (isKingdom && c == 'C') {
            e.kind = EntityKind::NPC;
            e.typeName = "CavaleiroDeTreino";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Instrutor de Combate";
            e.interactionMethod = "knight.train(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if (isForest && c == 'M') {
            e.kind = EntityKind::NPC;
            e.typeName = "Morgana";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Bruxa da Floresta";
            e.interactionMethod = "morgana.interact(hero);";
            e.state = "NPCDialog::READY";
            return e;
        }
        if (isCave && c == 'B') {
            e.kind = EntityKind::NPC;
            e.typeName = "BjornEnjaulado";
            e.baseClass = "NPC";
            e.roleOrArchetype = "Ferreiro Enjaulado";
            e.interactionMethod = "rescueBjorn(hero);";
            e.state = "NPCDialog::PRISONER";
            return e;
        }

        // 2. Verificações de Objetos Interativos:
        if (isForest && c == 'B') {
            e.kind = EntityKind::OBJECT;
            e.typeName = "BauDoTesouro";
            e.baseClass = "InteractiveObject";
            e.roleOrArchetype = "Container de Loot";
            e.interactionMethod = "chest.open(hero);";
            return e;
        }
        if (isVillage && c == 'P') {
            e.kind = EntityKind::OBJECT;
            e.typeName = "PlacaDaVila";
            e.baseClass = "WorldObject";
            e.roleOrArchetype = "Informativo";
            e.interactionMethod = "readSign(hero);";
            return e;
        }
        if (c == '^' || (isVillage && c == 'S')) {
            e.kind = EntityKind::TELEPORT;
            e.typeName = "PortalTeleporte";
            e.baseClass = "Trigger";
            e.roleOrArchetype = "Warp Setorial";
            e.interactionMethod = "teleport.warp(hero);";
            return e;
        }

        // 3. Inimigos / Monstros:
        e.kind = EntityKind::MONSTER;
        e.baseClass = "Monster";
        e.state = "AIState::HOSTILE_PATROL";
        e.interactionMethod = "combat.engage(hero, this);";

        switch (c) {
            case 'G':
                e.typeName = "Goblin";
                e.roleOrArchetype = "Monster::GoblinScout";
                e.hp = e.maxHp = 35; e.atk = 10; e.def = 3;
                break;
            case 'O':
                e.typeName = "Ork";
                e.roleOrArchetype = "Monster::OrkBrute";
                e.hp = e.maxHp = 60; e.atk = 14; e.def = 5;
                break;
            case 'S':
                e.typeName = "Slime";
                e.roleOrArchetype = "Monster::AcidSlime";
                e.hp = e.maxHp = 15; e.atk = 8; e.def = 2;
                break;
            case 'F':
                e.typeName = "Fada";
                e.roleOrArchetype = "Monster::CorruptedFairy";
                e.hp = e.maxHp = 25; e.atk = 12; e.def = 1;
                break;
            case 'A':
                e.typeName = "Abominacao";
                e.roleOrArchetype = "Monster::ForestAbomination";
                e.hp = e.maxHp = 70; e.atk = 16; e.def = 6;
                break;
            case 'T':
                e.typeName = "Troll";
                e.roleOrArchetype = "Monster::CaveTroll";
                e.hp = e.maxHp = 100; e.atk = 18; e.def = 8;
                break;
            case 'B':
                e.typeName = "Chefe";
                e.roleOrArchetype = "Monster::DungeonBoss";
                e.hp = e.maxHp = 150; e.atk = 25; e.def = 10;
                break;
            case 'C':
                e.typeName = "Cavaleiro";
                e.roleOrArchetype = "Monster::DarkKnight";
                e.hp = e.maxHp = 80; e.atk = 16; e.def = 12;
                break;
            default:
                e.typeName = "Monstro";
                e.roleOrArchetype = "Monster::Generic";
                e.hp = e.maxHp = 30; e.atk = 8; e.def = 2;
                break;
        }
        return e;
    }
}

std::string IDEInspector::getEntityTypeName(char c, const std::string& mapTitle) {
    auto e = classifyEntity(c, mapTitle);
    return e.typeName;
}

std::vector<std::string> IDEInspector::inspectPlayer(Character* player, int posX, int posY) {
    std::vector<std::string> lines;
    if (!player) return lines;

    lines.push_back(IDETheme::comment("// === WATCH: Entity ativa do Hero ==="));
    lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Characters") + IDETheme::punctuation(" {"));
    lines.push_back(IDETheme::keyword("class ") + IDETheme::type("Hero") + IDETheme::punctuation(" final : public ") + IDETheme::type("Player") + IDETheme::punctuation(" {"));
    lines.push_back(IDETheme::keyword("public:"));
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Vector2") + " " + IDETheme::variable("gridPos") + IDETheme::punctuation(" = { ") + IDETheme::number(posX) + IDETheme::punctuation(", ") + IDETheme::number(posY) + IDETheme::punctuation(" };"));

    std::string hpDisplay = IDETheme::renderCodeHealthBar(player->getHealth(), player->getMaxHealth(), 8);
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + "     " + IDETheme::variable("hp") + IDETheme::punctuation("      = ") + IDETheme::number(player->getHealth()) + IDETheme::punctuation("; ") + IDETheme::comment(hpDisplay));
    int playerGold = player->getInventory() ? player->getInventory()->getGold() : 0;
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + "     " + IDETheme::variable("gold") + IDETheme::punctuation("    = ") + IDETheme::number(playerGold) + IDETheme::punctuation(";"));
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + "     " + IDETheme::variable("level") + IDETheme::punctuation("   = ") + IDETheme::number(player->getLevel()) + IDETheme::punctuation(";"));

    Item* weapon = player->getWeapons();
    std::string weaponName = weapon ? weapon->getItemName() : "Desarmado";
    lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Weapon*") + " " + IDETheme::variable("eqWeapon") + IDETheme::punctuation(" = &heap[") + IDETheme::stringLiteral(weaponName) + IDETheme::punctuation("];"));

    Item* shield = player->getShield();
    if (shield) {
        lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Shield*") + " " + IDETheme::variable("eqShield") + IDETheme::punctuation(" = &heap[") + IDETheme::stringLiteral(shield->getItemName()) + IDETheme::punctuation("];"));
    }

    lines.push_back(IDETheme::punctuation("};"));
    lines.push_back(IDETheme::punctuation("} // namespace Domain::Characters"));
    return lines;
}

std::vector<std::string> IDEInspector::inspectNearestEntity(
    const std::vector<std::string>& mapMatrix,
    int playerX,
    int playerY,
    const std::string& mapTitle
) {
    std::vector<std::string> lines;
    if (mapMatrix.empty()) return lines;

    int bestX = -1, bestY = -1;
    float bestDist = 999.0f;
    char bestChar = ' ';

    int searchRadius = 16;
    int height = static_cast<int>(mapMatrix.size());

    for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
        int y = playerY + dy;
        if (y < 0 || y >= height) continue;
        int width = static_cast<int>(mapMatrix[y].length());
        for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
            int x = playerX + dx;
            if (x < 0 || x >= width) continue;
            char c = mapMatrix[y][x];
            std::string entities = "GOBFPMSTRCAQNI^@";
            if (entities.find(c) != std::string::npos) {
                float dist = std::sqrt(static_cast<float>(dx * dx + dy * dy));
                if (dist < bestDist) {
                    bestDist = dist;
                    bestX = x;
                    bestY = y;
                    bestChar = c;
                }
            }
        }
    }

    if (bestChar != ' ') {
        auto ent = classifyEntity(bestChar, mapTitle);

        std::ostringstream distStream;
        distStream << std::fixed << std::setprecision(1) << bestDist;

        if (ent.kind == EntityKind::NPC) {
            lines.push_back(IDETheme::comment("// === ALVO: Entity amigável (" + ent.typeName + ") ==="));
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::NPCs") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("class ") + IDETheme::type(ent.typeName) + IDETheme::punctuation(" final : public ") + IDETheme::type("NPC") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("public:"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Vector2") + "     " + IDETheme::variable("gridPos") + IDETheme::punctuation("        = { ") + IDETheme::number(bestX) + IDETheme::punctuation(", ") + IDETheme::number(bestY) + IDETheme::punctuation(" };"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("float") + "       " + IDETheme::variable("distanceToHero") + IDETheme::punctuation(" = ") + IDETheme::number(distStream.str()) + IDETheme::punctuation("f;"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("const char*") + " " + IDETheme::variable("role") + IDETheme::punctuation("           = ") + IDETheme::stringLiteral(ent.roleOrArchetype) + IDETheme::punctuation(";"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("NPCDialog") + "   " + IDETheme::variable("dialogState") + IDETheme::punctuation("    = ") + IDETheme::type(ent.state) + IDETheme::punctuation(";"));
            lines.push_back("");
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("void") + " " + IDETheme::function("interact") + IDETheme::punctuation("(") + IDETheme::type("Character*") + " " + IDETheme::variable("hero") + IDETheme::punctuation(") override;"));
            lines.push_back(IDETheme::punctuation("};"));
            lines.push_back(IDETheme::punctuation("} // namespace Domain::NPCs"));
        } else if (ent.kind == EntityKind::OBJECT) {
            lines.push_back(IDETheme::comment("// === ALVO: Object interativo (" + ent.typeName + ") ==="));
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Objects") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("class ") + IDETheme::type(ent.typeName) + IDETheme::punctuation(" final : public ") + IDETheme::type("InteractiveObject") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("public:"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Vector2") + " " + IDETheme::variable("gridPos") + IDETheme::punctuation(" = { ") + IDETheme::number(bestX) + IDETheme::punctuation(", ") + IDETheme::number(bestY) + IDETheme::punctuation(" };"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("float") + "   " + IDETheme::variable("distanceToHero") + IDETheme::punctuation(" = ") + IDETheme::number(distStream.str()) + IDETheme::punctuation("f;"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("void") + " " + IDETheme::function("open") + IDETheme::punctuation("(") + IDETheme::type("Character*") + " " + IDETheme::variable("hero") + IDETheme::punctuation(");"));
            lines.push_back(IDETheme::punctuation("};"));
            lines.push_back(IDETheme::punctuation("} // namespace Domain::Objects"));
        } else if (ent.kind == EntityKind::TELEPORT) {
            lines.push_back(IDETheme::comment("// === ALVO: Trigger de setor (Portal) ==="));
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("World::Triggers") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("class ") + IDETheme::type("SectorWarp") + IDETheme::punctuation(" final : public ") + IDETheme::type("Trigger") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("public:"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Vector2") + " " + IDETheme::variable("gridPos") + IDETheme::punctuation(" = { ") + IDETheme::number(bestX) + IDETheme::punctuation(", ") + IDETheme::number(bestY) + IDETheme::punctuation(" };"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("void") + " " + IDETheme::function("warp") + IDETheme::punctuation("(") + IDETheme::type("Character*") + " " + IDETheme::variable("hero") + IDETheme::punctuation(");"));
            lines.push_back(IDETheme::punctuation("};"));
            lines.push_back(IDETheme::punctuation("} // namespace World::Triggers"));
        } else {
            lines.push_back(IDETheme::comment("// === ALVO: Entity hostil (" + ent.typeName + ") ==="));
            lines.push_back(IDETheme::keyword("namespace ") + IDETheme::type("Domain::Monsters") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("class ") + IDETheme::type(ent.typeName) + IDETheme::punctuation(" final : public ") + IDETheme::type("Monster") + IDETheme::punctuation(" {"));
            lines.push_back(IDETheme::keyword("public:"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("Vector2") + " " + IDETheme::variable("gridPos") + IDETheme::punctuation("        = { ") + IDETheme::number(bestX) + IDETheme::punctuation(", ") + IDETheme::number(bestY) + IDETheme::punctuation(" };"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("float") + "   " + IDETheme::variable("distanceToHero") + IDETheme::punctuation(" = ") + IDETheme::number(distStream.str()) + IDETheme::punctuation("f;"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + "     " + IDETheme::variable("hp") + IDETheme::punctuation("             = ") + IDETheme::number(ent.hp) + IDETheme::punctuation("; ") + IDETheme::comment(IDETheme::renderCodeHealthBar(ent.hp, ent.maxHp, 6)));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + "     " + IDETheme::variable("strength") + IDETheme::punctuation("       = ") + IDETheme::number(ent.atk) + IDETheme::punctuation(";"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("int") + "     " + IDETheme::variable("resistance") + IDETheme::punctuation("     = ") + IDETheme::number(ent.def) + IDETheme::punctuation(";"));
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("AIState") + " " + IDETheme::variable("state") + IDETheme::punctuation("          = ") + IDETheme::type(ent.state) + IDETheme::punctuation(";"));
            lines.push_back("");
            lines.push_back(IDETheme::punctuation("    ") + IDETheme::type("void") + " " + IDETheme::function("onHit") + IDETheme::punctuation("(") + IDETheme::type("int") + " " + IDETheme::variable("dmg") + IDETheme::punctuation(");"));
            lines.push_back(IDETheme::punctuation("};"));
            lines.push_back(IDETheme::punctuation("} // namespace Domain::Monsters"));
        }
    } else {
        lines.push_back(IDETheme::comment("// === MEMORY WATCH: Radar de varredura ==="));
        lines.push_back(IDETheme::comment("// Raio de busca do radar: 16 células no grid."));
        lines.push_back(IDETheme::comment("// Nenhum pointer de entity no raio imediato."));
        lines.push_back(IDETheme::comment("// Heap: Estável, 0 memory leaks."));
    }

    return lines;
}
