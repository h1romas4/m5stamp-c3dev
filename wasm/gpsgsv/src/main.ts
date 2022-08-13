import * as c3dev from "./c3dev";

let gpsView: GpsView;

const MAX_SATELLITE: u32 = 12;

class Gvs {
    elevation: u32;
    azimuth: u32;
    snr: u32;
    handle: bool;
    lastUpdate: i64;
}

class GpsView {
    private satellites: Set<u32>;
    private gsv: Map<u32, Gvs>;

    constructor(
        private cx: u32,
        private cy: u32,
        private cr: u32
    ) {
        this.satellites = new Set<u32>();
        this.gsv = new Map<u32, Gvs>();
        for (let id: u32 = 0; id < MAX_SATELLITE; id++) {
            this.gsv.set(id, {
                elevation: 0,
                azimuth: 0,
                snr: 0,
                handle: false,
                lastUpdate: 0
            })
        }

        this.init();
    }

    public tick(): void {
    }

    public setSatellites(satellites: Int8Array): void {
        this.satellites.clear();
        for(let i: u32 = 0; i < MAX_SATELLITE; i++) {
            this.satellites.add(satellites[i]);
        }
    }

    public setGvs(id: u32, elevation: u32, azimuth: u32, snr: u32): void {
        this.gsv.set(id, {
            elevation: elevation,
            azimuth: azimuth,
            snr: snr,
            handle: this.satellites.has(id) ? true: false,
            lastUpdate: c3dev.now(),
        });
    }

    private init(): void {
    }
}

export function init(): void {
    // ToDo: Workaround: Initialize Wasm3 Stack
    // Without this line, the Wasm3 stack will not work properly.
    // For example, the argument of the clock() function is the destroyed value.
    memory.grow(1);
    // Test env.seed
    seed();
}

export function gpsgsv(x: u32, y: u32, r: u32): void {
    gpsView = new GpsView(x, y, r);
}

export function tick(): void {
    gpsView.tick();
}

export function createSatellitesArray(): Int8Array {
    return new Int8Array(MAX_SATELLITE);
}

export function setSatellites(satellites: Int8Array) : void {
    gpsView.setSatellites(satellites);
}

export function setGvs(id: u32, elevation: u32, azimuth: u32, snr: u32): void {
    gpsView.setGvs(id, elevation, azimuth, snr);
}
