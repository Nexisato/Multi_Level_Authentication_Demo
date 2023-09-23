use rug::{Integer, rand};
use std::io::{self, BufRead};
use std::fs::File;
use std::path::Path;
use sha2::{Digest, Sha256};
use num_primes::Generator;

fn generate_integers() -> (Integer, Integer) {
    let p = Integer::from_str_radix(&Generator::safe_prime(512).to_string(), 10).unwrap();
    let q = Integer::from_str_radix(&Generator::safe_prime(512).to_string(), 10).unwrap();
    let n = p * q;

    println!("Computed rigid integer: {n}");
    let mut rand = rand::RandState::new();
    let g = n.clone().random_below(&mut rand);
    println!("Computed base value: {g}");
    (n, g)
}

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
    let file = File::open(filename)?;
    Ok(io::BufReader::new(file).lines())
}

fn hash_message(message: &str) -> String {
    let mut hasher = Sha256::new();
    hasher.update(message);
    format!("{:x}", hasher.finalize())
}

fn accumulate_elements(g: &Integer, n: &Integer) -> (Vec<Integer>, Integer) {
    let mut prime_elements: Vec<Integer> = Vec::new();
    if let Ok(lines) = read_lines("emails.txt") {
        for email in lines.flatten() {
            let hash = Integer::from_str_radix(&hash_message(&email.to_string()), 16).unwrap();
            prime_elements.push(hash.next_prime());
        }
    }
    let mut product = Integer::from(1);
    for element in &prime_elements {
        product *= element;
    }
    (prime_elements, g.clone().secure_pow_mod(&product, n))
}

fn main() {
    let (n, g) = generate_integers();
    // let (_prime_elements, accumulated_value) = accumulate_elements(&g, &n);
    // println!("Accumulated value: {accumulated_value}");
}
