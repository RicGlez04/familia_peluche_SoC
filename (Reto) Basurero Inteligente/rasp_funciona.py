import cv2
import time
import serial
from ultralytics import YOLO

# Configuracion de Puertos
ruta_sensor = '/dev/serial/by-path/platform-xhci-hcd.1-usb-0:2:1.0'
ruta_motores = '/dev/serial/by-path/platform-xhci-hcd.0-usb-0:2:1.0'

puerto_sensor = serial.Serial(ruta_sensor, 9600, timeout=0.1)
puerto_motores = serial.Serial(ruta_motores, 9600, timeout=0.1)
modelo = YOLO("best.onnx", task='detect')
cap = cv2.VideoCapture(0)

# Umbrales
UMBRAL_CONFIANZA_MINIMA = 0.40 # Para considerar que hay algo
UMBRAL_CONFIANZA_SEGURA = 0.75 # Para clasificacion automatica

print("[SYSTEM] Esperando senal del PIR en PTC8...")

try:
    while True:
        # 1. ESPERA EL DISPARADOR DEL SENSOR (KL1)
        if puerto_sensor.in_waiting > 0:
            if puerto_sensor.read(1) == b'S':
                print("\n[EVENTO] PIR activado. Analizando con IA...")
                
                # Tiempo para que el objeto se estabilice frente a la camara
                time.sleep(0.7) 
                
                for _ in range(5): cap.grab()
                ret, frame = cap.read()
                
                if ret:
                    resultados = modelo(frame, verbose=False, conf=0.3)
                    
                    mejor_confianza = 0
                    tipo = ""
                    
                    for r in resultados:
                        for box in r.boxes:
                            conf = float(box.conf[0])
                            if conf > mejor_confianza:
                                mejor_confianza = conf
                                clase = modelo.names[int(box.cls[0])]
                                tipo = "Organico" if clase in ["comida", "organic"] else "Inorganico"
                    
                    # Logica de estados
                    if mejor_confianza >= UMBRAL_CONFIANZA_SEGURA:
                        print(f"[IA] Clasificacion segura: {tipo} ({mejor_confianza*100:.1f}%)")
                        cmd = b'D' if tipo == "Organico" else b'I'
                        puerto_motores.write(cmd)
                        puerto_sensor.write(cmd)
                        time.sleep(3.0)
                        puerto_sensor.write(b'R')
                        
                    elif mejor_confianza >= UMBRAL_CONFIANZA_MINIMA:
                        print(f"[IA] Duda: {tipo} ({mejor_confianza*100:.1f}%). Clasificacion manual requerida.")
                        puerto_sensor.write(b'U') # Led Amarillo
                        time.sleep(2.0)
                        puerto_sensor.write(b'R')
                        
                    else:
                        print("[IA] Nada detectado.")
                        puerto_sensor.write(b'E') # Led Rojo
                        time.sleep(2.0)
                        puerto_sensor.write(b'R')
                    
                    # Tiempo de espera entre detecciones para proteger el sistema
                    print("[SYSTEM] Esperando 4 segundos para siguiente deteccion...")
                    time.sleep(4.0)

except KeyboardInterrupt:
    print("[SYSTEM] Apagando...")
    cap.release()
    puerto_sensor.close()
    puerto_motores.close()
