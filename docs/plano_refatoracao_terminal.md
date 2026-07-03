# Plano de Refatoração: `ConfiguracaoTerminal`

## Motivação

Centralizar e organizar todo código de controle de terminal (input/output e aparência)
em uma árvore coesa, eliminando o arquivo gigante `Aparencia.cpp` (~1300 linhas) e
removendo dependências dispersas em `Core/Utilidades/`.

---

## Inventário atual — `Core/Utilidades/`

```
Core/Utilidades/
├── Aparencia.h              # 144 linhas (interface da classe Aparencia)
├── Aparencia.cpp            # ~1300 linhas (implementação) — ALVO PRINCIPAL
├── AparenciaTerminal.h/cpp  # Classe separada (~50/220 linhas) — estilização IDE
├── ControleDeInput.h/cpp    # ~44/365 linhas — input do terminal
├── ConversorString.h        # ~28 linhas — utilitário inline (só usado por Aparencia.cpp)
├── Cor.h                    # (futuro) — enum Cor extraído
└── demais utilitários       # FuncoesDialogo, GeradorAleatorio, RendererProvider, etc.
```

---

## Análise: o que NÃO pertence ao `Aparencia`

### ❌ Já movido para `TelaBase`

| Função | Motivo |
|--------|--------|
| `gerarBarraGradiente` | Gera barras de vida/XP — usado em telas de atributos, combate, vitória |
| `imprimirLinhaDivisoria` | Imprime divisor visual (`═══`) entre seções — usado em múltiplas telas IDE |

Ambos são utilitários de **renderização de tela** (não de aparência de terminal em si).
`TelaBase` já é o ponto central de lógica de telas.

### 🔴 Candidatos a mover para `TelaBase`

| Função | Chamadas | Motivo |
|--------|----------|--------|
| `criarCaixa` | 22 | Cria caixas bordadas com lógica IDE/3D (mesmo caso que `imprimirLinhaDivisoria`) |
| `exibirPopup` | várias | Lógica de popup com input — é uma tela/tela-base |

### 🔴 Candidatos a mover para `TelaCombate`

| Função | Chamadas | Motivo |
|--------|----------|--------|
| `margemCombate` | **89** | Retorna padding centralizado para a HUD de combate (91 chars de largura) |

Usado em: `Combate.cpp`, `Parry.cpp`, `InventarioCombate.cpp`, `Personagem.cpp`,
`ItemConsumivel.cpp`, classes, raças, inimigos, `TelaCombateIDE.cpp`.

### 🔴 Candidatos a utilitários independentes

| Função | Chamadas | Motivo |
|--------|----------|--------|
| `ordenarAlfabeticamente` | **25+** | Algoritmo de ordenação genérico, sem relação com terminal |
| `carregarArte` | 1 (interna) | Carrega arquivos de arte ASCII do disco — asset loading |
| `reduzirEscalaAscii` | 5 | Redimensiona arte ASCII — asset processing |
| `FATOR_COMPRESSAO_GLOBAL` | 3+ | Constante de compressão de sprites, não é aparência |

---

## Estrutura final proposta

```
src/Sistemas/ConfiguracaoTerminal/
├── ControleInputOutput/
│   ├── ControleDeInput.h
│   └── ControleDeInput.cpp
│
├── Aparencia/
│   ├── Cor.h                      # Enum Cor (extraído, ~45 linhas)
│   ├── Aparencia.h                # Header unificado (~20 linhas, inclui sub-headers)
│   ├── Console.cpp                # inicializarConsole, cursor, tela, fonte
│   ├── Cores.cpp                  # cor(), corRGB(), bgRGB(), obterCorRGBFade, fadarLinhaAnsi
│   ├── Exibicao.cpp               # imprimirCentralizado, imprimirLadoALado, exibirPainel*
│   ├── Popup.cpp                  # exibirPopup, renderizarCaixaPopupAnimada, lerInteiroEmPopupFlutuante
│   ├── Animacao.cpp               # animarFadeIn, exibirTelaIntro, digitação, transição 3D
│   ├── Overlay.cpp                # sobreporPainelNaLinhaAnsi, sobreporLogoAnsi
│   ├── Log.cpp                    # registrarLogBatalha, exibirUltimosLogs, exibirHistoricoCompleto
│   ├── Texto.cpp                  # removerCoresANSI, obterComprimentoVisual, espacosParaCentralizar
│   ├── AparenciaTerminal.h        # (movido)
│   └── AparenciaTerminal.cpp      # (movido) — estilização código-fonte IDE
│
└── Utilitarios/                   # (opcional) — o que não é terminal nem input
    ├── Ordenador.h/cpp            # ordenarAlfabeticamente + template
    ├── AssetLoader.h/cpp          # carregarArte, reduzirEscalaAscii, FATOR_COMPRESSAO_GLOBAL
```

### Após os movimentos, `Aparencia` conterá apenas:

| Grupo | Arquivo | Métodos | ~Linhas |
|-------|---------|---------|---------|
| Console | `Console.cpp` | `inicializarConsole`, `maximizarJanelaTerminal`, `ajustarFonteParaResolucao`, `ocultarCursor`, `mostrarCursor`, `definirCorFundoTerminal`, `limparTela`, `obterLarguraTerminal`, `obterAlturaTerminal`, `moverCursor`, `obterPosicaoCursorY` | 120 |
| Cores | `Cores.cpp` | `cor(Cor)`, `cor(Cor,Cor)`, `corRGB`, `bgRGB`, `obterCorRGBFade`, `fadarLinhaAnsi` | 120 |
| Exibição | `Exibicao.cpp` | `imprimirCentralizado`, `imprimirCentralizadoMultilinha`, `imprimirBlocoCentralizado`, `exibirPainel`, `exibirPainelTexto`, `exibirTituloPadrao`, `imprimirLadoALado`, `exibirPrompt`, `exibirPainelArte` | 200 |
| Popup | `Popup.cpp` | `iniciarInteracaoPopup`, `atualizarMinTamanhoPopup`, `obterMinLarguraPopup`, `obterMinAlturaPopup`, `exibirPopup`, `renderizarCaixaPopupAnimada`, `lerInteiroEmPopupFlutuante` | 180 |
| Animação | `Animacao.cpp` | `animarFadeIn`, `exibirTelaIntro`, `imprimirBlocoCentralizadoDigitando`, `imprimirCentralizadoDigitando`, `imprimirDigitando`, `imprimirVetorAnimado`, `animarTransicaoCena3D` | 200 |
| Overlay | `Overlay.cpp` | `sobreporPainelNaLinhaAnsi`, `sobreporLogoAnsi` | 120 |
| Log | `Log.cpp` | `registrarLogBatalha`, `limparLogBatalha`, `exibirUltimosLogs`, `exibirHistoricoCompleto` | 60 |
| Texto | `Texto.cpp` | `removerCoresANSI`, `obterComprimentoVisual`, `espacosParaCentralizar`, `centralizarTexto`, `padronizarTamanhoVetor` | 100 |

Total: ~1100 linhas espalhadas em 8 arquivos focados (vs 1 arquivo de 1300).

---

## Etapas de implementação

### Etapa 1 — Criar diretórios

```
src/Sistemas/ConfiguracaoTerminal/
  ControleInputOutput/
  Aparencia/
  Utilitarios/       (opcional)
```

### Etapa 2 — Extrair `Cor.h`

- Mover o enum `Cor` de `Aparencia.h` para `Aparencia/Cor.h`
- `Aparencia.h` inclui `Cor.h` em vez de definir inline

### Etapa 3 — Mover `ControleDeInput` → `ControleInputOutput/`

- Mover `ControleDeInput.h/cpp`
- Atualizar **~44 arquivos** que incluem o header
- Include: `Sistemas/ConfiguracaoTerminal/ControleInputOutput/ControleDeInput.h`

### Etapa 4 — Mover `AparenciaTerminal` → `Aparencia/`

- Mover `AparenciaTerminal.h/cpp`
- Atualizar includes nos arquivos que referenciam

### Etapa 5 — Mover `ConversorString` → `Aparencia/`

- Usado apenas por `Aparencia.cpp`

### Etapa 6 — Remover funções deslocadas

#### 6a. `criarCaixa` → `TelaBase`
- Mover declaração para `TelaBase.h`
- Mover implementação para `TelaBase.cpp`
- Atualizar **22 chamadas** de `Aparencia::criarCaixa` → `TelaBase::criarCaixa`

#### 6b. `margemCombate` → `TelaCombate`
- Mover declaração para `TelaCombate.h`
- Mover implementação para `TelaCombate.cpp`
- Atualizar **89 chamadas** de `Aparencia::margemCombate` → `TelaCombate::margemCombate`

### Etapa 7 — Fatiar `Aparencia.cpp` em 8 arquivos

Cada arquivo compila para a mesma classe `Aparencia`. Processo:
1. Criar cada `.cpp` no diretório `Aparencia/` com os includes necessários
2. Copiar os métodos correspondentes
3. Remover os métodos do `Aparencia.cpp` original
4. Manter `Aparencia.cpp` apenas com globals + includes dos sub-headers

### Etapa 8 — Rebuild

```bash
cmake -S . -B build && cmake --build build
```

---

## Riscos e mitigação

| Risco | Mitigação |
|-------|-----------|
| Inclusão cíclica entre headers | `Cor.h` totalmente independente; `Aparencia.h` inclui sub-headers sem dependência reversa |
| Paths de includes quebrados | Usar `src/` como base no `target_include_directories` + includes absolutos desde `src/` |
| Build lento (muitos .cpp) | Neutro ou positivo para rebuild incremental |
| Código duplicado no split | Cada método aparece em exatamente um .cpp |
| Esquecer de atualizar um caller | Grep de cada função movida + rebuild completo para detectar linking errors |
