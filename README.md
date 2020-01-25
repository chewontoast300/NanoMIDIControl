# NanoMIDIControl
Simple midi controller project for Arduino using the Arduino Nano
---
## !!WORK IN PROGRESS!!
#  NOT CURRENTLY FUNCTIONING


## Goals:
1. Simple, scalable, & stable midi controller platform.
2. Project designed around a midi keyboard instead of a MPC or knob/slider style controller.

## Future goals:
1. Simplify the process of designing/testing synths & other musical equipment through midi by having a complete modular, plug and play, midi interface.

---
## Current Hardware:
- Arduino Nano (I'm using a clone that works fine but you may need CH340G Driver for your computer) [Amazon Link](https://www.amazon.com/LAFVIN-Board-ATmega328P-Micro-Controller-Arduino/dp/B07G99NNXL/ref=sr_1_5?keywords=arduino+nano&qid=1578443246&sr=8-5)
!!Switching to an arduino micro or a teensy to support midi over usb natively.!!

- 128 x 32 oled display over i2c [Amazon Link](https://www.amazon.com/DSD-TECH-OLED-Display-Arduino/dp/B07D9H83R4/ref=sr_1_2?keywords=128x32&qid=1578443622&sr=8-2)

- tact switches or any other type of button or switch [Amazon Link](https://www.amazon.com/DAOKI-Miniature-Momentary-Tactile-Quality/dp/B01CGMP9GY/ref=sxin_2_ac_d_pm?ac_md=1-0-VW5kZXIgJDU%3D-ac_d_pm&cv_ct_cx=tact+switch&keywords=tact+switch&pd_rd_i=B01CGMP9GY&pd_rd_r=e06c377b-e856-4177-903d-20aee39197ed&pd_rd_w=rmtZ9&pd_rd_wg=UzxHy&pf_rd_p=709d2064-e546-4799-9e66-b352ea89951f&pf_rd_r=738NWDDKHZ4Y7Q70ZWFE&psc=1&qid=1578442853)
!!Looking at using these in 2nd prototype [Amazon Link](https://www.amazon.com/Adafruit-Soft-Tactile-Button-ADA3101/dp/B01JD3H5RK/ref=sr_1_5?keywords=soft+tact+switch&qid=1579208249&sr=8-5)

- Any type of potentiometers [TaydaElectronics.com Link](https://www.taydaelectronics.com/10k-ohm-linear-taper-potentiometer-round-shaft-pcb-9mm.html)
!!Looking at using these for 2nd prototype [Amazon Link](https://www.sparkfun.com/products/9806)

- 1N4148 Diodes for button matrix [Amazon Link](https://www.amazon.com/100-Pieces-1N4148-Switching-High-Speed/dp/B079KJ91JZ/ref=sr_1_3?crid=2A5PIVKLCQ2GZ&keywords=1n4148+diode&qid=1578445466&sprefix=1N4148%2Caps%2C149&sr=8-3)

- Resistors for i2c bus [Discussed Here](https://electronics.stackexchange.com/questions/1849/is-there-a-correct-resistance-value-for-i2c-pull-up-resistors)

- !!Looking at this thumbstick for 2nd prototype [adafruit.com Link](https://www.adafruit.com/product/2765)

- !!Looking at this RGB encoder for 2nd prototype [sparkfun.com Link](https://www.sparkfun.com/products/15141)
---

