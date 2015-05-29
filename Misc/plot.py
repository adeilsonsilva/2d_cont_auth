import matplotlib.pyplot as plt
import matplotlib.colors as colors
import numpy as np

safes = []
frames = []
frame = 1

change_user = 3425

f = open('results.txt', 'r')
for line in f:
        num = line[-6:]
        num = num.strip()
        frame+=1
        safes.append(num)
        frames.append(frame);
f.close()

plt.plot(frames, safes, '-', drawstyle='steps', linewidth=1.5)
plt.annotate('troca de usuario', xy=(2445, 0.6), xytext=(2445, 0.65),
            arrowprops=dict(facecolor='black', shrink=0.05),
            )
plt.title('Experimento 2')

plt.show()