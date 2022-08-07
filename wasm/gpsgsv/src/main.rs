use nmea::Nmea;

fn main() {
    let mut nmea = Nmea::new();
    let gga = "$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76";
    nmea.parse(gga).unwrap();
    println!("{}", nmea);
}
