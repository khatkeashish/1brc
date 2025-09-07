use std::collections::HashMap;
use std::fs::File;
use std::io::{self, BufRead, BufReader, BufWriter, Write};

#[derive(Debug, Clone, Copy)]
struct CityResult {
    min: f64,
    max: f64,
    counter: u32,
    sum: f64,
}

impl CityResult {
    #[inline]
    fn new_first(v: f64) -> Self {
        CityResult { min: v, max: v, counter: 1, sum: v }
    }
    #[inline]
    fn update(&mut self, v: f64) {
        if v < self.min { self.min = v; }
        if v > self.max { self.max = v; }
        self.counter += 1;
        self.sum += v;
    }
    #[inline]
    fn mean(&self) -> f64 {
        if self.counter == 0 { 0.0 } else { self.sum / (self.counter as f64) }
    }
}

fn process_file(input_path: &str) -> io::Result<HashMap<String, CityResult>> {
    let file = File::open(input_path)?;
    let mut reader = BufReader::with_capacity(1 << 20, file); // 1 MiB

    let mut map: HashMap<String, CityResult> = HashMap::with_capacity(1 << 15);

    // reuse a single buffer for lines
    let mut buf: Vec<u8> = Vec::with_capacity(256);

    loop {
        buf.clear();
        let n = reader.read_until(b'\n', &mut buf)?;
        if n == 0 { break; } // EOF

        // trim trailing \n / \r
        while matches!(buf.last(), Some(b'\n' | b'\r')) {
            buf.pop();
        }
        if buf.is_empty() { continue; }

        // find first ';'
        let mut sep = None;
        for (i, &b) in buf.iter().enumerate() {
            if b == b';' { sep = Some(i); break; }
        }
        let Some(i) = sep else { continue };

        let city_bytes = &buf[..i];
        let val_bytes  = &buf[i + 1..];
        if val_bytes.is_empty() { continue; }

        // parse f64 using std only (UTF-8 conversion + parse)
        let v = match std::str::from_utf8(val_bytes).ok().and_then(|s| s.parse::<f64>().ok()) {
            Some(x) => x,
            None => continue,
        };

        // fast path: update in place if city exists (no alloc)
        if let Ok(city_str) = std::str::from_utf8(city_bytes) {
            if let Some(cr) = map.get_mut(city_str) {
                cr.update(v);
                continue;
            }
            // insert (one allocation for the key)
            map.insert(city_str.to_owned(), CityResult::new_first(v));
        }
    }

    Ok(map)
}

fn dump_map(map: &HashMap<String, CityResult>, output_path: &str) -> io::Result<()> {
    let out = File::create(output_path)?;
    let mut w = BufWriter::with_capacity(1 << 20, out);

    // fixed 8 decimals
    for (city, cr) in map {
        writeln!(w, "{};{:.8};{:.8};{:.8}", city, cr.mean(), cr.min, cr.max)?;
    }
    w.flush()
}

fn main() -> io::Result<()> {
    let input = "test_sample.txt";
    let output = "test_sample_results_calculated.txt";
    let map = process_file(input)?;
    dump_map(&map, output)
}
