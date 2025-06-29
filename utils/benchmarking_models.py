import time

# Importing the model 10 times
for _ in range(10):
    import_start = time.time()
    
    from deepface import DeepFace
    
    import_end = time.time()
    print(f"Import x10 took {import_end - import_start:.2f} seconds")
    
# -----------------------------------------------------------------------
modelLoad_start = time.time()

print("Loading ArcFace...")
model = DeepFace.build_model("ArcFace")
print("Model loaded!")

modelLoad_end = time.time()
print(f"Model load took {modelLoad_end - modelLoad_start:.2f} seconds")

modelLoad_start1 = time.time()

print("Loading ArcFace...")
model = DeepFace.build_model("ArcFace")
print("Model loaded!")

modelLoad_end1 = time.time()
print(f"Model load 2 took {modelLoad_end1 - modelLoad_start1:.2f} seconds")
# -----------------------------------------------------------------------

exec_start = time.time()

result_ArcFace = DeepFace.verify(img1_path='farzam.png ', img2_path='WIN_20250511_01_34_13_Pro.jpg', model_name='ArcFace', detector_backend='opencv', enforce_detection=False)
print("Model: result_ArcFace", result_ArcFace['verified'])

exec_end = time.time()
print(f"ArcFace Model execution took {exec_end - exec_start:.2f} seconds")
# -----------------------------------------------------------------------

exec_start = time.time()

result_Facenet = DeepFace.verify(img1_path='farzam.png ', img2_path='WIN_20250511_01_34_13_Pro.jpg', model_name='Facenet', detector_backend='opencv', enforce_detection=False)
print("Model: result_Facenet", result_Facenet['verified'])

exec_end = time.time()
print(f"Facenet Model execution took {exec_end - exec_start:.2f} seconds")
# -----------------------------------------------------------------------


exec_start = time.time()

result_SFace = DeepFace.verify(img1_path='farzam.png ', img2_path='WIN_20250511_01_34_13_Pro.jpg', model_name='SFace', detector_backend='opencv', enforce_detection=False)
print("Model: result_SFace", result_SFace['verified'])

exec_end = time.time()
print(f"SFace Model execution took {exec_end - exec_start:.2f} seconds")
# -----------------------------------------------------------------------

exec_start = time.time()

GhostFaceNet_result = DeepFace.verify(img1_path='farzam.png ', img2_path='WIN_20250511_01_34_13_Pro.jpg', model_name='GhostFaceNet', detector_backend='opencv', enforce_detection=False)
print("Model: GhostFaceNet_result", GhostFaceNet_result['verified'])

exec_end = time.time()
print(f"GhostFaceNet Model execution took {exec_end - exec_start:.2f} seconds")
# -----------------------------------------------------------------------




