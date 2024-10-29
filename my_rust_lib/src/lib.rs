#[no_mangle] // Verhindert Namensmangling
pub extern "C" fn add() -> usize {
    println!("Hello, World!");
    0
}
