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

    // c3dev.drawString(String.UTF8.encode("TEST STRING1", true));
    // c3dev.drawString(String.UTF8.encode("TEST STRING2", true));
    // c3dev.delay(100);
    // c3dev.drawString(String.UTF8.encode("TEST STRING3", true));

    // c3dev.drawString(new ArrayBuffer(10));
    // String.UTF8.encode("TEST STRING1");

    // Execute Wasm3 heap allocation
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
