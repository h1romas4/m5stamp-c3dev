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
            canvasContext.fillStyle = convertRGB565toStyle(color);
            canvasContext.fillRect(x, y, 1, 1);
        },
        'draw_string': (i0, i1, i2, i3) => {
            console.log(`c3dev.draw_string: ${i0}, ${i1}, ${i2}, ${i3}`);
        },
        'now': () => {
            return BigInt(Date.now());
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

/**
 * Main
 */
(async function() {
    await loadWasm();
    wasmExports.clock(80, 64, 64);
    wasmExports.init();
    wasmExports.tick();
})();
