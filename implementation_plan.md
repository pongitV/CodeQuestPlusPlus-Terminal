# Otimização de Performance do Sistema de Renderização Raycaster

Após análise completa dos ~2500 linhas do motor raycaster, identifiquei **9 gargalos críticos** que, combinados, explicam o FPS baixo mesmo em hardware potente. As otimizações são ordenadas por impacto estimado.

---

## Gargalos Identificados e Correções Propostas

---

### 🔴 1. Iluminação recalculada POR PIXEL do chão (IMPACTO EXTREMO)

**Arquivo**: [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp#L331-L342)

**Problema**: Para cada pixel do chão, `getFloorPixel()` chama `Highlighter::applyFog()` que internamente chama `calculateInfoLight()`. Essa função itera sobre **todas as luzes visíveis**, faz cálculos de distância, e para cada luz próxima executa `checkOcclusion()` (ray march via Bresenham). Em uma tela 120×80 com ~40 linhas de chão × 120 colunas = **~4800 pixels de chão**, cada um fazendo ray marching contra cada luz.

**Correção**: Pré-calcular `InfoLight` por coluna (ou a cada N pixels) e interpolar linearmente entre amostras. Para paredes já existe `infoLightWall` pré-calculado — o chão precisa do mesmo tratamento.

#### [MODIFY] [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp)
- No loop do chão (linhas 331-342), calcular `InfoLight` apenas a cada 4 linhas verticais e interpolar nos pixels intermediários usando `applyLightPrecalculated()` ao invés de `applyFog()`.
- O mesmo para o teto indoor (linhas 288-297) onde `drawIndoorCeiling` chama `calculateInfoLight()` por pixel.

---

### 🔴 2. `std::vector<std::tuple<int,int,int>>` alocado e copiado por coluna (IMPACTO ALTO)

**Arquivo**: [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp#L256-L265)

**Problema**: Para cada coluna da tela, `wallLights` é criado como um novo `std::vector`, iterando todas as luzes e fazendo `push_back()`. Com 120 colunas × 60 FPS = **7200 alocações heap/segundo**. A função `getFloorPixel` com parâmetro `lights` também recebe o vetor completo e o refiltra internamente.

**Correção**: 
- Usar um buffer fixo `static thread_local` para `wallLights`, fazendo `.clear()` ao invés de alocar.
- Melhor ainda: pré-filtrar as luzes uma vez por quadro e passar apenas o array global, eliminando a cópia por coluna.

#### [MODIFY] [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp)
- Substituir `std::vector<std::tuple<int, int, int>> wallLights;` (L256) por `static thread_local std::vector<...> wallLights; wallLights.clear();`

---

### 🔴 3. `getFloorPixel` com `lights` recalcula iluminação completa (IMPACTO ALTO)

**Arquivo**: [RaycasterWorld.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterWorld.cpp#L267-L382)

**Problema**: A versão de `getFloorPixel` que recebe `lights` (L267) chama `Highlighter::applyFog()`, que recria `InfoLight` do zero. A versão otimizada com `InfoLight` pré-calculado (L383) **já existe mas não é usada** no loop principal de renderização.

**Correção**: No loop de chão do `RaycasterRendererBase.cpp`, amostrar `InfoLight` a cada N pixels e usar a sobrecarga `getFloorPixel(titleMap, x, y, dist, depth, infoLight)` (L383) que é muito mais barata.

---

### 🟠 4. `std::string` por pixel no buffer de saída (IMPACTO ALTO)

**Arquivo**: [Raycaster.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/Raycaster.cpp#L419-L421)

**Problema**: O vetor `screen` é `vector<string>` com `SCREEN_WIDTH * SCREEN_HEIGHT` strings. A cada frame, todas são resetadas com `screen[i] = " "` (L419-421) — isso causa **milhares de allocações/deallocações de string por frame**. Pior: no loop de composição (L432-492), cada pixel faz `string::append()` e comparações de string (`hudStr != " "`).

**Correção**:
- Substituir o sistema de HUD por um `vector<bool>` ou bitmask para marcar quais posições têm overlay HUD.
- Eliminar o reset por iteração e usar o frame 3D diretamente, sobrepondo o HUD apenas onde necessário.

#### [MODIFY] [Raycaster.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/Raycaster.cpp)
- Substituir `screen[i] = " "` por uma flag `hasHUD[i] = false`
- No loop de composição, checar `hasHUD[i]` (bool) ao invés de `hudStr != " "` (comparação de string)

---

### 🟠 5. `cosf`/`sinf` chamados por coluna sem cache (IMPACTO MÉDIO)

**Arquivo**: [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp#L154-L155)

**Problema**: Por coluna: `cosf(radiusAngle)`, `sinf(radiusAngle)`, `cosf(radiusAngle - angleVisa)` (L228). Além disso, nas linhas 382-385 do render de sprites: `cosf(angleVisa)`, `sinf(angleVisa)`, `tanf(fieldVisa/2)` são recalculados a cada frame mas são constantes.

**Correção**: Pré-calcular `cosf(angleVisa)`, `sinf(angleVisa)`, `tanf(fieldVisa/2)` fora do loop. Para os raios, pré-calcular a tabela `{eyeX[x], eyeY[x], cosCorrection[x]}` antes do loop de colunas.

#### [MODIFY] [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp)
- Mover cálculos trigonométricos constantes para fora do loop.
- Criar arrays pré-calculados `rayDirX[SCREEN_WIDTH]`, `rayDirY[SCREEN_WIDTH]`, `fisheyeCorrection[SCREEN_WIDTH]`.

---

### 🟠 6. `checkOcclusion` com Bresenham por pixel (IMPACTO MÉDIO)

**Arquivo**: [Illuminator.h](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/Illuminator.h#L20-L43)

**Problema**: `checkOcclusion()` executa Bresenham line-tracing para cada luz × cada pixel iluminado. Como é chamada tanto para point lights (L108-109) quanto para a sombra direcional do sol (L131), o custo é multiplicativo.

**Correção**: 
- Limitar `checkOcclusion` a luzes dentro de raio 2.0 (já existe `distLightSq < 4.0f`), mas a sombra direcional (L131) sempre executa — cachear resultado por tile (mapX, mapY) ao invés de por pixel.
- Usar um `shadow map` 2D pré-computado por frame: `bool shadowGrid[mapH][mapW]`.

---

### 🟡 7. `string::find` repetido para detecção de tema (IMPACTO BAIXO-MÉDIO)

**Arquivo**: [RaycasterWorld.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterWorld.cpp#L275-L390)

**Problema**: Em `getFloorPixel()` (L275, L354, L390, L487, L490), `flags.upperTitle.find("CORACAO")` e `flags.upperTitle.find("FLORESTA")` são chamados **por pixel**. Embora `find` em strings curtas (~20 chars) seja rápido, multiplicado por 4800 pixels de chão × 60 FPS = ~576.000 chamadas/segundo.

**Correção**: Os flags `isHeart`, `isForest` já existem no `MapFlags` mas **não são usados em alguns lugares**. Substituir todos os `find()` por flags booleanos pré-cacheados.

#### [MODIFY] [RaycasterWorld.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterWorld.cpp)
- L275: Substituir `flags.upperTitle.find("CORACAO") != std::string::npos` por `flags.isHeart`
- L390: Idem
- L354, L487, L490: Substituir `flags.upperTitle.find("FLORESTA")` por `flags.isForest`

---

### 🟡 8. `std::chrono::system_clock::now()` chamado POR COLUNA (IMPACTO BAIXO-MÉDIO)

**Arquivo**: [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp#L272-L274)

**Problema**: Dentro do loop por coluna (que roda dentro de threads), `std::chrono::system_clock::now()` é chamado para cada coluna em tema dinâmico (L272). System clock é um syscall relativamente caro.

**Correção**: Pré-calcular o timestamp e o `angleSlow` uma vez antes do loop de colunas, passar como parâmetro.

#### [MODIFY] [RaycasterRendererBase.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterRendererBase.cpp)
- Mover o cálculo `globalMs` e `angleSlow` para antes do `for (int i = 0; i < indexInChunks; i++)`.

---

### 🟡 9. `getPixelWater` cria vector vazio e `calculateInfoLight` por pixel (IMPACTO BAIXO)

**Arquivo**: [RaycasterWorld.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterWorld.cpp#L561-L562)

**Problema**: `getPixelWater()` cria `std::vector<std::tuple<int,int,int>> noLights;` vazio (L561) a cada chamada e passa para `calculateInfoLight`. Alocação heap desnecessária em hot path.

**Correção**: Usar um `static const` vetor vazio ou alterar para aceitar ponteiro nulo.

#### [MODIFY] [RaycasterWorld.cpp](file:///d:/git%20repos%20fixed/CodeQuestPlusPlus-Terminal/src/UI/Renderers/3D/EngineRaycaster/RaycasterWorld.cpp)
- Substituir `std::vector<...> noLights;` por `static const std::vector<...> noLights;`

---

## Resumo de Impacto Estimado

| # | Otimização | Impacto FPS | Complexidade |
|---|-----------|-------------|-------------|
| 1 | Interpolar iluminação do chão | **+40-60%** | Média |
| 2 | Eliminar alocações de `wallLights` | **+10-15%** | Baixa |
| 3 | Usar `getFloorPixel` com `InfoLight` pré-calculado | **+20-30%** | Média |
| 4 | Eliminar `string` por pixel no HUD | **+15-25%** | Média |
| 5 | Pré-calcular trigonometria | **+5-10%** | Baixa |
| 6 | Shadow map 2D para sombras | **+10-20%** | Alta |
| 7 | Usar flags booleanos ao invés de `find()` | **+3-5%** | Muito baixa |
| 8 | Cachear `system_clock::now()` | **+3-5%** | Muito baixa |
| 9 | Vetor vazio estático na água | **+1-2%** | Muito baixa |

> [!IMPORTANT]
> Os itens 1, 2, 3 e 4 sozinhos devem duplicar ou triplicar o FPS. Os itens 7, 8 e 9 são mudanças triviais de uma linha cada e podem ser aplicados imediatamente.

---

## Open Questions

> [!IMPORTANT]  
> **Qualidade visual vs. performance**: Os itens 1 e 3 (interpolação de iluminação) podem causar artefatos visuais sutis em transições luz/sombra no chão. Devo priorizar performance máxima (interpolar a cada 8 pixels) ou qualidade visual (interpolar a cada 2-4 pixels)?

> [!NOTE]
> **Shadow map (item 6)**: Esta é a mudança mais invasiva — requer um buffer 2D por frame e muda a interface do `Highlighter`. Deseja incluí-la nesta fase ou deixar para depois?

---

## Verification Plan

### Automated Tests
- Compilar o projeto e verificar que não há erros de compilação
- Executar o jogo e verificar FPS antes/após com o debug FPS counter

### Manual Verification
- Testar todos os mapas (Vila, Floresta, Reino, Caverna, Labirinto, Boss) para verificar que a iluminação ainda funciona corretamente
- Verificar que não há artefatos visuais no chão, paredes ou água
- Comparar screenshots antes/depois para garantir fidelidade visual
