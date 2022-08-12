# AssemblyScript Analog Clock

## Build

```
npm install
```

### for Web

```
npm run start
# http://localhost:1234
```

### for FG06-C3DEV/Wasm3

```
npm run asbuild
# and flash .wasm
cd ..
python ${IDF_PATH}/components/spiffs/spiffsgen.py 0x10000 resources/wasm resources/spiffs_wasm.bin
parttool.py --port write_partition --partition-name=wasm --partition-subtype=spiffs --input resources/spiffs_wasm.bin
```
