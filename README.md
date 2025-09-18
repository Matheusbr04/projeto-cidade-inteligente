 LDR Light Sensor com Arduino

Sistema simples e funcional utilizando um **sensor LDR** para detectar a luminosidade do ambiente e acionar um **LED indicador** em condições de baixa luz. Este projeto é ideal para iniciantes em Arduino e eletrônica básica, e pode ser expandido para sistemas automáticos de iluminação, alarmes noturnos e projetos de automação residencial.


 Descrição

Este projeto utiliza um divisor de tensão com um LDR (Light Dependent Resistor) e um resistor comum para medir a intensidade de luz ambiente. A leitura analógica é comparada com um valor de referência (`darkThreshold`) e, se estiver abaixo desse limite, um LED é aceso indicando que está escuro.


 Funcionalidades

-  Leitura em tempo real da intensidade de luz.
-  Acionamento automático de LED em ambiente escuro.
-  Saída serial com os valores lidos do LDR.
-  Threshold ajustável para diferentes condições de iluminação.
-  Fácil integração com sistemas maiores de automação.

| Componente        | Quantidade | Descrição                     |
| ----------------- | ---------- | ----------------------------- |
| Arduino Uno       | 1          | Ou qualquer placa compatível  |
| LDR               | 1          | Sensor de luz                 |
| Resistor          | 1          | 10kΩ para o divisor de tensão |
| LED               | 1          | LED comum                     |
| Resistor para LED | 1          | 220Ω ou 330Ω                  |
| Jumpers           | Vários     | Fios para conexão             |
| Protoboard        | 1          | Para montagem do circuito     |

Lógica do Funcionamento

O Arduino lê o valor de luz do LDR através do pino analógico A0.

Se o valor lido for menor que o limiar definido (darkThreshold), significa que o ambiente está escuro.

Nesse caso, o LED conectado ao pino 13 é aceso.

Se o valor for maior, o LED é apagado.

---

API (Serial Monitor)

O monitor serial pode ser usado como uma API para visualização em tempo real dos dados lidos.

 Saída Serial

Luz: 856
Luz: 870
Luz: 410
Luz: 320

  [5V] ----+----[LDR]----+----[A0]
           |            |
        [10kΩ]        [GND]
           |
         [GND]

  LED positivo (anodo) → D13
  LED negativo (catodo) → Resistor 220Ω → GND


const int ldrPin = A0;      // Entrada do divisor de tensão com LDR
const int ledPin = 13;      // LED indicador de escuridão



Imagem do Circuito

![assetscircuitopng](https://github.com/user-attachments/assets/ca3a992b-8f6c-446c-aa83-d8b2e74f32fa)
