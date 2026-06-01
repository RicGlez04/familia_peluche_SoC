import serial
import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib.animation import FuncAnimation

# --- CONFIGURACIÓN ---
PORT = 'COM5'  # <-- ¡CAMBIA ESTO AL PUERTO COM DE TU TARJETA!
BAUD = 9600
MAX_DISTANCE = 50  # Distancia máxima del radar en cm (ajusta según necesites)

try:
    ser = serial.Serial(PORT, BAUD, timeout=1)
except Exception as e:
    print(f"Error al conectar con {PORT}: {e}")
    exit()

# Arrays para guardar los datos de 0 a 180 grados
angles_rad = np.radians(np.arange(0, 181))
distances = np.zeros(181)

# Configurar el lienzo polar de matplotlib
fig = plt.figure(facecolor='black')
ax = fig.add_subplot(111, projection='polar')
ax.set_facecolor('black')
ax.tick_params(colors='green')
ax.grid(color='green', alpha=0.5)

# Configurar límites para medio círculo (0 a 180 grados)
ax.set_thetamin(0)
ax.set_thetamax(180)
ax.set_rmax(MAX_DISTANCE)
ax.set_title("Radar Ultrasónico - KL25Z", color='green', weight='bold', pad=20)

# Inicializar los puntos en la gráfica (Estilo "Matrix/Radar")
scatter = ax.scatter(angles_rad, distances, c=distances, cmap='hsv', s=50, alpha=0.75)
line, = ax.plot([0, 0], [0, MAX_DISTANCE], color='green', linewidth=2, alpha=0.8) # Línea de barrido

def update(frame):
    try:
        # Leer la línea del puerto serie y decodificar
        line_data = ser.readline().decode('utf-8').strip()
        if not line_data:
            return scatter, line

        # Extraer ángulo y distancia
        parts = line_data.split(',')
        if len(parts) == 2:
            angle = int(parts[0])
            dist = int(parts[1])
            
            # Limitar la distancia máxima para que el gráfico no se descomponga
            if dist > MAX_DISTANCE:
                dist = MAX_DISTANCE

            # Actualizar el array en el índice correspondiente
            if 0 <= angle <= 180:
                distances[angle] = dist
                
                # Actualizar posición de la línea de barrido
                current_angle_rad = math.radians(angle)
                line.set_data([current_angle_rad, current_angle_rad], [0, MAX_DISTANCE])

            # Actualizar los puntos
            scatter.set_offsets(np.c_[angles_rad, distances])
            # Cambiar colores según la distancia para mejor visualización
            scatter.set_array(distances)
            
    except ValueError:
        pass # Ignorar líneas corruptas o basura en el buffer
    except Exception as e:
        print(f"Error leyendo datos: {e}")
        
    return scatter, line

# Iniciar animación (se actualiza lo más rápido posible leyendo el buffer)
ani = FuncAnimation(fig, update, frames=None, interval=10, blit=True, cache_frame_data=False)

try:
    plt.show()
except KeyboardInterrupt:
    print("Cerrando programa...")
finally:
    ser.close()
