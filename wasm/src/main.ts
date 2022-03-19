import * as c3dev from "./c3dev";

let analogClock: AnalogClock;

/**
 * Analog Clock
 */
class AnalogClock {
    private HAND_LENGTH_HOUR: f32 = 0.55;
    private HAND_LENGTH_MINUTE: f32 = 0.85;
    private HAND_LENGTH_SECOND: f32 = 0.9;

    private cx: u32;
    private cy: u32;
    private cr: u32;

    private rhour: f32;
    private rminute: f32;
    private rsecond: f32;

    private envTmpRaito: i32;
    private envHumRaito: i32;
    private envPressureRaito: i32;

    constructor(cx: u32, cy: u32, cr:u32) {
        this.cx = cx;
        this.cy = cy;
        this.cr = cr;

        this.init();
    }

    init(): void {
        circle(this.cx, this.cy, this.cr, c3dev.COLOR.BLUE);
        circle(this.cx, this.cy, 2, c3dev.COLOR.BLUE);

        for(let angle: f32 = 0; angle < 360; angle += 6) {
            const rad: f32 = (angle / 180) * Math.PI;
            const sx: u32 = this.cx + <i32>(Math.cos(rad) * (<f32>this.cr - 6));
            const sy: u32 = this.cy + <i32>(Math.sin(rad) * (<f32>this.cr - 6));
            const tx: u32 = this.cx + <i32>(Math.cos(rad) * (<f32>this.cr - 1));
            const ty: u32 = this.cy + <i32>(Math.sin(rad) * (<f32>this.cr - 1));
            let color = 0x0015;
            if(<u32>angle % 30 == 0) color = c3dev.COLOR.BLUE;
            line(sx, sy, tx, ty, color);
        }
    }

    tick(): void {
        const now: Date = new Date(c3dev.now());

        const nowHands = this.calcHands(now);
        if(nowHands[0] != this.rsecond) {
            this.drawHand(this.rsecond, <f32>this.cr * this.HAND_LENGTH_SECOND, c3dev.COLOR.BLACK);
            this.drawHand(nowHands[0], <f32>this.cr * this.HAND_LENGTH_SECOND, 0x0015);
            this.rsecond = nowHands[0];
        }
        if(nowHands[1] != this.rminute) {
            this.drawHand(this.rminute, <f32>this.cr * this.HAND_LENGTH_MINUTE, c3dev.COLOR.BLACK);
            this.rminute = nowHands[1];
        }
        this.drawHand(nowHands[1], <f32>this.cr * this.HAND_LENGTH_MINUTE, c3dev.COLOR.ORANGE);
        if(nowHands[2] != this.rhour) {
            this.drawHand(this.rhour, <f32>this.cr * this.HAND_LENGTH_HOUR, c3dev.COLOR.BLACK);
            this.rhour = nowHands[2];
        }
        this.drawHand(nowHands[2], <f32>this.cr * this.HAND_LENGTH_HOUR, c3dev.COLOR.ORANGE);

        const tmp = c3dev.get_env_tmp();
        const tmpRaito = <i32>Math.round((tmp + 20) * 1.2);
        if(tmpRaito != this.envTmpRaito) {
            this.drawArcMeter(60, -60, <f32>tmpRaito, c3dev.COLOR.RED, c3dev.COLOR.BLUE);
            this.envTmpRaito = tmpRaito;
            c3dev.drawString(136, 116, c3dev.COLOR.RED, `${tmp}`.slice(0, 4) + "C");
        }
        const hum = c3dev.get_env_hum();
        const humRaito = <i32>Math.round(hum);
        if(humRaito != this.envHumRaito) {
            this.drawArcMeter(120, 180, <f32>humRaito, c3dev.COLOR.MAGENTA, c3dev.COLOR.BLUE);
            this.envHumRaito = humRaito;
            c3dev.drawString(0, 116, c3dev.COLOR.MAGENTA, `${hum}`.slice(0, 4) + "%");
        }
        const pressure = c3dev.get_env_pressure();
        const pressureRaito = <i32>Math.round(pressure / 20);
        if(pressureRaito != this.envPressureRaito) {
            this.drawArcMeter(180, 240, <f32>pressureRaito, c3dev.COLOR.GREEN, c3dev.COLOR.BLUE);
            this.envPressureRaito = pressureRaito;
            c3dev.drawString(0, 0, c3dev.COLOR.GREEN, `${pressure}`.slice(0, 5) + "hP");
        }
    }

    drawHand(radian: f32, length: f32, color: c3dev.COLOR): void {
        const cos = Math.cos(radian);
        const sin = Math.sin(radian);

        line(this.cx + <i32>(cos * 4), this.cy + <i32>(sin * 4), this.cx + <i32>(cos * length), this.cy + <i32>(sin * length), color);
    }

    drawArcMeter(sangle: f32, eangle: f32, ratio: f32, fc: c3dev.COLOR, bc: c3dev.COLOR): void {
        if(ratio > 100.0) {
            ratio = 100.0;
        }
        let start = sangle;
        let stop = eangle;
        if(sangle < eangle) {
            start = eangle;
            stop = sangle;
        }
        let step: f32 = (100.0 / (start - stop)) / 10;

        let sraito: f32 = 0.0;
        for(let angle: f32 = start; angle > stop; angle -= 0.1) {
            const rad: f32 = (angle / 180) * Math.PI;
            const sx: u32 = this.cx + <i32>(Math.cos(rad) * (<f32>this.cr + 10));
            const sy: u32 = this.cy + <i32>(Math.sin(rad) * (<f32>this.cr + 10));
            const tx: u32 = this.cx + <i32>(Math.cos(rad) * (<f32>this.cr + 8));
            const ty: u32 = this.cy + <i32>(Math.sin(rad) * (<f32>this.cr + 8));
            let color = fc;
            if(sraito > ratio) {
                color = bc;
            }
            line(sx, sy, tx, ty, color);
            sraito += step;
        }
    }

    calcHands(date: Date): Float32Array {
        let hands = new Float32Array(3);

        hands[0] = ((<f32>date.getUTCSeconds() * 6.0) - 90.0) / 180.0 * Math.PI;
        hands[1] = ((<f32>date.getUTCMinutes() * 6.0) - 90.0) / 180.0 * Math.PI;
        hands[2] = ((Math.abs(12 - <f32>date.getUTCHours()) * 30.0 + <f32>date.getUTCMinutes() * 0.5) - 90.0) / 180.0 * Math.PI;

        return hands;
    }
}

export function init(): void {
    // ToDo: Workaround: Initialize Wasm3 Stack
    new ArrayBuffer(1);
    // Test env.seed
    (u16)(Math.random() * 65535);
}

export function clock(x: u32, y: u32, r: u32): void {
    analogClock = new AnalogClock(x, y, r);
}

export function tick(): void {
    if(analogClock != null) {
        analogClock.tick();
    }
    // c3dev.drawString(28, 16 * 3, c3dev.COLOR.WHITE, now.toDateString());
    // c3dev.drawString(52, 16 * 4, c3dev.COLOR.WHITE, now.toTimeString());
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
    let dx: i32 = <i32>Math.abs(<f32>x1 - <f32>x0);
    let dy: i32 = <i32>Math.abs(<f32>y1 - <f32>y0);
    let sx: i32 = x0 < x1 ? 1: -1;
    let sy: i32 = y0 < y1 ? 1: -1;

    let x: i32 = x0;
    let y: i32 = y0;
    let err: i32 = dx - dy;

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
