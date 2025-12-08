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

def xor_encrypt_decrypt(data, key):
    """Универсальная функция для шифрования/расшифровки XOR"""
    key_bytes = key.encode('utf-8')
    data_bytes = data.encode('utf-8')
    result = bytearray()
    
    for i in range(len(data_bytes)):
        key_byte = key_bytes[i % len(key_bytes)]
        result.append(data_bytes[i] ^ key_byte)
    
    return result.decode('utf-8', errors='replace')

def encrypt():
    """Шифрование текста с использованием XOR"""
    text = input_text.get("1.0", tk.END).strip()
    key = key_entry.get().strip()
    
    if not key:
        messagebox.showerror("Ошибка", "Введите ключ шифрования!")
        return
    
    if not text:
        messagebox.showerror("Ошибка", "Введите текст для шифрования!")
        return
    
    try:
        encrypted = xor_encrypt_decrypt(text, key)
        output_text.delete("1.0", tk.END)
        output_text.insert(tk.END, encrypted)
    except Exception as e:
        messagebox.showerror("Ошибка", f"Не удалось зашифровать: {str(e)}")

def decrypt():
    """Расшифровка текста с использованием XOR"""
    encrypted_text = output_text.get("1.0", tk.END).strip()
    key = key_entry.get().strip()
    
    if not key:
        messagebox.showerror("Ошибка", "Введите ключ расшифровки!")
        return
    
    if not encrypted_text:
        messagebox.showerror("Ошибка", "Нет данных для расшифровки!")
        return
    
    try:
        decrypted = xor_encrypt_decrypt(encrypted_text, key)
        input_text.delete("1.0", tk.END)
        input_text.insert(tk.END, decrypted)
    except Exception as e:
        messagebox.showerror("Ошибка", f"Не удалось расшифровать: {str(e)}")

# Создание главного окна
root = tk.Tk()
root.title("XOR Шифрование")
root.geometry("600x500")
root.configure(bg="#f0f0f0")

# Заголовок
title_label = tk.Label(root, text="Шифрование методом XOR", 
                      font=("Arial", 14, "bold"), 
                      bg="#f0f0f0", fg="#333")
title_label.pack(pady=(15, 10))

# Поле для исходного текста
tk.Label(root, text="Исходный текст:", font=("Arial", 10, "bold"), 
         bg="#f0f0f0").pack(anchor="w", padx=20)
input_text = tk.Text(root, height=6, width=70, font=("Consolas", 10), 
                    bg="#ffffff", relief=tk.SOLID, borderwidth=1)
input_text.pack(padx=20, pady=(0, 10), fill=tk.BOTH, expand=True)

# Поле для ключа
tk.Label(root, text="Ключ:", font=("Arial", 10, "bold"), 
         bg="#f0f0f0").pack(anchor="w", padx=20)
key_entry = tk.Entry(root, width=70, font=("Consolas", 10),
                    bg="#ffffff", relief=tk.SOLID, borderwidth=1)
key_entry.pack(padx=20, pady=(0, 10), fill=tk.X)

# Поле для результата
tk.Label(root, text="Результат:", font=("Arial", 10, "bold"), 
         bg="#f0f0f0").pack(anchor="w", padx=20)
output_text = tk.Text(root, height=6, width=70, font=("Consolas", 10), 
                     bg="#ffffff", relief=tk.SOLID, borderwidth=1)
output_text.pack(padx=20, pady=(0, 15), fill=tk.BOTH, expand=True)

# Кнопки управления
button_frame = tk.Frame(root, bg="#f0f0f0")
button_frame.pack(pady=5)

encrypt_btn = tk.Button(button_frame, text="Шифровать", command=encrypt,
                       font=("Arial", 10, "bold"), bg="#4CAF50", fg="white",
                       width=12, height=1, relief=tk.FLAT)
encrypt_btn.pack(side=tk.LEFT, padx=15)

decrypt_btn = tk.Button(button_frame, text="Расшифровать", command=decrypt,
                       font=("Arial", 10, "bold"), bg="#2196F3", fg="white",
                       width=12, height=1, relief=tk.FLAT)
decrypt_btn.pack(side=tk.LEFT, padx=15)

# Информация об алгоритме
info_label = tk.Label(root, text="Алгоритм: Побитовое XOR с циклическим ключом\n",
                     font=("Arial", 8), bg="#f0f0f0", fg="#666")
info_label.pack(pady=(0, 10))

root.mainloop()