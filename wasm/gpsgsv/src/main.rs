#![no_std]
#![no_main]

use nmea0183::{Parser, ParseResult};

#[no_mangle]
fn test() {
    let nmea = b"$GPGGA,145659.00,5956.695396,N,03022.454999,E,2,07,0.6,9.0,M,18.0,M,,*62\r\n$GPGGA,,,,,,,,,,,,,,*00\r\n";
    let mut parser = Parser::new();
    for b in &nmea[..] {
        if let Some(result) = parser.parse_from_byte(*b) {
            match result {
                Ok(ParseResult::GGA(Some(gga))) => {
                    // println!("{:#?}", gga);
                }, // Got GGA sentence
                Ok(ParseResult::GGA(None)) => {
                    // println!("Got GGA sentence without valid data, receiver ok but has no solution");
                },
                Ok(_) => {
                    // println!("Some other sentences..");
                },
                Err(e) => {
                    // println!("Got parse error");
                }
            }
        }
    }
}

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}
