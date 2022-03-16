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
    canvas.setAttribute('width', CANVAS_WIDTH);
    canvas.setAttribute('height', CANVAS_HEIGHT);
    canvas.style.width = CANVAS_WIDTH * 2 + "px";
    canvas.style.heigth = CANVAS_HEIGHT * 2 + "px";
    canvasContext = canvas.getContext('2d');
    canvasContext.font = `${CANVAS_FONT_SIZE}px sans-serif`;
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
            canvasContext.fillStyle = 'red';
            canvasContext.fillRect(x, y, 1, 1);
            console.log(`c3dev.draw_pixel: ${x}, ${y}, ${color}`);
        },
        'draw_string': (i0, i1, i2, i3) => {
            console.log(`c3dev.draw_string: ${i0}, ${i1}, ${i2}, ${i3}`);
        },
        'now': () => {
            return Date.now();
        }
    };

    return imports;
}

/**
 * Convert RGB565 to 888
 * @param {string} rgb565
 */
function convertRGB565to888String(rgb565) {
    const r5 = rgb565 & 0b1111100000000000 >> 11;
    const g6 = rgb565 & 0b0000011111100000 >> 5;
    const b6 = rgb565 & 0b0000000000011111;
    const r8 = (r5 * 255 + 15) / 31;
    const g8 = (g6 * 255 + 31) / 63;
    const b8 = (b5 * 255 + 15) / 31;
}

/**
 * Main
 */
(async function() {
    await loadWasm();
    wasmExports.clock(80, 64, 64);
})();
