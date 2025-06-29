from flask import Flask, request, render_template, make_response
import os
import csv
from datetime import datetime
from threading import Thread
from image_enhance import filter_apply

from face_attendance import (
    CSV_configuration,
    executing_loop,
    UPLOADS_DIR,
    PROCESSED_DIR,
    IMG_DIR,
    ATTENDANCE_FILE,
    MODEL_NAME,
    DETECTOR
)


app = Flask(__name__)
os.makedirs(IMG_DIR, exist_ok=True)

# === Module 1: Attendance Dashboard ===
@app.route("/", methods=["GET"])
def dashboard():
    records = []
    try:
        with open(ATTENDANCE_FILE, newline='') as file:
            reader = csv.DictReader(file)
            for row in reader:
                records.append({
                    "timestamp": row["timestamp"],
                    "person": row["person"],
                    "image": row["image"],
                    "confidence": str(round(float(row["confidence"]) * 100, 2))
                })
    except Exception as e:
        print(f"[ERROR] Could not read CSV: {e}")
    return render_template("attendance.html", records=records)

# === Module 2: Upload Endpoint for ESP32 ===
@app.route('/upload', methods=['POST'])
def upload_image():
    try:
        image_data = request.data
        filename = os.path.join(IMG_DIR, f"capture_{datetime.now().strftime('%d-%m_%H-%M-%S')}.jpg")
        with open(filename, "wb") as f:
            f.write(image_data)

        print(f"[✅] Image saved: {filename}")
        filter_apply(IMG_DIR, UPLOADS_DIR, PROCESSED_DIR)

        # Explicitly return headers
        response = make_response("Image saved!", 200)
        response.headers["Content-Type"] = "text/plain"
        response.headers["Content-Length"] = str(len("Image saved!"))
        return response

    except Exception as e:
        print(f"[❌] Upload error: {e}")
        return 'Upload failed', 500

# === Background Thread: Face Recognition Loop ===
def start_background_processing():
    CSV_configuration(ATTENDANCE_FILE)
    loop_thread = Thread(target=executing_loop, args=(UPLOADS_DIR, DETECTOR, MODEL_NAME))
    loop_thread.daemon = True
    loop_thread.start()
    print("📸 Face recognition loop started...")

# === Entry Point ===
if __name__ == "__main__":
    start_background_processing()
    app.run(host="0.0.0.0", port=5000, debug=True, use_reloader=False)
