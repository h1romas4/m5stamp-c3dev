# m5stamp-c3dev

![Main Board](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_01.jpg)

This is a development board for the [M5Stamp C3](https://shop.m5stack.com/products/m5stamp-c3-mate-with-pin-headers) (RISC-V/FreeRTOS).

- External USB-C port for JTAG debugging
- Support for LCD panel and SD card
- Selecting a power supply
- Switch(GPIO9) to enter loader mode
- Pin headers to expose usable GPIOs to the outside
- The size is just Japanese business card

This repository contains MIT Licensed PCB data and example programs.

📼 [YouTube Demo](https://youtu.be/46I3Uo5Xivg)

## Schematic

![Circuit Diagram](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/circuit_diagram_01.png)

|#Unit|Name|Note|Where to get it|
|----|----|----|----|
|U1|AE-USB2.0-TYPE-C|USB 2.0 Type-C Break out board|[akizukidenshi.com (JP)](https://akizukidenshi.com/catalog/g/gK-13080/)|
|U2|M5Stamp C3|-|+ [M5Stack](https://shop.m5stack.com/products/m5stamp-c3-mate-with-pin-headers?variant=40724631257260)<br />+ [SWITCH SCIENCE (JP)](https://www.switch-science.com/catalog/7474/)|
|U3|KMR-1.8SPI|KMR-1.8 SPI marked LCD and SD card interface|+ [amazon.co.jp (1)](https://www.amazon.co.jp/dp/B010SHK0Y0/)<br />+ [amazon.co.jp (2)](https://www.amazon.co.jp/gp/product/B07RG8SJVB/)<br />+ [ja.aliexpress.com](https://ja.aliexpress.com/item/4000511275898.html)|
|R1 - R2|5.2K Resistor|for USB Power supply|-|
|R3 - R7|10K Resistor|SPI pullup|-|
|JP1|Jumper pin|Select external power supply|[akizukidenshi.com (JP)](https://akizukidenshi.com/catalog/g/gP-03687/)|
|SW1|Tact switch|for loader mode and utility|[akizukidenshi.com (JP)](https://akizukidenshi.com/catalog/g/gP-03647/)|
|J1|12 Pin header|You can use the one that comes with the M5Stamp C3 Mate||

## PCB and Gerber data

- [KiCad 6 PCB](https://github.com/h1romas4/m5stamp-c3dev/tree/main/pcb/fg06-c3dev-revb)

- [Gerber Data](https://github.com/h1romas4/m5stamp-c3dev/raw/main/pcb/fg06-c3dev-revb/fg06-c3dev-revb-gerber.zip)

## Example Source

### Require

- [Setup ESF-IDF v4.4](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/#step-1-install-prerequisites)
- [(Optional)Setup OpenOCD with ESP32 JTAG support](https://github.com/espressif/openocd-esp32)

get_idf

```
$ get_idf
Detecting the Python interpreter
... snip ...
Done! You can now compile ESP-IDF projects.
Go to the project directory and run:

  idf.py build

$ echo ${IDF_PATH}
/home/hiromasa/devel/toolchain/esp/esp-idf

$ riscv32-esp-elf-gcc -v
Using built-in specs.
COLLECT_GCC=riscv32-esp-elf-gcc
... snip ...
gcc version 8.4.0 (crosstool-NG esp-2021r2)
```

openocd

```
$ openocd
Open On-Chip Debugger  v0.10.0-esp32-20211111 (2021-11-10-21:40)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
```

### Build and Execute

git clone and build

```
git clone  --recursive https://github.com/h1romas4/m5stamp-c3dev
idf.py build flash
```

Write TypeType font to SPIFFS

```
parttool.py --port "/dev/ttyACM0" write_partition --partition-name=font --partition-subtype=spiffs --input resources/spiffs_font.bin
```

Restart M5Stamp C3

```
idf.py monitor
```

### JTAG debug with Visual Studio Code

1. Set ESP32_TOOLCHAIN_HOME

```
$ echo ${ESP32_TOOLCHAIN_HOME}
/home/hiromasa/.espressif/tools/riscv32-esp-elf
```

2. Connect the PC to the USB Type-C of the U1
3. Open the source file in Visual Studio Code.
4. Run Task "openocd (debug)" @see [.vscode/tasks.json](https://github.com/h1romas4/m5stamp-c3dev/blob/main/.vscode/tasks.json#L7-L13)
5. Set a breakpoint in the source code.
6. Debug Launch (GDB) @see [.vscode/launch.json](https://github.com/h1romas4/m5stamp-c3dev/blob/main/.vscode/launch.json#L8-L23)
    The first time you start the program, it will often fail, so if you get an error, retry.

![vscode](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_02.png)

### Note

- Create SPIFFS parteation file

```
python ${IDF_PATH}/components/spiffs/spiffsgen.py 0x100000 resources/font resources/spiffs_font.bin
```

- Change the output destination of the log to U1 Serial/JTAG.(Don't forget to put it back)

Component config → ESP System Settings → Channel for console output

```
idf.py menuconfig
```

![vscode](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_03.png)

## Dependencies

Thanks for all the open source.

|Name|Version|License|
|-|-|--|
|[esp-idf](https://docs.espressif.com/projects/esp-idf/en/release-v4.4/esp32/get-started/index.html)|`v3.2.3`|BSD License|
|[esp32-arduino](https://github.com/espressif/arduino-esp32)|2.0.x master (`50e9772`)|LGPL-2.1 License|
|[M5EPD](https://github.com/m5stack/M5EPD)|0.1.x master(`bf4bd28`)|FreeType Part(The FreeType License)|
|[Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)|`1.10.13`|BSD License|
|[Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library)|`1.9.1`|MIT license|
|[tinyPNG](https://github.com/olliiiver/tinyPNG)|`0.11`|MIT License|
|[源真ゴシック](http://jikasei.me/font/genshin/)|-|SIL Open Font License 1.1|

## License

MIT License (includes PCB data and example source)
