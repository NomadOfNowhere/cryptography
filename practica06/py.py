# Multiplicación en GF(2^8) usando el polinomio irreducible x^8 + x^4 + x^3 + x + 1 (0x11B)
def gf_mult(a, b, mod_poly=0x11B):
    result = 0
    for _ in range(8):
        if b & 1:
            result ^= a  # suma en GF(2)
        high_bit_set = a & 0x80
        a <<= 1
        if high_bit_set:
            a ^= mod_poly
        a &= 0xFF
        b >>= 1
    return result

# Generar tabla de multiplicación para GF(2^8) y guardarla en un archivo de texto
def generate_multiplication_table(mod_poly=0x11B):
    table = []
    with open("gf256_table.txt", "w") as f:
        for a in range(256):
            row = []
            for b in range(256):
                product = gf_mult(a, b, mod_poly)
                row.append(f"{product:02X}")
            table.append(row)
            f.write(' '.join(row) + '\n')
    return table

# Obtener el inverso multiplicativo de un elemento en GF(2^8) usando la tabla
def multiplicative_inverse(a_hex, mult_table):
    a = int(a_hex, 16)
    if a == 0:
        return None  # 0 no tiene inverso
    for b in range(1, 256):
        if int(mult_table[a][b], 16) == 1:
            return f"{b:02X}"
    return None  # No debería pasar en GF(2^8) para a ≠ 0

# Función principal de prueba
def main():
    # Parte 1: Ejemplo de multiplicación
    a = 0x57
    b = 0x83
    result = gf_mult(a, b)
    print(f"{a:02X} * {b:02X} mod m(x) = {result:02X}")

    # Parte 2: Generar tabla de multiplicación
    print("Generando tabla de multiplicación GF(2^8)...")
    table = generate_multiplication_table()
    print("Tabla guardada como 'gf256_table.txt'.")

    # # Parte 3: Inverso multiplicativo
    # a_hex = "57"
    # inv = multiplicative_inverse(a_hex, table)
    # if inv:
    #     print(f"Inverso multiplicativo de 0x{a_hex}: {inv}")
    # else:
    #     print(f"El elemento 0x{a_hex} no tiene inverso.")

if __name__ == "__main__":
    main()
