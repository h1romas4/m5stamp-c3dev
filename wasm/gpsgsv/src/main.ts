import * as c3dev from "./c3dev";

let gpsView: GpsView;

const MAX_SATELLITE: u32 = 12;
const DEG_TO_RAD: f32 = Math.PI / 180;
const ELAPSED_MINUTE_10 = 10 * 60 * 1000;

class Gsv {
    elevation: u32;
    azimuth: u32;
    snr: u32;
    lastUpdate: i64;
}

class Pos {
    sx: u32;
    sy: u32;
    color: c3dev.COLOR;
}

class GpsView {
    private satellites: Set<u32>;
    private gsv: Map<u32, Gsv>;

    constructor(
        private cx: u32,
        private cy: u32,
        private cr: u32
    ) {
        this.satellites = new Set<u32>();
        this.gsv = new Map<u32, Gsv>();
    }

    public tick(clear: bool): void {
        const cx = this.cx;
        const cy = this.cy;
        const cr = <f32>this.cr;

        const gsv = this.gsv;
        const now = c3dev.now();
        const ids = gsv.keys();

        if(clear) this.initView();

        let pos = new Map<u32, Pos>();
        for(let index = 0; index < ids.length; index++) {
            const id = ids[index];
            const stl = gsv.get(id);
            // last update
            if(stl.lastUpdate == 0 || now - stl.lastUpdate > ELAPSED_MINUTE_10) {
                // clear
                stl.lastUpdate = 0;
                continue;
            }
            // calc satellite pos
            const ev = <f32>((90 - <f32>stl.elevation) / 90 * cr);
            const sx = <u32>(<f32>cx + Mathf.sin(<f32>stl.azimuth) * ev);
            const sy = <u32>(<f32>cy + Mathf.cos(<f32>stl.azimuth) * ev);
            const color = this.satellites.has(id)? c3dev.COLOR.RED : c3dev.COLOR.GREEN;
            pos.set(id, { sx: sx, sy: sy, color: color});
            // background
            line(cx, cy, sx, sy, c3dev.COLOR.BLUE);
        }
        const keys = pos.keys();
        for(let index = 0; index < keys.length; index++) {
            const stl = pos.get(keys[index]);
            // foregraund
            circle(stl.sx, stl.sy, 2, stl.color);
            if(clear) {
                c3dev.drawString(stl.sx, stl.sy, c3dev.COLOR.WHITE, `${keys[index]}`)
            }
        }
    }

    public setSatellites(satellites: Int8Array): void {
        this.satellites.clear();
        for(let i: u32 = 0; i < MAX_SATELLITE; i++) {
            this.satellites.add(satellites[i]);
        }
    }

    public setGsv(id: u32, elevation: u32, azimuth: u32, snr: u32): void {
        this.gsv.set(id, {
            elevation: elevation,
            azimuth: azimuth,
            snr: snr,
            lastUpdate: c3dev.now(),
        });
    }

    private initView(): void {
        const cx = this.cx;
        const cy = this.cy;
        const cr = <f32>this.cr;

        this.clearView();
        circle(cx, cy, this.cr, c3dev.COLOR.BLUE);
        circle(cx, cy, 2, c3dev.COLOR.BLUE);

        for(let angle: u32 = 0; angle < 360; angle += 6) {
            const rad = <f32>angle * DEG_TO_RAD;
            const cos = Mathf.cos(rad);
            const sin = Mathf.sin(rad);
            const sx = cx + <i32>(cos * (cr - 6));
            const sy = cy + <i32>(sin * (cr - 6));
            const tx = cx + <i32>(cos * (cr - 1));
            const ty = cy + <i32>(sin * (cr - 1));
            const color = angle % 30 == 0
                ? c3dev.COLOR.BLUE
                : 0x0015;
            line(sx, sy, tx, ty, color);
        }

        c3dev.drawString(cx - 2,  0, c3dev.COLOR.WHITE, "N");
    }

    private clearView(): void {
        const width = this.cr * 2;
        const left = this.cx - this.cr;
        const right = left + width + 1;

        for(let y: u32 = 0; y < width; y++) {
            c3dev.draw_line(left, y, right, y, c3dev.COLOR.BLACK);
        }
    }
}

export function init(): void {
    // ToDo: Workaround: Initialize Wasm3 Stack
    // Without this line, the Wasm3 stack will not work properly.
    // For example, the argument of the gpsgsv() function is the destroyed value.
    memory.grow(1);
    // Test env.seed
    seed();
}

export function gpsgsv(x: u32, y: u32, r: u32): void {
    gpsView = new GpsView(x, y, r);
}

export function tick(clear: bool): void {
    gpsView.tick(clear);
}

export function createSatellitesArray(): Int8Array {
    return new Int8Array(MAX_SATELLITE);
}

export function setSatellites(satellites: Int8Array) : void {
    gpsView.setSatellites(satellites);
}

export function setGsv(id: u32, elevation: u32, azimuth: u32, snr: u32): void {
    gpsView.setGsv(id, elevation, azimuth, snr);
}

function circle(x: u32, y: u32, r: u32, color: c3dev.COLOR): void {
    let xx: u32 = r;
    let yy: u32 = 0;
    let err = 0;

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

function line(x0: u32, y0: u32, x1: u32, y1: u32, color: c3dev.COLOR): void {
    let dx = abs<i32>(x1 - x0);
    let dy = abs<i32>(y1 - y0);
    let sx = x0 < x1 ? 1: -1;
    let sy = y0 < y1 ? 1: -1;

    let x: i32 = x0;
    let y: i32 = y0;
    let err = dx - dy;

    while(true) {
        c3dev.draw_pixel(x, y, color);
        if(x == x1 && y == y1) {
            break;
        }
        let e2 = 2 * err;
        if(e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if(e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}
