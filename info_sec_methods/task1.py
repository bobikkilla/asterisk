#   Алгоритм шифрования
# Цель работы: проанализировать существующие простейшие алгоритмы шифрования.
# Задания:
# 1. Разработать собственный алгоритм шифрования.
# 2. Разработать форму, содержащую: 3 текстовых поля, кнопку «Шифровать», кнопку «Расшифровать».
# 3. Реализовать разработанный алгоритм на любом языке программирования.
# Методика выполнения практической работы. Ознакомиться с существующими простыми алгоритмами шифрования. В результате изучения алгоритмов и анализа литературы сформировать принципы работы алгоритмов шифрования.
# Требования к работе:
# - программа шифрования должна быть наделена понятным и удобным пользовательским интерфейсом;
# - алгоритм шифрования в обязательном порядке должен использовать математические и/или побитовые функции;
# - разработанный алгоритм должен приводить к полной утрате всех статистических закономерностей исходного сообщения.

import tkinter as tk
from tkinter import messagebox

def xor_encrypt(data: str, key: str) -> str:
    """Шифрует строку → возвращает hex-строку"""
    if not key:
        raise ValueError("Ключ не может быть пустым")
    
    key_bytes = key.encode('utf-8')
    data_bytes = data.encode('utf-8')
    encrypted_bytes = bytearray()
    
    for i in range(len(data_bytes)):
        k = key_bytes[i % len(key_bytes)]
        encrypted_bytes.append(data_bytes[i] ^ k)
    
    return encrypted_bytes.hex()

def xor_decrypt(hex_data: str, key: str) -> str:
    """Расшифровывает hex-строку → возвращает исходный текст"""
    if not key:
        raise ValueError("Ключ не может быть пустым")
    
    try:
        encrypted_bytes = bytes.fromhex(hex_data)
    except ValueError as e:
        raise ValueError("Некорректный формат: ожидается hex-строка (0-9, a-f)")
    
    key_bytes = key.encode('utf-8')
    decrypted_bytes = bytearray()
    
    for i in range(len(encrypted_bytes)):
        k = key_bytes[i % len(key_bytes)]
        decrypted_bytes.append(encrypted_bytes[i] ^ k)
    
    return decrypted_bytes.decode('utf-8')

# GUI-обработчики
def encrypt():
    text = input_text.get("1.0", tk.END).strip()
    key = key_entry.get().strip()
    
    if not key:
        messagebox.showerror("Ошибка", "Введите ключ шифрования!")
        return
    if not text:
        messagebox.showerror("Ошибка", "Введите текст для шифрования!")
        return
    
    try:
        encrypted_hex = xor_encrypt(text, key)
        output_text.delete("1.0", tk.END)
        output_text.insert(tk.END, encrypted_hex)
    except Exception as e:
        messagebox.showerror("Ошибка", f"Шифрование не удалось:\n{e}")

def decrypt():
    hex_text = output_text.get("1.0", tk.END).strip()
    key = key_entry.get().strip()
    
    if not key:
        messagebox.showerror("Ошибка", "Введите ключ расшифровки!")
        return
    if not hex_text:
        messagebox.showerror("Ошибка", "Нет данных для расшифровки!")
        return
    
    try:
        decrypted = xor_decrypt(hex_text, key)
        input_text.delete("1.0", tk.END)
        input_text.insert(tk.END, decrypted)
    except ValueError as e:
        messagebox.showerror("Ошибка формата", f"Неверные данные:\n{e}")
    except UnicodeDecodeError:
        messagebox.showerror("Ошибка", "Расшифровка прошла, но текст повреждён.\nВозможно, неверный ключ.")
    except Exception as e:
        messagebox.showerror("Ошибка", f"Расшифровка не удалась:\n{e}")

root = tk.Tk()
root.title("XOR Шифрование (hex-режим)")
root.geometry("600x500")
root.configure(bg="#f0f0f0")

tk.Label(root, text="Шифрование XOR → безопасный hex-вывод", 
         font=("Arial", 14, "bold"), bg="#f0f0f0").pack(pady=(15, 10))

tk.Label(root, text="Исходный текст:", font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
input_text = tk.Text(root, height=6, width=70, font=("Consolas", 10))
input_text.pack(padx=20, pady=(0, 10), fill=tk.BOTH, expand=True)

tk.Label(root, text="Ключ:", font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
key_entry = tk.Entry(root, width=70, font=("Consolas", 10))
key_entry.pack(padx=20, pady=(0, 10), fill=tk.X)

tk.Label(root, text="Зашифровано (hex):", font=("Arial", 10, "bold"), bg="#f0f0f0").pack(anchor="w", padx=20)
output_text = tk.Text(root, height=6, width=70, font=("Consolas", 10))
output_text.pack(padx=20, pady=(0, 15), fill=tk.BOTH, expand=True)

button_frame = tk.Frame(root, bg="#f0f0f0")
button_frame.pack(pady=5)

tk.Button(button_frame, text="Шифровать", command=encrypt,
          font=("Arial", 10, "bold"), bg="#4CAF50", fg="white", width=12).pack(side=tk.LEFT, padx=15)
tk.Button(button_frame, text="Расшифровать", command=decrypt,
          font=("Arial", 10, "bold"), bg="#2196F3", fg="white", width=12).pack(side=tk.LEFT, padx=15)

root.mainloop()