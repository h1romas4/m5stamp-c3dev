import * as c3dev from "./c3dev";

let analogClock: AnalogClock;

const HAND_LENGTH_HOUR: f32   = 0.55;
const HAND_LENGTH_MINUTE: f32 = 0.85;
const HAND_LENGTH_SECOND: f32 = 0.9;

class Hands {
    seconds: f32;
    minutes: f32;
    hours: f32;
}

/**
 * Analog Clock
 */
class AnalogClock {
    private rhour: f32;
    private rminute: f32;
    private rsecond: f32;

    private envTmpRaito: i32;
    private envHumRaito: i32;
    private envPressureRaito: i32;

    constructor(private cx: u32, private cy: u32, private cr: u32) {
        this.init();
    }

    init(): void {
        const cx = this.cx;
        const cy = this.cy;
        const cr = <f32>this.cr;
        
        circle(cx, cy, this.cr, c3dev.COLOR.BLUE);
        circle(cx, cy, 2, c3dev.COLOR.BLUE);

        for(let angle: u32 = 0; angle < 360; angle += 6) {
            const rad = <f32>angle * (Mathf.PI / 180);
            const cos = Mathf.cos(rad);
            const sin = Mathf.sin(rad);
            const sx = cx + <i32>(cos * (cr - 6));
            const sy = cy + <i32>(sin * (cr - 6));
            const tx = cx + <i32>(cos * (cr - 1));
            const ty = cy + <i32>(sin * (cr - 1));
            let color = angle % 30 == 0 
                ? c3dev.COLOR.BLUE 
                : 0x0015;
            line(sx, sy, tx, ty, color);
        }
    }

    tick(): void {
        const cr = <f32>this.cr;
        const now = new Date(c3dev.now());

        const hands = this.calcHands(now);
        const hs = hands.seconds;
        const hm = hands.minutes;
        const hh = hands.hours;
        
        if(hs != this.rsecond) {
            this.drawHand(this.rsecond, cr * HAND_LENGTH_SECOND, c3dev.COLOR.BLACK);
            this.drawHand(hs, cr * HAND_LENGTH_SECOND, 0x0015);
            this.rsecond = hs;
        }
        if(hm != this.rminute) {
            this.drawHand(this.rminute, cr * HAND_LENGTH_MINUTE, c3dev.COLOR.BLACK);
            this.rminute = hm;
        }
        this.drawHand(hm, cr * HAND_LENGTH_MINUTE, c3dev.COLOR.ORANGE);
        if(hh != this.rhour) {
            this.drawHand(this.rhour, cr * HAND_LENGTH_HOUR, c3dev.COLOR.BLACK);
            this.rhour = hh;
        }
        this.drawHand(hh, cr * HAND_LENGTH_HOUR, c3dev.COLOR.ORANGE);

        const tmp = c3dev.get_env_tmp();
        const tmpRaito = <i32>Mathf.round((tmp + 20) * 1.2);
        if(tmpRaito != this.envTmpRaito) {
            this.drawArcMeter(60, -60, <f32>tmpRaito, c3dev.COLOR.RED, c3dev.COLOR.BLUE);
            this.envTmpRaito = tmpRaito;
            c3dev.drawString(136, 116, c3dev.COLOR.RED, `${tmp}`.slice(0, 4) + "C");
        }
        const hum = c3dev.get_env_hum();
        const humRaito = <i32>Mathf.round(hum);
        if(humRaito != this.envHumRaito) {
            this.drawArcMeter(120, 180, <f32>humRaito, c3dev.COLOR.MAGENTA, c3dev.COLOR.BLUE);
            this.envHumRaito = humRaito;
            c3dev.drawString(0, 116, c3dev.COLOR.MAGENTA, `${hum}`.slice(0, 4) + "%");
        }
        const pressure = c3dev.get_env_pressure();
        const pressureRaito = <i32>Mathf.round(pressure / 20);
        if(pressureRaito != this.envPressureRaito) {
            this.drawArcMeter(180, 240, <f32>pressureRaito, c3dev.COLOR.GREEN, c3dev.COLOR.BLUE);
            this.envPressureRaito = pressureRaito;
            c3dev.drawString(0, 0, c3dev.COLOR.GREEN, `${pressure}`.slice(0, 5) + "hP");
        }
    }

    drawHand(rad: f32, length: f32, color: c3dev.COLOR): void {
        const cx  = this.cx;
        const cy  = this.cy;
        const cos = Mathf.cos(rad);
        const sin = Mathf.sin(rad);

        line(
            cx + <i32>(cos * 4.0),
            cy + <i32>(sin * 4.0),
            cx + <i32>(cos * length),
            cy + <i32>(sin * length),
            color
        );
    }

    drawArcMeter(sangle: f32, eangle: f32, ratio: f32, fc: c3dev.COLOR, bc: c3dev.COLOR): void {
        if(ratio > 100) {
            ratio = 100;
        }
        let start = sangle;
        let stop = eangle;
        
        if(sangle < eangle) {
            start = eangle;
            stop = sangle;
        }
        let step: f32 = (100.0 / (start - stop)) / 10;

        const cx = this.cx;
        const cy = this.cy;
        const cr = <f32>this.cr;
        
        let sraito: f32 = 0;
        for(let angle: f32 = start; angle > stop; angle -= 0.1) {
            const rad = angle * (Mathf.PI / 180);
            const cos = Mathf.cos(rad);
            const sin = Mathf.sin(rad);
            
            const sx = cx + <i32>(cos * (cr + 10));
            const sy = cy + <i32>(sin * (cr + 10));
            const tx = cx + <i32>(cos * (cr + 8));
            const ty = cy + <i32>(sin * (cr + 8));

            let color = sraito > ratio ? bc : fc;
            line(sx, sy, tx, ty, color);
            sraito += step;
        }
    }

    calcHands(date: Date): Hands {
        return {
            seconds: ((<f32>date.getUTCSeconds() * 6) - 90) * (Mathf.PI / 180),
            minutes: ((<f32>date.getUTCMinutes() * 6) - 90) * (Mathf.PI / 180),
            hours:   ((<f32>date.getUTCHours() * 30 + <f32>date.getUTCMinutes() * 0.5) - 90) * (Mathf.PI / 180)
        };
    }
}

export function init(): void {
    // ToDo: Workaround: Initialize Wasm3 Stack
    new ArrayBuffer(1);
    // Test env.seed
    (u16)(Mathf.random() * 65535);
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
    
    let dx: i32 = <i32>Mathf.abs(<f32>x1 - <f32>x0);
    let dy: i32 = <i32>Mathf.abs(<f32>y1 - <f32>y0);
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
