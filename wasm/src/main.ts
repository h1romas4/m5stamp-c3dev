import * as c3dev from "./c3dev";

export function init(): void {
    // ToDo: Workaround: Initialize Wasm3 Stack
    new ArrayBuffer(1);
    (u16)(Math.random() * 65535);
}

export function clock(x: u32, y: u32, r: u32): void {
    let xx: u32 = r;
    let yy: u32 = 0;
    let err = 0;
    const color = c3dev.COLOR.RED;

    while(xx >= yy) {
        c3dev.draw_pixel(x + xx, y + yy, color);
        c3dev.draw_pixel(x + yy, y + xx, color);
        c3dev.draw_pixel(x - yy, y + xx, color);
        c3dev.draw_pixel(x - xx, y + yy, color);
        c3dev.draw_pixel(x - xx, y - yy, color);
        c3dev.draw_pixel(x - yy, y - xx, color);
        c3dev.draw_pixel(x + yy, y - xx, color);
        c3dev.draw_pixel(x + xx, y - yy, color);
        if(err <= 0) {
            yy++;
            err += 2 * yy + 1;
        } else {
            xx--;
            err -= 2 * xx + 1;
        }
    }
}

export function tick(): void {
    const now: Date = new Date(c3dev.now());

    c3dev.drawString(18, 16 * 2, 0xfff0, now.toDateString());
    c3dev.drawString(48, 16 * 4, 0xfff0, now.toTimeString());
}
