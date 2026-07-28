# Casa Inteligente

Este projeto foi desenvolvido para a disciplina de **Laboratório de Processadores** da **PUCRS**.

O objetivo foi criar uma **casa inteligente**, desenvolvendo tanto o software quanto o hardware do projeto. O código foi implementado utilizando a **STM32CubeIDE (CubeIDE)** e, além da programação, também foi realizada a montagem do circuito eletrônico e a construção de uma maquete para a apresentação final.

## Componentes utilizados

Durante o desenvolvimento foram utilizados os seguintes componentes:

- LDR (sensor de luminosidade)
- SHT15 (sensor de temperatura e umidade)
- PIR DYP (sensor de movimento)
- MQ-7 (sensor de monóxido de carbono)
- Reed Switch (interruptor magnético)
- Buzzer
- Displays LCD (96x64 e 16x2)
- Keypad 4x3
- Servomotores
- Smart Card

## Protocolos de comunicação

Para realizar a comunicação entre os componentes e também com o terminal do computador, foram utilizados os seguintes protocolos:

- I2C
- SPI
- UART

## Algumas fotos da maquete final

<img src="Fotos/CasaCompleta.pdf" alt="Maquete completa" width="600">
<img src="Fotos/CasaSemTelhado.pdf" alt="Maquete sem telhado" width="600">
<img src="Fotos/Circuito.pdf" alt="Circuito montado" width="600">

## Conclusão

Este projeto proporcionou uma experiência prática no desenvolvimento de sistemas embarcados, integrando software e hardware em uma única aplicação. Durante sua execução foi possível aplicar conceitos de programação em microcontroladores, utilização de sensores e atuadores, protocolos de comunicação (I2C, SPI e UART), além da montagem e validação de circuitos eletrônicos.
