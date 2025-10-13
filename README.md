WT32_KG

WT32-ETH01 (ESP32) – Front-Pin Layout (Version 1.4)
===================================================

Left Pin Column                             Right Pin Column
-------------------------------------------------------------
EN / RST / BOOT                                3.3V  → Versorgung

GPIO00   BOOT / Flash strapping             GPIO01   TX0  / UART0 TX
GPIO02   strapping (low @ boot)             GPIO03   RX0  / UART0 RX
GPIO04   GPIO / frei                        GPIO05   GPIO / frei
GPIO12   1-Wire  (open-drain + 4.7k)        GPIO13   GPIO / frei
GPIO14   PWM / LEDC output                  GPIO15   strapping (must be low)
GPIO16   UART2 RX                           GPIO17   UART2 TX / **On-board LED**
GPIO18   ETH_MDIO (LAN8720)                 GPIO19   ETH_TXD0 (LAN8720)
GPIO20   GPIO / frei                        GPIO21   ETH_CLK_OUT (LAN8720)
GPIO22   ETH_RXD0 (LAN8720)                 GPIO23   ETH_MDC (LAN8720)
GPIO24   GPIO / frei                        GPIO25   ETH_TX_EN (LAN8720)
GPIO26   ETH_RX_ER (LAN8720)                GPIO27   ETH_CRS_DV (LAN8720)
GPIO32   I2C SCL                            GPIO33   I2C SDA
GPIO34   Input only / ADC                   GPIO35   Input only / ADC
GPIO36   Input only / ADC (VP)              GPIO37   GPIO / frei
GPIO38   GPIO / frei                        GPIO39   Input only / ADC (VN)

GND        Masse / Ground                   GND       Masse / Ground
-------------------------------------------------------------

Hinweise:
- On-Board LED → **GPIO17** (aktiv HIGH)
- LAN/PHY reserviert: 18,19,21,22,23,25,26,27 → nicht verwenden
- Empfohlene User-Pins:
    • I²C  → GPIO32 (SCL), GPIO33 (SDA)
    • UART → GPIO16 (RX2), GPIO17 (TX2 / LED)
    • PWM  → GPIO14
    • 1-Wire → GPIO12 (mit 4.7k Pull-up auf 3.3V)
- Input-only: GPIO34–39 (keine Ausgänge)
- Bootrelevante Pins: GPIO0, GPIO2, GPIO12, GPIO15




