# Ancoragem Atual

## Goal
Aplicar o PlanoReorganizacao.md: renomear diretórios, criar TelasRaycaster stubs, criar EngineIDE, separar lógica TelasBase da renderização das perspectivas, e verificar compilação.

## Constraints & Preferences
- Manter padrão PascalCase + português + siglas (HUD, NPC, IDE, 3D)
- TelasBase contém lógica/estado puro; TelasRaycaster/IDE contêm apenas renderização
- CMakeLists.txt usa GLOB_RECURSE para detectar novos .cpp automaticamente
- `GerenciadorVisao::isVisao3DAtiva()` como gate de delegação entre perspectivas

## Progress

### Completed
- Fase 1 (Diretórios): Renomeados `Visoes/` → `Perspectiva/`, `Terminal/` → `PerspectivaIDE/`, `Engine3D/` → `PerspectivaRaycaster/EngineRaycaster/`, `Telas3D/` → `PerspectivaRaycaster/TelasRaycaster/`, `Mundo/` → `Mapas/`
- Fase 1.5: Criados 9 pares .h/.cpp de TelasRaycaster stub (TelaCombate, Inventario, Atributos, Diario, Bestiario, Mapa, Menu, Pause, Vitoria)
- Fase 1.6: Criados IDECodeFormatter.h/cpp e IDEAnimator.h/cpp em `EngineIDE/`
- Fase 4: Criados TelaMenuIDE.h/cpp e TelaPauseIDE.h/cpp em `PerspectivaIDE/TelasIDE/`
- Corrigido include `TelaDerrota3D.h` → `TelaDerrotaRaycaster.h` em `TelasBase/Derrota/TelaDerrota.cpp`
- Corrigido include `Terminal/TemaTerminal.h` → `PerspectivaIDE/TemaIDE.h` em `Core/Utilidades/AparenciaTerminal.cpp`
- Corrigidos 7 arquivos com `../../TelasBase/` → `../../../Perspectiva/TelasBase/` nos NPCs e Itens
- Removidas todas as referências a `Visoes/`, `Terminal/`, `Engine3D/`, `Telas3D/` do código
- Build compila 100% (apenas warnings pré-existentes)

### In Progress
- (none)

### Blocked
- (none)

## Key Decisions
- Seguir o padrão já usado por `TelaDerrota` (TelasBase delega via `GerenciadorVisao::isVisao3DAtiva()`) para todas as demais telas na Fase 2
- TelasRaycaster stubs criados com corpo vazio e assinaturas baseadas no padrão do plano (métodos estáticos `exibir*`)
- Stubs deliberadamente mínimos para permitir compilação e progresso incremental

## Next Steps
1. Fase 2: Extrair lógica de TelasBase (TelaCombate, TelaInventario, TelaAtributos) para delegar renderização a `Tela*Raycaster` ou `Tela*IDE` via `GerenciadorVisao`
2. Fase 3: Refatorar telas existentes removendo acoplamento lógica+aparência

## Relevant Files
- `docs/PlanoReorganizacao.md`: Plano de 5 fases (Fase 1 concluída, Fase 1.5-4 concluída)
- `src/Perspectiva/PerspectivaRaycaster/TelasRaycaster/Tela*Raycaster.h/cpp`: 9 stubs (18 arquivos)
- `src/Perspectiva/PerspectivaIDE/EngineIDE/IDECodeFormatter.h/cpp`: Formatador de código C++
- `src/Perspectiva/PerspectivaIDE/EngineIDE/IDEAnimator.h/cpp`: Animações IDE
- `src/Perspectiva/PerspectivaIDE/EngineIDE/IDERenderer.h`: Renderizador IDE (conecta Aparencia+ControleDeInput ao IVisaoRenderer)
- `src/Perspectiva/PerspectivaIDE/TelasIDE/TelaMenuIDE.h/cpp`: Menu principal IDE (stub)
- `src/Perspectiva/PerspectivaIDE/TelasIDE/TelaPauseIDE.h/cpp`: Pause IDE (stub)
- `src/Perspectiva/GerenciadorVisao.h/cpp`: Singleton para alternar entre visão 3D e IDE
- `cmake/CMakeLists.txt`: Build config com GLOB_RECURSE + target_include_directories(`src/`)
