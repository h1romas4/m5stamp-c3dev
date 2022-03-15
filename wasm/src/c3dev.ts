export declare function now(): i64;
export declare function delay(wait: i32): void;
export declare function draw_pixel(x: i32, y: i32, color: i32): void;
export declare function draw_line(x0: i32, y0: i32, x1: i32, y1: i32, color: i32): void;
export declare function draw_string(x: i32, y: i32, color: i32, string: ArrayBuffer): void;
export declare function log(string: ArrayBuffer): void;

export function consoleLog(string: string): void {
    log(String.UTF8.encode(string, true));
}

export function drawString(x: i32, y: i32, color: i32, string: string): void {
    draw_string(x, y, color, String.UTF8.encode(string, true));
}
