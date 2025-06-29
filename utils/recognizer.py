import os
import time
import csv
from datetime import datetime
from deepface import DeepFace

# 1. Configuration
KNOWN_FACES_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "known_faces")
UPLOADS_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "uploads")
ATTENDANCE_FILE = "attendance.csv"
MODEL_NAME = "ArcFace"

# 2. Load Model Once
print("[INFO] Loading DeepFace model...")
model = DeepFace.build_model(MODEL_NAME)
print("[INFO] Model loaded.")

# 3. Setup CSV File (if doesn't exist)
if not os.path.exists(ATTENDANCE_FILE):
    with open(ATTENDANCE_FILE, mode='w', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(["timestamp", "person", "image", "confidence"])

# 4. Attendance Record Tracker (to avoid duplicate marking)
marked_today = set()

# 5. Continuous Loop
print("[INFO] Starting face verification loop...")
while True:
    if os.listdir(UPLOADS_DIR) == []:
        print("[LOG] The folder seems to be empty of pictures, waiting for images in the /uploads")
    else:
        for filename in os.listdir(UPLOADS_DIR):
            img_path = os.path.join(UPLOADS_DIR, filename)
            
            if not filename.lower().endswith(('.jpg', '.jpeg', '.png')):
                continue  # Skip non-image files

            try:
                # 6. Compare with known faces using DeepFace.find()
                result = DeepFace.find(
                    img_path=img_path,
                    db_path=KNOWN_FACES_DIR,
                    model_name=MODEL_NAME,
                    detector_backend='yolov8',
                    enforce_detection=False
                )

                if result and not result[0].empty:
                    # Top match
                    match = result[0].iloc[0]
                    identity_path = match["identity"]
                    confidence = match["distance"]

                    person_name = os.path.basename(identity_path).split('.')[0]

                    # Avoid duplicate entries
                    if person_name not in marked_today:
                        with open(ATTENDANCE_FILE, mode='a', newline='') as file:
                            writer = csv.writer(file)
                            writer.writerow([
                                datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
                                person_name,
                                filename,
                                round(confidence, 4)
                            ])
                        print(f"[MARKED] {person_name} at {datetime.now().strftime('%H:%M:%S')}")
                        marked_today.add(person_name)
                    else:
                        print(f"[SKIP] {person_name} already marked today.")

                else:
                    print(f"[NO MATCH] for image {filename}")

            except Exception as e:
                print(f"[ERROR] Processing {filename}: {e}")

            # Optional: Move or delete processed image
            # os.remove(img_path)

    time.sleep(1.5)  # Polling interval
