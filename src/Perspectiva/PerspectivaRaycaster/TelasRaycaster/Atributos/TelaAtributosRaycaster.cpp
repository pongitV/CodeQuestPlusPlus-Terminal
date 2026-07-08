#include "TelaAtributosRaycaster.h"
#include "../../../TelasBase/Atributos/TelaAtributos.h"
#include "../../../TelasBase/Atributos/TelaAtributosLayout.h"
#include "../../../TelasBase/TelaBase.h"
#include "../../../../Core/Utilidades/Aparencia.h"
#include "../../../../Core/Utilidades/ControleDeInput.h"
#include "../../../../Sistemas/Inventario/Item.h"
#include "../../../../Entidades/Racas/RacaBase.h"
#include "../../../../Entidades/Classes/ClasseBase.h"
#include "../../EngineRaycaster/Raycaster.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>

struct EfeitoInfo {
    EfeitoID efeitoId;
    Cor corId;
    const char* exibirNome;
    bool mostrarTurnos;
};

static void exibirTituloFlutuante(int startY) {
    int larguraConsole = Aparencia::obterLarguraTerminal();
    int logoHeight = ArtesAtributos::logoFicha.size();
    int logoY = startY - 1 - logoHeight;
    if (logoY < 0) logoY = 0;
    
    int compVisualLogo = 0;
    for (const auto& linha : ArtesAtributos::logoFicha) {
        int comp = Aparencia::obterComprimentoVisual(linha);
        if (comp > compVisualLogo) compVisualLogo = comp;
    }
    int logoX = (larguraConsole - compVisualLogo) / 2;
    if (logoX < 0) logoX = 0;
    
    std::string corTitulo = Aparencia::cor(Cor::MAGENTA);
    for (int i = 0; i < logoHeight; ++i) {
        Aparencia::moverCursor(logoX, logoY + i);
        const std::string& linha = ArtesAtributos::logoFicha[i];
        
        std::string buffer = corTitulo;
        int spaceCount = 0;
        for (size_t j = 0; j < linha.length(); ) {
            if (linha[j] == ' ') {
                spaceCount++;
                j++;
            } else {
                if (spaceCount > 0) {
                    buffer += "\033[" + std::to_string(spaceCount) + "C";
                    spaceCount = 0;
                }
                unsigned char uc = linha[j];
                int charLen = 1;
                if ((uc & 0x80) == 0) charLen = 1;
                else if ((uc & 0xE0) == 0xC0) charLen = 2;
                else if ((uc & 0xF0) == 0xE0) charLen = 3;
                else if ((uc & 0xF8) == 0xF0) charLen = 4;
                buffer += linha.substr(j, charLen);
                j += charLen;
            }
        }
        buffer += "\033[0m";
        std::cout << buffer;
    }
}

void TelaAtributosRaycaster::exibir(Personagem* jogadorAtual) {}

enum EstadoAtributos { PRINCIPAL, HABILIDADES, DETALHES, SUBIR_NIVEL, ERRO_NIVEL };

void TelaAtributosRaycaster::gerenciarFichaDoJogador(Personagem* jogadorAtual) {
    if (!jogadorAtual) return;
    
    int selecaoAtual = 0;
    int selecaoSubir = 0;
    std::vector<std::string> opcoes = {"Subir de Nivel", "Habilidades e Equipamento", "Detalhes de Atributos", "Voltar"};
    EstadoAtributos estado = PRINCIPAL;
    
    bool executando = true;
    
    int ultimoStartX = -1;
    int ultimoStartY = -1;
    int ultimoW = -1;
    int ultimoH = -1;

    while (executando) {
        std::cout << "\033[?25l";
        
        int larguraConsole = Aparencia::obterLarguraTerminal();
        double multiplicadorDeAtributosAtual = jogadorAtual->obterMultiplicador();
        DebuffInfo debuff = TelaAtributos::calcularDebuff(jogadorAtual);
        bool temBuff = debuff.temBuff;
        
        std::vector<std::string> linhasPrincipal;
        std::vector<std::string> linhasHabilidades;
        std::vector<std::string> linhasDetalhes;
        std::vector<std::string> linhasSubirNivel;
        std::vector<std::string> linhasErroNivel;

        // --- 1. PREPARAR PRINCIPAL ---
        linhasPrincipal.push_back("NOME: " + jogadorAtual->obterNome() + "   RACA: " + jogadorAtual->obterRaca()->obterNomeRaca() + "   CLASSE: " + jogadorAtual->obterNomeClasse());
        double porcentagemXp = static_cast<double>(jogadorAtual->obterXpAtual()) / std::max(1, jogadorAtual->obterXpParaSubir());
        std::string barraXp = TelaBase::gerarBarraGradiente(porcentagemXp, 10, Cor::CIANO);
        double pctVida = static_cast<double>(jogadorAtual->obterVida()) / std::max(1, jogadorAtual->obterVidaMaxima());
        Cor corVidaEnum = (pctVida > 0.7) ? Cor::VERDE : (pctVida > 0.3) ? Cor::AMARELO : Cor::VERMELHO;
        std::string barraVida = TelaBase::gerarBarraGradiente(pctVida, 10, corVidaEnum);

        linhasPrincipal.push_back("NIVEL: " + std::to_string(jogadorAtual->obterNivel()) + " [" + barraXp + Aparencia::cor(Cor::RESET) + "]   HP: [" + barraVida + Aparencia::cor(Cor::RESET) + "] " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()) + "   OURO: " + Aparencia::cor(Cor::AMARELO) + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G" + Aparencia::cor(Cor::RESET));

        std::string difStr = "DIFICULDADE: ";
        switch (jogadorAtual->obterDificuldade()) {
            case DificuldadeJogo::Facil: difStr += Aparencia::cor(Cor::VERDE) + "Facil"; break;
            case DificuldadeJogo::Normal: difStr += Aparencia::cor(Cor::AMARELO) + "Normal"; break;
            case DificuldadeJogo::Dificil: difStr += Aparencia::cor(Cor::VERMELHO) + "Dificil"; break;
        }
        difStr += Aparencia::cor(Cor::RESET) + "   [PARRY]: ";
        difStr += jogadorAtual->obterParryAtivado() ? (Aparencia::cor(Cor::VERDE) + "Ligado" + Aparencia::cor(Cor::RESET)) : (Aparencia::cor(Cor::CINZA) + "Desligado" + Aparencia::cor(Cor::RESET));
        linhasPrincipal.push_back(difStr);
        
        static const EfeitoInfo efeitosParaExibir[] = {
            {EfeitoID::BuffAtributos, Cor::VERDE_CLARO, "Buff Atributos", true},
            {EfeitoID::Lentidao, Cor::MAGENTA, "Lentidao", true},
            {EfeitoID::Sangramento, Cor::VERMELHO, "Sangramento", true},
            {EfeitoID::Fraqueza, Cor::VERMELHO, "Fraqueza", true},
            {EfeitoID::QuebraResistencia, Cor::CIANO, "Quebra de Resistencia", false},
            {EfeitoID::RodaAdaptacao, Cor::AMARELO, "Adaptacao Divina", false},
            {EfeitoID::Necrose, Cor::MAGENTA, "Necrose", true},
        };
        std::string statusAtuais = "STATUS: ";
        bool temStatus = false;
        for (const auto& info : efeitosParaExibir) {
            if (jogadorAtual->possuiEfeito(info.efeitoId)) {
                if (temStatus) statusAtuais += ", ";
                statusAtuais += Aparencia::cor(info.corId) + info.exibirNome;
                if (info.mostrarTurnos) statusAtuais += " (" + std::to_string(jogadorAtual->obterTurnosEfeito(info.efeitoId)) + ")";
                statusAtuais += Aparencia::cor(Cor::RESET);
                temStatus = true;
            }
        }
        if (!temStatus) statusAtuais += "Nenhum";
        linhasPrincipal.push_back(statusAtuais);
        linhasPrincipal.push_back("");

        auto formatarAtr = [temBuff, multiplicadorDeAtributosAtual](std::string nomeDoAtributo, int valorBaseDoAtributo, int valorPerdidoPorDebuff, Cor corBase) -> std::string {
            int bonusBuff = temBuff ? static_cast<int>(valorBaseDoAtributo * multiplicadorDeAtributosAtual) - valorBaseDoAtributo : 0;
            double pct = std::min(1.0, valorBaseDoAtributo / 50.0);
            std::string barra = TelaBase::gerarBarraGradiente(pct, 10, corBase);
            std::ostringstream ss;
            ss << std::left << std::setw(13) << nomeDoAtributo << ": " << std::setw(3) << valorBaseDoAtributo << " [" << barra << Aparencia::cor(Cor::RESET) << "]";
            if (temBuff && bonusBuff > 0) ss << " " << Aparencia::cor(Cor::VERDE_CLARO) << "(+" << bonusBuff << ")" << Aparencia::cor(Cor::RESET);
            else if (valorPerdidoPorDebuff > 0) ss << " " << Aparencia::cor(Cor::VERMELHO) << "(-" << valorPerdidoPorDebuff << ")" << Aparencia::cor(Cor::RESET);
            return ss.str();
        };

        linhasPrincipal.push_back(formatarAtr("Forca", jogadorAtual->obterForca(), debuff.forcaPerdida, Cor::VERMELHO) + "   " + formatarAtr("Inteligencia", jogadorAtual->obterInteligencia(), 0, Cor::ROXO));
        linhasPrincipal.push_back(formatarAtr("Destreza", jogadorAtual->obterDestreza(), debuff.destrezaPerdida, Cor::LARANJA) + "   " + formatarAtr("Sabedoria", jogadorAtual->obterSabedoria(), 0, Cor::LILAS));
        linhasPrincipal.push_back(formatarAtr("Resistencia", jogadorAtual->obterResistencia(), debuff.resPerdida, Cor::AZUL) + "   " + formatarAtr("Constituicao", jogadorAtual->obterConstituicao(), debuff.constPerdida, Cor::CIANO));
        linhasPrincipal.push_back("");

        PoderCombate poder = TelaAtributos::calcularPoderCombate(jogadorAtual, multiplicadorDeAtributosAtual);
        std::string sFis = Aparencia::cor(Cor::VERMELHO) + std::to_string(poder.danoFisEst) + Aparencia::cor(Cor::RESET);
        std::string sMag = Aparencia::cor(Cor::VERMELHO) + std::to_string(poder.danoMagEst) + Aparencia::cor(Cor::RESET);
        std::string sDef = Aparencia::cor(Cor::AZUL) + std::to_string(poder.defFixa) + Aparencia::cor(Cor::RESET);
        std::ostringstream ssMit; ssMit << std::fixed << std::setprecision(1) << poder.mitigacao;
        std::string sMit = Aparencia::cor(Cor::CIANO) + ssMit.str() + "%" + Aparencia::cor(Cor::RESET);
        
        linhasPrincipal.push_back("PODER DE COMBATE: Dano Fisico: " + sFis + " | Dano Magico: " + sMag + " | Defesa Fixa: " + sDef + " | Mitigacao: " + sMit);
        linhasPrincipal.push_back("");
        
        for (int i = 0; i < (int)opcoes.size(); ++i) {
            std::string cursor = (i == selecaoAtual && estado == PRINCIPAL) ? (Aparencia::cor(Cor::VERDE) + " > ") : "   ";
            std::string corOpcao = (i == selecaoAtual && estado == PRINCIPAL) ? Aparencia::cor(Cor::VERDE) : Aparencia::cor(Cor::BRANCO);
            linhasPrincipal.push_back(cursor + corOpcao + opcoes[i] + Aparencia::cor(Cor::RESET));
        }

        // --- 2. PREPARAR HABILIDADES ---
        auto adicionarDescricaoSplit = [&](const std::string& texto) {
            std::istringstream stream(texto);
            std::string linhaDesc;
            while (std::getline(stream, linhaDesc)) linhasHabilidades.push_back(" - " + Aparencia::cor(Cor::CINZA) + linhaDesc + Aparencia::cor(Cor::RESET));
        };
        linhasHabilidades.push_back("[HAB. Passiva de Raca]  : " + jogadorAtual->obterRaca()->obterNomeHabilidadeRaca());
        adicionarDescricaoSplit(jogadorAtual->obterRaca()->obterDescricaoHabilidadeRaca());
        linhasHabilidades.push_back("[HAB. Passiva de Classe]: " + jogadorAtual->obterClasse()->obterNomePassivaClasse());
        adicionarDescricaoSplit(jogadorAtual->obterClasse()->obterDescricaoPassivaClasse());
        linhasHabilidades.push_back("[HAB. Ativa de Classe]  : " + jogadorAtual->obterClasse()->obterNomeHabilidadeClasse());
        adicionarDescricaoSplit(jogadorAtual->obterClasse()->obterDescricaoHabilidadeClasse());
        linhasHabilidades.push_back("");
        linhasHabilidades.push_back("EQUIPAMENTOS:");
        std::string arma = jogadorAtual->obterArma() ? jogadorAtual->obterArma()->obterNomeItem() + jogadorAtual->obterArma()->obterInfoStatus() : "Punhos";
        std::string escudo = jogadorAtual->obterEscudo() ? jogadorAtual->obterEscudo()->obterNomeItem() + jogadorAtual->obterEscudo()->obterInfoStatus() : "Nenhum";
        std::string armadura = jogadorAtual->obterArmadura() ? jogadorAtual->obterArmadura()->obterNomeItem() + jogadorAtual->obterArmadura()->obterInfoStatus() : "Trapos";
        linhasHabilidades.push_back(" > Arma    : " + arma);
        linhasHabilidades.push_back(" > Escudo  : " + escudo);
        linhasHabilidades.push_back(" > Armadura: " + armadura);
        linhasHabilidades.push_back("");
        linhasHabilidades.push_back(Aparencia::cor(Cor::BRANCO) + " [ Voltar ]" + Aparencia::cor(Cor::RESET));

        // --- 3. PREPARAR DETALHES ---
        std::vector<std::string> arteRaca = jogadorAtual->obterRaca()->obterAparenciaRaca();
        std::vector<std::string> arteClasse = jogadorAtual->obterClasse()->obterAparenciaClasseMenu();
        
        int maxArt = std::max(arteRaca.size(), arteClasse.size());
        int largArteBase = 45; 
        linhasDetalhes.push_back("RACA: " + jogadorAtual->obterRaca()->obterNomeRaca() + std::string(largArteBase - 6 - jogadorAtual->obterRaca()->obterNomeRaca().length(), ' ') + "CLASSE: " + jogadorAtual->obterNomeClasse());
        linhasDetalhes.push_back("");
        
        for (int i = 0; i < maxArt; ++i) {
            std::string rL = (i < (int)arteRaca.size()) ? arteRaca[i] : "";
            std::string cL = (i < (int)arteClasse.size()) ? arteClasse[i] : "";
            
            int lenR = Aparencia::obterComprimentoVisual(rL);
            if (lenR < largArteBase) rL += std::string(largArteBase - lenR, ' ');
            linhasDetalhes.push_back(" " + rL + " " + cL);
        }
        linhasDetalhes.push_back("");

        std::string cB = Aparencia::cor(Cor::BRANCO);
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::VERDE)          + "Vida         " + cB + ": Pontos de vida (HP) maximos do personagem.");
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::VERMELHO)       + "Forca        " + cB + ": Aumenta o dano base de ataques fisicos.");
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::LARANJA)        + "Destreza     " + cB + ": Aumenta o dano fisico % e define a ordem de turno.");
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::AZUL)           + "Resistencia  " + cB + ": Reduz o dano recebido de forma fixa.");
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::CIANO)          + "Constituicao " + cB + ": Reduz o dano recebido em porcentagem.");
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::ROXO)           + "Inteligencia " + cB + ": Aumenta o dano base de ataques magicos.");
        linhasDetalhes.push_back(" " + Aparencia::cor(Cor::LILAS)          + "Sabedoria    " + cB + ": Aumenta o dano magico % e a potencia de curas.");
        linhasDetalhes.push_back("");
        linhasDetalhes.push_back("═══ ATRIBUTOS RECOMENDADOS PARA A CLASSE " + jogadorAtual->obterNomeClasse() + " ═══");
        
        TipoClasse tipo = jogadorAtual->obterTipoClasse();
        if (tipo == TipoClasse::Guerreiro) {
            linhasDetalhes.push_back(" 1. " + Aparencia::cor(Cor::VERMELHO)    + "Forca    " + cB + ": Aumenta o dano base, essencial para armas pesadas.");
            linhasDetalhes.push_back(" 2. " + Aparencia::cor(Cor::LARANJA)     + "Destreza " + cB + ": Multiplica o dano final, util para qualquer build fisica.");
        } else if (tipo == TipoClasse::Arqueiro) {
            linhasDetalhes.push_back(" 1. " + Aparencia::cor(Cor::LARANJA)     + "Destreza " + cB + ": Atributo principal, aumenta o dano e define a ordem de turno.");
            linhasDetalhes.push_back(" 2. " + Aparencia::cor(Cor::VERMELHO)    + "Forca    " + cB + ": Aumenta o dano base, fortalecendo o dano fisico geral.");
        } else if (tipo == TipoClasse::Mago) {
            linhasDetalhes.push_back(" 1. " + Aparencia::cor(Cor::ROXO)        + "Inteligencia " + cB + ": Essencial, aumenta drasticamente o dano base de magias.");
            linhasDetalhes.push_back(" 2. " + Aparencia::cor(Cor::LILAS)       + "Sabedoria    " + cB + ": Multiplica o dano magico final e fortalece habilidades.");
        } else if (tipo == TipoClasse::Bardo) {
            linhasDetalhes.push_back(" 1. " + Aparencia::cor(Cor::LILAS)       + "Sabedoria    " + cB + ": Fortalece intensamente os efeitos das curas e utilidade do Bardo.");
            linhasDetalhes.push_back(" 2. " + Aparencia::cor(Cor::ROXO)        + "Inteligencia " + cB + ": Melhora o dano magico, permitindo que o Bardo lute efetivamente.");
        } else if (tipo == TipoClasse::NECROMANTE) {
            linhasDetalhes.push_back(" 1. " + Aparencia::cor(Cor::LILAS)       + "Sabedoria    " + cB + ": Melhora magias e a durabilidade dos seus lacaios invocados.");
            linhasDetalhes.push_back(" 2. " + Aparencia::cor(Cor::CIANO)       + "Constituicao " + cB + ": Importante para sobrevivencia enquanto suas invocacoes lutam.");
        } else {
            linhasDetalhes.push_back(" Nenhum atributo de dano especifico definido para esta classe.");
        }
        linhasDetalhes.push_back("");
        linhasDetalhes.push_back(Aparencia::cor(Cor::BRANCO) + " [ Voltar ]" + Aparencia::cor(Cor::RESET));

        // --- 4. PREPARAR SUBIR NIVEL ---
        std::vector<std::string> nomesAtr = {"Vida", "Forca", "Destreza", "Resistencia", "Constituicao", "Inteligencia", "Sabedoria"};
        std::vector<std::string> opcoesAtr;
        for (int i = 1; i <= 7; ++i) {
            auto clonePreview = jogadorAtual->clone();
            clonePreview->subirDeNivel(static_cast<TipoAtributo>(i));

            int valAtual = 0, valNovo = 0;
            switch (i) {
                case 1: valAtual = jogadorAtual->obterVidaMaxima(); valNovo = clonePreview->obterVidaMaxima(); break;
                case 2: valAtual = jogadorAtual->obterForca(); valNovo = clonePreview->obterForca(); break;
                case 3: valAtual = jogadorAtual->obterDestreza(); valNovo = clonePreview->obterDestreza(); break;
                case 4: valAtual = jogadorAtual->obterResistencia(); valNovo = clonePreview->obterResistencia(); break;
                case 5: valAtual = jogadorAtual->obterConstituicao(); valNovo = clonePreview->obterConstituicao(); break;
                case 6: valAtual = jogadorAtual->obterInteligencia(); valNovo = clonePreview->obterInteligencia(); break;
                case 7: valAtual = jogadorAtual->obterSabedoria(); valNovo = clonePreview->obterSabedoria(); break;
            }
            int ganho = valNovo - valAtual;
            opcoesAtr.push_back(nomesAtr[i - 1] + " " + Aparencia::cor(Cor::CINZA) + "(" + std::to_string(valAtual) + " -> " + std::to_string(valNovo) + " [" + Aparencia::cor(Cor::VERDE) + "+" + std::to_string(ganho) + Aparencia::cor(Cor::CINZA) + "])" + Aparencia::cor(Cor::RESET));
        }
        opcoesAtr.push_back("Cancelar");

        linhasSubirNivel.push_back("Escolha o atributo para melhorar:");
        linhasSubirNivel.push_back("");
        for (int i = 0; i < (int)opcoesAtr.size(); ++i) {
            std::string cursor = (i == selecaoSubir && estado == SUBIR_NIVEL) ? (Aparencia::cor(Cor::VERDE) + " > ") : "   ";
            std::string corOpcao = (i == selecaoSubir && estado == SUBIR_NIVEL) ? Aparencia::cor(Cor::VERDE) : Aparencia::cor(Cor::BRANCO);
            linhasSubirNivel.push_back(cursor + corOpcao + opcoesAtr[i] + Aparencia::cor(Cor::RESET));
        }

        // --- 5. PREPARAR ERRO NIVEL ---
        linhasErroNivel.push_back("");
        linhasErroNivel.push_back("");
        linhasErroNivel.push_back(Aparencia::cor(Cor::VERMELHO) + " Voce nao tem XP suficiente para subir de nivel!" + Aparencia::cor(Cor::RESET));
        linhasErroNivel.push_back("");
        linhasErroNivel.push_back("");
        linhasErroNivel.push_back(Aparencia::cor(Cor::BRANCO) + " [ Voltar ]" + Aparencia::cor(Cor::RESET));

        // --- PADDING & RENDERIZACAO ---
        std::vector<std::string>* linhasAlvo = nullptr;
        std::string tituloCaixa = "";
        
        if (estado == PRINCIPAL) { linhasAlvo = &linhasPrincipal; }
        else if (estado == HABILIDADES) { linhasAlvo = &linhasHabilidades; tituloCaixa = "Habilidades & Equipamentos"; }
        else if (estado == DETALHES) { linhasAlvo = &linhasDetalhes; tituloCaixa = "Detalhes de Atributos"; }
        else if (estado == SUBIR_NIVEL) { linhasAlvo = &linhasSubirNivel; tituloCaixa = "Subir de Nivel"; }
        else if (estado == ERRO_NIVEL) { linhasAlvo = &linhasErroNivel; tituloCaixa = "Aviso"; }

        int boxW = 80;
        for (const auto& l : *linhasAlvo) {
            int len = Aparencia::obterComprimentoVisual(l);
            if (len > boxW) boxW = len;
        }

        std::vector<std::string> caixaFinal = TelaBase::criarCaixa(*linhasAlvo, tituloCaixa, boxW, Cor::MAGENTA, "\033[48;2;25;25;25m");
        
        int startY = (Aparencia::obterAlturaTerminal() - caixaFinal.size()) / 2;
        if (startY < 0) startY = 0;
        
        int startX = (larguraConsole - Aparencia::obterComprimentoVisual(caixaFinal[0])) / 2;

        // Limpa o fantasma da caixa anterior desenhando o fundo 3D salvo novamente
        if (ultimoH > 0 && ultimoW > 0) {
            Raycaster::restaurarUltimoQuadro();
        }
        
        ultimoStartX = startX;
        ultimoStartY = startY;
        ultimoW = Aparencia::obterComprimentoVisual(caixaFinal[0]);
        ultimoH = caixaFinal.size();

        exibirTituloFlutuante(startY);
        
        for (size_t i = 0; i < caixaFinal.size(); ++i) {
            Aparencia::moverCursor(startX, startY + i);
            std::cout << caixaFinal[i];
        }
        std::cout << std::flush;
        
        // --- INPUT ---
        char c = ControleDeInput::lerTecla();
        if (estado == PRINCIPAL) {
            if (c == 'w' || c == 'W' || c == 72) {
                selecaoAtual--; if (selecaoAtual < 0) selecaoAtual = opcoes.size() - 1;
            } else if (c == 's' || c == 'S' || c == 80) {
                selecaoAtual++; if (selecaoAtual >= (int)opcoes.size()) selecaoAtual = 0;
            } else if (c == '\r' || c == '\n') {
                if (selecaoAtual == 0) {
                    if (!jogadorAtual->podeSubirDeNivel()) estado = ERRO_NIVEL;
                    else { estado = SUBIR_NIVEL; selecaoSubir = 0; }
                }
                else if (selecaoAtual == 1) estado = HABILIDADES;
                else if (selecaoAtual == 2) estado = DETALHES;
                else if (selecaoAtual == 3) executando = false;
            } else if (c == 27) {
                executando = false;
            }
        } else if (estado == SUBIR_NIVEL) {
            if (c == 'w' || c == 'W' || c == 72) {
                selecaoSubir--; if (selecaoSubir < 0) selecaoSubir = 7;
            } else if (c == 's' || c == 'S' || c == 80) {
                selecaoSubir++; if (selecaoSubir > 7) selecaoSubir = 0;
            } else if (c == '\r' || c == '\n') {
                if (selecaoSubir == 7) { 
                    estado = PRINCIPAL;
                } else {
                    jogadorAtual->subirDeNivel(static_cast<TipoAtributo>(selecaoSubir + 1));
                    estado = PRINCIPAL;
                }
            } else if (c == 27) {
                estado = PRINCIPAL;
            }
        } else {
            if (c == '\r' || c == '\n' || c == 27) {
                estado = PRINCIPAL;
            }
        }
    }
}

void TelaAtributosRaycaster::exibirDetalhesAtributos(Personagem*) {}
