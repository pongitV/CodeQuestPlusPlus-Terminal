#pragma once

#include <string>
#include <vector>

#include "../Classes/ClasseBase.h"

class ClasseBaseInimigo : public ClasseBase
{
public:
    std::string obterNomeClasse() const override;
    TipoClasse obterTipoClasse() const override { return TipoClasse::Nenhum; }
    Atributos obterAtributosClasse() const override;
    const std::vector<std::string>& obterAparenciaClasseMenu() const override;
    std::vector<std::unique_ptr<Item>> obterEquipamentoClasse() const override;

    std::string obterNomePassivaClasse() const override;
    std::string obterDescricaoPassivaClasse() const override;
    std::string obterRecargaHabilidadeClasse() const override;

    void usarHabilidadeClasse(Combate* combate, Personagem* personagemUsuario, std::vector<Personagem*>& listaDeInimigos) override;
    std::string obterNomeHabilidadeClasse() const override;
    std::string obterDescricaoHabilidadeClasse() const override;
    
    TipoAtaque obterTipoAtaque() const override;
    bool habilidadeConsomeTurno() const override;
};
