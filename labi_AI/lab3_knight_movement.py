import tkinter as tk
from tkinter import ttk
import time


class KnightTourApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Лабораторная работа 2: Задача о ходе коня")

        # Поля ввода
        self.n_entry = ttk.Entry(root)
        self.m_entry = ttk.Entry(root)
        self.start_x_entry = ttk.Entry(root)
        self.start_y_entry = ttk.Entry(root)

        # Кнопка
        self.find_path_btn = ttk.Button(root, text="Найти путь", command=self.find_path)

        # Canvas для отрисовки
        self.canvas = tk.Canvas(root, width=600, height=600)

        # Размещение элементов
        self.n_entry.grid(row=0, column=1)
        self.m_entry.grid(row=1, column=1)
        self.start_x_entry.grid(row=2, column=1)
        self.start_y_entry.grid(row=3, column=1)

        ttk.Label(root, text="n:").grid(row=0, column=0)
        ttk.Label(root, text="m:").grid(row=1, column=0)
        ttk.Label(root, text="Start X:").grid(row=2, column=0)
        ttk.Label(root, text="Start Y:").grid(row=3, column=0)

        self.find_path_btn.grid(row=4, column=0, columnspan=2)
        self.canvas.grid(row=5, column=0, columnspan=2)

        self.path = []
        self.board_size = None

    def find_path(self):
        try:
            n = int(self.n_entry.get())
            m = int(self.m_entry.get())
            start_x = int(self.start_x_entry.get())
            start_y = int(self.start_y_entry.get())
        except ValueError:
            self.show_error("Введите целые числа!")
            return

        if not (0 <= start_x < n and 0 <= start_y < m):
            self.show_error("Начальная позиция вне доски!")
            return

        if n * m > 64:
            self.show_error("Размер доски не должен превышать 8x8")
            return

        self.board_size = (n, m)
        self.path = self.knight_tour_warnsdorff(n, m, start_x, start_y)

        if self.path:
            self.draw_path()
        else:
            self.canvas.delete("all")
            self.canvas.create_text(300, 300, text="Решения нет", font=("Arial", 20))

    def knight_tour_warnsdorff(self, n, m, start_x, start_y):
        move_x = [2, 1, -1, -2, -2, -1, 1, 2]
        move_y = [1, 2, 2, 1, -1, -2, -2, -1]

        board = [[-1 for _ in range(m)] for _ in range(n)]
        path = []

        def is_valid(x, y):
            return 0 <= x < n and 0 <= y < m and board[x][y] == -1

        def count_moves(x, y):
            count = 0
            for i in range(8):
                nx = x + move_x[i]
                ny = y + move_y[i]
                if is_valid(nx, ny):
                    count += 1
            return count

        x, y = start_x, start_y
        step = 0

        while step < n * m:
            board[x][y] = step
            path.append((x, y))
            moves = []

            for i in range(8):
                nx = x + move_x[i]
                ny = y + move_y[i]
                if is_valid(nx, ny):
                    moves.append((nx, ny, count_moves(nx, ny)))

            moves.sort(key=lambda move: move[2])  # Сортировка по числу следующих ходов
            if moves:
                x, y, _ = moves[0]
            elif step < n * m - 1:
                return []  # Нет решения

            step += 1

        return path

    def draw_path(self):
        self.canvas.delete("all")
        n, m = self.board_size
        cell_size = min(600 // n, 600 // m)

        for i in range(n):
            for j in range(m):
                x1 = j * cell_size
                y1 = i * cell_size
                x2 = x1 + cell_size
                y2 = y1 + cell_size
                color = "white" if (i + j) % 2 else "lightgray"
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color)

        for idx, (x, y) in enumerate(self.path):
            x1 = y * cell_size
            y1 = x * cell_size
            x2 = x1 + cell_size
            y2 = y1 + cell_size
            self.canvas.create_oval(x1 + 5, y1 + 5, x2 - 5, y2 - 5, fill="blue")
            self.canvas.create_text((x1 + x2) // 2, (y1 + y2) // 2,
                                    text=str(idx + 1), font=("Arial", 16))
            self.canvas.update()
            time.sleep(0.2)

    def show_error(self, message):
        self.canvas.delete("all")
        self.canvas.create_text(300, 300, text=message, font=("Arial", 16))


if __name__ == "__main__":
    root = tk.Tk()
    app = KnightTourApp(root)
    root.mainloop()