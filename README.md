# 📸 pixID – Smart Offline Facial Recognition Attendance System

**pixID** is a lightweight, offline-capable attendance system that uses facial recognition, built using the **ESP32-S3 CAM** and a local **Flask server**. Designed with minimal hardware and no reliance on cloud services, it’s perfect for classroom, lab, or small office environments.

---

## 🎯 Project Objective

To create a **smart, secure, and offline-first facial recognition system** that:
- Automatically logs attendance using facial data
- Works without internet access
- Is cost-effective and hardware-friendly
- Can later sync with cloud-based systems

---

## 🧠 How It Works

1. **ESP32-S3 CAM** acts as a Wi-Fi access point.
2. When triggered (manually or by IR sensor), it captures a face image and sends it to a local server.
3. A **Flask backend**:
   - Processes the image
   - Matches it with known faces using `face_recognition` (Python)
   - Logs attendance in a CSV file
   - Updates a simple web dashboard
4. LED feedback on the ESP32 provides status cues.

---

## 🚀 Features

- 🔒 **Offline support** (no internet required after setup)
- 🎞️ Face capture via `/capture` endpoint or IR gesture
- 🧠 Local face recognition using Python + `face_recognition`
- 🧾 Attendance logged in `attendance_log.csv`
- 🖥️ Web dashboard to view attendance records
- 💡 LED status indicator (capturing / success / failure)
- 🔌 Powered via standard 12V laptop adapter

---

## 📦 Tech Stack

| Component       | Tech Used                      |
|----------------|---------------------------------|
| Camera Board    | ESP32-S3 CAM                   |
| Server Backend  | Flask (Python)                 |
| Recognition     | `face_recognition` (ArcFace)   |
| Communication   | HTTP (image POST to Flask)     |
| Storage         | CSV log, local known_faces DB  |
| Triggering      | Manual + IR gesture-based      |

---

## 📁 Project Structure

```

app/
├── app.py                   ← Main Flask server
├── data/
│   ├── embeds/              ← Known faces & embeddings.pkl
│   ├── logs/                ← Attendance CSV file
│   └── processed/           ← Original photo records
├── templates/               ← Frontend HTML pages
├── static/                  ← CSS & JS assets
├── uploads/                 ← Enhanced images for recognition
├── images/                  ← Raw images from ESP32
├── utils/
│   └── prepare_embeds.py    ← Use this after adding new known faces
├── requirements.txt

````

---

## 🧪 Setup Instructions

### 1. 🔧 ESP32 Setup

- Flash the ESP32-S3 CAM with `pixid_cam_code.ino`
- Ensure it:
  - Creates a SoftAP network
  - Runs a `/capture` route
  - Takes and POSTs images to Flask server

### 2. 🧠 Flask Server Setup

```bash
git clone https://github.com/yourusername/pixID.git
cd pixID/server

# Create virtual environment (optional)
python -m venv venv
source venv/bin/activate    # For Windows: venv\Scripts\activate

# Install dependencies
pip install -r ../requirements.txt

# Run server
python app.py
````

Server will listen at `http://192.168.4.1:5000` (ESP32’s network)

---

### 3. 📂 Enroll Faces

* Add clear, front-facing images in `dataset/known_faces/`
* Filenames will be used as names (e.g., `farzam.jpg` → "Farzam")

---

### 4. 🎯 Trigger Attendance Capture

* Connect to ESP32 Wi-Fi
* Visit `http://192.168.4.1/capture` to trigger capture manually
* IR-based gesture capture also supported (customizable in `.ino` file)

---

## 📊 Dashboard

* Flask hosts a basic web dashboard at `/dashboard`
* View recent attendance records with timestamps

---

## 📝 Notes

* No cloud service used — all computation and logging are local
* You can later integrate cloud sync (e.g., Firebase or Google Sheets API)
* Recognition is best with clear, evenly lit faces from front view
* IR sensor is optional but adds hands-free interaction

---

## 🔮 Future Plans

* Add real-time face detection on ESP32 preview
* Implement cloud-based sync (Firebase or S3)
* Optimize power usage (deep sleep cycles)
* Add mobile-friendly dashboard

---

## 🙌 Acknowledgements

* Inspired by CS50, AI, and IoT coursework
* Built with ❤️ by Farzam Asad as an exploration into practical AI + embedded systems

---

> *pixID is a proof-of-concept project for learning, exploration, and showcasing embedded + AI integration.*

