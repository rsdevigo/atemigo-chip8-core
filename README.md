# Atemigo CHIP-8

Core [libretro](https://www.libretro.com/) de CHIP-8 escrito em C++20, feito como exercício de estudo de emulação. Roda em qualquer frontend libretro (RetroArch, por exemplo) como biblioteca dinâmica.

## Recursos

- As 34 instruções do CHIP-8 original, incluindo `DXYN` com detecção de colisão.
- Tela de 64×32 a 60 quadros por segundo, com cor do pixel configurável.
- Som por onda quadrada enquanto o timer de som estiver ativo, com frequência configurável.
- Save states com cabeçalho `ATEMIGO` e versão de formato (4147 bytes).
- *Quirks* configuráveis para compatibilidade com ROMs de diferentes épocas.
- Opcode inválido para a máquina (halt) e o frontend recebe um aviso; o reset volta ao normal.

## Estrutura

```
src/
├── core/       Máquina CHIP-8 (CPU, memória, registradores, input, save state).
│               Não depende de libretro.
├── libretro/   Camada fina: LibretroCore e as funções retro_* exportadas.
│               libretro.h é o cabeçalho oficial da API.
└── utils/      Funções auxiliares (leitura/escrita de bytes, geração de seed).
```

O core é *header-only*: a única unidade de compilação é `src/libretro/libretro.cpp`.

## Compilação

Requer um compilador com suporte a C++20.

**Linux**

```sh
g++ -std=c++20 -O2 -fPIC -shared src/libretro/libretro.cpp -o atemigo_chip8_libretro.so
```

**macOS**

```sh
clang++ -std=c++20 -O2 -dynamiclib src/libretro/libretro.cpp -o atemigo_chip8_libretro.dylib
```

**Windows (MSVC, no Developer Command Prompt)**

```bat
cl /std:c++20 /O2 /EHsc /MT /LD src\libretro\libretro.cpp /Fe:atemigo_chip8_libretro.dll
```

**Windows (clang do Visual Studio)**

```sh
clang++ -std=c++20 -O2 -shared src/libretro/libretro.cpp -o atemigo_chip8_libretro.dll
```

## Uso no RetroArch

1. Copie a biblioteca para a pasta `cores` do RetroArch.
2. *Carregar Núcleo* → *Atemigo CHIP-8*.
3. *Carregar Conteúdo* → escolha uma ROM (`.ch8`, `.c8`, `.chip8` ou `.bin`).

### Controles

O teclado hexadecimal do CHIP-8 é mapeado nos botões do RetroPad (porta 1):

| Tecla | RetroPad | Tecla | RetroPad |
|:-----:|----------|:-----:|----------|
| 0 | B        | 8 | L      |
| 1 | Cima     | 9 | R      |
| 2 | Baixo    | A | L2     |
| 3 | Esquerda | B | R2     |
| 4 | Direita  | C | Select |
| 5 | A        | D | Start  |
| 6 | X        | E | L3     |
| 7 | Y        | F | R3     |

### Opções do core

| Opção | Valores (padrão primeiro) | Descrição |
|-------|---------------------------|-----------|
| `atemigo_chip8_pixel_color` | white, green, amber | Cor dos pixels acesos. |
| `atemigo_chip8_sound_freq` | 440, 220, 880 | Frequência do bipe em Hz. |
| `atemigo_chip8_instructions` | 12, 8, 10, 15, 20, 30 | Instruções executadas por quadro. |
| `atemigo_chip8_eightx_behavior` | enabled, disabled | `8XY6`/`8XYE` deslocam VX direto (desligado: copia VY antes). |
| `atemigo_chip8_bnnn_behavior` | disabled, enabled | `BNNN` usa VX em vez de V0 (estilo SUPER-CHIP). |
| `atemigo_chip8_fx1e_behavior` | enabled, disabled | `FX1E` liga VF quando I passa de 0xFFF. |
| `atemigo_chip8_fxs_behavior` | enabled, disabled | `FX55`/`FX65` mantêm I inalterado (desligado: incrementa I). |

As opções são da sessão e não ficam gravadas nos save states.

## Releases

A workflow [.github/workflows/release.yml](.github/workflows/release.yml) roda a cada tag enviada ao GitHub. Ela compila o core para Linux (x86_64), Windows (x86_64) e macOS (arm64), empacota cada biblioteca com este README em um `.zip` e publica tudo numa release com o nome da tag.

```sh
git tag v0.1.0
git push origin v0.1.0
```

A versão exibida pelo frontend vem de `library_version` em `src/libretro/libretro.cpp`. Atualize-a antes de criar a tag.
