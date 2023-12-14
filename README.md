# STM32 Calculator with Nokia5110LCD & Qt Interface

## Description
This project implements calculator functionalities based on the STM32 microcontroller. The display screen utilized is the Nokia5110LCD, while the upper computer (host) adopts the Qt software architecture. Communication between the upper computer and STM32 is achieved via UART. The project aims to demonstrate how to utilize STM32 to implement calculator functionalities and establish communication and control with a specific display screen and upper computer software.
该项目为北理特立自动化大三上单片机课设选题之一。
项目基于STM32微控制器实现了计算器功能。显示屏采用Nokia5110LCD，上位机采用Qt软件架构。上位机与STM32之间通过UART通信。本项目旨在展示如何利用STM32实现计算器功能，并结合特定显示屏和上位机软件进行通信与控制。

## Key Features
- **STM32 Implementation**: Implementing basic calculator functionalities on STM32.
- **Nokia5110LCD**: Utilizing Nokia5110LCD display for information presentation.
- **Qt Upper Computer**: Employing Qt software as the upper computer control interface.

## Installation
To use this project, follow these steps:

Clone the repository: `git clone https://github.com/Lnearfar/stm32Calculator.git`


## Usage
Once installed, follow these instructions to use the project:

1. Refer to the diagram for **hardware connections**.
2. For the **stm32** section: Open the Keil project at MDK-ARM/Calculator/Calculator.uvprojx, compile, and flash it onto the stm32f103c8.
3. For the **Host** section(上位机): Open the Qt project at Host/stm32CalculatorHost/stm32CalculatorHost.pro.

## Contribution
We welcome contributions from the community! To contribute to the project, follow these steps:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature/awesome-feature`).
3. Make your changes.
4. Commit your changes (`git commit -am 'Add awesome feature'`).
5. Push to the branch (`git push origin feature/awesome-feature`).
6. Create a Pull Request.

## License
MIT

## Contact
For any inquiries or support regarding the project, feel free to contact us at [nearfar1jy@gmail.com].