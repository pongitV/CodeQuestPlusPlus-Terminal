#pragma once

#include <memory>

#include "../../Entidades/Personagem.h"

class MenuJogo
{
public:
    static std::unique_ptr<Personagem> menuPrincipal();
    static std::unique_ptr<Personagem> iniciarCriacaoDeSistemaPersonagem();

private:
    static std::vector<std::string> lerInformacoesDosSaves(const std::vector<std::string>& saves);
};
