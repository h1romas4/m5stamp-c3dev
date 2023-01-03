import * as c3dev from "./c3dev";

let imu: Imu | null;

const DEG_TO_RAD: f32 = Math.PI / 180;
const CUBE_POINTS: f32[][] = [
    [-1, -1,  1],
    [ 1, -1,  1],
    [ 1,  1,  1],
    [-1,  1,  1],
    [-1, -1, -1],
    [ 1, -1, -1],
    [ 1,  1, -1],
    [-1,  1, -1]
];
const CUBE_LENGTH = 8;

const DISTANCE: f32 = 4;
const SCALE: f32 = 240;

/**
 * IMU sensor
 */
class Imu {
    private center_x: f32 = 0;
    private center_y: f32 = 0;
    private angle: f32 = 0;
    private prev_proj_2d_points: u32[][] = [];

    constructor(
        private width: u32,
        private height: u32
    ) {
        this.init();
    }

    init(): void {
        this.center_x = this.width as f32 / 2.0;
        this.center_y = this.height as f32 / 2.0;
        for(let i = 0; i < CUBE_LENGTH; i++) {
            this.prev_proj_2d_points[i] = [0, 0];
        }
    }

    tick(): void {
        const angle: f32 = this.angle;

        const cos: f32 = Mathf.cos(angle);
        const sin: f32 = Mathf.sin(angle);

        const rot_x: f32[][] = [
            [    1,    0,    0],
            [    0,  cos, -sin],
            [    0,  sin,  cos]
        ];
        const rot_y: f32[][] = [
            [  cos,    0,  -sin],
            [    0,    1,     0],
            [  sin,    0,   cos]
        ];
        const rot_z: f32[][] = [
            [  cos, -sin,     0],
            [  sin,  cos,     0],
            [    0,    0,     1]
        ];

        let proj_2d_points: u32[][] = [];
        let rot_2d: f32[];
        for(let i = 0; i < CUBE_LENGTH; i++) {
            rot_2d = matrix_multiple(rot_y, CUBE_POINTS[i]);
            rot_2d = matrix_multiple(rot_x, rot_2d);
            rot_2d = matrix_multiple(rot_z, rot_2d);
            let z: f32 = 1 / (DISTANCE - rot_2d[2]);
            const proj_2d: f32[] = matrix_multiple(
                [
                    [z, 0, 0],
                    [0, z, 0]
                ],
                rot_2d
            );
            const x = ((proj_2d[0] * SCALE) + this.center_x) as u32;
            const y = ((proj_2d[1] * SCALE) + this.center_y) as u32;
            proj_2d_points[i] = [x, y];
        }

        // Draw
        // c3dev.fill_rect(0, 0, this.width, this.height, c3dev.COLOR.BLACK);

        c3dev.start_write();
        // clear prev lines
        for(let i = 0; i < CUBE_LENGTH / 2; i++) {
            this.connect(i, (i + 1) % 4, this.prev_proj_2d_points, c3dev.COLOR.BLACK);
            this.connect(i + 4, (i + 1) % 4 + 4, this.prev_proj_2d_points, c3dev.COLOR.BLACK);
            this.connect(i, i + 4, this.prev_proj_2d_points, c3dev.COLOR.BLACK);
        }
        // draw lines
        for(let i = 0; i < CUBE_LENGTH / 2; i++) {
            this.connect(i, (i + 1) % 4, proj_2d_points, c3dev.COLOR.GREEN);
            this.connect(i + 4, (i + 1) % 4 + 4, proj_2d_points, c3dev.COLOR.GREEN);
            this.connect(i, i + 4, proj_2d_points, c3dev.COLOR.GREEN);
        }
        // draw points and save prev
        for(let i = 0; i < CUBE_LENGTH; i++) {
            const x = proj_2d_points[i][0];
            const y = proj_2d_points[i][1]
            c3dev.draw_pixel(x, y, c3dev.COLOR.RED);
            this.prev_proj_2d_points[i] = [x, y];
        }
        c3dev.end_write();

        this.angle += 0.04;
    }

    connect(i: u32, j: u32, k: u32[][], color: c3dev.COLOR): void {
        const a = k[i];
        const b = k[j];
        c3dev.draw_line(a[0], a[1], b[0], b[1], color);
    }
}

export function init(width: u32, height: u32): void {
    imu = new Imu(width, height);
}

export function tick(): void {
    if(imu != null) {
        imu!.tick();
    }
}

function matrix_multiple(a: f32[][], b: f32[]): f32[] {
    let result: f32[] = [];

    for(let i = 0; i < a.length; i++) {
        let sum: f32 = 0;
        for(let j = 0; j < 3; j++) {
            sum += a[i][j] * b[j];
        }
        result[i] = sum;
    }

    return result;
}
