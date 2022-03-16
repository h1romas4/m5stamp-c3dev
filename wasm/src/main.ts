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

    for(let angle: f32 = 0; angle < 360; angle++) {
        let rad: f32 = (angle / 180) * Math.PI;
        let tx: u32 = x + <i32>(Math.cos(rad) * <f32>r);
        let ty: u32 = y + <i32>(Math.sin(rad) * <f32>r);
        c3dev.draw_line(x, y, tx, ty, <u32>((angle / 360) * 0xffff));
    }

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

    c3dev.drawString(28, 16 * 3, c3dev.COLOR.WHITE, now.toDateString());
    c3dev.drawString(52, 16 * 4, c3dev.COLOR.WHITE, now.toTimeString());
}
