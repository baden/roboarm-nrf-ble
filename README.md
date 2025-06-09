
## Platformio

Довелось вручну довстановлювати

https://github.com/ICantMakeThings/Nicenano-NRF52-Supermini-PlatformIO-Support




## Распиновка

#define PIN_104  36  // P1.04 (SDA) (SPECIFY &Wire)
#define PIN_106  38  // P1.06 (SCL) (SPECIFY &Wire)


Підключення шагового двигуна вісі №3

Підключаю до якогось драйверу, шо перший попався, на HR4988

На шаговіку 4 дроти:

- чорний - B
- червоний - !A
- зелений - A
- синій - !B


## Компіляція та прошивка

Прошиваю через вбудований в плату бутлоадер. Викликається двойним натисканням RESET

Далі треба із хекста створити uf2.

```
pio run && python uf2conv.py .pio/build/nicenano/firmware.hex -c -f 0xADA52840
```


## Керування роборукою

Можна керувати з [WEB-додатка](https://baden.github.io/roboarm-nrf-ble/)

