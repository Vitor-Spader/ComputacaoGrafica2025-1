# Projeto OpenGL – Renderização e Animação 3D

Este projeto implementa um sistema básico de renderização 3D usando OpenGL, GLFW, GLAD e GLM, com suporte a múltiplos objetos, câmera, iluminação, curvas para movimentação e shaders personalizados. A configuração inicial dos objetos, câmera e luzes é carregada via arquivo JSON.

---

## ✨ Funcionalidades principais

- ✅ Renderização de modelos 3D no formato OBJ, com suporte a texturas.
- ✅ Controle de câmera livre via teclado e mouse.
- ✅ Sistema de luz com componentes ambiente, difusa e especular.
- ✅ Animação de um personagem seguindo uma curva paramétrica suavemente.
- ✅ Gerenciamento de múltiplos objetos com controle individual.
- ✅ Uso de shaders vertex e fragment hardcoded para efeitos básicos de iluminação.
- ✅ Configuração via arquivo JSON externo para facilitar ajustes sem recompilar.

---

## 📦 Dependências

- C++17 ou superior
- [GLFW](https://www.glfw.org/)
- [GLAD](https://glad.dav1d.de/)
- [GLM](https://glm.g-truc.net/)
- [nlohmann/json](https://github.com/nlohmann/json) – para leitura de arquivos JSON
- CMake 3.10 ou superior
- Compilador compatível (GCC, Clang, MSVC, etc.)

---

## 📁 Estrutura do projeto

```
.
├── src/              # Código-fonte C++
├── include/          # Arquivos de cabeçalho
├── assets/           # Modelos 3D e texturas
├── config/           # Arquivo JSON de configuração
└── CMakeLists.txt    # Build do projeto
```

---

## 🛠️ Como compilar (usando CMake)

```bash
# 1. Clone o repositório
git clone <URL_DO_REPOSITORIO>
cd <PASTA_DO_PROJETO>

# 2. Crie uma pasta build e acesse
mkdir build && cd build

# 3. Gere os arquivos de build
cmake ..

# 4. Compile
cmake --build .
```

Isso irá gerar o executável na pasta `build`.

---

## 🚀 Como executar

1. Certifique-se de que o arquivo `config/config.json` existe e está corretamente configurado.
2. Execute o programa:
   ```bash
   ./TarefaModulo6
   ```

---

## 🎮 Controles

| Comando         | Ação                             |
|----------------|----------------------------------|
| `W`, `A`, `S`, `D` | Movimentam a câmera             |
| `0`, `1`        | Alternam entre objetos           |
| `X`, `Y`, `Z`   | Rotacionam o objeto selecionado  |
| `[`, `]`        | Escalam o objeto selecionado     |
| `I`, `J`, `A`, `D`, `W`, `S` | Movem o objeto no espaço       |
| `O`             | Alterna entre controle de câmera e de objeto |
| **Mouse**       | Controla a direção da câmera     |
| **Scroll**      | Controla o zoom da câmera        |

---

## 📄 Exemplo de `config/config.json`

```json
{
  "light": {
    "ka": 0.2,
    "kd": 0.5,
    "ks": 0.5,
    "q": 20.0,
    "position": [
      [0.9, 0.8, 0.9],
      [-0.9, -0.8, 0.0],
      [1.0, -0.5, -5.0]
    ]
  },
  "camera": {
    "position": [0.0, 0.0, 3.0],
    "front": [0.0, 0.0, -1.0],
    "up": [0.0, 1.0, 0.0],
    "moveSpeed": 0.2
  },
  "object": [
    {
      "objFilePath": "../assets/Modelos3D/Suzanne.obj",
      "hasTexture": true,
      "initial": {
        "position": [0.0, 0.0, 0.0],
        "moveFactor": 0.1,
        "scale": -5.0
      }
    },
    {
      "objFilePath": "../assets/Modelos3D/Suzanne.obj",
      "hasTexture": true,
      "initial": {
        "position": [0.5, 0.0, 0.0],
        "moveFactor": 0.1,
        "scale": -9.0
      }
    }
  ]
}
```

---

## 🧠 Notas adicionais

- Certifique-se de que os caminhos das texturas e arquivos OBJ estejam corretos.
- A janela tem resolução padrão de 1000x1000 pixels.
- O projeto utiliza shaders GLSL simples e pode ser expandido com efeitos visuais mais complexos.
- Logs e mensagens de erro são exibidos no terminal.

---

## 📬 Contato

Para dúvidas ou sugestões, abra uma *issue* ou entre em contato com o desenvolvedor.

---

> Projeto acadêmico com fins educacionais e aprendizado de OpenGL.
