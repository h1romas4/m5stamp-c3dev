# m5stamp-c3dev

![](https://github.com/h1romas4/m5stamp-c3dev/workflows/Build/badge.svg)

![Main Board](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_01.jpg)

This is a development board for the [M5Stamp C3](https://shop.m5stack.com/products/m5stamp-c3-mate-with-pin-headers) (RISC-V/FreeRTOS).

**Hardware**

- External USB-C port for JTAG debugging
- Support for LCD panel and SD card
- Selecting a power supply
- Switch(GPIO9) to enter loader mode
- Pin headers to expose usable GPIOs to the outside
- The size is just Japanese business card

**Sample Sources Included**

![AssemblyScript and Wasm3](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_04.jpg)

![AssemblyScript and Wasm3](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_05.jpg)

- TrueType font with Japanese output to LCD - [test_freetype.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_freetype.cpp)
- Output PNG images in SD card to LCD - [test_tinypng.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_tinypng.cpp)
- NTP synchronization via WiFi connection - [test_nvs_wifi.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_nvs_wifi.cpp)
- Test ADC on GPIO0
    - UNIT Light - [main.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/main.cpp#L108-L111)
- Test I2C on GPIO18, 19
    - UNIT ENV III - [test_i2c_gpio1819.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_i2c_gpio1819.cpp#L43-L50)
    - UNIT ULTRASONIC - [test_i2c_gpio1819.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_i2c_gpio1819.cpp#L52-L61)
- Test UART on GPIO18, 19
    - UNIT GPS - [test_uart_gpio1819.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_uart_gpio1819.cpp)
- Test DIGITAL on GPIO18, 19
    - UNIT IR - [test_digit_gpio1819.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_digit_gpio1819.cpp)
- WebAssembly execution with Wasm3
    - Analog clock using AssemblyScript - [test_wasm3_clockenv.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_wasm3_clockenv.cpp)
    - GPS GSV viewer using AssemblyScript - [test_wasm3_gpsgsv.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_wasm3_gpsgsv.cpp)
    - 3D Cube Demo using AssemblyScript - [test_wasm3_imu6886.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/test_wasm3_imu6886.cpp)
- Support RGB LED (SK6812) - [main.cpp](https://github.com/h1romas4/m5stamp-c3dev/blob/main/main/main.cpp#L35-L38)
- Usage of SPIFFS, which stores TrueType fonts and .wasm binaries (parttool.py and spiffsgen.py tools)
- Use of NVS (cryptographically enabled key value store) that stores WiFi passwords (nvs_partition_gen.py tool)
- Building libraries and managing dependencies using the esp-idf build system
- Visual Studio Code C/C++ Extention configuration and JTAG debugging configuration in conjunction with openocd
- AssembyScript sharing method between web browser and microcontroller - [wasm](https://github.com/h1romas4/m5stamp-c3dev/tree/main/wasm)

This repository contains MIT Licensed PCB data and example programs.

## Demo

### Video

- 📼 [Demo(LCD)](https://youtu.be/46I3Uo5Xivg)
- 📼 [Demo(Wasm3)](https://youtu.be/TagQuPtwKCg)
- 📼 [Demo(3D Cube)](https://twitter.com/h1romas4/status/1610228824607985664)

### Web Simulation

- 🕸 [AssemblyScript Analog Clock](https://h1romas4.github.io/m5stamp-c3dev/asclock/)
- 🕸 [AssemblyScript GPS GSV Viewer](https://h1romas4.github.io/m5stamp-c3dev/asgps/)

## Schematic

![Circuit Diagram](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/circuit_diagram_01.png)

|#Unit|Name|Note|Where to get it|
|----|----|----|----|
|U1|AE-USB2.0-TYPE-C|USB 2.0 Type-C Break out board|[akizukidenshi.com (JP)](https://akizukidenshi.com/catalog/g/gK-13080/)|
|U2|M5STAMP-C3|M5Stamp C3|+ [M5Stack](https://shop.m5stack.com/products/m5stamp-c3-mate-with-pin-headers?variant=40724631257260)<br />+ [SWITCH SCIENCE (JP)](https://www.switch-science.com/catalog/7474/)|
|U3|KMR-1.8SPI|KMR-1.8 SPI marked LCD and SD card interface|+ [amazon.co.jp (1)](https://www.amazon.co.jp/gp/product/B07RG8SJVB/)<br />+ [ja.aliexpress.com](https://ja.aliexpress.com/item/4000511275898.html)|
|R1 - R2|5.2K Resistor|Pull-down for USB Power supply|-|
|R3 - R7|10K Resistor|SPI pull-up|-|
|JP1|Jumper pin|Select external power supply|[akizukidenshi.com (JP)](https://akizukidenshi.com/catalog/g/gP-03687/)|
|SW1|Tact switch|for loader mode and utility|[akizukidenshi.com (JP)](https://akizukidenshi.com/catalog/g/gP-03647/)|
|J1|12 Pin header|You can use the one that comes with the M5Stamp C3 Mate|-|

## PCB and Gerber data

- [KiCad 6 PCB](https://github.com/h1romas4/m5stamp-c3dev/tree/main/pcb/fg06-c3dev-revb)
- [Gerber Data](https://github.com/h1romas4/m5stamp-c3dev/raw/main/pcb/fg06-c3dev-revb/fg06-c3dev-revb-gerber.zip)
- [FG06-C3DEV REV.B (2枚) - M5Stamp C3 向けの開発用基板 - 部品なし基板のみ - 130円](https://h1romas4.booth.pm/items/3688492)

## Example Source

### Require

- [Setup ESF-IDF **v4.4.4**](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32c3/get-started/index.html#installation-step-by-step)

get_idf or (Windows) ESP-IDF **`4.4.4`** command prompt

```
# Linux or macOS ->
$ alias get_idf='. $HOME/esp/esp-idf/export.sh'
$ get_idf
# <-

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
gcc version 8.4.0 (crosstool-NG esp-2021r2-patch5)
```

openocd (Optional)

```
$ openocd -v
Open On-Chip Debugger  v0.11.0-esp32-20220706 (2022-07-06-15:48)
Licensed under GNU GPL v2
For bug reports, read
        http://openocd.org/doc/doxygen/bugs.html
```

### Build and Execute

1. git clone and build (Please add `--recursice`)

```
git clone --recursive https://github.com/h1romas4/m5stamp-c3dev
cd m5stamp-c3dev
idf.py build
```

**Note**: If you get a compile error, change the `sdkconfig` back to the Git one. In some cases, the target of `sdkconfig` is changed to `esp32` instead of `eps32c3`.

2. Write Partition table

```
idf.py partition-table-flash
```

3. Write TypeType font to SPIFFS

```
parttool.py write_partition --partition-name=font --partition-subtype=spiffs --input resources/spiffs_font.bin
```

4. Write WebAssembly(.wasm) to SPIFFS

```
parttool.py write_partition --partition-name=wasm --partition-subtype=spiffs --input resources/spiffs_wasm.bin
```

5. Write main program to go!

```
idf.py flash monitor
```

### Setup WiFi (Optional)

1. Change WiFi Setting

`nvs_partition.csv`: Set own `[ssid]`, `[password]`

```
key,type,encoding,value
wifi,namespace,,
ssid,data,string,[ssid]
passwd,data,string,[password]
```

2. Create NVS Partation file

```
python ${IDF_PATH}/components/nvs_flash/nvs_partition_generator/nvs_partition_gen.py generate nvs_partition.csv nvs_partition.bin 0x6000
```

3. Write NVS Partation

```
esptool.py write_flash 0x9000 nvs_partition.bin
```

4. Run

NTP synchronization is performed by pressing the SW1 after the startup logo.

```
idf.py monitor
```

### Select connected GPIO18 and GPIO19 device (Optional)

```
idf.py menuconfig
```

C3DEV Configuration → Select GPIO 18/19

![](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_05.png)

`3D Cube Demo`

![3D Cube](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_06.jpg)

### JTAG debug with Visual Studio Code

Requre setup ESP32_TOOLCHAIN_HOME

```
$ echo ${ESP32_TOOLCHAIN_HOME}
/home/hiromasa/.espressif/tools/riscv32-esp-elf
```

1. Connect the PC to the USB Type-C of the U1
2. Open the source file in Visual Studio Code.
3. Run Task "openocd (debug)" @see [.vscode/tasks.json](https://github.com/h1romas4/m5stamp-c3dev/blob/main/.vscode/tasks.json#L7-L13)
4. Set a breakpoint in the source code.
5. Debug Launch (GDB) @see [.vscode/launch.json](https://github.com/h1romas4/m5stamp-c3dev/blob/main/.vscode/launch.json#L8-L23)
    The first time you start the program, it will often fail, so if you get an error, retry.

![vscode](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_02.png)

### Note

#### If the LCD color is inverted.

`main/main.cpp`

```c
    // If the color is inverted, set to 1.
    tft.invertDisplay(0);
    // tft.invertDisplay(1);
```

#### Create SPIFFS parteation file from file system

```
python ${IDF_PATH}/components/spiffs/spiffsgen.py 0x100000 resources/font resources/spiffs_font.bin
python ${IDF_PATH}/components/spiffs/spiffsgen.py 0x10000 resources/wasm resources/spiffs_wasm.bin
```

#### Change the output destination of the log to U1 Serial/JTAG.(Don't forget to put it back)

Component config → ESP System Settings → Channel for console output

```
idf.py menuconfig
```

![vscode](https://raw.githubusercontent.com/h1romas4/m5stamp-c3dev/main/docs/images/m5stamp_c3dev_03.png)

## AssemblyScript and Wasm3

## Build AssemblyScript

```
cd wasm/clockenv # or wasm/gpsgsv or wasm/imu6886
npm install
```

**Web Browser Development**

```
npm run asbuild:web
npm run start
# http://localhost:1234/
```

**SPIFFS Build and Flash**

```
npm run asbuild
cd ../../
python ${IDF_PATH}/components/spiffs/spiffsgen.py 0x10000 resources/wasm resources/spiffs_wasm.bin
parttool.py write_partition --partition-name=wasm --partition-subtype=spiffs --input resources/spiffs_wasm.bin
```

## Dependencies

Thanks for all the open source.

|Name|Version|License|
|-|-|--|
|[esp-idf](https://docs.espressif.com/projects/esp-idf/en/v4.4.4/esp32c3/get-started/index.html)|`v4.4.4`|BSD License|
|[arduino-esp32](https://github.com/espressif/arduino-esp32/releases/tag/2.0.9)|`2.0.9`|LGPL-2.1 License|
|[M5EPD](https://github.com/m5stack/M5EPD)|`0.1.4`|FreeType Part(The FreeType License)|
|[UNIT_ENV](https://github.com/m5stack/UNIT_ENV)|`0.0.5`|MIT License|
|[M5Unit-Sonic](https://github.com/m5stack/M5Unit-Sonic)|`0.0.2`|MIT License|
|[Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)|`1.11.5`|BSD License|
|[Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO)|`1.14.1`|MIT License|
|[Adafruit-ST7735-Library](https://github.com/adafruit/Adafruit-ST7735-Library)|`1.10.0`|MIT License|
|[Adafruit_NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)|`1.11.0`|LGPL-3.0 License|
|[tinyPNG](https://github.com/olliiiver/tinyPNG)|`0.11`|MIT License|
|[lwgps](https://github.com/MaJerle/lwgps)|`v2.1.0`|MIT License|
|[IRremoteESP8266](https://github.com/crankyoldgit/IRremoteESP8266)|`v2.8.2`|LGPL-2.1 license|
|[Wasm3](https://github.com/wasm3/wasm3)|master(`045040a9`)|MIT License|
|[AsselblyScript](https://github.com/AssemblyScript/assemblyscript)|`0.27.4`|Apache-2.0 License|
|[源真ゴシック](http://jikasei.me/font/genshin/)|-|SIL Open Font License 1.1|

## License

MIT License (includes PCB data and example source)
