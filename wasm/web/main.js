import { VgmPlay, XgmPlay, setWasmExport } from "../dist/app.wasm";

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

(function() {
    canvas = document.getElementById('screen');
    canvas.setAttribute('width', CANVAS_WIDTH);
    canvas.setAttribute('height', CANVAS_HEIGHT);
    canvas.style.width = CANVAS_WIDTH * 2 + "px";
    canvas.style.heigth = CANVAS_HEIGHT * 2 + "px";
    canvasContext = canvas.getContext('2d');
    canvasContext.font = `${CANVAS_FONT_SIZE}px sans-serif`;
    // canvasContext.fillStyle = '#00a040';
    // canvasContext.fillRect(0, 0, CANVAS_WIDTH - 1, CANVAS_HEIGHT - 1);
})();

(async function() {
    const response = await fetch(new URL('../dist/app.wasm', import.meta.url));
    const responseArrayBuffer = new Uint8Array(await response.arrayBuffer());
    const wasm_bytes = new Uint8Array(responseArrayBuffer).buffer;
    let module = await WebAssembly.compile(wasm_bytes);
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
            return Math.random();
        }
    };
    const instance = await WebAssembly.instantiate(module, {
        ...imports
    });
    console.log(instance.exports);
    instance.exports.clock(80, 64, 64);
})();
