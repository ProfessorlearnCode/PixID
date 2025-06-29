from flask import Flask, request
import os
from datetime import datetime
app = Flask(__name__)
UPLOAD_FOLDER = './data'
os.makedirs(UPLOAD_FOLDER, exist_ok=True)

@app.route('/upload', methods=['POST'])
def upload_image():
    image_data = request.data
    filename = f"{UPLOAD_FOLDER}/capture{datetime.now().strftime("%d-%m_%H-%M-%S")}.jpg"
    with open(filename, "wb") as f:
        f.write(image_data)
    return 'Image saved!', 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
