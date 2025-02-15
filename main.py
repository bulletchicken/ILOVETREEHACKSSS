import cv2
from ultralytics import YOLO
from time import sleep
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
            # Get all human bounding boxes
            human_coords = human_boxes.xyxy.cpu().numpy()
            
            # Calculate areas of bounding boxes to find closest person
            largest_area = 0
            closest_coords = None
            
            for coords in human_coords:
                x1, y1, x2, y2 = coords
                # Calculate area of bounding box
                area = (x2 - x1) * (y2 - y1)
                
                if area > largest_area:
                    largest_area = area
                    closest_coords = ((x1 + x2) / 2, (y1 + y2) / 2)
            
            if closest_coords is not None:
                # Map coordinates to angles (assuming 0-180 degree range)
                x_angle = int((1 - closest_coords[0] / frame.shape[1]) * 180)
                y_angle = int(closest_coords[1] * 180 / frame.shape[0])
                
                # Send coordinates to Arduino
                command = f"{x_angle},{y_angle}\n"
                arduino.write(command.encode())
                print(f"Sent to Arduino: {command}")
    
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
