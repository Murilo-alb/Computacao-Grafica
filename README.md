# 🖥️ Motor Gráfico 3D Interativo (Qt/C++)

Este repositório contém a implementação de um **Motor Gráfico 3D** desenvolvido em C++ utilizando o framework Qt. O projeto permite a leitura, renderização e manipulação em tempo real de objetos tridimensionais, curvas e polígonos, suportando múltiplas projeções e recursos avançados de visualização geométrica.

O projeto foi construído de forma iterativa, culminando na **Entrega 7**, que introduz técnicas complexas de rasterização, iluminação e otimização de profundidade.

---

## 👥 Desenvolvedores

| Nome 
| :--- |
| **Murilo Albuquerque**
| **Julia Yukari**
| **Marcus Vinicius**

---

## ✨ Destaques da Última Versão (Entrega 7)

Esta versão coroa o projeto com as seguintes implementações avançadas:

* **Renderização Híbrida:** Capacidade de mesclar diferentes técnicas de renderização na mesma cena.
* **Algoritmo do Pintor (Painter's Algorithm):** Ordenação dinâmica de profundidade para garantir que objetos mais distantes sejam desenhados antes dos mais próximos, corrigindo a sobreposição visual.
* **Clipping Volumétrico:** Recorte preciso de objetos baseados nos limites do volume de visão (Frustum/Box).
* **Iluminação e Sombras:** Sistema de luz integrado com projeção de sombras projetadas diretamente sobre o chão da cena.
* **Câmera Interativa:** Controle fluido da câmera 3D diretamente através do mouse.
* **Suporte a Curvas:** Criação e manipulação de Curvas (ex: Bezier) renderizadas no espaço 3D.

---

## 🛠️ Funcionalidades Base

Além dos recursos avançados, o motor gráfico suporta as operações fundamentais de modelagem e visualização:

### Manipulação de Malhas e Objetos
* **Importação de Arquivos `.obj`:** Carregamento de malhas 3D (Wavefront OBJ) com leitura de vértices (`v`) e faces (`f`).
* **Criação Dinâmica:** Geração manual de novos Polígonos e Curvas através da interface.
* **Transformações Geométricas Locais e Globais:**
    * Translação (dx, dy, dz).
    * Escala (sx, sy, sz).
    * Rotação nos três eixos (X, Y, Z).

### Projeções e Visualização
* **Projeção Perspectiva:** Com controle de Zoom e plano de corte frontal (`zNear`).
* **Projeção Ortogonal:** Renderização planificada sem distorção de profundidade.
* **Gerenciamento de Cores:** Atribuição automática de cores iterativas (Cyan, Magenta, Green, Red, etc.) para diferenciar objetos instanciados.

> **⚠️ Nota de Comportamento Visual:** A renderização estrutural (vértices e arestas) de polígonos isolados ou retas apresentará diferenças visuais nativas ao alternar entre o modo Ortogonal e Perspectivo devido à matemática de projeção aplicada.

---

## 🚀 Como Executar o Projeto

### Pré-requisitos
* **Qt Creator** (Versão 5.x ou superior recomendada).
* Compilador C++ compatível (MinGW, MSVC, ou GCC).
