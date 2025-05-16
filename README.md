# **EM DESENVOLVIMENTO: Estufa inteligente IOT**
Este projeto implementa um sistema de monitoramento e controle para estufas utilizando o microcontrolador Raspberry Pi Pico W, LEDs endereçáveis WS2812 e display OLED ssd1306.O objetivo é otimizar o uso de recursos naturais, como água e fertilizantes, além de melhorar a eficiência energética e a qualidade da produção.

## **Funcionalidades**
- Monitoramento da umidade do ar.
- Monitoramento da umidade do solo.
- Monitoramento da temperatura.
- Controle de umidade.
- Controle de irrigação.
- Controle da ventilação.
- Controle da fertirrigação.

### **Pré-requisitos para utilização**
Certifique-se de que os seguintes itens estão instalados:

1. **Visual Studio Code**  
   - Ambiente de desenvolvimento integrado (IDE) para edição, compilação e depuração do código.

2. **Git**  
   - Ferramenta para versionamento de código e controle de repositórios.  

3. **Extensões do Visual Studio Code**  
   - **C/C++**: Suporte à linguagem C/C++ para análise de código, depuração e IntelliSense.  
   - **CMake Tools**: Para gerenciar e configurar projetos que utilizam o CMake.  
   - **Raspberry Pi Pico**: Extensão para facilitar o desenvolvimento e a comunicação com a placa Raspberry Pi Pico.  

4. **Zadig**  
   - Software necessário para instalar drivers USB.  
   - Permite a conexão via USB da placa Raspberry Pi Pico com o desktop ou notebook.  

5. **Versão 2.0.0+ do Raspberry Pi Pico SDK**  
   - Certifique-se de que a versão 2.0.0 ou superior do SDK para Raspberry Pi Pico está instalada.  
---
## 🚀 Como Executar

### **1. Configuração Inicial**
1. Certifique-se de que todas as dependências acima estão instaladas no seu computador.
2. Clone o repositório do projeto usando os comandos abaixo no terminal:  
   ```bash
   git init
   ```
   ```bash
   git clone https://github.com/Arthuros0/U7-Projeto-Final.git
   ```
3. Abra o projeto clonado no VS Code

### **2. Compilação e Carregamento na Placa**
1. Compile o projeto clicando na opção "**Compile Project**" da extensão Raspberry Pi Pico no **Visual Studio Code**.
2. Após a compilação bem-sucedida, conecte a Raspberry Pi Pico W ao computador utilizando um cabo USB.
3. Coloque a placa em modo **BootSel**:
   - Pressione e segure o botão **BOOTSEL** enquanto conecta a placa pelo cabo USB.
   - Caso esteja utilizando a BitDogLab, aperte o botão **RESET** enquanto pressiona e segura o botão **BOOTSEL** (este método não exige a reconexão do cabo USB) 
4. Localize o arquivo gerado com extensão .uf2 (como **Estufa_webserver.uf2**) e arraste-o para o dispositivo que aparecerá no computador. Ou selecione a opção "**Run Project(USB)**" da extensão Raspberry Pi Pico no **Visual Studio Code**.
