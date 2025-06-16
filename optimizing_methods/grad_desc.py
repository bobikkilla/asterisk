import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

# x^2 + (y-2)^2

delta_t = 10 ** -4
epsilon = 10 ** -6


def calculateFunction(x, y):
    return x**2 + (y - 2)**2

def calculateGradient(x, y):
    return 2 * x, 2 * y - 4
    
def gradientDescent(x, y):
    global delta_t, epsilon
    
    grad_x, grad_y  = calculateGradient(x, y)
    
    xk, yk = x, y

    xkp = xk - grad_x * delta_t
    ykp = yk - grad_y * delta_t

    while max(calculateGradient(xkp, ykp)) >= epsilon:
        xk, yk = xkp, ykp
        grad_x, grad_y = calculateGradient(xk, yk)
        xkp = xk - grad_x * delta_t
        ykp = yk - grad_y * delta_t

    return xkp, ykp
        
fig = plt.figure(figsize = (14, 8))
ax_3d = fig.add_subplot(111, projection = '3d') 
ax_3d.set_xlabel('x')
ax_3d.set_ylabel('y')
ax_3d.set_zlabel('z')
ax_3d.set_title('F(x, y) = x^2 + (y - 2)^2')

x = np.linspace(-444, 444, 150) 
y = np.linspace(-444, 444, 150) 
x, y = np.meshgrid(x, y)

#z = x**2 + (y - 2)**2
z = calculateFunction(x, y)

ax_3d.plot_wireframe(x, y, z)
#ax_3d.plot_surface(x, y, z)

x0 = int(input('x0 = '))
y0 = int(input('y0 = '))

print(gradientDescent(x0, y0))

x, y = gradientDescent(x0, y0)

ax_3d.scatter(x, y, calculateFunction(x, y), s = 256, color = 'red')

plt.show()
