# 🛡️ Sistema de Detecção de Intrusão (IDS) com Machine Learning

Um Sistema de Detecção de Intrusão (IDS) híbrido e de alta performance, construído com uma arquitetura Cliente-Servidor que une a velocidade da linguagem **C** (para gerenciamento de rede e concorrência) com o poder analítico preditivo do **Python** (Machine Learning).

Este projeto foi desenvolvido para identificar e bloquear anomalias de rede e ataques cibernéticos (incluindo ameaças *Zero-Day*) em tempo real, utilizando o modelo estatístico **Random Forest** treinado sobre o dataset acadêmico **NSL-KDD**.

## 🏗️ Arquitetura do Sistema

O fluxo do sistema é dividido em três módulos independentes que se comunicam com latência virtualmente nula através de *Inter-Process Communication* (IPC):

* **1. Cliente Injetor (`client.c`):** Atua na ponta da rede externa. É responsável por ler os logs de tráfego e disparar as requisições via TCP/IPv4.
* **2. Servidor Principal TCP (`server.c`):** O coração da rede. Opera na porta `8080` de forma multithreaded (uma thread independente para cada cliente), com isolamento estrito de memória. Ele recebe o tráfego externo e o repassa imediatamente via Sockets de Domínio Unix (`/tmp/ai_socket`).
* **3. Servidor Analítico de IA (`ai_server.py`):** Roda em *background* escutando o Socket Unix local. Recebe o dado bruto, aplica o *One-Hot Encoding*, força o alinhamento de colunas usando um molde pré-treinado (`colunas_treino.pkl`) e submete o pacote ao cérebro da IA. O veredito (Tráfego Normal ou Alerta de Anomalia) é devolvido para a camada C e repassado ao cliente.

## ✨ Principais Funcionalidades

* **Detecção Baseada em Anomalia:** Supera firewalls tradicionais ao detectar padrões maliciosos em vez de depender de um banco de assinaturas estático.
* **Alta Concorrência:** Implementação segura de multithreading na API POSIX em C, evitando condições de corrida (*race conditions*).
* **Comunicação IPC Ultra-rápida:** O uso de Sockets Unix elimina o *overhead* da pilha TCP/IP (como *checksums* e roteamento lógico) na troca de mensagens internas entre o C e o Python.
* **Tratamento Dinâmico de Dimensionalidade:** Sistema anti-crash que preenche características estruturais faltantes em requisições isoladas.

## 🛠️ Tecnologias Utilizadas

* **Linguagem C:** Sockets API (TCP/IP e AF_UNIX), Pthreads, alocação dinâmica de memória.
* **Linguagem Python:** `socket`, `threading`.
* **Ciência de Dados / IA:** `scikit-learn` (Random Forest Classifier), `pandas` (One-Hot Encoding), `joblib` (serialização de modelos).

## 🚀 Como Executar o Projeto

**1. Clone o repositório:**
```bash
git clone [https://github.com/JonasLuizP/Sistema-de-Detec-o-de-Intrus-o-Baseado-em-Machine-Learning.git](https://github.com/JonasLuizP/Sistema-de-Detec-o-de-Intrus-o-Baseado-em-Machine-Learning.git)
cd nome-do-repositorio

# Certifique-se de ter as bibliotecas instaladas: pip install pandas scikit-learn joblib
python ai_server.py

gcc server.c -o server -lpthread
./server

gcc client.c -o client
./client
```
## 📊 Resultados e Validação

O modelo preditivo foi submetido ao rigoroso arquivo de testes cruzados KDDTest+, que contém 17 categorias de ataques inéditos que a IA não havia visto durante a fase de treinamento. Mesmo sob essas condições de ataques Zero-Day, o modelo Random Forest sustentou uma precisão global de 76,73%, comprovando sua capacidade de isolar anomalias matemáticas e generalizar ameaças em tempo real de forma "Caixa Branca" (White-Box).
