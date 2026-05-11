import serial
import math
import matplotlib.pyplot as plt
import sys

puerto_kl25z= "COM5"
baudios = 115200 #Tenemos que ver que tantos baudios vamos a utilizar
timeout = 1

ser: None
try: 
    ser = serial.Serial(puerto_kl25z,baudios, timeout)
    print(f"Conectando a {puerto_kl25z}")

except Exception as e:
    print("Error: No se logro conectar al puerto..... :( )")
    print(f"Esta mal, en que esta mal si meti todo bien, esta mal en algo {e}")
    sys.exit()


#-----Preparacion de la grafica ------------------#
plt.ion() #Activar el modo interactivo para actualizar la grafica a tiempo real
fig, ax = plt.subplots()
ax.set_title("Radar - Sensor Ultrasonico KL25Z")
ax.set_xlabel("Distancia X (cm)")
ax.set_ylabel("Distancia Y (cm)")

#-------Ajustar los limites  ---------------#

ax.set_xlim(-100, 100) # (-100, 100)
ax.set_ylim(-100, 100) # (-100, 100)
ax.grid(True) # malla 

#------------- Creador de punto ----------------------#
puntos, = ax.plot([],[],'go',markersize=2) # go = green circle 
x_data = []
y_data = []

print("Esperando datos....... >:v ")

# ------------Codigo principal ----------------#

try: 
    while True: 
        #1. Leer linea desde la KL25Z
        linea = ser.readline().decode('utf-8', errors= 'ignore').strip()

        if linea:
            try:
                # 2. Separar datos (asumiendo formato "angulo,distancia")
                datos = linea.split(',')

                if len(datos) != 2:
                    continue

                angulo_grados = float(datos[0])
                distancia = float(datos[1])
                
                # 3. Convertir de Polares a Cartesianas
                # Nota: math.sin/cos usan radianes
                angulo_rad = math.radians(angulo_grados)
                x = distancia * math.cos(angulo_rad)
                y = distancia * math.sin(angulo_rad)

                # Limpiar puntos anteriores
                x_data.clear
                y_data.clear
                
                # 4. Actualizar listas de datos
                x_data.append(x)
                y_data.append(y)
                
                # Opcional: Mantener solo los últimos 50 puntos para que no se sature
                if len(x_data) > 50:
                    x_data.pop(0)
                    y_data.pop(0)
                
                # 5. Actualizar la gráfica
                puntos.set_data(x_data, y_data)
                plt.draw()
                plt.pause(0.01) # Pausa necesaria para que matplotlib se renderice
                
            except (ValueError, IndexError):
                # Ignora líneas que no tengan el formato correcto
                continue

except KeyboardInterrupt:
    print("\nPrograma detenido por el usuario.")
finally:
    ser.close()
    print("Ya se cerro")
