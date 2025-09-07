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
        CityResult {
            min: v,
            max: v,
            counter: 1,
            sum: v,
        }
    }

    #[inline]
    fn update(&mut self, v: f64) {
        if v < self.min {
            self.min = v;
        }
        if v > self.max {
            self.max = v;
        }
        self.counter += 1;
        self.sum += v;
    }

    #[inline]
    fn mean(&self) -> f64 {
        if self.counter == 0 { 0.0 } else { self.sum / (self.counter as f64) }
    }
}

fn process_file(input_path: &str) -> io::Result<HashMap<String, CityResult>> {
    // Large input buffer to reduce syscalls
    let file = File::open(input_path)?;
    let mut reader = BufReader::with_capacity(1 << 20, file); // 1 MiB buffer

    let mut map: HashMap<String, CityResult> = HashMap::with_capacity(1 << 15);

    // Reuse a single String buffer for lines
    let mut line = String::with_capacity(256);

    loop {
        line.clear();
        let n = reader.read_line(&mut line)?;
        if n == 0 {
            break; // EOF
        }

        // Trim trailing newline(s) without allocating
        while line.ends_with('\n') || line.ends_with('\r') {
            line.pop();
        }
        if line.is_empty() {
            continue;
        }

        // Find first ';' quickly on bytes (no split allocations)
        let bytes = line.as_bytes();
        let mut sep_idx: Option<usize> = None;
        for (i, b) in bytes.iter().enumerate() {
            if *b == b';' {
                sep_idx = Some(i);
                break;
            }
        }
        let Some(i) = sep_idx else { continue }; // skip malformed

        // SAFETY: i is a valid UTF-8 boundary because it matched a single-byte ASCII ';'
        let (city_part, value_part) = line.split_at(i);
        // value_part starts with ';'
        let value_str = &value_part[1..];

        // Parse number (fast path). If parse fails, skip line.
        let v = match value_str.parse::<f64>() {
            Ok(x) => x,
            Err(_) => continue,
        };

        // Insert or update using the entry API (one lookup)
        use std::collections::hash_map::Entry;
        match map.entry(city_part.to_string()) {
            Entry::Occupied(mut e) => e.get_mut().update(v),
            Entry::Vacant(e) => {
                e.insert(CityResult::new_first(v));
            }
        }
    }

    Ok(map)
}

fn dump_map(map: &HashMap<String, CityResult>, output_path: &str) -> io::Result<()> {
    // Large output buffer
    let out = File::create(output_path)?;
    let mut w = BufWriter::with_capacity(1 << 20, out);

    // Write in the requested format: city;mean;min;max
    // Use write! with minimal formatting cost; "%.6" equivalent via {:.6}
    for (city, cr) in map {
        writeln!(
            w,
            "{};{:.6};{:.6};{:.6}",
            city,
            cr.mean(),
            cr.min,
            cr.max
        )?;
    }
    w.flush()?;
    Ok(())
}

fn main() -> io::Result<()> {
    let input = "test_sample.txt";
    let output = "test_sample_results_calculated.txt";

    let map = process_file(input)?;
    dump_map(&map, output)?;
    Ok(())
}
