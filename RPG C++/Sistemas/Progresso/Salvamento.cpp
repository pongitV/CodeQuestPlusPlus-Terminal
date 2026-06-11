#include "Salvamento.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <functional>

#include "../../Entidades/Classes/Arqueiro.h"
#include "../../Entidades/Classes/Bardo.h"
#include "../../Entidades/Classes/Guerreiro.h"
#include "../../Entidades/Classes/Mago.h"
#include "../../Entidades/Classes/Necromante.h"
#include "../../Entidades/Classes/CloneNecro.h"
#include "../Inventario/Equipamentos/EquipamentoArma.h"
#include "../Inventario/Equipamentos/EquipamentoArmadura.h"
#include "../Inventario/FabricaItens.h"
#include "../Inventario/Item.h"
#include "../../Entidades/Racas/Anao.h"
#include "../../Entidades/Racas/Elfo.h"
#include "../../Entidades/Racas/Humano.h"
#include "../../Entidades/Racas/Orc.h"
#include "Bestiario.h"
#include "Diario.h"
#include "Progressao.h"
#include "../../Core/Utilidades/Aparencia.h"
#include "../../Core/Utilidades/FuncoesDialogo.h"
#include "../../Entidades/Personagem.h"

namespace {
    std::unique_ptr<Item> recriarItemComModificadores(const std::string& nomeSalvo) {
        std::string cleanName = nomeSalvo;
        bool isSangrenta = false, isViscosa = false, isQuebraDefesas = false, isImbuida = false;
        bool isArcoMagico = false, isCajadoCipos = false, isViolaoMagico = false;
        
        if (cleanName.find(" (Sangrenta)") != std::string::npos) { isSangrenta = true; cleanName.erase(cleanName.find(" (Sangrenta)"), 12); }
        if (cleanName.find(" (Viscosa)") != std::string::npos) { isViscosa = true; cleanName.erase(cleanName.find(" (Viscosa)"), 10); }
        if (cleanName.find(" (Quebra-Defesas)") != std::string::npos) { isQuebraDefesas = true; cleanName.erase(cleanName.find(" (Quebra-Defesas)"), 17); }
        if (cleanName.find(" (Imbuida)") != std::string::npos) { isImbuida = true; cleanName.erase(cleanName.find(" (Imbuida)"), 10); }

        int nivelMelhoria = 0;
        size_t posPlus = cleanName.find_last_of('+');
        if (posPlus != std::string::npos && posPlus > 0 && cleanName[posPlus-1] == ' ') {
            try {
                nivelMelhoria = std::stoi(cleanName.substr(posPlus + 1));
                cleanName.erase(posPlus - 1);
            } catch (...) {}
        }

        if (cleanName.find("Arco recurvo de madeira enfeiticada") != std::string::npos) { isArcoMagico = true; cleanName.replace(cleanName.find("Arco recurvo de madeira enfeiticada"), 35, "Arco recurvo de madeira"); }
        else if (cleanName.find("Cajado de cipos") != std::string::npos) { isCajadoCipos = true; cleanName.replace(cleanName.find("Cajado de cipos"), 15, "Cajado de cristal magico"); }
        else if (cleanName.find("Violao enfeiticado") != std::string::npos) { isViolaoMagico = true; cleanName.replace(cleanName.find("Violao enfeiticado"), 18, "Violao encantado"); }

        auto novoItem = FabricaItens::criarItem(cleanName);
        if (novoItem) {
            for (int lvl = 0; lvl < nivelMelhoria; ++lvl) {
                if (auto copia = novoItem->gerarCopiaMelhorada()) {
                    novoItem = std::move(copia);
                }
            }

            if (auto arma = dynamic_cast<EquipamentoArma*>(novoItem.get())) {
                if (isSangrenta) arma->aplicarEfeitoSangramento();
                if (isViscosa) arma->aplicarEfeitoLentidao();
                if (isQuebraDefesas) arma->adicionarPropriedade(Propriedade::Penetrante);
                
                if (isArcoMagico) {
                    int novoDanoMagico = arma->obterDanoMagico() + (arma->obterDanoFisico() / 2);
                    auto novoArcoObj = std::make_unique<EquipamentoArma>(
                        nomeSalvo, arma->obterDanoFisico(), novoDanoMagico, 
                        arma->obterReqForca(), arma->obterReqDestreza(), 
                        arma->obterReqInteligencia(), arma->obterReqSabedoria(), 0);
                    if (isSangrenta) novoArcoObj->aplicarEfeitoSangramento();
                    if (isViscosa) novoArcoObj->aplicarEfeitoLentidao();
                    if (isQuebraDefesas) novoArcoObj->adicionarPropriedade(Propriedade::Penetrante);
                    for (Propriedade prop : arma->obterPropriedades()) novoArcoObj->adicionarPropriedade(prop);
                    novoArcoObj->adicionarPropriedade(Propriedade::Magica);
                    novoItem = std::move(novoArcoObj);
                } 
                else if (isCajadoCipos) {
                    arma->adicionarPropriedade(Propriedade::CipoPrisao);
                    arma->alterarNome(nomeSalvo);
                }
                else if (isViolaoMagico) {
                    arma->adicionarPropriedade(Propriedade::ViolaoMagico);
                    arma->alterarNome(nomeSalvo);
                } 
                else {
                    arma->alterarNome(nomeSalvo);
                }
            }
            
            if (isImbuida) {
                if (auto armadura = dynamic_cast<EquipamentoArmadura*>(novoItem.get())) {
                    auto novaArmadura = std::make_unique<EquipamentoArmadura>(
                        nomeSalvo, 
                        armadura->obterReducaoFixa() + 3, 
                        armadura->obterReqResistencia(), 
                        armadura->obterReqConstituicao(), 
                        armadura->obterPrecoVenda() + 200
                    );
                    for (Propriedade prop : armadura->obterPropriedades()) novaArmadura->adicionarPropriedade(prop);
                    novaArmadura->adicionarPropriedade(Propriedade::MelhoradoMaterial);
                    novoItem = std::move(novaArmadura);
                }
            }
        }
        return novoItem;
    }
}

bool Salvamento::saveExiste() {
    return !listarSaves().empty();
}

std::vector<std::string> Salvamento::listarSaves() {
    std::vector<std::string> saves;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(".")) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                // Verifica se começa com "save_" e termina com ".txt"
                if (filename.find("save_") == 0 && filename.size() >= 4 && filename.substr(filename.size() - 4) == ".txt") {
                    saves.push_back(filename);
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << FuncoesDialogo::formatarMsgSistema(std::string("Erro ao ler diretorio de saves: ") + e.what(), Cor::VERMELHO) << "\n";
    }
    return saves;
}

void Salvamento::salvarJogo(Personagem* jogador) {
    if (!jogador) return;
    std::string nomeArquivo = "save_" + jogador->obterNome() + ".txt";
    std::ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) return;

    arquivo << jogador->obterNome() << "\n";
    arquivo << jogador->obterRaca()->obterNomeRaca() << "\n";
    arquivo << jogador->obterNomeClasse() << "\n";
    
    arquivo << jogador->obterNivel() << " " << jogador->obterXpAtual() << " " << jogador->obterXpParaSubir() << "\n";
    arquivo << jogador->obterVida() << " " << jogador->obterInventario()->obterOuro() << " " << static_cast<int>(jogador->obterDificuldade()) << " " << (jogador->obterLabirintoDesbloqueado() ? 1 : 0) << "\n";

    auto& attr = jogador->obterAtributosFinais();
    arquivo << attr.vida << " " << attr.forca << " " << attr.destreza << " " << attr.resistencia << " " << attr.constituicao << " " << attr.inteligencia << " " << attr.sabedoria << "\n";

    auto itens = jogador->obterInventario()->obterTodosOsItens();
    arquivo << itens.size() << "\n";
    for (Item* item : itens) {
        int equipSlot = 0;
        if (item == jogador->obterArma()) equipSlot = 1;
        else if (item == jogador->obterEscudo()) equipSlot = 2;
        else if (item == jogador->obterArmadura()) equipSlot = 3;
        else if (item == jogador->obterConsumivelRapido()) equipSlot = 4;
        arquivo << equipSlot << " " << item->obterNomeItem() << "\n";
    }

    Bestiario::instancia().salvar(arquivo);
    Diario::instancia().salvar(arquivo);
    arquivo << (jogador->possuiRegeneracaoTroll() ? 1 : 0) << "\n";
    arquivo << (jogador->obterParryAtivado() ? 1 : 0) << "\n";
    arquivo << (jogador->podeUsarRessurreicao() ? 1 : 0) << "\n";
    arquivo << jogador->obterIconeJogador() << "\n";
    arquivo << static_cast<uint32_t>(jogador->obterCorJogador()) << "\n";

    Progressao::instancia().salvar(arquivo);

    // Salvar as Almas Coletadas (Necromante)
    auto& almas = jogador->obterAlmas();
    arquivo << almas.size() << "\n";
    for (const auto& alma : almas) {
        arquivo << alma->obterNome() << "\n";
        arquivo << alma->obterRaca()->obterNomeRaca() << "\n";
        
        auto aparencia = alma->obterRaca()->obterAparenciaRaca();
        arquivo << aparencia.size() << "\n";
        for (const auto& linha : aparencia) {
            arquivo << linha << "\n";
        }
        
        auto& attr = alma->obterAtributosFinais();
        arquivo << attr.vida << " " << attr.forca << " " << attr.destreza << " " << attr.resistencia << " " << attr.constituicao << " " << attr.inteligencia << " " << attr.sabedoria << "\n";
        arquivo << alma->obterVida() << "\n";
        
        auto itensAlma = alma->obterInventario()->obterTodosOsItens();
        arquivo << itensAlma.size() << "\n";
        for (Item* item : itensAlma) {
            int equipSlot = 0;
            if (item == alma->obterArma()) equipSlot = 1;
            else if (item == alma->obterEscudo()) equipSlot = 2;
            else if (item == alma->obterArmadura()) equipSlot = 3;
            else if (item == alma->obterConsumivelRapido()) equipSlot = 4;
            arquivo << equipSlot << " " << item->obterNomeItem() << "\n";
        }
    }

    arquivo.close();
}

std::unique_ptr<Personagem> Salvamento::carregarJogo(const std::string& nomeArquivo) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) return nullptr;

    std::string nome, racaStr, classeStr;
    std::getline(arquivo, nome);
    std::getline(arquivo, racaStr);
    std::getline(arquivo, classeStr);

    int nivel, xpAtual, xpParaSubir, vida, ouro, dificuldade, labirinto;
    arquivo >> nivel >> xpAtual >> xpParaSubir >> vida >> ouro >> dificuldade >> labirinto;

    Atributos attr;
    arquivo >> attr.vida >> attr.forca >> attr.destreza >> attr.resistencia >> attr.constituicao >> attr.inteligencia >> attr.sabedoria;

    static const std::unordered_map<std::string, std::function<std::unique_ptr<RacaBase>()>> registroRacas = {
        {"Dwarf", []() { return std::make_unique<Dwarf>(); }},
        {"Elfo", []() { return std::make_unique<Elfo>(); }},
        {"Humano", []() { return std::make_unique<Humano>(); }},
        {"Ork", []() { return std::make_unique<Ork>(); }}
    };

    static const std::unordered_map<std::string, std::function<std::unique_ptr<ClasseBase>()>> registroClasses = {
        {"Arqueiro", []() { return std::make_unique<Arqueiro>(); }},
        {"Bardo", []() { return std::make_unique<Bardo>(); }},
        {"Guerreiro", []() { return std::make_unique<Guerreiro>(); }},
        {"Mago", []() { return std::make_unique<Mago>(); }},
        {"Necromante", []() { return std::make_unique<Necromante>(); }}
    };

    std::unique_ptr<RacaBase> raca;
    if (auto it = registroRacas.find(racaStr); it != registroRacas.end()) raca = it->second();

    std::unique_ptr<ClasseBase> classe;
    if (auto it = registroClasses.find(classeStr); it != registroClasses.end()) classe = it->second();

    if (!raca || !classe) return nullptr;

    auto jogador = std::make_unique<Personagem>(nome, std::move(raca), std::move(classe));

    jogador->desequiparArma(); jogador->desequiparEscudo(); jogador->desequiparArmadura();
    auto itensPadrao = jogador->obterInventario()->obterTodosOsItens();
    std::vector<std::string> nomesItensPadrao;
    for (Item* i : itensPadrao) nomesItensPadrao.push_back(i->obterNomeItem());
    for (const std::string& nomeItem : nomesItensPadrao) jogador->obterInventario()->removerItem(nomeItem);

    jogador->definirNivel(nivel); jogador->definirXpAtual(xpAtual); jogador->definirXpParaSubir(xpParaSubir);
    jogador->obterAtributosFinais() = attr; jogador->definirVida(vida); jogador->definirDificuldade(static_cast<DificuldadeJogo>(dificuldade));
    if (labirinto == 1) jogador->desbloquearLabirinto();
    
    jogador->obterInventario()->adicionarOuro(-jogador->obterInventario()->obterOuro());
    jogador->obterInventario()->adicionarOuro(ouro);

    size_t qtdItens; arquivo >> qtdItens;
    std::string linhaLixo; std::getline(arquivo, linhaLixo); // Limpa o \n

    for (size_t i = 0; i < qtdItens; ++i) {
        int equipSlot; arquivo >> equipSlot;
        arquivo.ignore(); // Consome o espaco entre o numero do slot e o nome do item
        std::string nomeItem; std::getline(arquivo, nomeItem);
        
        auto novoItem = recriarItemComModificadores(nomeItem);
        if (novoItem) {
            Item* ptr = novoItem.get();
            jogador->obterInventario()->adicionarItem(std::move(novoItem));
            if (equipSlot != 0) jogador->equiparItem(ptr);
        }
    }

    Bestiario::instancia().carregar(arquivo);
    Diario::instancia().carregar(arquivo);
    
    std::streampos posMark = arquivo.tellg();
    int test1, test2;
    
    if (arquivo >> test1 >> test2) {
        // FORMATO NOVO: Consegue ler os ints do regTroll e parryAtivado
        arquivo.clear();
        arquivo.seekg(posMark);
        
        int regTroll = 0;
        if (arquivo >> regTroll && regTroll == 1) jogador->desbloquearRegeneracaoTroll();
        
        int parryAtivado = 0;
        if (arquivo >> parryAtivado) jogador->definirParryAtivado(parryAtivado == 1);

        int podeReviver = 1;
        if (arquivo >> podeReviver && podeReviver == 0) jogador->consumirRessurreicao();

        char icone = '@';
        if (arquivo >> icone) jogador->definirIconeJogador(icone);
        
        uint32_t cor = static_cast<uint32_t>(Cor::VERDE);
        if (arquivo >> cor) jogador->definirCorJogador(static_cast<Cor>(cor));

        Progressao::instancia().carregar(arquivo);
    } else {
        // FORMATO ANTIGO (Retrocompatibilidade): 
        // O proximo campo seria o tamanho da Progressao seguido por uma String, falhando no "test2"
        arquivo.clear();
        arquivo.seekg(posMark);
        Progressao::instancia().carregar(arquivo);
    }

    // Carregar as Almas Coletadas (Retrocompativel com saves antigos)
    size_t almasSize = 0;
    if (arquivo >> almasSize) {
        std::string lixoAlmas; std::getline(arquivo, lixoAlmas); // Consome \n
        for (size_t i = 0; i < almasSize; ++i) {
            std::string almaNome, almaRacaNome;
            std::getline(arquivo, almaNome);
            std::getline(arquivo, almaRacaNome);
            
            size_t aparenciaSize = 0;
            arquivo >> aparenciaSize;
            std::getline(arquivo, lixoAlmas);
            std::vector<std::string> almaAparencia;
            for (size_t j = 0; j < aparenciaSize; ++j) {
                std::string linhaAparencia;
                std::getline(arquivo, linhaAparencia);
                almaAparencia.push_back(linhaAparencia);
            }
            
            Atributos almaAttr;
            arquivo >> almaAttr.vida >> almaAttr.forca >> almaAttr.destreza >> almaAttr.resistencia >> almaAttr.constituicao >> almaAttr.inteligencia >> almaAttr.sabedoria;
            
            int almaVida;
            arquivo >> almaVida;
            std::getline(arquivo, lixoAlmas);
            
            auto almaRaca = std::make_unique<RacaClone>(almaRacaNome, almaAparencia);
            auto almaClasse = std::make_unique<ClasseClone>();
            auto almaPersonagem = std::make_unique<Personagem>(almaNome, std::move(almaRaca), std::move(almaClasse));
            
            almaPersonagem->obterAtributosFinais() = almaAttr;
            almaPersonagem->definirVida(almaVida);
            almaPersonagem->setAsMinion(true);
            
            size_t itensAlmaSize = 0;
            arquivo >> itensAlmaSize;
            std::getline(arquivo, lixoAlmas);
            for (size_t j = 0; j < itensAlmaSize; ++j) {
                int equipSlot; arquivo >> equipSlot;
                arquivo.ignore(); // Consome o espaco
                std::string nomeItem; std::getline(arquivo, nomeItem);
                
                auto novoItem = recriarItemComModificadores(nomeItem);
                if (novoItem) {
                    Item* ptr = novoItem.get();
                    almaPersonagem->obterInventario()->adicionarItem(std::move(novoItem));
                    if (equipSlot != 0) almaPersonagem->equiparItem(ptr);
                }
            }
            
            jogador->adicionarAlma(std::move(almaPersonagem));
        }
    }

    arquivo.close();
    return jogador;
}

void Salvamento::deletarSave(const std::string& nomeArquivo) {
    try {
        std::filesystem::remove(nomeArquivo);
    } catch (const std::exception& e) {
        std::cerr << FuncoesDialogo::formatarMsgSistema(std::string("Erro ao deletar save: ") + e.what(), Cor::VERMELHO) << "\n";
    }
}
