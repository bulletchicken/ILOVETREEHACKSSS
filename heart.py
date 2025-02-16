import pyautogui
import time
import webbrowser
import os
import random
from serial import Serial

arduino = Serial('/dev/cu.usbmodemFX2348N1', 9600)

def interact_with_elevenlabs():
  
    try:
        # Get screen size
        screen_width, screen_height = pyautogui.size()
        
        # Move to bottom right corner and click "Go Live" button
        pyautogui.moveTo(screen_width - 210, screen_height - 88)
        pyautogui.click()
        print("Clicked Go Live button")
        
        # Wait briefly then click the widget button that appears
        time.sleep(3)
        pyautogui.moveTo(screen_width - 150, screen_height - 150)
        pyautogui.click()
        time.sleep(0.5)
        pyautogui.moveTo(screen_width/2, screen_height/2 + 10)
        pyautogui.click()
        print("Clicked widget button")
        
        # Periodically wag tail after last click
        while True:
            time.sleep(random.uniform(5, 10))  # Random interval between 5-10 seconds
            arduino.write(b'2')
        
    except Exception as e:
        print(f"An error occurred: {e}")

if __name__ == "__main__":
    interact_with_elevenlabs()
