import cv2
from ultralytics import YOLO
from gptcall import analyze_scene
from serial import Serial

# Initialize serial connection to Arduino
arduino = Serial('/dev/cu.usbmodemFX2348N1', 115200)

# Load the YOLOv8 model
model = YOLO('yolov8n.pt')  # Using YOLOv8 nano model

# Initialize camera
cap = cv2.VideoCapture(0)

while True:
    # Read frame from camera
    ret, frame = cap.read()
    if not ret:
        break
        
    # Run YOLOv8 inference on the frame
    results = model(frame)
    
    # Filter for only human detections (class 0)
    for result in results:
        boxes = result.boxes
        human_boxes = boxes[boxes.cls == 0]
        result.boxes = human_boxes
        
        # If humans detected, get coordinates of closest person
        if len(human_boxes) > 0:
            # Take 10 frames to let camera adjust/stabilize
            for _ in range(50):
                ret, frame = cap.read()
                if not ret:
                    break
            if ret:
                # Save the frame with detected person
                cv2.imwrite('person_detected.jpg', frame)
                
                # Analyze scene and send to Arduino
                scene_analysis = analyze_scene()
                print(scene_analysis)
                # Release resources and exit after taking photo
                cap.release()
                cv2.destroyAllWindows()
    
    # Visualize the results on the frame
    annotated_frame = results[0].plot()
    
    # Display the frame
    cv2.imshow('YOLOv8 Detection', annotated_frame)
    
    # Break loop on 'q' press
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release resources
cap.release()
cv2.destroyAllWindows()
arduino.close()
