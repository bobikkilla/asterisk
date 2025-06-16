# Разработать программную модель системы автоматического управления высотой полёта самолёта, используя релейный 
# закон управления. Задача заключается в том, чтобы самолёт, начав движение с некоторой высоты 
# u(0) = uk, приближался к заданной высоте h и удерживался на ней. Для моделирования использовать метод 
# численного интегрирования с фиксированным шагом Δt. 
# Построить график изменения высоты от времени и проанализировать поведение системы.

from matplotlib import pyplot as plt
import math
import time

flight_time = 50
delta_t = 0.1
h = 1
uk = 7 

def func(u, h):
    return 1 if u <= h else -1

N = int(flight_time // delta_t) + 1
t_points = [i * delta_t for i in range(N)]
u_points = [uk]

plt.figure(figsize=(15, 10))
plt.axhline(y = h, color = 'g', linestyle = '--')
plt.title("Поддержание самолетом высоты h")
plt.xlabel('t')
plt.ylabel('u(t)')
plt.legend(c)
plt.grid(True)

for i in range(1, N):
    u_points.append(u_points[i - 1] + func(u_points[i - 1], h) * delta_t)

plt.plot(t_points, u_points)
plt.show()

