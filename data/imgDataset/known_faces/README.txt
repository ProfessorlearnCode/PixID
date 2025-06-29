📁 INSTRUCTIONS: ADDING YOUR OWN KNOWN FACES TO pixID
------------------------------------------------------

This folder previously contained sample face data. For privacy reasons, the original known faces have been deleted.

To add new users to the system, follow these steps:

-------------------------------------
1. 📸 IMAGE DATASET REQUIREMENTS
-------------------------------------
• For each individual (specimen), create a **separate folder** inside this directory.

  Example:
  known_faces/
  ├── alice/
  └── bob/

• Inside each folder, add **at least 8–10 clear photos** of the person from different angles.

• Name the files using a simple numbered format:
  └── 1.jpg, 2.jpg, 3.jpg ... etc.

• Make sure all images are:
  - Well-lit and front-facing
  - Not blurry or overexposed
  - Similar in size and quality

-------------------------------------
2. 🧠 TRAINING THE MODEL (EMBEDDINGS)
-------------------------------------
Once you've added your custom face folders:

i) Navigate to the utils folder:
   > cd utils

ii) Run the following script:
   > python prepare_embeds.py

This will scan all folders inside `known_faces/` and regenerate `embeddings.pkl`.

-------------------------------------
3. 🧪 RUNNING pixID
-------------------------------------

▶ 3.1 DEFAULT USE (WITH PRE-TRAINED EMBEDDINGS)

Use this mode if you **already have embeddings** ready and just want to test recognition.

Step-by-Step:
i) Power the ESP32-CAM via USB or 5V.
   a) Connect your laptop to the ESP32 Wi-Fi network (e.g., ESP32-AP).

ii) Two green LEDs will blink upon successful connection.

iii) Register your device:
   > curl http://192.168.4.1/register
   (This assigns your laptop IP 192.168.4.2)

iv) Start the Flask backend:
   > python app.py

v) In your browser, visit:
   a) http://192.168.4.1 → Click **Capture**

vi) The backend:
   - Enhances the image
   - Recognizes the face
   - Logs attendance in CSV

vii) To view attendance:
   → http://127.0.0.1:5000

▶ 3.2 ADVANCED USE (ADDING NEW FACES)

To add new known users:

i) Add images in:
   > data/embeds/imgDataset/known_faces/<person_name>/

ii) Ensure filenames are sequential (e.g., 1.jpg, 2.jpg...)

iii) Rebuild embeddings:
   > cd utils
   > python prepare_embeds.py

iv) Return to 3.1 to run the app as normal.

-------------------------------------
4. ⚙️ HOW pixID WORKS (UNDER THE HOOD)
-------------------------------------
• ESP32-CAM captures a photo and POSTs it to the Flask server.
• Flask:
   - Enhances the image (`image_enhance.py`)
   - Compares it against `embeddings.pkl`
   - Marks attendance in `/logs/attendance.csv`
• The dashboard is served on `localhost:5000`.

-------------------------------------
5. 📌 NOTES & CAUTIONS
-------------------------------------
• One attendance entry per session is logged to avoid duplicates.
• `/utils/` contains extra scripts; only `prepare_embeds.py` is required.
• This system works **entirely offline** — no internet required.
• If any folder errors occur, ensure `/images`, `/uploads`, and `/processed` exist and are not in use.

-------------------------------------
6. ❓ FAQ – COMMON ISSUES
-------------------------------------
Q1. I don’t have an ESP32. Can I still test?
→ Yes. Copy an image from `/processed/` to `/uploads/` and run `app.py`.

Q2. Flask server won’t start?
→ Make sure virtual environment is active:
   > .\venv\Scripts\activate

Q3. Images aren’t saving?
→ Confirm folders aren’t open or locked. Create them manually if missing.

Q4. Can this work fully offline?
→ Yes. No cloud, internet, or API required.

Q5. ESP32 won’t connect?
→ Reboot and check Wi-Fi settings. Reconnect manually if needed.

Q6. How to exit virtual environment?
→ Just type:
   > deactivate

-------------------------------------
7. 🧼 TIPS FOR A CLEAN DEMO
-------------------------------------
• Clear `/uploads/`, `/images/`, and optionally `/logs/attendance.csv`
• Restart ESP32 before each new session
• Use Chrome or Firefox for best UI rendering

-------------------------------------
8. 🗂️ FOLDER STRUCTURE (INSIDE /app)
-------------------------------------

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

-------------------------------------
🔐 KEEP THIS FOLDER PRIVATE IF USING REAL DATA
