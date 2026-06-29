#include "ItemConsumivel.h"
#include "../../../Core/Controladores/Status.h"
#include "../../../Entidades/Personagem.h"
#include "../../../Core/Utilidades/Aparencia.h"
#include "../../../Interface/Telas/Combate/TelaCombate.h"
#include <functional>
#include <unordered_map>
#include "../FabricaItens.h"

ItemConsumivel::ItemConsumivel(const std::string& nome, int preco) : Item(preco), nome(nome)
{
}

std::string ItemConsumivel::obterNomeItem() const { return nome; }

TipoEquipamento ItemConsumivel::obterTipo() const { return TipoEquipamento::CONSUMIVEL; }

std::vector<std::string> ItemConsumivel::obterDetalhesInspecao(Personagem* /*personagem*/) const {
    std::vector<std::string> detalhes;
    detalhes.push_back(" > Tipo: Consumivel");
    if (!descricaoInspecao.empty()) {
        for (const auto& desc : descricaoInspecao) detalhes.push_back(" > Efeitos: " + desc);
    } else {
        detalhes.push_back(" > Efeitos: Pode ser consumido para aplicar efeitos.");
    }
    return detalhes;
}

std::unique_ptr<Item> fabricarItemConsumivel(ItemID id) {
    auto criarPocaoCura = []() {
        auto cura = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::PocaoCura30), 6);
        cura->adicionarPropriedade(Propriedade::ConsumivelCura);
        cura->definirDescricaoInspecao("Restaura 30% da sua Vida Maxima.");
        cura->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (usuario->obterVida() >= usuario->obterVidaMaxima()) {
                std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Sua vida ja esta cheia!\n";
                return true;
            }
            int vidaAntes = usuario->obterVida();
            int curaEstimada = static_cast<int>(usuario->obterVidaMaxima() * 0.30);
            usuario->modificarVida(curaEstimada);
            int vidaDepois = usuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE) << "[SISTEMA]: " << item->obterNomeItem() << " usada! +" << curaReal << " HP. (Vida atual: " << vidaDepois << "/" << usuario->obterVidaMaxima() << ")" << Aparencia::cor(Cor::RESET) << "\n";
            Aparencia::registrarLogBatalha(Aparencia::cor(Cor::VERDE) + "[SISTEMA]: " + item->obterNomeItem() + " usada! +" + std::to_string(curaReal) + " HP." + Aparencia::cor(Cor::RESET));
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
                std::string nomeDesteItem = item->obterNomeItem();
                for (auto* outroItem : usuario->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != item && outroItem->obterNomeItem() == nomeDesteItem) {
                        usuario->equiparItem(outroItem);
                        break;
                    }
                }
            }

            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return cura;
    };

    auto criarTalisma = [](ItemID id, Propriedade prop, TipoAtributo buffAtr, TipoAtributo debuffAtr, const std::string& desc) {
        auto t = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(id), 120);
        t->adicionarPropriedade(prop);
        t->definirDescricaoInspecao(desc);
        t->definirAcaoInventario([buffAtr, debuffAtr](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            usuario->alterarAtributoEstatico(buffAtr, 5);
            usuario->alterarAtributoEstatico(debuffAtr, -5);
            std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: " << item->obterNomeItem() << " consumido!\n";
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
                std::string nomeDesteItem = item->obterNomeItem();
                for (auto* outroItem : usuario->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != item && outroItem->obterNomeItem() == nomeDesteItem) {
                        usuario->equiparItem(outroItem);
                        break;
                    }
                }
            }

            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return t;
    };

    auto criarBuffAtributos = [](ItemID id) {
        auto buff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(id), 3);
        buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
        buff->definirDescricaoInspecao("Aumenta seus atributos em 1.5x por 2 turnos.");
        buff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n"; return true; }
            usuario->adicionarEfeito(std::make_unique<EfeitoBuffAtributos>(2));
            usuario->definirMultiplicador(1.5);
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE_CLARO) << "[SISTEMA]: " << item->obterNomeItem() << " consumida! Atributos ampliados em 1.5x por 2 turnos!" << Aparencia::cor(Cor::RESET) << "\n";
            Aparencia::registrarLogBatalha(Aparencia::cor(Cor::VERDE_CLARO) + "[SISTEMA]: " + item->obterNomeItem() + " consumida! Atributos ampliados em 1.5x por 2 turnos!" + Aparencia::cor(Cor::RESET));
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
                std::string nomeDesteItem = item->obterNomeItem();
                for (auto* outroItem : usuario->obterInventario()->obterTodosOsItens()) {
                    if (outroItem != item && outroItem->obterNomeItem() == nomeDesteItem) {
                        usuario->equiparItem(outroItem);
                        break;
                    }
                }
            }

            usuario->obterInventario()->removerItem(item);
            *turnoFoiConsumido = true;
            return true;
        });
        return buff;
    };

    auto criarComida = [](ItemID id, int curaHP, int preco, const std::string& desc) {
        auto comida = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(id), preco);
        comida->adicionarPropriedade(Propriedade::ConsumivelCura);
        comida->definirDescricaoInspecao(desc);
        comida->definirAcaoInventario([curaHP](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (usuario->obterVida() >= usuario->obterVidaMaxima()) {
                std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Sua vida ja esta cheia!\n";
                return true;
            }
            int vidaAntes = usuario->obterVida();
            usuario->modificarVida(curaHP);
            int vidaDepois = usuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE) << "[SISTEMA]: " << item->obterNomeItem() << " consumido(a)! +" << curaReal << " HP. (Vida atual: " << vidaDepois << "/" << usuario->obterVidaMaxima() << ")" << Aparencia::cor(Cor::RESET) << "\n";
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
            }
            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return comida;
    };

    auto criarPocaoCuraGrande = []() {
        auto cura = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::PocaoCuraGrande), 30);
        cura->adicionarPropriedade(Propriedade::ConsumivelCura);
        cura->definirDescricaoInspecao("Restaura 50% da sua Vida Maxima.");
        cura->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (usuario->obterVida() >= usuario->obterVidaMaxima()) {
                std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Sua vida ja esta cheia!\n";
                return true;
            }
            int vidaAntes = usuario->obterVida();
            int curaEstimada = static_cast<int>(usuario->obterVidaMaxima() * 0.50);
            usuario->modificarVida(curaEstimada);
            int vidaDepois = usuario->obterVida();
            int curaReal = vidaDepois - vidaAntes;
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE) << "[SISTEMA]: " << item->obterNomeItem() << " usada! +" << curaReal << " HP. (Vida atual: " << vidaDepois << "/" << usuario->obterVidaMaxima() << ")" << Aparencia::cor(Cor::RESET) << "\n";
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
            }
            usuario->obterInventario()->removerItem(item);
            if (turnoFoiConsumido) *turnoFoiConsumido = true;
            return true;
        });
        return cura;
    };

    auto criarPocaoForcaAlquimica = []() {
        auto buff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::PocaoForcaAlquimica), 40);
        buff->adicionarPropriedade(Propriedade::ConsumivelBuff);
        buff->definirDescricaoInspecao("Aumenta Forca em +5 e Destreza em +3 por 3 turnos em combate.");
        buff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Pocoes de buff so podem ser usadas em combate!\n"; return true; }
            usuario->adicionarEfeito(std::make_unique<EfeitoGritoGuerra>(3, 5, 3));
            std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE_CLARO) << "[SISTEMA]: " << item->obterNomeItem() << " consumida! +5 Forca e +3 Destreza por 3 turnos!" << Aparencia::cor(Cor::RESET) << "\n";
            
            if (usuario->obterConsumivelRapido() == item) {
                usuario->desequiparConsumivel();
            }
            usuario->obterInventario()->removerItem(item);
            *turnoFoiConsumido = true;
            return true;
        });
        return buff;
    };

    auto criarPocaoVenenoAlquimica = []() {
        auto debuff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::PocaoVenenoAlquimica), 35);
        debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
        debuff->definirDescricaoInspecao("Aplica Necrose no alvo por 3 turnos (causa 12 de dano por turno).");
        debuff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Pocoes de arremesso so podem ser usadas em combate!\n"; return true; }
            usuario->definirItemSelecionadoParaUso(item);
            return true;
        });
        debuff->definirAcaoUsar([](Personagem* /*usuario*/, Personagem* alvo) {
            if (!Personagem::isValido(alvo) || alvo->obterVida() <= 0) return;
            alvo->adicionarEfeito(std::make_unique<EfeitoNecrose>(3, 12));
            TelaCombate::adicionarMensagemFixa("\n" + Aparencia::margemCombate() + Aparencia::cor(Cor::VERMELHO) + ">> Voce arremessou a pocao! " + alvo->obterNome() + " sofreu necrose (12 dano/turno) por 3 turnos!" + Aparencia::cor(Cor::RESET) + "\n");
        });
        return debuff;
    };

    auto criarPocaoLentidaoAlquimica = []() {
        auto debuff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::PocaoLentidaoAlquimica), 35);
        debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
        debuff->definirDescricaoInspecao("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
        debuff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
            if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Pocoes de arremesso so podem ser usadas em combate!\n"; return true; }
            usuario->definirItemSelecionadoParaUso(item);
            return true;
        });
        debuff->definirAcaoUsar([](Personagem* /*usuario*/, Personagem* alvo) {
            if (!Personagem::isValido(alvo) || alvo->obterVida() <= 0) return;
            alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            TelaCombate::adicionarMensagemFixa("\n" + Aparencia::margemCombate() + Aparencia::cor(Cor::MAGENTA) + ">> Voce arremessou a pocao! " + alvo->obterNome() + " esta sob efeito de Lentidao por 3 turnos!" + Aparencia::cor(Cor::RESET) + "\n");
        });
        return debuff;
    };

    static const std::unordered_map<ItemID, std::function<std::unique_ptr<Item>()>> construtores = {
        {ItemID::Maca, [criarComida]() { return criarComida(ItemID::Maca, 15, 5, "Restaura 15 HP fixo."); }},
        {ItemID::Pao, [criarComida]() { return criarComida(ItemID::Pao, 25, 10, "Restaura 25 HP fixo."); }},
        {ItemID::Queijo, [criarComida]() { return criarComida(ItemID::Queijo, 40, 18, "Restaura 40 HP fixo."); }},
        {ItemID::CarneSeca, [criarComida]() { return criarComida(ItemID::CarneSeca, 60, 30, "Restaura 60 HP fixo."); }},
        {ItemID::PocaoCuraGrande, criarPocaoCuraGrande},
        {ItemID::PocaoForcaAlquimica, criarPocaoForcaAlquimica},
        {ItemID::PocaoVenenoAlquimica, criarPocaoVenenoAlquimica},
        {ItemID::PocaoLentidaoAlquimica, criarPocaoLentidaoAlquimica},
        {ItemID::PocaoCura30, criarPocaoCura},
        {ItemID::PocaoFuria, [criarBuffAtributos]() { return criarBuffAtributos(ItemID::PocaoFuria); }},
        {ItemID::ElixirArcano, [criarBuffAtributos]() { return criarBuffAtributos(ItemID::ElixirArcano); }},
        {ItemID::FrascoGosma, []() {
            auto debuff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::FrascoGosma));
            debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffLentidao);
            debuff->definirDescricaoInspecao("Aplica Lentidao no alvo por 3 turnos (Reduz Destreza).");
            debuff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
                if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return true; }
                usuario->definirItemSelecionadoParaUso(item);
                return true;
            });
            debuff->definirAcaoUsar([](Personagem* /*usuario*/, Personagem* alvo) {
                if (!Personagem::isValido(alvo) || alvo->obterVida() <= 0) return;
                alvo->adicionarEfeito(std::make_unique<EfeitoLentidao>(3));
            TelaCombate::adicionarMensagemFixa("\n" + Aparencia::margemCombate() + Aparencia::cor(Cor::MAGENTA) + ">> Voce jogou o frasco! " + alvo->obterNome() + " esta com lentidao por 3 turnos!" + Aparencia::cor(Cor::RESET) + "\n");
            });
            return debuff;
        }},
        {ItemID::FrascoFraqueza, []() {
            auto debuff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::FrascoFraqueza));
            debuff->adicionarPropriedade(Propriedade::ConsumivelDebuffFraqueza);
            debuff->definirDescricaoInspecao("Aplica Fraqueza no alvo por 3 turnos (-25% Forca).");
            debuff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
                if (!turnoFoiConsumido) { std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: Frascos de debuff so podem ser usados em combate!\n"; return true; }
                usuario->definirItemSelecionadoParaUso(item);
                return true;
            });
            debuff->definirAcaoUsar([](Personagem* /*usuario*/, Personagem* alvo) {
                if (!Personagem::isValido(alvo) || alvo->obterVida() <= 0) return;
                alvo->adicionarEfeito(std::make_unique<EfeitoFraqueza>(3));
            TelaCombate::adicionarMensagemFixa("\n" + Aparencia::margemCombate() + Aparencia::cor(Cor::VERMELHO) + ">> Voce jogou o frasco! " + alvo->obterNome() + " teve sua forca reduzida em 25% por 3 turnos!" + Aparencia::cor(Cor::RESET) + "\n");
            });
            return debuff;
        }},
        {ItemID::OrgaoRegenerador, []() { 
            auto buff = std::make_unique<ItemConsumivel>(FabricaItens::obterNomeDeID(ItemID::OrgaoRegenerador), 500); 
            buff->adicionarPropriedade(Propriedade::ConsumivelPoderTroll); 
            buff->definirDescricaoInspecao("Concede a regeneracao do Troll permanentemente (cura 100% HP apos batalhas).");
            buff->definirAcaoInventario([](Item* item, Personagem* usuario, bool* turnoFoiConsumido) {
                if (usuario->possuiRegeneracaoTroll()) {
                    std::cout << "\n" << Aparencia::margemCombate() << "[SISTEMA]: O poder regenerador do Troll ja corre em suas veias!\n";
                } else {
                    usuario->desbloquearRegeneracaoTroll();
                    usuario->modificarVida(usuario->obterVidaMaxima());
                    std::cout << "\n" << Aparencia::margemCombate() << Aparencia::cor(Cor::VERDE) << "[SISTEMA]: " << item->obterNomeItem() << " consumido! Voce agora curara 100% do seu HP apos cada combate!" << Aparencia::cor(Cor::RESET) << "\n";
                    
                    if (usuario->obterConsumivelRapido() == item) usuario->desequiparConsumivel();
                    
                    usuario->obterInventario()->removerItem(item);
                }
                if (turnoFoiConsumido) *turnoFoiConsumido = true;
                return true;
            });
            return buff; 
        }},
        {ItemID::TalismaUrso, [criarTalisma]() { return criarTalisma(ItemID::TalismaUrso, Propriedade::TalismaForca, TipoAtributo::Forca, TipoAtributo::Inteligencia, "Concede +5 Forca e -5 Inteligencia permanentemente."); }},
        {ItemID::TalismaCorvo, [criarTalisma]() { return criarTalisma(ItemID::TalismaCorvo, Propriedade::TalismaInteligencia, TipoAtributo::Inteligencia, TipoAtributo::Forca, "Concede +5 Inteligencia e -5 Forca permanentemente."); }},
        {ItemID::TalismaLeopardo, [criarTalisma]() { return criarTalisma(ItemID::TalismaLeopardo, Propriedade::TalismaDestreza, TipoAtributo::Destreza, TipoAtributo::Sabedoria, "Concede +5 Destreza e -5 Sabedoria permanentemente."); }},
        {ItemID::TalismaCoruja, [criarTalisma]() { return criarTalisma(ItemID::TalismaCoruja, Propriedade::TalismaSabedoria, TipoAtributo::Sabedoria, TipoAtributo::Destreza, "Concede +5 Sabedoria e -5 Destreza permanentemente."); }}
    };
    auto it = construtores.find(id);
    if (it != construtores.end()) return it->second();
    return nullptr;
}
