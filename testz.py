import math
from matplotlib import pyplot as plt

X_arr = []
Z_arr = []
for i in range(180):
    val = 2 * i / (180 / math.pi)
    X_pos = math.sin(val) * 200 + 550
    Z_pos = math.cos(val) * 500 + 2000

    X_arr.append(X_pos)
    Z_arr.append(Z_pos)

    command = "X " + str(X_pos) + " Y 800 Z " + str(Z_pos) + " CP 520 CY 155\n"

plt.plot(X_arr, Z_arr)
plt.show()
