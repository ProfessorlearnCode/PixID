import os
import numpy as np
import pickle
from deepface import DeepFace


MODEL_NAME = "ArcFace"
BASE_DIR = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(os.path.dirname(BASE_DIR), "data")
DATASET_DIR = os.path.join(DATA_DIR, "imgDataset", "known_faces")

EMBEDS_DIR = os.path.join(DATA_DIR, "embeds")
OUTPUT_EMBEDDINGS = os.path.join(EMBEDS_DIR, "embeddings.pkl")



embeddings_dict = {}

for person in os.listdir(DATASET_DIR):
    person_path = os.path.join(DATASET_DIR, person)
    person_embeddings = []
    print(person_path)

    for img_name in os.listdir(person_path):
        img_path = os.path.join(person_path, img_name)
        try:
            result = DeepFace.represent(img_path=img_path, model_name=MODEL_NAME)[0]
            person_embeddings.append(result["embedding"])
        except Exception as e:
            print(f"[WARN] Couldn't process {img_name} for {person}: {e}")

    if person_embeddings:
        avg_embedding = np.mean(person_embeddings, axis=0)
        embeddings_dict[person] = avg_embedding

# Save embeddings
with open(OUTPUT_EMBEDDINGS, "wb") as f:
    pickle.dump(embeddings_dict, f)

print("[✅] Embeddings prepared and saved.")
