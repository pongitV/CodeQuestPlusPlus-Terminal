# Plano de Implementação — Fluxo do Menu Principal

## Visão Geral

Sequência de telas do início do jogo:

```
TelaAbertura → TelaNome → TelaRaca → TelaClasse → TelaDificuldade → TelaParry → TelaIntroducao
```

Cada etapa é uma classe em `TelasBase/Menu/` seguindo o padrão Facade + IDE/Raycaster.

---

## 1. TelaAbertura

**Arquivos:** `TelasBase/Menu/TelaAbertura.h/.cpp`
**Facade que delega para** `TelaAberturaIDE` / `TelaAberturaRaycaster`

- Título do jogo com fade-in (reaproveitar `exibirPainelLogoJogo` existente)
- Abaixo: animação ASCII (ex: `TelaMenuLayout.h` já tem sprites de classe/animação)
- Abaixo: `[PRESSIONE ENTER PARA CONTINUAR]` pulsando (usando timer + overwrite)
- Aguarda ENTER via `ControleDeInput::aguardarEnter()`

**Responsabilidade:** `TelaMenu` atual já tem `exibirPainelLogoJogo`. Extrair para `TelaAbertura` + adicionar animação + prompt enter.

---

## 2. TelaNome

**Arquivos:** `TelasBase/Menu/TelaNome.h/.cpp`

- Fundo com arte ASCII decorativa
- `"DIGITE O NOME DO SEU PERSONAGEM:"` centralizado
- Input via `ControleDeInput::lerEntradaProtegida()`
- Validação: nome não vazio, máximo 20 caracteres
- Confirmação: exibe nome digitado + "Confirma? (S/N)"

**Responsabilidade:** `TelaMenu::exibirPromptNome()` já existe. Extrair para `TelaNome` com layout melhor e confirmação.

---

## 3. TelaRaca

**Arquivos:** `TelasBase/Menu/TelaRaca.h/.cpp`

- Arte ASCII da raça selecionada à esquerda
- Descrição + atributos base à direita
- Navegação por setas (esquerda/direita troca raça)
- Enter confirma, exibe confirmação lado a lado

**Responsabilidade:** `TelaMenu::exibirPromptRaca()` já faz isso. Extrair com layout mais rico.

---

## 4. TelaClasse

**Arquivos:** `TelasBase/Menu/TelaClasse.h/.cpp`

- Mesmo layout da raça: arte + descrição + atributos
- `TelaMenu::comporQuadroDeAtributos()` já existe
- `TelaMenu::exibirPromptClasse()` já existe
- Pode incluir pré-visualização de habilidade única

---

## 5. TelaDificuldade

**Arquivos:** `TelasBase/Menu/TelaDificuldade.h/.cpp`

- Mostra besta/icone ASCII representando cada dificuldade
- Navegação por setas
- Exibe descrição da dificuldade e modificadores

**Responsabilidade:** `TelaMenu::exibirPromptDificuldade()` já existe.

---

## 6. TelaParry

**Arquivos:** `TelasBase/Menu/TelaParry.h/.cpp`

- Opção: "Sistema de Parry" ON/OFF
- Se ON: "Deseja fazer o tutorial?" S/N
- Se SIM: chama `TelaTutorial::exibirTutorialDeParry()`
- Exibir diferença entre os modos em texto

**Responsabilidade:** `TelaMenu::exibirPromptParry()` + `exibirTutorialDeParry()` já existem.

---

## 7. TelaIntroducao

**Arquivos:** `TelasBase/Menu/TelaIntroducao.h/.cpp`

- Tela preta (fundo `Cor::PRETO` ou `\033[48;2;0;0;0m`)
- Diálogo estilo digitação (`Aparencia::imprimirStringAnimada`)
- Texto de contexto da história (abertura narrativa)
- Ao final, ENTER para prosseguir ao jogo

**Responsabilidade:** `TelaMenu::exibirIntroducaoJornada()` já existe. Extrair com animação de digitação no lugar de texto estático.

---

## Fluxo de Orquestração

`MenuJogo::iniciarCriacaoDeSistemaPersonagem()` atualizado:

```cpp
Personagem* personagem = new Personagem();

// 1. Tela de abertura
TelaAbertura::exibir();  // fade-in + animação + ENTER

// 2. Nome
personagem->definirNome(TelaNome::exibir());

// 3-6. Seleções
TelaRaca::Resultado raca = TelaRaca::exibir();
TelaClasse::Resultado classe = TelaClasse::exibir(raca);
TelaDificuldade::Resultado diff = TelaDificuldade::exibir();
TelaParry::Resultado parry = TelaParry::exibir();
if (parry.tutorial) TelaTutorial::exibirTutorialDeParry();

// Aplicar escolhas ao personagem
personagem->aplicarRaca(raca);
personagem->aplicarClasse(classe);
personagem->aplicarDificuldade(diff);

// 7. Introdução
TelaIntroducao::exibir(diff);

return personagem;
```

## Padrão de Implementação

Cada tela segue:

```cpp
// TelaXxx.h
class TelaXxx {
public:
    struct Resultado { /* campos específicos */ };
    static Resultado exibir();
};
```

```cpp
// TelaXxx.cpp
#include "TelaXxx.h"
#include "TelaXxxLayout.h"  // se tiver arte ASCII
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaXxxIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaXxxRaycaster.h"

TelaXxx::Resultado TelaXxx::exibir() {
    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva())
        return TelaXxxRaycaster::exibir();
    return TelaXxxIDE::exibir();
}
```

## Dependências

- `TelaRaca` e `TelaClasse` precisam das artes ASCII de `TelaMenuLayout.h`
- `TelaDificuldade` precisa das artes de besta em `TelaMenuLayout.h`
- `TelaIntroducao` usa `Aparencia::imprimirStringAnimada()` e `Aparencia::limparTela()`
- Todas usam `ControleDeInput` para input
- Todas usam `TelaBase` para layout (caixas, linhas divisórias, etc.)

## Próximos Passos (ordem)

1. Criar `TelaAbertura` — mais simples, reúne arte + animação + ENTER
2. Extrair `TelaNome` com validação e confirmação
3. Extrair `TelaRaca` com navegação por setas
4. Extrair `TelaClasse` com pré-visualização de atributos
5. Extrair `TelaDificuldade` com descrição de modificadores
6. Extrair `TelaParry` com opção de tutorial
7. Extrair `TelaIntroducao` com digitação animada
8. Atualizar `MenuJogo` para usar as novas telas
9. Remover métodos antigos de `TelaMenu` que foram extraídos
10. Build + testes
