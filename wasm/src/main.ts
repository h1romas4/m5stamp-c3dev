import * as c3dev from "./c3dev";

export function circle_fill_random(x: u32, y: u32): void {
    for (let r: u32 = 0; r < x / 2; r++) {
        circle(x / 2, y / 2, r, (u16)(Math.random() * 65535));
    }
}

export function circle(x: u32, y: u32, r: u32, color: u16): void {
    let xx: u32 = r;
    let yy: u32 = 0;
    let err = 0;

    // Execute Wasm3 heap allocation
    // Wasm3: -Dd_m3FixedHeap=131070
    //  heap_caps_get_free_size: 86664
    // AS: --lowMemoryLimit 64
    // ERROR AS104: Low memory limit exceeded by static data: 340 > 64
    // AS: --lowMemoryLimit 2048
    // c3dev.drawString(String.UTF8.encode(`${x}, ${y}, ${r}, ${color}`, true));
    new ArrayBuffer(1);
    // TODO: All the variables under this will be broken.

    while(xx >= yy) {
        c3dev.pset(x + xx, y + yy, color);
        c3dev.pset(x + yy, y + xx, color);
        c3dev.pset(x - yy, y + xx, color);
        c3dev.pset(x - xx, y + yy, color);
        c3dev.pset(x - xx, y - yy, color);
        c3dev.pset(x - yy, y - xx, color);
        c3dev.pset(x + yy, y - xx, color);
        c3dev.pset(x + xx, y - yy, color);
        if(err <= 0) {
            yy++;
            err += 2 * yy + 1;
        } else {
            xx--;
            err -= 2 * xx + 1;
        }
    }
}
