# Plano de Reorganização do Código

## Objetivo
Reorganizar a estrutura de diretórios e arquivos para melhor separação de responsabilidades entre as duas perspectivas (Raycaster 3D e IDE/Terminal), isolando temas e aparências enquanto compartilha lógica base.

---

## Estrutura Atual (pós-Fase 3)

```
src/
├── Core/                          # (inalterado) Lógica central do jogo
│   ├── Controladores/
│   └── Utilidades/
│
├── Entidades/                     # (inalterado) Personagens, inimigos, etc.
│   ├── Classes/
│   ├── Inimigos/
│   ├── NPCs/
│   ├── Racas/
│   └── Personagem.h/cpp
│
├── Mapas/                         # (renomeado de Mundo/)
│   ├── ControleMapa.h/cpp
│   ├── MapaInteracao.h
│   ├── Floresta/
│   ├── Reino/
│   └── Vila/
│
├── Sistemas/                      # (inalterado) Sistemas de gameplay
│   ├── Combate/
│   ├── Inventario/
│   ├── Minigames/
│   └── Progresso/
│
├── Perspectiva/                   # (renomeado de Visoes/)
│   ├── GerenciadorVisao.h/cpp
│   ├── IVisaoRenderer.h
│   │
│   ├── TelasBase/                # Lógica de funcionamento + delegação para IDE/Raycaster
│   │   ├── TelaBase.h/cpp
│   │   ├── TelaCena3D.h/cpp
│   │   ├── Combate/       (TelaCombate.h/cpp, TelaCombateLayout.h)
│   │   ├── Inventario/    (TelaInventario.h/cpp, TelaInventarioLayout.h)
│   │   ├── Atributos/     (TelaAtributos.h/cpp, TelaAtributosLayout.h) — fórmulas de dano/debuff
│   │   ├── Diario/        (TelaDiario.h/cpp, TelaDiarioLayout.h, TelaDiarioLogic.h/cpp) — categorização
│   │   ├── Bestiario/     (TelaBestiario.h/cpp, TelaBestiarioLayout.h)
│   │   ├── MapaMundial/   (TelaMapaMundial.h/cpp) — validação de viagem
│   │   ├── Menu/          (TelaMenu.h/cpp, TelaMenuLayout.h, TelaTutorial.h/cpp) — tutorial Parry
│   │   ├── Pause/         (TelaPause.h/cpp) — config de jogo (dificuldade, parry, save)
│   │   ├── Vitoria/       (TelaVitoria.h/cpp, TelaVitoriaLayout.h) — contagem drops + level-up
│   │   └── Derrota/       (TelaDerrota.h/cpp, TelaDerrotaLayout.h)
│   │
│   ├── PerspectivaRaycaster/      # (renomeado de Raycaster/)
│   │   ├── EngineRaycaster/               # (renomeado de Engine3D/)
│   │   │   ├── Raycaster.h/cpp
│   │   │   ├── RaycasterControles.h/cpp   # (renomeado de RaycasterControls)
│   │   │   ├── RaycasterHUD.h/cpp
│   │   │   ├── RaycasterInimigos.h/cpp
│   │   │   ├── RaycasterMundo.h/cpp
│   │   │   ├── RaycasterNPCs.h/cpp
│   │   │   ├── RaycasterRenderizador.h/cpp     # (renomeado de RaycasterRenderer)
│   │   │   ├── RaycasterRenderizadorCombate.h/cpp
│   │   │   └── RaycasterSprites.h/cpp
│   │   │
│   │   └── TelasRaycaster/               # Stubs (renderização 3D real pendente)
│   │       ├── TelaCombateRaycaster.h/cpp    (stub)
│   │       ├── TelaInventarioRaycaster.h/cpp (stub)
│   │       ├── TelaAtributosRaycaster.h/cpp  (stub)
│   │       ├── TelaDiarioRaycaster.h/cpp     (stub)
│   │       ├── TelaBestiarioRaycaster.h/cpp  (stub)
│   │       ├── TelaMapaRaycaster.h/cpp       (stub)
│   │       ├── TelaMenuRaycaster.h/cpp       (stub)
│   │       ├── TelaPauseRaycaster.h/cpp      (stub)
│   │       ├── TelaVitoriaRaycaster.h/cpp    (stub)
│   │       └── TelaDerrotaRaycaster.h/cpp
│   │
│   └── PerspectivaIDE/            # (renomeado de Terminal/)
│       ├── EngineIDE/
│       │   ├── IDERenderer.h
│       │   ├── IDECodeFormatter.h/cpp
│       │   └── IDEAnimator.h/cpp
│       │
│       ├── TelasIDE/              # Apenas renderização (sem lógica de jogo)
│       │   ├── TelaCombateIDE.h/cpp       # ⚠ contém lógica (obterAcao*, obterAlvo*)
│       │   ├── TelaInventarioIDE.h/cpp
│       │   ├── TelaAtributosIDE.h/cpp     # renderiza fórmulas calculadas por TelasBase
│       │   ├── TelaBestiarioIDE.h/cpp
│       │   ├── TelaDiarioIDE.h/cpp        # renderiza usando TelaDiarioLogic (TelasBase)
│       │   ├── TelaMapaIDE.h/cpp          # apenas navegação + renderização
│       │   ├── TelaVitoriaIDE.h/cpp       # apenas renderização (dados pré-calculados)
│       │   ├── TelaDerrotaIDE.h/cpp
│       │   ├── TelaMenuIDE.h/cpp          # renderização apenas (tutorial em TelaTutorial)
│       │   └── TelaPauseIDE.h/cpp         # apenas renderização (lógica em TelasBase)
│       │
│       └── TemaIDE.h         # namespace TemaIDE (renomeado de TemaTerminal)
│
└── Principal.cpp
```

---

## Fases de Implementação

### Fase 1: Renomeação e Estrutura de Pastas (✅ CONCLUÍDA)

#### 1.1 Renomear `src/Visoes/` para `src/Perspectiva/` ✅
#### 1.2 Renomear `src/Perspectiva/Raycaster/` para `src/Perspectiva/PerspectivaRaycaster/` ✅
#### 1.3 Renomear `src/Perspectiva/Terminal/` para `src/Perspectiva/PerspectivaIDE/` ✅
#### 1.4 Renomear `src/Mundo/` para `src/Mapas/` ✅

#### 1.5 Padronização de Nomenclatura ✅

| Item | Status |
|---|---|
| Renomear `RaycasterControls` → `RaycasterControles` | ✅ |
| Renomear `RaycasterRenderer` → `RaycasterRenderizador` | ✅ |
| Deletar `RaycastRenderer.h/cpp` (arquivos vazios) | ✅ |
| Renomear `Tela*Terminal` → `Tela*IDE` | ✅ |
| Renomear `TemaTerminal.h` → `TemaIDE.h` | ✅ |
| Renomear `TelaDerrota3D` → `TelaDerrotaRaycaster` | ✅ |
| Mover `Combate3DRenderer` → `EngineRaycaster/RaycasterRenderizadorCombate` | ✅ |
| Mover `TerminalRenderer.h` → `EngineIDE/IDERenderer.h` | ✅ |

#### 1.6 Criar stubs TelasRaycaster (✅ CONCLUÍDA)
9 stubs criados (Combate, Inventario, Atributos, Diario, Bestiario, Mapa, Menu, Pause, Vitoria).

#### 1.7 Criar EngineIDE (✅ CONCLUÍDA)
`IDERenderer.h`, `IDECodeFormatter.h/cpp`, `IDEAnimator.h/cpp`

---

### Fase 2: Separação TelasBase vs Telas de Perspectiva (✅ CONCLUÍDA)

**Padrão**: TelasBase dispatchers delegam para `Tela*IDE` ou `Tela*Raycaster` via `GerenciadorVisao::isVisao3DAtiva()`.

| Tela | Antes (linhas) | Depois (linhas) | Corpo movido para |
|---|---|---|---|
| TelaDerrota | 294 | 12 | `TelaDerrotaIDE` |
| TelaPause | 228 | 11 | `TelaPauseIDE` |
| TelaAtributos | 306 | 77 | `TelaAtributosIDE` |
| TelaVitoria | 294 | 12 | `TelaVitoriaIDE` |
| TelaInventario | 97 | 87 | `TelaInventarioIDE` |
| TelaBestiario | 220 | 11 | `TelaBestiarioIDE` |
| TelaMapaMundial | 196 | 12 | `TelaMapaIDE` |
| TelaDiario | 563 | 12 | `TelaDiarioIDE` |
| TelaMenu | 814 | 107 | `TelaMenuIDE` |
| TelaCombate | 1858 | 107 | `TelaCombateIDE` |

---

### Fase 3: Separação Lógica vs Aparência (🔄 PARCIAL)

**Padrão**: TelasBase contém lógica de funcionamento (decisões, estado, cálculos). TelasIDE/TelasRaycaster contêm apenas renderização.

#### ✅ Concluído

| Tela | Lógica movida para TelasBase |
|---|---|
| **TelaPause** | Menu loop, alteração de dificuldade, toggle Parry, troca de cor/ícone, sensibilidade mouse, salvar/sair |
| **TelaMapaMundial** | Validação de viagem (verifica descoberta, local atual, mapeia índice → destino) |
| **TelaVitoria** | Contagem de drops, verificação de level-up, consulta de novas descobertas |
| **TelaAtributos** | Fórmulas de dano (físico/mágico), cálculos de debuff/buff (`calcularPoderCombate`, `calcularDebuff`) |
| **Namespace** | `TemaTerminal` → `TemaIDE` (arquivo + todas as referências) |
| **TelaMenu** | Tutorial de Parry movido para `TelaTutorial` (TelasBase/Menu/): `rodarTutorialDigitacao`, `rodarTutorialMovimento`, `exibirTutorialDeParry` com `exibirInfoBox` |
| **TelaDiario** | Categorização movida para `TelaDiarioLogic` (TelasBase/Diario/): `obterProgresso()`, `categorizarItens()`, `categorizarMissoes()` — dados e structs (`CategoriaProgresso`, `ItemProgresso`, `MissaoRegistro`, `registroDeMissoes`, `itensDeProgresso`) |

#### 🔄 Parcial

| Tela | Lógica movida para TelasBase | Ainda delegando para IDE |
|---|---|---|
| **TelaCombate** | `definirTurnoVisivel`, `configurarContexto3D` (setters de estado implementados diretamente em TelasBase) | `obterAcaoDoJogador`, `obterAlvoAtaque`, `obterAlvoItem`, `obterEscolhaDeEscudo`, `selecionarHUDDeAliado`, notificações |

---

### Fase 4: Compilação e Ajustes Finais (✅ CONCLUÍDA)

- CMakeLists.txt com GLOB_RECURSE
- Projeto compila 100% sem erros

---

## Resumo do Cronograma

| Fase | Descrição | Esforço | Status |
|---|---|---|---|---|
| 1 | Renomeação de pastas, arquivos e includes | 2-3h | ✅ |
| 1.5 | Padronização de nomenclatura | 2-3h | ✅ |
| 1.6 | Stubs TelasRaycaster + EngineIDE | 3h | ✅ |
| 2 | Separação TelasBase vs Telas de Perspectiva | 4-6h | ✅ |
| 3 | Separação Lógica vs Aparência | 6-8h | ✅ 6/7 telas (Combate parcial) |
| 4 | Compilação e ajustes finais | 2-3h | ✅ |
| **Total** | | **~22-30h** | **~22h feitos** |

---

## Padronização de Nomenclatura

### Regras
1. **PascalCase** — Primeira letra de cada palavra em maiúscula, sem separadores
2. **Idioma: Português** — manutenção do padrão atual dominante no código
3. **Sem abreviações** — exceto siglas consolidadas (`HUD`, `NPC`, `IDE`, `3D`)
4. **Arquivo `.h` + `.cpp` sempre em par** — exceto headers puros (só `.h`) tipo layouts

### Convenção de Prefixos e Sufixos

| Tipo | Padrão | Exemplos |
|---|---|---|
| TelasBase | `Tela<Nome>.h/cpp` | `TelaCombate`, `TelaAtributos` |
| TelasBase Layout | `Tela<Nome>Layout.h` | `TelaCombateLayout.h` |
| Telas Raycaster | `Tela<Nome>Raycaster.h/cpp` | `TelaCombateRaycaster` |
| Telas IDE | `Tela<Nome>IDE.h/cpp` | `TelaCombateIDE` |
| Engine Raycaster | `Raycaster<Nome>.h/cpp` | `RaycasterRenderizador` |
| Engine IDE | `IDE<Nome>.h/cpp` | `IDERenderer`, `IDECodeFormatter` |
| Controladores | `<Nome>.h/cpp` | `MenuJogo`, `CriadorInimigos` |
| Utilidades | `<Nome>.h/cpp` | `Aparencia`, `ControleDeInput` |
| Entidades | `<Nome>.h/cpp` | `Personagem`, `Goblin` |
| Mapas | `Mapa<N>/<Nome>Layout.h` | `Mapa1Vila` |
| Progresso | `<Nome>.h/cpp` | `Progressao`, `Bestiario` |
| Itens | `<Nome>.h/cpp` | `ItemConsumivel` |

### Nomes em Inglês mantidos por decisão
`Debug`, `Drops`, `Status`, `HackConsole`, `Parry`, `RendererProvider`
