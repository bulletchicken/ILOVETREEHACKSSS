import cv2
from ultralytics import YOLO

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
