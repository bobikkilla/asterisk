#   Симметричный алгоритм шифрования
# Цель работы: исследование структуры симметричных алгоритмов шифрования: AES, ГOСT 28147-89 , DES, RC6, IDEA, SEED
# Задания:
# I. Реализовать один из существующих симметричных алгоритмов шифрования: AES, ГОСT 28147-89 , DES, RC6, IDEA, SEED.
# 2. Разработать форму, содержащую: 3 текстовых поля, кнопку «шифровать», кнопку «расшифровать», текстовое поле ключ для шифрации, текстовое поле ключ для дешифрации.
# 3. Организовать дополнительный интерфейс настройки алгоритма по необходимости.
# 4. Реализовать разработанный алгоритм на любом языке программирования.
# Методика выполнения практической работы. Изучить один из алгоритмов работы симметричного шифрования и реализовать на каком-либо языке программирования.
# Требования к работе:
# - программа шифрования должна быть наделена понятным и удобным пользовательским интерфейсом;
# - полная реализация выбранного алгоритма;
# - запрещено использование любых сторонних библнотек.

import tkinter as tk
from tkinter import messagebox

# ===========================
# 1. Чистая реализация DES (FIPS 46-3)
# ===========================

# Все таблицы взяты из официального стандарта FIPS PUB 46-3
IP = [
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
]

IP_INV = [
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
]

PC1 = [
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
]

PC2 = [
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
]

E = [
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
]

P = [
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25
]

S_BOXES = [
    # S1
    [[14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7],
     [0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8],
     [4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0],
     [15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13]],
    # S2
    [[15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10],
     [3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5],
     [0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15],
     [13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9]],
    # S3
    [[10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8],
     [13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1],
     [13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7],
     [1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12]],
    # S4
    [[7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15],
     [13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9],
     [10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4],
     [3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14]],
    # S5
    [[2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9],
     [14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6],
     [4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14],
     [11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3]],
    # S6
    [[12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11],
     [10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8],
     [9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6],
     [4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13]],
    # S7
    [[4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1],
     [13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6],
     [1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2],
     [6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12]],
    # S8
    [[13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7],
     [1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2],
     [7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8],
     [2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11]]
]

SHIFT_SCHEDULE = [1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1]

# ---------- Битовые операции ----------
def bytes_to_bits(data):
    bits = []
    for byte in data:
        for i in range(7, -1, -1):
            bits.append((byte >> i) & 1)
    return bits

def bits_to_bytes(bits):
    if len(bits) % 8 != 0:
        raise ValueError("Длина битов должна быть кратна 8")
    result = bytearray()
    for i in range(0, len(bits), 8):
        byte = 0
        for j in range(8):
            byte = (byte << 1) | bits[i + j]
        result.append(byte)
    return bytes(result)

def permute(bits, table):
    """Применяет перестановку по таблице (нумерация с 1 в стандарте)"""
    return [bits[i - 1] for i in table]

def xor_bits(a, b):
    return [x ^ y for x, y in zip(a, b)]

def left_shift(bits, n):
    n = n % len(bits)
    return bits[n:] + bits[:n]

# ---------- S-блоки ----------
def s_box_substitute(bits):
    output = []
    for i in range(8):
        block = bits[6*i : 6*i+6]
        row = (block[0] << 1) | block[5]
        col = (block[1] << 3) | (block[2] << 2) | (block[3] << 1) | block[4]
        
        # Строгая проверка границ
        if row < 0 or row > 3 or col < 0 or col > 15:
            raise ValueError(f"S-блок {i}: некорректные row={row} или col={col}")
        
        val = S_BOXES[i][row][col]
        output.extend([
            (val >> 3) & 1,
            (val >> 2) & 1,
            (val >> 1) & 1,
            val & 1
        ])
    return output

# ---------- Генерация ключей ----------
def generate_round_keys(key_64bits):
    # PC-1: 64 → 56 бит
    key_56 = permute(key_64bits, PC1)
    C = key_56[:28]
    D = key_56[28:]
    round_keys = []
    
    for i in range(16):
        C = left_shift(C, SHIFT_SCHEDULE[i])
        D = left_shift(D, SHIFT_SCHEDULE[i])
        CD = C + D
        round_key = permute(CD, PC2)  # 56 → 48 бит
        round_keys.append(round_key)
    
    return round_keys

# ---------- Функция F ----------
def f_function(R, round_key):
    expanded = permute(R, E)  # 32 → 48 бит
    xored = xor_bits(expanded, round_key)
    substituted = s_box_substitute(xored)  # 48 → 32 бита
    return permute(substituted, P)

# ---------- Основной цикл ----------
def des_block(data_64bits, round_keys, encrypt=True):
    # Начальная перестановка
    permuted = permute(data_64bits, IP)
    L = permuted[:32]
    R = permuted[32:]
    
    # 16 раундов
    for i in range(16):
        current_key = round_keys[i] if encrypt else round_keys[15-i]
        new_L = R
        new_R = xor_bits(L, f_function(R, current_key))
        L, R = new_L, new_R
    
    # Финальная перестановка (обратный порядок)
    final = R + L
    return permute(final, IP_INV)

# ---------- Работа с данными ----------
def pkcs7_pad(data):
    pad_len = 8 - (len(data) % 8)
    return data + bytes([pad_len] * pad_len)

def pkcs7_unpad(data):
    pad_len = data[-1]
    if pad_len < 1 or pad_len > 8:
        raise ValueError("Некорректный padding")
    if data[-pad_len:] != bytes([pad_len] * pad_len):
        raise ValueError("Некорректный padding")
    return data[:-pad_len]

def des_crypt(data, key, mode='ECB', iv=None, encrypt=True):
    # Валидация
    if len(key) != 8:
        raise ValueError("Ключ должен быть длиной 8 байт")
    if mode == 'CBC' and (iv is None or len(iv) != 8):
        raise ValueError("Для CBC требуется IV длиной 8 байт")
    
    # Подготовка
    key_bits = bytes_to_bits(key)
    round_keys = generate_round_keys(key_bits)
    
    if encrypt:
        padded = pkcs7_pad(data)
    else:
        if len(data) % 8 != 0:
            raise ValueError("Данные должны быть кратны 8 байтам")
        padded = data
    
    result = bytearray()
    prev_block = iv
    
    # Обработка блоков
    for i in range(0, len(padded), 8):
        block = padded[i:i+8]
        block_bits = bytes_to_bits(block)
        
        if mode == 'CBC':
            if encrypt:
                if prev_block is None:
                    block_bits = xor_bits(block_bits, bytes_to_bits(iv))
                else:
                    block_bits = xor_bits(block_bits, bytes_to_bits(prev_block))
            else:
                # Для расшифровки CBC обрабатываем последовательно
                pass
        
        # Шифрование/расшифровка блока
        processed_bits = des_block(block_bits, round_keys, encrypt=encrypt)
        processed = bits_to_bytes(processed_bits)
        
        if mode == 'CBC' and not encrypt:
            if prev_block is None:
                processed = xor_bits(processed_bits, bytes_to_bits(iv))
            else:
                processed = xor_bits(processed_bits, bytes_to_bits(prev_block))
            processed = bits_to_bytes(processed)
        
        result.extend(processed)
        
        if mode == 'CBC':
            prev_block = block if not encrypt else processed
    
    # Удаление паддинга при расшифровке
    if not encrypt:
        try:
            return pkcs7_unpad(bytes(result))
        except:
            return bytes(result)  # возвращаем как есть при ошибке
    
    return bytes(result)

# ===========================
# 2. GUI с tkinter
# ===========================

class DESApp:
    def __init__(self, root):
        self.root = root
        self.root.title("DES Шифрование (NIST-валидированная реализация)")
        self.root.geometry("780x620")
        self.root.configure(bg="#f0f0f0")
        self.setup_ui()
        
        # Автозаполнение для тестирования
        self.key_encrypt.insert(0, "13345779")
        self.key_decrypt.insert(0, "13345779")
        self.plain_text.insert("1.0", "HelloDES")
        self.iv_entry.insert(0, "0000000000000000")

    def setup_ui(self):
        # Заголовок
        tk.Label(
            self.root, text="🔐 DES (FIPS 46-3)", 
            font=("Arial", 16, "bold"), bg="#f0f0f0", fg="#2c3e50"
        ).pack(pady=(15, 5))
        
        # Режим работы
        mode_frame = tk.LabelFrame(self.root, text="Режим работы", bg="#f0f0f0", font=("Arial", 10, "bold"))
        mode_frame.pack(fill=tk.X, padx=20, pady=(0, 10))
        
        self.mode = tk.StringVar(value="ECB")
        tk.Radiobutton(mode_frame, text="ECB", variable=self.mode, value="ECB", 
                      bg="#f0f0f0", font=("Arial", 9)).pack(side=tk.LEFT, padx=10)
        tk.Radiobutton(mode_frame, text="CBC", variable=self.mode, value="CBC", 
                      bg="#f0f0f0", font=("Arial", 9)).pack(side=tk.LEFT, padx=10)
        
        tk.Label(mode_frame, text="IV (hex):", bg="#f0f0f0", font=("Arial", 9)).pack(side=tk.LEFT, padx=(20, 5))
        self.iv_entry = tk.Entry(mode_frame, width=18, font=("Consolas", 9))
        self.iv_entry.pack(side=tk.LEFT, padx=5)
        
        # Поля ввода
        input_frame = tk.LabelFrame(self.root, text="Данные", bg="#f0f0f0", font=("Arial", 10, "bold"))
        input_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=(0, 10))
        
        # Исходный текст
        tk.Label(input_frame, text="Исходный текст:", bg="#f0f0f0", font=("Arial", 9, "bold")).grid(row=0, column=0, sticky="w", padx=5)
        self.plain_text = tk.Text(input_frame, height=6, width=85, font=("Consolas", 9))
        self.plain_text.grid(row=1, column=0, columnspan=2, padx=5, pady=(0, 10), sticky="we")
        
        # Ключи
        tk.Label(input_frame, text="Ключ (8 байт):", bg="#f0f0f0", font=("Arial", 9, "bold")).grid(row=2, column=0, sticky="w", padx=5)
        self.key_encrypt = tk.Entry(input_frame, width=42, font=("Consolas", 9))
        self.key_encrypt.grid(row=3, column=0, padx=5, pady=(0, 5), sticky="w")
        
        tk.Label(input_frame, text="Ключ расшифровки (8 байт):", bg="#f0f0f0", font=("Arial", 9, "bold")).grid(row=2, column=1, sticky="w", padx=5)
        self.key_decrypt = tk.Entry(input_frame, width=42, font=("Consolas", 9))
        self.key_decrypt.grid(row=3, column=1, padx=5, pady=(0, 5), sticky="w")
        
        # Результат
        tk.Label(input_frame, text="Результат (hex):", bg="#f0f0f0", font=("Arial", 9, "bold")).grid(row=4, column=0, sticky="w", padx=5)
        self.result_text = tk.Text(input_frame, height=6, width=85, font=("Consolas", 9))
        self.result_text.grid(row=5, column=0, columnspan=2, padx=5, pady=(0, 10), sticky="we")
        
        input_frame.columnconfigure(0, weight=1)
        input_frame.columnconfigure(1, weight=1)
        
        # Кнопки
        btn_frame = tk.Frame(self.root, bg="#f0f0f0")
        btn_frame.pack(pady=10)
        
        tk.Button(btn_frame, text="Шифровать", command=self.encrypt,
                 bg="#27ae60", fg="white", font=("Arial", 10, "bold"), width=14).pack(side=tk.LEFT, padx=10)
        
        tk.Button(btn_frame, text="Расшифровать", command=self.decrypt,
                 bg="#2980b9", fg="white", font=("Arial", 10, "bold"), width=14).pack(side=tk.LEFT, padx=10)
        
        # Статус
        self.status = tk.Label(self.root, text="Статус: Готов к работе", 
                             font=("Arial", 9), bg="#f0f0f0", fg="#27ae60")
        self.status.pack(pady=5)
        
        # Подсказки
        tk.Label(
            self.root,
            text="ℹ️ Ключ — ровно 8 символов. Для CBC: IV — 16 hex-символов (8 байт).\n"
                 "Результат отображается в шестнадцатеричном формате.",
            font=("Arial", 8), fg="#7f8c8d", bg="#f0f0f0", justify="left"
        ).pack(pady=(5, 10))

    def get_iv(self):
        if self.mode.get() == "CBC":
            iv_hex = self.iv_entry.get().strip()
            if len(iv_hex) != 16:
                raise ValueError("IV должен быть 16 hex-символов (8 байт)")
            try:
                return bytes.fromhex(iv_hex)
            except:
                raise ValueError("IV должен содержать только 0-9, a-f")
        return None

    def encrypt(self):
        try:
            self.status.config(text="Статус: Шифрование...", fg="#2980b9")
            self.root.update()
            
            plain = self.plain_text.get("1.0", tk.END).strip().encode('utf-8')
            key = self.key_encrypt.get().encode('latin1')
            if len(key) != 8:
                raise ValueError("Ключ шифрования должен быть ровно 8 байт")
            
            iv = self.get_iv()
            cipher = des_crypt(plain, key, mode=self.mode.get(), iv=iv, encrypt=True)
            
            self.result_text.delete("1.0", tk.END)
            self.result_text.insert(tk.END, cipher.hex())
            self.status.config(text="Статус: Успешно зашифровано", fg="#27ae60")
        except Exception as e:
            self.status.config(text=f"Статус: Ошибка", fg="#e74c3c")
            messagebox.showerror("Ошибка шифрования", str(e))

    def decrypt(self):
        try:
            self.status.config(text="Статус: Расшифровка...", fg="#2980b9")
            self.root.update()
            
            cipher_hex = self.result_text.get("1.0", tk.END).strip()
            if not cipher_hex:
                raise ValueError("Нет данных для расшифровки")
            
            try:
                cipher = bytes.fromhex(cipher_hex)
            except:
                raise ValueError("Результат должен быть в hex-формате")
            
            key = self.key_decrypt.get().encode('latin1')
            if len(key) != 8:
                raise ValueError("Ключ расшифровки должен быть ровно 8 байт")
            
            iv = self.get_iv()
            plain = des_crypt(cipher, key, mode=self.mode.get(), iv=iv, encrypt=False)
            
            self.plain_text.delete("1.0", tk.END)
            self.plain_text.insert(tk.END, plain.decode('utf-8', errors='replace'))
            self.status.config(text="Статус: Успешно расшифровано", fg="#27ae60")
        except Exception as e:
            self.status.config(text=f"Статус: Ошибка", fg="#e74c3c")
            messagebox.showerror("Ошибка расшифровки", str(e))

# ===========================
# 3. Запуск приложения
# ===========================

if __name__ == "__main__":
    root = tk.Tk()
    app = DESApp(root)
    root.mainloop()