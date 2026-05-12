import serial
import math
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import sys

puerto = "COM4" # Cambia si es necesario
rango_max = 50

try:
    ser = serial.Serial(puerto, 9600, timeout=0.1)
    print(f"✅ Radar conectado en {puerto}")
except:
    print("❌ Error de conexión"); sys.exit()

plt.ion()
fig, ax = plt.subplots(figsize=(8, 8))
ax.set_aspect('equal')
ax.set_xlim(-rango_max-5, rango_max+5)
ax.set_ylim(-rango_max-5, rango_max+5)
ax.axis('off')

# Dibujo del Radar
circ_fondo = patches.Circle((0,0), rango_max, color='#0F172A', zorder=0)
circ_borde = patches.Circle((0,0), rango_max, edgecolor='#22D3EE', fill=False, linewidth=2, zorder=5)
ax.add_patch(circ_fondo)
ax.add_patch(circ_borde)

# Línea de barrido y puntos
haz, = ax.plot([], [], color='#22D3EE', linewidth=3, alpha=0.8)
puntos, = ax.plot([], [], 'o', color='#F87171', markersize=5, alpha=0.6)

x_hist, y_hist = [], []

try:
    while True:
        linea = ser.readline().decode('utf-8', errors='ignore').strip()
        if ',' in linea:
            try:
                ang, dist = map(float, linea.split(','))
                rad = math.radians(ang)
                
                # Conversión X, Y
                x = dist * math.cos(rad)
                y = dist * math.sin(rad)
                
                x_hist.append(x); y_hist.append(y)
                if len(x_hist) > 40: x_hist.pop(0); y_hist.pop(0)
                
                puntos.set_data(x_hist, y_hist)
                haz.set_data([0, x], [0, y])
                fig.canvas.draw_idle()
                fig.canvas.flush_events()
            except: continue
        plt.pause(0.001)
except KeyboardInterrupt:
    ser.close(); plt.close()
