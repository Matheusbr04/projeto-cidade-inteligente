#!/usr/bin/env python3
# smart_light_yolo.py
import cv2
import time
import requests
import sys
import argparse
import mysql.connector
from ultralytics import YOLO

# ---------- Configurações ----------
# ✅ IP fixo do ESP32
ESP_HOST = "192.168.0.13"
COD_POSTE_ATUAL = 1  # <--- ALTERADO: Adicione um ID para identificar este poste/dispositivo

ESP_SENSOR_PATH = "/sensor"
ESP_RELAY_PATH = "/relay"
LDR_THRESHOLD = 1600           # ajuste conforme seu LDR
PERSON_CONF_THRESHOLD = 0.35
ABSENCE_SECONDS = 25
CAMERA_INDEX = 0

# ✅ Banco MySQL remoto
DB_CONFIG = {
    "host": "31.97.167.65",
    "user": "admin",
    "password": "1234",
    "database": "esp_test",
    #"port": 41062
}

def get_db_connection():
    return mysql.connector.connect(**DB_CONFIG)

def ensure_tables():
    conn = get_db_connection()
    cur = conn.cursor()
    cur.execute("""
    CREATE TABLE IF NOT EXISTS detecoes_ia (
      id INT AUTO_INCREMENT PRIMARY KEY,
      pessoas_detectadas TINYINT(1),
      confidence FLOAT,
      data_hora DATETIME DEFAULT CURRENT_TIMESTAMP
    ) ENGINE=InnoDB;
    """)
    cur.execute("""
    CREATE TABLE IF NOT EXISTS operacao_lampada (
      id INT AUTO_INCREMENT PRIMARY KEY,
      pino INT,
      estado VARCHAR(4),
      origem VARCHAR(32),
      data_hora DATETIME DEFAULT CURRENT_TIMESTAMP
    ) ENGINE=InnoDB;
    """)
    # Nota: Este CREATE TABLE pode estar diferente da sua tabela real.
    # A correção no código lida com a tabela que já existe no seu servidor.
    cur.execute("""
    CREATE TABLE IF NOT EXISTS leituras_sensores (
      id INT AUTO_INCREMENT PRIMARY KEY,
      tipo VARCHAR(32),
      valor INT,
      cod_poste INT, -- Adicionado para consistência, embora a correção principal esteja no 'insert_leitura'
      data_hora DATETIME DEFAULT CURRENT_TIMESTAMP
    ) ENGINE=InnoDB;
    """)
    conn.commit()
    cur.close()
    conn.close()

def insert_detecao(pessoas, confidence):
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("INSERT INTO detecoes_ia (pessoas_detectadas, confidence) VALUES (%s, %s)",
                    (1 if pessoas else 0, float(confidence)))
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("[db] erro insert_detecao:", e)

def insert_operacao(pino, estado, origem):
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("INSERT INTO operacao_lampada (pino, estado, origem) VALUES (%s, %s, %s)",
                    (pino, estado, origem))
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("[db] erro insert_operacao:", e)

# <--- ALTERADO: A função agora aceita 'cod_poste'
def insert_leitura(tipo, valor, cod_poste):
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        # <--- ALTERADO: O comando INSERT agora inclui 'cod_poste'
        cur.execute("INSERT INTO leituras_sensores (tipo, valor, cod_poste) VALUES (%s, %s, %s)", (tipo, int(valor), cod_poste))
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("[db] erro insert_leitura:", e)

def read_ldr(esp_host, timeout=1.0):
    url = f"http://{esp_host}{ESP_SENSOR_PATH}"
    try:
        r = requests.get(url, timeout=timeout)
        if r.status_code == 200:
            js = r.json()
            if "ldr" in js:
                return int(js["ldr"])
    except Exception as e:
        print("[esp] read_ldr erro:", e)
    return None

def set_relay(esp_host, pin, state):
    url = f"http://{esp_host}{ESP_RELAY_PATH}?pin={pin}&state={state}"
    try:
        r = requests.get(url, timeout=1.0)
        if r.status_code == 200:
            print(f"[esp] relay {pin} -> {state}")
            insert_operacao(pin, state.upper(), "python")
            return True
    except Exception as e:
        print("[esp] set_relay erro:", e)
    return False

def init_yolo(model_name="yolov8n.pt"):
    print("[yolo] carregando modelo", model_name)
    model = YOLO(model_name)
    return model

def detect_person(model, frame):
    results = model(frame, verbose=False)
    try:
        res = results[0]
        boxes = getattr(res, "boxes", None)
        if boxes is None:
            return False, 0.0
        cls = boxes.cls.cpu().numpy() if hasattr(boxes, "cls") else None
        conf = boxes.conf.cpu().numpy() if hasattr(boxes, "conf") else None
        if cls is None or conf is None:
            return False, 0.0
        person_mask = (cls == 0)
        if person_mask.any():
            best_conf = float(conf[person_mask].max())
            return True, best_conf
        else:
            return False, 0.0
    except Exception:
        try:
            df = results[0].pandas().xyxy[0]
            persons = df[(df['name'] == 'person') & (df['confidence'] >= PERSON_CONF_THRESHOLD)]
            if len(persons) > 0:
                return True, float(persons['confidence'].max())
            return False, 0.0
        except Exception as e:
            print("[yolo] erro ao interpretar resultado:", e)
            return False, 0.0

def main_loop(camera_index=0):
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print("Erro ao abrir camera index", camera_index)
        return

    model = init_yolo()

    last_person_time = None
    relay26_state = False
    relay27_state = False

    try:
        while True:
            ret, frame = cap.read()
            if not ret:
                time.sleep(0.1)
                continue

            ldr = read_ldr(ESP_HOST)
            if ldr is not None:
                # <--- ALTERADO: Passando o código do poste para a função
                insert_leitura("LDR", ldr, COD_POSTE_ATUAL)

            dark = (ldr is not None and ldr > LDR_THRESHOLD)
            if not dark:
                if relay26_state:
                    set_relay(ESP_HOST, 26, "off")
                    relay26_state = False
                if relay27_state:
                    set_relay(ESP_HOST, 27, "off")
                    relay27_state = False
                print("[main] ambiente claro: reles desligados")
                time.sleep(0.5)
                continue

            has_person, conf = detect_person(model, frame)
            print(f"[detec] pessoa: {has_person}, conf={conf:.2f}, LDR={ldr}")
            insert_detecao(has_person, conf)

            if has_person and conf >= PERSON_CONF_THRESHOLD:
                if not relay26_state:
                    if set_relay(ESP_HOST, 26, "on"):
                        relay26_state = True
                if relay27_state:
                    if set_relay(ESP_HOST, 27, "off"):
                        relay27_state = False
                last_person_time = time.time()
            else:
                if last_person_time is None:
                    last_person_time = time.time()
                elapsed = time.time() - last_person_time
                if elapsed >= ABSENCE_SECONDS:
                    if not relay27_state:
                        if set_relay(ESP_HOST, 27, "on"):
                            relay27_state = True
                    if relay26_state:
                        if set_relay(ESP_HOST, 26, "off"):
                            relay26_state = False

            time.sleep(0.2)

    except KeyboardInterrupt:
        print("Encerrando...")
    finally:
        cap.release()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--camera", type=int, default=CAMERA_INDEX)
    parser.add_argument("--ldr-threshold", type=int, default=LDR_THRESHOLD)
    args = parser.parse_args()

    CAMERA_INDEX = args.camera
    LDR_THRESHOLD = args.ldr_threshold

    ensure_tables()
    print(f"Iniciando com ESP32 em {ESP_HOST} | Poste ID: {COD_POSTE_ATUAL}")
    main_loop(CAMERA_INDEX)