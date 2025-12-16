#   Хеш-функции
# Цель работы: исследование структуры алгоритмов хеширования: MDS, MD6, CRC, HAVAL, SHA-2, ГОСТ P 34.11-94.
# Задания:
# 1. Реализовать одну из существующих функций хеширования: MD5, MD6, CRC, HAVAL, SHA-2, ГОСT P 34.11-94.
# 2. Реализовать алгориты работы функции хеширования на любом языке программирования.
# Методика вылолнения практической работы. Рассмотреть одну из функций хеширования. Изучить алгоритм работы выбранной функции и после этого реализовать на каком-либо языке программирования.
# Требования к работе:
# - программа шифрования должна быть наделена понятным и удобным пользовательским интерфейсом;
# - полная реализация выбранной функции хеширования.

import tkinter as tk
from tkinter import messagebox

# support functions for MD5 implementation
def left_rotate(x, c):
    """Циклический сдвиг 32-битного слова влево на c бит"""
    return ((x << c) | (x >> (32 - c))) & 0xFFFFFFFF

def F(X, Y, Z):
    return (X & Y) | ((~X) & Z)

def G(X, Y, Z):
    return (X & Z) | (Y & (~Z))

def H(X, Y, Z):
    return X ^ Y ^ Z

def I(X, Y, Z):
    return Y ^ (X | (~Z))

# T[i] = floor(2^32 * abs(sin(i+1)))
T = [
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
]

# shift table
S = [
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  # round 1
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  # round 2
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  # round 3
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  # round 4
]

def md5_padding(message: bytes) -> bytes:
    """Добавляет padding по стандарту MD5:
       - 1 бит '1'
       - нули
       - 64-битная длина исходного сообщения в битах (little-endian)
    """
    orig_len_in_bits = (8 * len(message)) & 0xFFFFFFFFFFFFFFFF
    message += b'\x80'  # 1000 0000
    while (len(message) * 8) % 512 != 448:
        message += b'\x00'
    # adding length (8 byte, little-endian)
    message += orig_len_in_bits.to_bytes(8, 'little')
    return message

def md5_hash(message: str) -> str:
    data = message.encode('utf-8')
    padded = md5_padding(data)
    
    # Начальные значения
    A, B, C, D = 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476

    # g index sequences from RFC 1321, section 3.4 
    # round 1: i = 0..15 → g = [0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15]
    # round 2: i = 16..31 → g = [1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12]
    # round 3: i = 32..47 → g = [5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2]
    # round 4: i = 48..63 → g = [0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9]
    g_indices = (
        list(range(16)) +  # round 1
        [1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12] +  # round 2
        [5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2] +  # round 3
        [0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9]     # round 4
    )

    # md5 functions right order
    functions = [F] * 16 + [G] * 16 + [H] * 16 + [I] * 16

    for i in range(0, len(padded), 64):
        block = padded[i:i+64]
        X = [int.from_bytes(block[j:j+4], 'little') for j in range(0, 64, 4)]

        a, b, c, d = A, B, C, D

        for idx in range(64):
            f = functions[idx](b, c, d)
            g = g_indices[idx]
            shift = S[idx]
            add_const = T[idx]

            # new value for a
            temp = (a + f + X[g] + add_const) & 0xFFFFFFFF
            temp = left_rotate(temp, shift)
            a = (b + temp) & 0xFFFFFFFF

            # cycle shift: a←d, b←a, c←b, d←c
            a, b, c, d = d, a, b, c

        A = (A + a) & 0xFFFFFFFF
        B = (B + b) & 0xFFFFFFFF
        C = (C + c) & 0xFFFFFFFF
        D = (D + d) & 0xFFFFFFFF

    # result (little-endian for each part)
    return (
        A.to_bytes(4, 'little') +
        B.to_bytes(4, 'little') +
        C.to_bytes(4, 'little') +
        D.to_bytes(4, 'little')
    ).hex()

class MD5App:
    def __init__(self, root):
        self.root = root
        self.root.title("MD5 Хеширование")
        self.root.geometry("700x480")
        self.root.configure(bg="#f0f0f0")
        self.setup_ui()

    def setup_ui(self):
        # title
        tk.Label(
            self.root, text="MD5 (RFC 1321)", 
            font=("Arial", 16, "bold"), bg="#f0f0f0", fg="#2c3e50"
        ).pack(pady=(15, 10))

        # message for hash
        tk.Label(self.root, text="Сообщение для хеширования:", 
                 font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
        self.input_text = tk.Text(self.root, height=6, width=80, font=("Consolas", 10))
        self.input_text.pack(padx=20, pady=(0, 10), fill=tk.BOTH, expand=True)
        self.input_text.insert("1.0", "Wow!")

        # hash
        tk.Label(self.root, text="MD5-хеш (hex):", 
                 font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
        self.hash_text = tk.Text(self.root, height=2, width=80, font=("Consolas", 10))
        self.hash_text.pack(padx=20, pady=(0, 15), fill=tk.X)
        self.hash_text.insert("1.0", "[нажмите «Вычислить хеш»]")
        self.hash_text.config(state=tk.DISABLED)

        # button
        btn_frame = tk.Frame(self.root, bg="#f0f0f0")
        btn_frame.pack(pady=5)
        
        tk.Button(
            btn_frame, text="Вычислить хеш", command=self.compute_hash,
            font=("Arial", 10, "bold"), bg="#27ae60", fg="white", width=16
        ).pack()

        # status
        self.status = tk.Label(
            self.root, text="Статус: введите сообщение", 
            font=("Arial", 9), bg="#f0f0f0", fg="#7f8c8d"
        )
        self.status.pack(pady=(10, 5))

    def compute_hash(self):
        try:
            msg = self.input_text.get("1.0", tk.END).strip()
            if not msg:
                messagebox.showwarning("Внимание", "Введите сообщение.")
                return
            
            self.status.config(text="Вычисляю хеш...", fg="#2980b9")
            self.root.update()
            
            h = md5_hash(msg)
            
            self.hash_text.config(state=tk.NORMAL)
            self.hash_text.delete("1.0", tk.END)
            self.hash_text.insert("1.0", h)
            self.hash_text.config(state=tk.DISABLED)
            
            self.status.config(text="Хеш вычислен", fg="#27ae60")
        except Exception as ex:
            self.status.config(text="Ошибка", fg="#e74c3c")
            messagebox.showerror("Ошибка", str(ex))

if __name__ == "__main__":
    root = tk.Tk()
    app = MD5App(root)
    root.mainloop()