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


SECRET_KEY = "secretkey"

def xor_encrypt_decrypt_hex(data: str, key: str) -> str:
    """Шифрует строку → hex, или расшифровывает hex → строку (универсальная)"""
    key_bytes = key.encode('utf-8')
    data_bytes = data.encode('utf-8')
    result = bytearray()
    
    for i in range(len(data_bytes)):
        k = key_bytes[i % len(key_bytes)]
        result.append(data_bytes[i] ^ k)
    
    return result.hex()


def xor_decrypt_hex(hex_data: str, key: str) -> str:
    """Расшифровывает hex → исходный текст"""
    try:
        encrypted_bytes = bytes.fromhex(hex_data)
    except ValueError:
        raise ValueError("Некорректный формат: ожидается hex-строка (0–9, a–f, без пробелов)")
    
    key_bytes = key.encode('utf-8')
    result = bytearray()
    
    for i in range(len(encrypted_bytes)):
        k = key_bytes[i % len(key_bytes)]
        result.append(encrypted_bytes[i] ^ k)
    
    return result.decode('utf-8')


def encrypt():
    text = input_text.get("1.0", tk.END).strip()
    if not text:
        messagebox.showwarning("Внимание", "Введите текст для шифрования.")
        return
    
    try:
        encrypted_hex = xor_encrypt_decrypt_hex(text, SECRET_KEY)
        output_text.delete("1.0", tk.END)
        output_text.insert(tk.END, encrypted_hex)
    except Exception as e:
        messagebox.showerror("Ошибка", f"Шифрование не удалось:\n{e}")


def decrypt():
    hex_text = output_text.get("1.0", tk.END).strip()
    if not hex_text:
        messagebox.showwarning("Внимание", "Нет данных для расшифровки.")
        return
    
    try:
        decrypted = xor_decrypt_hex(hex_text, SECRET_KEY)
        input_text.delete("1.0", tk.END)
        input_text.insert(tk.END, decrypted)
    except ValueError as e:
        messagebox.showerror("Ошибка формата", f"Данные повреждены или не в формате hex:\n{e}")
    except UnicodeDecodeError:
        messagebox.showerror("Ошибка", "Расшифровка невозможна — возможно, данные зашифрованы другим ключом.")
    except Exception as e:
        messagebox.showerror("Ошибка", f"Расшифровка не удалась:\n{e}")


# =================== GUI ===================
root = tk.Tk()
root.title("XOR Шифрование")
root.geometry("620x480")
root.configure(bg="#f5f5f5")

# header
tk.Label(
    root, text="XOR Шифрование", 
    font=("Arial", 16, "bold"), 
    bg="#f5f5f5", fg="#2c3e50"
).pack(pady=(15, 5))

# input text
tk.Label(root, text="Исходный текст:", font=("Arial", 10, "bold"), bg="#f5f5f5").pack(anchor="w", padx=25)
input_text = tk.Text(root, height=7, width=75, font=("Consolas", 10), wrap=tk.WORD)
input_text.pack(padx=25, pady=(0, 12), fill=tk.BOTH, expand=True)

# crypted text
tk.Label(root, text="Зашифрованный текст (hex):", font=("Arial", 10, "bold"), bg="#f5f5f5").pack(anchor="w", padx=25)
output_text = tk.Text(root, height=7, width=75, font=("Consolas", 10), wrap=tk.WORD)
output_text.pack(padx=25, pady=(0, 15), fill=tk.BOTH, expand=True)

# buttons
btn_frame = tk.Frame(root, bg="#f5f5f5")
btn_frame.pack()

tk.Button(
    btn_frame, text="Шифровать", command=encrypt,
    font=("Arial", 10, "bold"), bg="#27ae60", fg="white",
    width=14, height=1, relief=tk.FLAT
).pack(side=tk.LEFT, padx=12)

tk.Button(
    btn_frame, text="Расшифровать", command=decrypt,
    font=("Arial", 10, "bold"), bg="#2980b9", fg="white",
    width=14, height=1, relief=tk.FLAT
).pack(side=tk.LEFT, padx=12)

root.mainloop()