import matplotlib.pyplot as plt
from matplotlib.widgets import TextBox, Button
import numpy as np

def calcEffectiveSpeed(boat_speed, current_speed, angle_deg):
    """Вычисляет эффективную скорость лодки с учётом течения."""
    angle_rad = np.radians(angle_deg)
    current_x = current_speed * np.cos(angle_rad)
    current_y = current_speed * np.sin(angle_rad)
    return boat_speed + current_x, boat_speed + current_y

def timeSand(x, x1, y1, sand_speed):
    return np.sqrt((x - x1)**2 + y1**2) / sand_speed

def timeWater(x, x2, y2, boat_speed, current_speed, angle_deg):
    v_eff_x, v_eff_y = calcEffectiveSpeed(boat_speed, current_speed, angle_deg)
    distance = np.sqrt((x2 - x)**2 + y2**2)
    # Учитываем направление течения через эффективную скорость
    if v_eff_x == 0 and v_eff_y == 0:
        return float('inf')
    return distance / np.hypot(v_eff_x, v_eff_y)

def derivativeTime(x, x1, y1, x2, y2, sand_speed, boat_speed, current_speed, angle_deg):
    """Производная времени по координате x (точка входа в воду)."""
    dx = 1e-5
    t1 = timeSand(x + dx, x1, y1, sand_speed) + timeWater(x + dx, x2, y2, boat_speed, current_speed, angle_deg)
    t2 = timeSand(x - dx, x1, y1, sand_speed) + timeWater(x - dx, x2, y2, boat_speed, current_speed, angle_deg)
    return (t1 - t2) / (2 * dx)

def solveOptimalX(x1, y1, x2, y2, sand_speed, boat_speed, current_speed, angle_deg):
    """Метод Ньютона для нахождения оптимальной точки входа в воду."""
    x = (x1 + x2) / 2  # Начальное приближение
    for _ in range(100):
        f = derivativeTime(x, x1, y1, x2, y2, sand_speed, boat_speed, current_speed, angle_deg)
        df = (derivativeTime(x + 1e-5, x1, y1, x2, y2, sand_speed, boat_speed, current_speed, angle_deg) - f) / 1e-5
        if abs(f) < 1e-5:
            break
        x -= f / df
    return x

def drawSolution(ax, x1, y1, x2, y2, sand_speed, boat_speed, current_speed, angle_deg):
    ax.clear()
    ax.axhline(y=0, color='b', linestyle='--')  # Граница воды и песка
    
    # Стрелка течения
    ax.arrow(x1, y1, 5 * np.cos(np.radians(angle_deg)), 5 * np.sin(np.radians(angle_deg)),
             head_width=0.5, length_includes_head=True, color='c', label='Течение')
    
    if y1 * y2 > 0:  # Точки в одной среде
        ax.plot([x1, x2], [y1, y2], "ro-")
    else:
        x_opt = solveOptimalX(x1, y1, x2, y2, sand_speed, boat_speed, current_speed, angle_deg)
        ax.plot([x_opt, x1], [0, y1], 'yo-')  # Путь на песке
        ax.plot([x2, x_opt], [y2, 0], 'bo-')  # Путь в воде
        ax.text(x_opt, 0, f'(x={x_opt:.2f})', fontsize=10, verticalalignment='bottom', horizontalalignment='right')
    
    ax.text(x1, y1, f'(x={x1}, y={y1})', fontsize=10, verticalalignment='bottom', horizontalalignment='right')
    ax.text(x2, y2, f'(x={x2}, y={y2})', fontsize=10, verticalalignment='bottom', horizontalalignment='right')
    ax.set_xlim(min(x1, x2) - 5, max(x1, x2) + 5)
    ax.set_ylim(min(y1, y2) - 5, max(y1, y2) + 5)
    ax.legend(['Граница воды/песка', 'Течение'], loc='upper right')

# GUI
fig, ax = plt.subplots(figsize=(12, 8))
plt.subplots_adjust(bottom=0.35)

# Поля ввода
params = {
    'x1': 10, 'y1': 10, 'x2': 40, 'y2': -5,
    'sand_speed': 5, 'boat_speed': 3, 'current_speed': 2, 'angle_deg': 0
}

ax_boxes = {
    name: plt.axes([0.2 + i*0.15, 0.25, 0.1, 0.04]) 
    for i, name in enumerate(params)
}
text_boxes = {name: TextBox(ax_boxes[name], name, initial=str(val)) for name, val in params.items()}

button_ax = plt.axes([0.45, 0.15, 0.1, 0.04])
button = Button(button_ax, 'Обновить')

def update(event):
    try:
        new_params = {name: float(tb.text) for name, tb in text_boxes.items()}
        drawSolution(ax, **new_params)
        fig.canvas.draw_idle()
    except ValueError:
        print("Ошибка: Введите числовые значения")

button.on_clicked(update)
plt.show()