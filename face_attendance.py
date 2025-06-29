import os
import time
import csv
from datetime import datetime

DeepFace = None

def load_deepface_model():
    global DeepFace
    if DeepFace is None:
        print("[INFO] Loading DeepFace model...")
        from deepface import DeepFace as DF
        DeepFace = DF
        print("[INFO] Model loaded 🟢")


from scipy.spatial.distance import cosine
import numpy as np
import pickle
import cv2

# --- 0. Configuration ---
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(BASE_DIR, "data")

DATASET_DIR = os.path.join(DATA_DIR, "imgDataset", "known_faces")
EMBEDS_DIR = os.path.join(DATA_DIR, "embeds")
LOGS_DIR = os.path.join(DATA_DIR, "logs")
PROCESSED_DIR = os.path.join(DATA_DIR, "processed")

UPLOADS_DIR = os.path.join(BASE_DIR, "uploads")
IMG_DIR = os.path.join(BASE_DIR, "images")

ATTENDANCE_FILE = os.path.join(LOGS_DIR, "attendance.csv")

MODEL_NAME = "ArcFace"
DETECTOR = "opencv"
THRESHOLD = 0.35

# --- 1. Setup CSV File (if it doesn't exist) ---
def CSV_configuration(attendance_file):
    attendance_filePath = attendance_file
    if not os.path.exists(attendance_filePath):
        with open(attendance_filePath, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["timestamp", 
                             "person", 
                             "image", 
                             "confidence"])
        print("[LOG] The CSV file is created! 🟢")


def CSV_mark_attendance(attendance_file, attendance):
    attendance_filePath = attendance_file
    with open(attendance_filePath, mode='a', newline='') as file:
        marker = csv.writer(file)
        marker.writerow([attendance['TIMESTAMP'], 
                         attendance['PERSON'], 
                         attendance['IMAGE FILE'], 
                         attendance['CONFIDENCE']])
    print('🟢')

# --- 2. Load Known Embeddings ---
embeddeings = os.path.join(EMBEDS_DIR, "embeddings.pkl")
with open(embeddeings, "rb") as f:
    known_embeddings = pickle.load(f)


# --- 3. Recognizer Function ---
marked_today = set()
def recognizer(img_path, filename, threshold, detector, model_name):
    try:
        load_deepface_model()
        result = DeepFace.represent(
            img_path=img_path,
            model_name=model_name,
            detector_backend=detector,
            enforce_detection=False
        )[0]

        target_embedding = np.array(result["embedding"])
        best_match = None
        best_score = 1.0

        for name, embedding in known_embeddings.items():
            distance = cosine(target_embedding, embedding)
            if distance < best_score and distance < threshold:
                best_match = name
                best_score = distance

        if best_match:
            if best_match not in marked_today:
                attendance_record = {
                    'PERSON': best_match,
                    'IMAGE FILE': filename,
                    'CONFIDENCE': round(best_score, 4),
                    'TIMESTAMP': datetime.now().strftime('%H:%M:%S')
                }
                marked_today.add(best_match)
                print(f"[MARKED] {best_match} at {attendance_record['TIMESTAMP']} 🙋🏻‍♂️", end=' ')
                return attendance_record
            else:
                print(f"[SKIP] {best_match} already marked today. 🙋🏻‍♂️")
        else:
            print(f"[NO MATCH] for image {filename} 🙅🏻‍♂️")

    except Exception as e:
        print(f"[ERROR] Processing {filename}: {e} ❓")


# --- 4. Background Execution Loop ---
def executing_loop(upload_dir, detector, model_name):
    print("[INFO] Starting face verification loop...")

    while True:
        filenames = os.listdir(upload_dir)
        if not filenames:
            print("[LOG] The folder is empty. Waiting for images in /uploads")
        else:
            for filename in filenames:
                if not filename.lower().endswith(('.jpg', '.jpeg', '.png')):
                    continue

                img_path = os.path.join(upload_dir, filename)
                time.sleep(0.2)

                img = cv2.imread(img_path)
                if img is None:
                    print(f"[ERROR] Failed to load {img_path}")
                    continue

                attendance = recognizer(
                    filename=filename,
                    img_path=img_path,
                    threshold=THRESHOLD,
                    detector=detector,
                    model_name=model_name
                )

                if attendance:
                    CSV_mark_attendance(ATTENDANCE_FILE, attendance)

                del img
                try:
                    os.remove(img_path)
                except PermissionError as e:
                    print(f"[WARN] Couldn't delete {img_path}: {e}")

        time.sleep(1.5)
