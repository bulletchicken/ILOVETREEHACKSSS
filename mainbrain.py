import os
import cv2
from ultralytics import YOLO
from mouth import analyze_scene
from serial import Serial
import asyncio
import time
from leftbrain import start_listening
from heart import interact_with_elevenlabs

arduino = Serial('/dev/cu.usbmodemFX2348N1', 9600)
time.sleep(2)  # Increase delay to 2 seconds

# Clear any pending data in the serial buffer
arduino.reset_input_buffer()
arduino.reset_output_buffer()

#the stand on hind legs first, then drop back down, then walk.
#step 1: Initiate -> stand up -> walk -> look around
arduino.write(b'1')
os.system("afplay spotify/siren.mp3")
arduino.flush()  # Ensure the data is written
time.sleep(10)

#step 2: Computer vision AI stuffy
# whenever arduino hears "2", wag tail a bit.


# Load the YOLOv8 model
model = YOLO('yolov8n.pt')  # Using YOLOv8 nano model

# Initialize camera
cap = cv2.VideoCapture(0)

async def main():
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
                # Wait for 3 more frames to confirm detection
                for _ in range(10):
                    ret, frame = cap.read()
                    if not ret:
                        break
                    results = model(frame)
                    boxes = results[0].boxes
                    if len(boxes[boxes.cls == 0]) == 0:
                        break
                    # Display frame to keep UI responsive
                    annotated_frame = results[0].plot()
                    cv2.imshow('YOLOv8 Detection', annotated_frame)
                    cv2.waitKey(1)
                else:  # Only executes if loop completes without break
                    cv2.imwrite('person_detected.jpg', frame)
                    arduino.write(b'2') #wags tail
                    
                    #sends a report + call all right here
                    os.system("afplay spotify/spotted.mp3")
                    await analyze_scene("Quickly summarize the scene in 2-3 sentences as if you are talking to a person. Describe 1) Number of people 2) Any details about the person's age, gender, whether they look injured or distressed, and ignore their clothing 3) A quick one word guess of the room they are in.", 'person_detected.jpg')
                    arduino.write(b'2') #wags tail
                    #enter conversational mode for the rest of the time
                    #start_listening()
                    
                    
                    cap.release()
                    cv2.destroyAllWindows()
                    return
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

# Run the async main function
if __name__ == "__main__":
    asyncio.run(main())
    
    #demo delay
    time.sleep(10)
    interact_with_elevenlabs()
