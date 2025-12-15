#   Асимметричный алгоритм шифрования
# Цель работы: исследование структуры асимметричных алгоритмов шифрования: RSA, DSA, схема Эль-Гамаля, ГОСТ P 34.10- 2001.
# Задания:
# 1. Реализовать один из существующих асимметричных алгоритмов шифрования: RSA, DSA, схема Эль-Гамаля, ГОСТ P 34.10- 2001.
# 2. Реализовать разработанный алгоритм на любом языке программирования.
# Методика выполнения практической работы. 
#   Выбрать один из алгоритмов симметричного шифрования, изучить алгоритм его работы и реализовать на каком-либо языке программирования.
# Требования к работе:
# - программа шифрования должна быть наделена понятным и удобным пользовательским интерфейсом;
# - полная реализация выбранного алгоритма;
# - запрещено использование любых сторонних библиотек.

import tkinter as tk
from tkinter import messagebox
import random
import base64

def gcd(a: int, b: int) -> int:
    while b:
        a, b = b, a % b
    return a

def extended_gcd(a: int, b: int) -> tuple[int, int, int]:
    if a == 0:
        return b, 0, 1
    g, x1, y1 = extended_gcd(b % a, a)
    return g, y1 - (b // a) * x1, x1

def modinv(a: int, m: int) -> int:
    g, x, _ = extended_gcd(a, m)
    if g != 1:
        raise ValueError("Обратный элемент не существует")
    return x % m

def is_prime(n: int, k: int = 3) -> bool:
    if n < 2:
        return False
    for p in [2, 3, 5, 7, 11, 13, 17, 19, 23, 29]:
        if n % p == 0:
            return n == p
    s = 0
    d = n - 1
    while d % 2 == 0:
        d //= 2
        s += 1
    for _ in range(k):
        a = random.randrange(2, n - 1)
        x = pow(a, d, n)
        if x == 1 or x == n - 1:
            continue
        for __ in range(s - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    return True

def generate_prime(bits: int) -> int:
    while True:
        candidate = random.getrandbits(bits)
        candidate |= (1 << bits - 1) | 1
        if is_prime(candidate):
            return candidate

def rsa_generate_keys(bits: int = 256) -> tuple[tuple[int, int], tuple[int, int]]:
    p = generate_prime(bits)
    q = generate_prime(bits)
    while p == q:
        q = generate_prime(bits)
    n = p * q
    phi = (p - 1) * (q - 1)
    e = 65537
    if gcd(e, phi) != 1:
        e = 3
        while gcd(e, phi) != 1:
            e += 2
    d = modinv(e, phi)
    return (e, n), (d, n)

def rsa_encrypt_bytes(data: bytes, e: int, n: int) -> bytes:
    max_block_len = (n.bit_length() - 1) // 8
    if max_block_len == 0:
        raise ValueError("Ключ слишком мал для шифрования")
    cipher_blocks = []
    for i in range(0, len(data), max_block_len):
        block = data[i:i + max_block_len]
        m = int.from_bytes(block, 'big')
        c = pow(m, e, n)
        c_bytes = c.to_bytes((n.bit_length() + 7) // 8, 'big')
        cipher_blocks.append(c_bytes)
    return b''.join(cipher_blocks)

def rsa_decrypt_bytes(cipher: bytes, d: int, n: int) -> bytes:
    block_len = (n.bit_length() + 7) // 8
    if len(cipher) % block_len != 0:
        raise ValueError("Некорректная длина")
    plain_blocks = []
    for i in range(0, len(cipher), block_len):
        c_bytes = cipher[i:i + block_len]
        c = int.from_bytes(c_bytes, 'big')
        m = pow(c, d, n)
        m_bytes = m.to_bytes(max(1, (m.bit_length() + 7) // 8), 'big')
        plain_blocks.append(m_bytes)
    return b''.join(plain_blocks)

class RSAApp:
    def __init__(self, root):
        self.root = root
        self.root.title("RSA Шифрование (асимметричный алгоритм)")
        self.root.geometry("700x580")
        self.root.configure(bg="#f0f0f0")
        self.public_key = None   # (e, n)
        self.private_key = None  # (d, n)
        self.setup_ui()

    def setup_ui(self):
        # Заголовок
        tk.Label(
            self.root, text="RSA", 
            font=("Arial", 16, "bold"), bg="#f0f0f0", fg="#2c3e50"
        ).pack(pady=(15, 5))

        # Поля ключей (как в DES — но два: публичный и приватный)
        tk.Label(self.root, text="Публичный ключ (e, n):", 
                 font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
        self.pub_key_entry = tk.Entry(self.root, width=85, font=("Consolas", 9))
        self.pub_key_entry.pack(padx=20, pady=(0, 5), fill=tk.X)
        self.pub_key_entry.insert(0, "[не сгенерировано]")
        self.pub_key_entry.config(state='readonly')

        tk.Label(self.root, text="Приватный ключ (d, n):", 
                 font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
        self.priv_key_entry = tk.Entry(self.root, width=85, font=("Consolas", 9))
        self.priv_key_entry.pack(padx=20, pady=(0, 10), fill=tk.X)
        self.priv_key_entry.insert(0, "[не сгенерировано]")
        self.priv_key_entry.config(state='readonly')

        # Кнопка генерации
        tk.Button(
            self.root, text="Сгенерировать ключи", command=self.generate_keys,
            bg="#8e44ad", fg="white", font=("Arial", 10, "bold")
        ).pack(pady=(0, 10))

        # === ИСХОДНЫЙ ТЕКСТ ===
        tk.Label(self.root, text="Исходный текст:", 
                 font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
        self.plain_text = tk.Text(self.root, height=6, width=80, font=("Consolas", 10))
        self.plain_text.pack(padx=20, pady=(0, 10), fill=tk.BOTH, expand=True)
        self.plain_text.insert("1.0", "Secret message!")

        # === ЗАШИФРОВАННЫЙ ТЕКСТ ===
        tk.Label(self.root, text="Зашифрованный текст (base64):", 
                 font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
        self.cipher_text = tk.Text(self.root, height=6, width=80, font=("Consolas", 10))
        self.cipher_text.pack(padx=20, pady=(0, 15), fill=tk.BOTH, expand=True)

        btn_frame = tk.Frame(self.root, bg="#f0f0f0")
        btn_frame.pack(pady=5)

        tk.Button(
            btn_frame, text="Зашифровать", command=self.encrypt,
            font=("Arial", 10, "bold"), bg="#27ae60", fg="white", width=12
        ).pack(side=tk.LEFT, padx=15)

        tk.Button(
            btn_frame, text="Расшифровать", command=self.decrypt,
            font=("Arial", 10, "bold"), bg="#2980b9", fg="white", width=12
        ).pack(side=tk.LEFT, padx=15)

        # Статус-бар
        self.status = tk.Label(
            self.root, text="Статус: сгенерируйте ключи", 
            font=("Arial", 9), bg="#f0f0f0", fg="#7f8c8d"
        )
        self.status.pack(pady=(5, 10))

    def set_entry_readonly(self, entry, text):
        entry.config(state='normal')
        entry.delete(0, tk.END)
        entry.insert(0, text)
        entry.config(state='readonly')

    def generate_keys(self):
        try:
            self.status.config(text="Генерация ключей...", fg="#2980b9")
            self.root.update()
            
            pub, priv = rsa_generate_keys(bits=256)
            self.public_key = pub
            self.private_key = priv
            
            e, n = pub
            d, _ = priv
            
            self.set_entry_readonly(self.pub_key_entry, f"e={e}, n={n}")
            self.set_entry_readonly(self.priv_key_entry, f"d={d}, n={n}")
            self.status.config(text="Ключи сгенерированы", fg="#27ae60")
        except Exception as ex:
            self.status.config(text="Ошибка генерации", fg="#e74c3c")
            messagebox.showerror("Ошибка", str(ex))

    def encrypt(self):
        if not self.public_key:
            messagebox.showwarning("Ошибка", "Сначала сгенерируйте ключи!")
            return
        try:
            plain = self.plain_text.get("1.0", tk.END).strip()
            if not plain:
                messagebox.showwarning("Внимание", "Введите текст для шифрования.")
                return
            
            self.status.config(text="Шифрую...", fg="#2980b9")
            self.root.update()
            
            e, n = self.public_key
            data = plain.encode('utf-8')
            cipher_bytes = rsa_encrypt_bytes(data, e, n)
            cipher_b64 = base64.b64encode(cipher_bytes).decode('ascii')
            
            self.cipher_text.delete("1.0", tk.END)
            self.cipher_text.insert("1.0", cipher_b64)
            self.status.config(text="Успешно зашифровано", fg="#27ae60")
        except Exception as ex:
            self.status.config(text="Ошибка шифрования", fg="#e74c3c")
            messagebox.showerror("Ошибка", str(ex))

    def decrypt(self):
        if not self.private_key:
            messagebox.showwarning("Ошибка", "Приватный ключ отсутствует!")
            return
        try:
            cipher_b64 = self.cipher_text.get("1.0", tk.END).strip()
            if not cipher_b64:
                messagebox.showwarning("Внимание", "Введите зашифрованный текст.")
                return
            
            self.status.config(text="Расшифровываю...", fg="#2980b9")
            self.root.update()
            
            cipher_bytes = base64.b64decode(cipher_b64)
            d, n = self.private_key
            plain_bytes = rsa_decrypt_bytes(cipher_bytes, d, n)
            plain = plain_bytes.decode('utf-8')
            
            self.plain_text.delete("1.0", tk.END)
            self.plain_text.insert("1.0", plain)
            self.status.config(text="Успешно расшифровано", fg="#27ae60")
        except Exception as ex:
            self.status.config(text="Ошибка расшифровки", fg="#e74c3c")
            messagebox.showerror("Ошибка", str(ex))

if __name__ == "__main__":
    root = tk.Tk()
    app = RSAApp(root)
    root.mainloop()