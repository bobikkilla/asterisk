#   Защита программ от несанкционированного копировании
# Цель работы: исследование защиты программ от несанкционированного копирования.
# Задание:
# 1. Разработать приложение, которос при установке на другой компьютер выдавало бы сообщение о нелегальности использовании,
# 2. Испольтуя ранее реализованную функцию хеширования, создать хеш-код для строки, содержащей индивидуальную информацию о компьютере, в качестве такой информации должна служить информация о компьютере.
# Методика выполнения практической работы, Ознакомиться со способами защиты программ от несанкционированного копирования, способами получения индивидуальной информации о компьютере. Используя результаты практической работы № 4, реализовать зашищенное от копирования приложение.
# Требовании к работе:
# - генерируемый хэш-код должен быть привязан к программному и аппаратному обеспечению;
# - смена аппаратного и программного обеспечения должна приводить к сообщению о нелегальном использовании программы; - запрещено использование любых сторонних библиотек.

import tkinter as tk
from tkinter import ttk, messagebox
import platform
import uuid
import os

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


def get_machine_fingerprint() -> str:
    """Сбор уникальных параметров компьютера."""
    try:
        # hostname
        hostname = platform.node()
        if not hostname:
            hostname = "unknown_host"

        # mac address
        mac_num = uuid.getnode()
        # correction check(not fake and not multicast)
        if (mac_num >> 40) % 2 == 0 and mac_num != 0 and mac_num < (1 << 48):
            mac = ':'.join(("%012X" % mac_num)[i:i+2] for i in range(0,12,2))
        else:
            # if error, using hostname instead
            mac = md5_hash(hostname)[:12].upper()

        os_info = platform.platform(aliased=True)

        # device architecture
        arch = platform.machine()

        # username
        try:
            user = os.getlogin()
        except:
            user = os.environ.get('USERNAME') or os.environ.get('USER') or 'unknown_user'

        # making string for unique print
        fp = f"HOST={hostname};MAC={mac};OS={os_info};ARCH={arch};USER={user}"
        return fp

    except Exception as e:
        return f"ERROR_FP_{str(e)}"

def compute_machine_hash() -> str:
    fp = get_machine_fingerprint()
    return md5_hash(fp)

# app 
class ProtectedApp:

    LICENSED_HASH = "d7f14f0bfe935253e71a6e141577ce42"  # change to validate

    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("Защищённое приложение")
        self.root.geometry("700x400")

        # license check
        self.current_hash = compute_machine_hash()
        self.is_licensed = (self.current_hash == self.LICENSED_HASH)

        # header
        status_color = "green" if self.is_licensed else "red"
        status_text = "Лицензия действительна" if self.is_licensed else "Нарушение лицензии!"
        ttk.Label(
            root, text=status_text,
            font=("Arial", 14, "bold"),
            foreground=status_color
        ).pack(pady=15)

        if not self.is_licensed:
            ttk.Label(
                root,
                text="Использование нелицензионного ПО уголовно наказуемо.\n"
                     "(146 УК РФ, 7.12 КоАП РФ)",
                foreground="red", font=("Arial", 10)
            ).pack(pady=5)

        # fingerprint info
        fp = get_machine_fingerprint()
        ttk.Label(root, text="Текущий уникальный отпечаток компьютера:", font=("Arial", 10, "underline")).pack(pady=(20,5))
        self.fp_text = tk.Text(root, height=4, width=85, font=("Courier", 9))
        self.fp_text.insert("1.0", fp)
        self.fp_text.config(state='disabled')
        self.fp_text.pack(padx=20)

        ttk.Label(root, text="MD5-хеш отпечатка:", font=("Arial", 10, "underline")).pack(pady=(10,5))
        self.hash_text = tk.Entry(root, width=60, font=("Courier", 10))
        self.hash_text.insert(0, self.current_hash)
        self.hash_text.config(state='readonly')
        self.hash_text.pack()

        # if license correct
        if self.is_licensed:
            ttk.Label(root, text="\nДобро пожаловать в защищённое приложение!", font=("Arial", 12)).pack()
            ttk.Button(root, text="Лицензионный функционал", command=self.do_work).pack(pady=20)
        else:
            ttk.Button(root, text="Выйти", command=root.quit).pack(pady=10)

    def do_work(self):
        user = os.getlogin()
        if not user:
            user = "Пользователь"
        output = "Добро пожаловать, " + user + "!"
        messagebox.showinfo(title = "Спасибо за покупку лицензии!", message="Hello world!               ")
        messagebox.Message("output")

if __name__ == "__main__":
    fp = get_machine_fingerprint()
    h = md5_hash(fp)
    print("Unique hash for this pc: ", h)
    root = tk.Tk()
    app = ProtectedApp(root)
    root.mainloop()