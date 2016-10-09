import math
import matplotlib.pyplot as plt
import numpy as np 
from noise import perlin
from mpl_toolkits.mplot3d import Axes3D
noise = perlin.SimplexNoise()
vals = np.zeros([128],float)
rang = np.zeros([128],float)
for i in range(0,128):
	rang[i] = float(i)
x = float(0.0)
for x in range(0,128):
	vals[x] = (math.exp(-1.5*((x) / 50))**2)


print(np.min(vals))
print(np.max(vals))	
plt.plot(vals)
plt.show()


