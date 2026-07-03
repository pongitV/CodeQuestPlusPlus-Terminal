#pragma once

#include <string>
#include <vector>
#include "Aparencia.h"

class FuncoesDialogo {
public:
    static void imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::string& texto, bool novaLinhaAntes = true, bool novaLinhaDepois = true);
    static void imprimirDialogoNPC(const std::string& npcNome, Cor npcCor, const std::vector<std::string>& linhas);
    
    static std::string formatarMsgNarracao(const std::string& texto);
    static std::string formatarMsgSistema(const std::string& texto, Cor corTema = Cor::AMARELO);
    static std::string formatarMsgHabilidade(const std::string& texto, Cor corTema = Cor::VERDE_CLARO);
    static std::string formatarMsgStatus(const std::string& texto, Cor corTema = Cor::AMARELO);
    static std::string formatarMsgDrop(const std::string& texto, Cor corTema = Cor::BRANCO);
    static std::string formatarMsgCombate(const std::string& texto, Cor corTema = Cor::BRANCO);
    static std::string formatarMsgInteracao(const std::string& texto, Cor corTema = Cor::CIANO);
};