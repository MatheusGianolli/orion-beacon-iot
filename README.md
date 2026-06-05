#  Orion Beacon - Monitoramento Lunar IoT


Integrantes:

Matheus Gianolli — RM: 565258
Enzo Xavier Coelho — RM: 563379
Gustavo Ribeiro Permagnani — RM: 564995
Larissa Juvenal de Magalhaes — RM: 566457
Julia Menezes — RM: 565568

##  Visão Geral do Projeto
O Orion Beacon é um sistema de Internet das Coisas (IoT) desenvolvido para a monitoração em tempo real das condições de solo na Cratera A-17 (Lua). O sistema analisa a temperatura e os índices de hidratação do solo, classificando o estado da água local para futuras missões de extração.

##  Arquitetura e Tecnologias
Para garantir a entrega de dados em tempo real sem problemas de bloqueio de rede (CORS/Firewall), a comunicação foi construída com arquitetura híbrida:
- **Hardware/Simulador:** ESP32, Sensor DHT22, Potenciômetro e Display LCD I2C.
- **Protocolo de Tempo Real:** MQTT via WebSockets seguros (HiveMQ).
- **Interface (Front-end):** Dashboard local em HTML/JS com atualização assíncrona.

##  Endpoints e Tópicos MQTT
Atendendo aos requisitos de arquitetura distribuída, o ESP32 publica telemetria no formato JSON em **3 tópicos MQTT distintos** no broker público `broker.hivemq.com`:

1. **`fiap/orion/gianolli`** (Status Consolidado)
   - *Payload:* `{"temp":"25.0", "hidra":"0.80", "status":"ALTA: H2O LIQ"}`
2. **`fiap/orion/gianolli/temperatura`** (Dado Bruto Termal)
   - *Payload:* `{"temperaturaC": 25.0}`
3. **`fiap/orion/gianolli/hidratacao`** (Dado Bruto Capacitivo)
   - *Payload:* `{"indiceHidratacao": 0.80}`

---

##  Guia de Execução 

Para testar o sistema completo do zero, siga os passos abaixo:

**1. Ligar o Hardware (Simulação):**
- Acesse o link público do projeto no Wokwi: https://wokwi.com/projects/465954654353615873
- Clique no botão verde **"Play"** para iniciar a simulação.
- Aguarde o terminal exibir `Wi-Fi Conectado!` e os logs de telemetria iniciarem.

**2. Iniciar o Dashboard (Interface Visual):**
- Faça o download do arquivo `dashboard.html` presente neste repositório.
- Dê um clique duplo no arquivo para abri-lo em qualquer navegador de internet (Chrome, Edge, etc). *Não é necessário instalar Node.js ou servidores locais.*

**3. Teste em Tempo Real:**
- Com a simulação rodando e o Dashboard aberto lado a lado, altere a Temperatura no DHT22 ou mova o Potenciômetro (Hidratação) no Wokwi.
- Observe o Dashboard HTML atualizar os valores numéricos e as cores de classificação de estado da água em tempo real, consumindo os dados via MQTT.
