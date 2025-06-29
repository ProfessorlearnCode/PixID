import cv2 as cv
import os
import shutil

def move_to_processed(image_path, processed_dir):
    os.makedirs(processed_dir, exist_ok=True)
    
    filename = os.path.basename(image_path)
    destination = os.path.join(processed_dir, filename)
    shutil.move(image_path, destination)
    print(f"Moved {filename} to {processed_dir}")

def filter_apply(input_filePath, output_filePath, processed_dir):
    os.makedirs(output_filePath, exist_ok=True)
    
    print("Processing the image ⛔")
    
    for n, image_file in enumerate(os.listdir(input_filePath)):
        image_path = os.path.join(input_filePath, image_file)
        image = cv.imread(image_path)

        denoised = cv.fastNlMeansDenoisingColored(image, None, h=4, hColor=4, templateWindowSize=7, searchWindowSize=21)
        gray = cv.cvtColor(denoised, cv.COLOR_BGR2GRAY)
        clahe = cv.createCLAHE(clipLimit=2.0, tileGridSize=(8, 8))
        equalized = clahe.apply(gray)
        equalized_bgr = cv.cvtColor(equalized, cv.COLOR_GRAY2BGR)
        sharpen_kernel = cv.GaussianBlur(equalized_bgr, (0, 0), 3)
        sharpened = cv.addWeighted(equalized_bgr, 1.5, sharpen_kernel, -0.5, 0)
        upscaled = cv.resize(sharpened, None, fx=2, fy=2, interpolation=cv.INTER_CUBIC)

        # Save edited image
        output_filename = f'upscaled-{n}.png'
        cv.imwrite(os.path.join(output_filePath, output_filename), upscaled)

        # Move original to processed
        move_to_processed(image_path, processed_dir)
        
        print("Processed the image 💨")
