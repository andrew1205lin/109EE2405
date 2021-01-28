import matplotlib.pyplot as plt

import numpy as np

import serial

import time

serdev = '/dev/ttyACM0'

t = np.arange(0,10,0.1) # time vector; create 100 samples between 0 and 10.0 sec.

s = serial.Serial(serdev,baudrate=115200)

x = np.arange(0,10,0.1)
y = np.arange(0,10,0.1)
z = np.arange(0,10,0.1)
tilt = np.arange(0,10,0.1)

for i in range(100):
     line=s.readline()
     
     x[i]=line.split()[0]
     y[i]=line.split()[1]
     z[i]=line.split()[2]
     tilt[i]=line.split()[3]

     

fig,ax = plt.subplots(2, 1)
for i in range(100):
    ax[1].plot([t[i],t[i]],[0,tilt[i]],color="orange", linewidth=2.5, linestyle="-")
    ax[1].scatter([t[i],], [tilt[i],],50 , color="purple")
#ax[1].plot(t, tilt, color="black", linewidth=2.5, linestyle="dotted", label="tilt")
plt.yticks([0, +1])
ax[0].plot(t, x, color="red", linewidth=2.5, linestyle="-", label="x")
ax[0].plot(t, y, color="green", linewidth=2.5, linestyle="-", label="y")
ax[0].plot(t, z, color="blue", linewidth=2.5, linestyle="-", label="z")
ax[0].legend(loc='best')
plt.show()