import cv2
import time
import serial
import firebase_admin
from firebase_admin import credentials, firestore
from ultralytics import YOLO

# 1. INICIALIZACION DE FIREBASE
try:
    cred = credentials.Certificate("opyimus-trash-firebase-adminsdk-fbsvc-4f9796ce39.json")
    firebase_admin.initialize_app(cred)
    db = firestore.client()
    print("[FIREBASE] Conectado exitosamente.")
except Exception as e:
    print(f"[FIREBASE ERROR] No se pudo conectar: {e}")
    db = None

# 2. CONFIGURACION DE PUERTOS SERIALES
try:
    # PUERTO 0: KL25Z (Sensor PIR, LCD, LEDs)
    puerto_kl25 = serial.Serial('/dev/serial/by-path/platform-xhci-hcd.1-usb-0:2:1.0', 9600, timeout=0.1)
    # PUERTO 1: ESP32 (Motores)
    puerto_esp32 = serial.Serial('/dev/serial/by-path/platform-xhci-hcd.0-usb-0:2:1.0-port0', 115200, timeout=1)
    print("[SYSTEM] Comunicacion establecida con ambas placas.")
except Exception as e:
    print(f"[ERROR] Error al conectar puertos seriales: {e}")
    exit()

# 3. CEREBRO IA
modelo = YOLO("best.onnx", task='detect')
cap = cv2.VideoCapture(0)

# UMBRALES
UMBRAL_MINIMO = 0.55
UMBRAL_SEGURA = 0.75

print("[SYSTEM] Bote activo. Esperando PIR en KL25Z...")

try:
    while True:
        # Esperar seÃƒÂ±al 'S' del sensor PIR (KL25Z)
        if puerto_kl25.in_waiting > 0:
            if puerto_kl25.read(1) == b'S':
                puerto_kl25.write(b'A') # LED AZUL: Procesando
                print("\n[EVENTO] PIR detectado. Capturando...")
                
                mejor_confianza = 0
                clase_detectada = ""
                
                # AnÃƒÂ¡lisis de frames
                for _ in range(30):
                    ret, frame = cap.read()
                    if not ret: continue
                    resultados = modelo(frame, verbose=False, conf=0.2)
                    for r in resultados:
                        for box in r.boxes:
                            conf = float(box.conf[0])
                            if conf > mejor_confianza:
                                mejor_confianza = conf
                                clase_id = int(box.cls[0])
                                clase_detectada = modelo.names[clase_id]
                    time.sleep(0.03)
                
                # --- LÃƒâ€œGICA DE ESTADOS ---
                if mejor_confianza >= UMBRAL_SEGURA:
                    tipo = "Organico" if clase_detectada in ["comida", "organic"] else "Inorganico"
                    print(f"[IA] Clasificacion segura: {tipo} ({mejor_confianza*100:.1f}%)")
                    
                    # 1. Avisar a KL25Z para que actualice la LCD
                    puerto_kl25.write(b'O' if tipo == "Organico" else b'I')
                    
                    # 2. Ordenar a ESP32 abrir tapa ('D' OrgÃƒÂ¡nico, 'I' InorgÃƒÂ¡nico)
                    cmd_motor = b'D' if tipo == "Organico" else b'I'
                    puerto_esp32.write(cmd_motor)
                    
                    # 3. Registro Firebase
                    if db:
                        db.collection('registros').add({
                            'tipo': tipo,
                            'confianza': mejor_confianza,
                            'timestamp': firestore.SERVER_TIMESTAMP
                        })
                    
                    time.sleep(6.0) # Tiempo de operaciÃƒÂ³n de motores
                    puerto_kl25.write(b'R') # Reset a modo espera (Verde)
                    
                elif mejor_confianza >= UMBRAL_MINIMO:
                    print(f"[IA] Incertidumbre: {clase_detectada} ({mejor_confianza*100:.1f}%).")
                    puerto_kl25.write(b'U') # LED AMARILLO
                    
                    confirmado = False
                    start_time = time.time()
                    while time.time() - start_time < 15: # 15s para decidir
                        if puerto_kl25.in_waiting > 0:
                            resp = puerto_kl25.read(1)
                            if resp in [b'D', b'I']: 
                                print(f"[BOTÃƒâ€œN KL25Z] DecisiÃƒÂ³n: {resp}")
                                # Reenviar a ESP32
                                puerto_esp32.write(resp)
                                
                                # Avisar a KL25Z para que actualice la LCD
                                puerto_kl25.write(b'O' if resp == b'D' else b'I')

                                # Firebase manual
                                if db:
                                    tipo_manual = "Organico" if resp == b'D' else "Inorganico"
                                    db.collection('registros').add({
                                        'tipo': tipo_manual,
                                        'confianza': mejor_confianza,
                                        'timestamp': firestore.SERVER_TIMESTAMP
                                    })
                                confirmado = True
                                break
                        time.sleep(0.1)
                    
                    if confirmado: time.sleep(6.0)
                    puerto_kl25.write(b'R') # Reset KL25Z
                    
                else:
                    print(f"[IA] Nada detectado.")
                    puerto_kl25.write(b'E') # LED ROJO
                
                print("[SYSTEM] Esperando nuevo movimiento...")
                time.sleep(1.0)

except KeyboardInterrupt:
    print("\n[SYSTEM] Apagado.")
    cap.release()
    puerto_kl25.close()
    puerto_esp32.close()
