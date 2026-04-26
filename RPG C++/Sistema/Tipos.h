#pragma once
#include <string_view>

namespace EfeitoNomes {
    constexpr const char* SANGRAMENTO = "Sangramento";
    constexpr const char* LENTIDAO = "Lentidao";
    constexpr const char* FRAQUEZA = "Fraqueza";
    constexpr const char* QUEBRA_RESISTENCIA = "QuebraResistencia";
    constexpr const char* INVOIVEL = "Inviolavel";
    constexpr const char* METADE_DANO = "MetadeDano";
    constexpr const char* SUGA_SANGUE = "SugaSangue";
    constexpr const char* ATORDOAMENTO = "Atordoamento";
    constexpr const char* BUFF_ATRIBUTOS = "BuffAtributos";
    constexpr const char* GRITO_DE_GUERRA = "GritoDeGuerra";
}

enum class TipoClasse 
{
    Nenhum,
    Arqueiro,
    Bardo,
    Guerreiro,
    Mago,
    InimigoPadrao
};

enum class TipoRaca 
{
    Nenhum,
    Dwarf,
    Elfo,
    Humano,
    Ork,
    OrkExilado,
    Goblin,
    Fada,
    Slime,
    AbominacaoFloresta
};
