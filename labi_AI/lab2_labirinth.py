import tkinter as tk
from tkinter import messagebox
import heapq
import random

# Константы
CELL_SIZE = 20
MAZE_WIDTH = 30   # Увеличили ширину
MAZE_HEIGHT = 20  # Высота
WALL_PROBABILITY = 30  # Вероятность стены при генерации (%)

class MazeApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Лабиринт: Поиск Кратчайшего Пути")

        # Инициализация лабиринта
        self.maze = [[0 for _ in range(MAZE_WIDTH)] for _ in range(MAZE_HEIGHT)]
        self.start = None
        self.end = None
        self.paths = []

        # Текущий инструмент
        self.current_tool = "wall"  # wall / start / end

        # Графический интерфейс
        canvas_width = CELL_SIZE * MAZE_WIDTH
        canvas_height = CELL_SIZE * MAZE_HEIGHT
        self.canvas = tk.Canvas(root, width=canvas_width, height=canvas_height, bg="black")
        self.canvas.pack()

        # Кнопки управления
        self.buttons_frame = tk.Frame(root)
        self.buttons_frame.pack(pady=5)

        tk.Button(self.buttons_frame, text="Стена", command=lambda: self.set_tool("wall")).grid(row=0, column=0)
        tk.Button(self.buttons_frame, text="Вход (S)", command=lambda: self.set_tool("start")).grid(row=0, column=1)
        tk.Button(self.buttons_frame, text="Выход (E)", command=lambda: self.set_tool("end")).grid(row=0, column=2)
        tk.Button(self.buttons_frame, text="Случайный лабиринт", command=self.generate_random_maze).grid(row=0, column=3)
        tk.Button(self.buttons_frame, text="Найти Путь", command=self.find_path).grid(row=0, column=4)
        tk.Button(self.buttons_frame, text="Сбросить", command=self.reset).grid(row=0, column=5)

        # Информационное окно
        self.info_label = tk.Label(root, text="Выберите инструмент и укажите Вход (S) и Выход (E)")
        self.info_label.pack()

        # События мыши
        self.canvas.bind("<Button-1>", self.on_mouse_click)

        # Отрисовка начального лабиринта
        self.draw_maze()

    def set_tool(self, tool):
        self.current_tool = tool
        tools = {"wall": "Редактирование стен", "start": "Выбор входа", "end": "Выбор выхода"}
        self.info_label.config(text=f"Инструмент: {tools[tool]}")

    def draw_maze(self):
        self.canvas.delete("all")
        for r in range(MAZE_HEIGHT):
            for c in range(MAZE_WIDTH):
                x1, y1 = c * CELL_SIZE, r * CELL_SIZE
                x2, y2 = x1 + CELL_SIZE, y1 + CELL_SIZE
                color = "white" if self.maze[r][c] == 1 else "black"
                self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="gray")

        if self.start:
            self.draw_cell(self.start, "blue", "S")
        if self.end:
            self.draw_cell(self.end, "red", "E")

        for path in self.paths:
            self.draw_path(path)

    def draw_cell(self, cell, color, label=""):
        r, c = cell
        x1, y1 = c * CELL_SIZE, r * CELL_SIZE
        x2, y2 = x1 + CELL_SIZE, y1 + CELL_SIZE
        self.canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="gray")
        self.canvas.create_text((x1 + x2) // 2, (y1 + y2) // 2, text=label, fill="white", font=("Arial", 9))

    def on_mouse_click(self, event):
        col = event.x // CELL_SIZE
        row = event.y // CELL_SIZE

        if 0 <= row < MAZE_HEIGHT and 0 <= col < MAZE_WIDTH:
            if self.current_tool == "wall":
                self.maze[row][col] = 1 - self.maze[row][col]
            elif self.current_tool == "start":
                self.start = (row, col)
            elif self.current_tool == "end":
                self.end = (row, col)
            self.draw_maze()

    def generate_random_maze(self):
        for r in range(MAZE_HEIGHT):
            for c in range(MAZE_WIDTH):
                self.maze[r][c] = 1 if random.randint(0, 100) < WALL_PROBABILITY else 0
        self.start = None
        self.end = None
        self.paths = []
        self.draw_maze()
        self.info_label.config(text="Случайный лабиринт создан.")

    def reset(self):
        self.maze = [[0 for _ in range(MAZE_WIDTH)] for _ in range(MAZE_HEIGHT)]
        self.start = None
        self.end = None
        self.paths = []
        self.draw_maze()
        self.info_label.config(text="Лабиринт сброшен.")

    def find_path(self):
        if not self.start:
            messagebox.showwarning("Ошибка", "Укажите вход (S)!")
            return
        if not self.end:
            messagebox.showwarning("Ошибка", "Укажите выход (E)!")
            return

        # A* алгоритм
        start = self.start
        end = self.end
        queue = [(0, start)]
        came_from = {start: None}
        cost_so_far = {start: 0}

        while queue:
            _, current = heapq.heappop(queue)

            if current == end:
                break

            r, c = current
            neighbors = [
                (r - 1, c), (r + 1, c),
                (r, c - 1), (r, c + 1)
            ]

            for nr, nc in neighbors:
                if 0 <= nr < MAZE_HEIGHT and 0 <= nc < MAZE_WIDTH and self.maze[nr][nc] == 0:
                    new_cost = cost_so_far[current] + 1
                    neighbor = (nr, nc)
                    if neighbor not in cost_so_far or new_cost < cost_so_far[neighbor]:
                        cost_so_far[neighbor] = new_cost
                        priority = new_cost + abs(nr - end[0]) + abs(nc - end[1])
                        heapq.heappush(queue, (priority, neighbor))
                        came_from[neighbor] = current

        # Восстановление пути
        path = []
        current = end
        try:
            while current != start:
                path.append(current)
                current = came_from[current]
            path.append(start)
            path.reverse()
            self.paths.append(path)
            self.draw_path(path)
            self.info_label.config(text=f"Путь найден! Длина: {len(path)} клеток.")
        except KeyError:
            messagebox.showinfo("Нет пути", "Между входом и выходом нет проходимого пути!")

    def draw_path(self, path):
        for cell in path:
            r, c = cell
            x1, y1 = c * CELL_SIZE, r * CELL_SIZE
            x2, y2 = x1 + CELL_SIZE, y1 + CELL_SIZE
            self.canvas.create_rectangle(x1 + 2, y1 + 2, x2 - 2, y2 - 2, fill="green", outline="")

if __name__ == "__main__":
    root = tk.Tk()
    app = MazeApp(root)
    root.mainloop()