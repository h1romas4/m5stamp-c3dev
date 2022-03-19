/**
 * Canvas settings
 */
const CANVAS_WIDTH = 160;
const CANVAS_HEIGHT = 128;
const CANVAS_FONT_SIZE = 14;

/**
 * Canvas
 * @type {HTMLCanvasElement}
 */
let canvas;

/**
 * CanvasContext
 * @type {CanvasRenderingContext2D}
 */
let canvasContext;

/**
 * WebAssembly Exports
 * @type {WebAssembly.Instance.exports}
 */
let wasmExports;

/**
 * Setting Canvas
 */
(function() {
    canvas = document.getElementById('screen');
    canvas.setAttribute('width', CANVAS_WIDTH * 2);
    canvas.setAttribute('height', CANVAS_HEIGHT * 2);
    canvas.style.width = CANVAS_WIDTH * 2 + "px";
    canvas.style.heigth = CANVAS_HEIGHT * 2 + "px";
    canvasContext = canvas.getContext('2d');
    canvasContext.scale(2, 2);
    canvasContext.font = `${CANVAS_FONT_SIZE}px sans-serif`;
    canvasContext.lineWidth = 1;
    canvasContext.imageSmoothingEnabled = true;
})();

/**
 * Load WebAssembly
 */
async function loadWasm() {
    const response = await fetch(new URL('../dist/app.wasm', import.meta.url));
    const responseArrayBuffer = new Uint8Array(await response.arrayBuffer());
    const wasm_bytes = new Uint8Array(responseArrayBuffer).buffer;
    let module = await WebAssembly.compile(wasm_bytes);
    const instance = await WebAssembly.instantiate(module, {
        ...createImports()
    });
    wasmExports = instance.exports;
};

/**
 * Create Imports C3DEV interface
 * @returns imports
 */
function createImports() {
    let imports = [];
    imports['env'] = {
        'abort': (i0, i1, i2, i3) => {
            console.log(`env.abort: ${i0}, ${i1}, ${i2}, ${i3}`);
        },
        'seed': () => {
            return Math.random();
        }
    };
    imports['c3dev'] = {
        'draw_pixel': (x, y, color) => {
            canvasContext.fillStyle = convertRGB565toStyle(color);
            canvasContext.fillRect(x, y, 1, 1);
        },
        'draw_string': (x, y, color, string) => {
            canvasContext.fillStyle = convertRGB565toStyle(color);
            canvasContext.fillText(decodeUTF8(string), x, y + CANVAS_FONT_SIZE);
        },
        'draw_line': (x0, y0, x1, y1, color) => {
            canvasContext.strokeStyle = convertRGB565toStyle(color);
            canvasContext.beginPath();
            canvasContext.moveTo(x0, y0);
            canvasContext.lineTo(x1, y1);
            canvasContext.stroke();
        },
        'log': (string) => {
            console.log(decodeUTF8(string));
        },
        'now': () => {
            return BigInt(Date.now() + /* UTC+9 */ 3600000 * 9);
        }
    };

    return imports;
}

/**
 * Convert RGB565 to Canvas style
 * @param {string} rgb565
 */
function convertRGB565toStyle(rgb565) {
    const r8 = (rgb565 & 0xF800) >> 8;
    const g8 = (rgb565 & 0x07E0) >> 3;
    const b8 = (rgb565 & 0x001F) << 3;

    return `#` +
        `${(Number(r8).toString(16)).padStart(2, '0')}` +
        `${(Number(g8).toString(16)).padStart(2, '0')}` +
        `${(Number(b8).toString(16)).padStart(2, '0')}`;
}

function decodeUTF8(wasmPtr) {
    const memory = wasmExports.memory.buffer;
    // https://www.assemblyscript.org/memory.html#internals
    // TODO: internal: #rtSize
    const rtSize = new Uint32Array(memory, wasmPtr - 4, 1) - /* null terminated */ 1;
    // ArrayBuffer
    const utf8 = new Uint8Array(memory, wasmPtr, rtSize);
    var decoder = new TextDecoder('utf-8');

    return decoder.decode(utf8);
}

/**
 * Main
 */
(async function() {
    await loadWasm();
    wasmExports.clock(80, 64, 63);
    wasmExports.init();
    setInterval(() => {
        wasmExports.tick();
        wasmExports.__collect() // clean up all garbage
    }, 500);
})();
