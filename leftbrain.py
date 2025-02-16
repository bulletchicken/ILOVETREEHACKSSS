import assemblyai as aai
import os
import cv2  
from dotenv import load_dotenv
import time

# Load environment variables
load_dotenv()

# Replace with your API key
aai.settings.api_key = os.getenv("ASSEMBLYAI_API_KEY")

# Initialize camera
cap = cv2.VideoCapture(0)

# Track silence duration
last_speech_time = time.time()
SILENCE_THRESHOLD = 2.0  # seconds of silence to consider speech finished

def on_open(session_opened: aai.RealtimeSessionOpened):
  "This function is called when the connection has been established."
  print("Session ID:", session_opened.session_id)

def on_data(transcript: aai.RealtimeTranscript):
  "This function is called when a new transcript has been received."
  global last_speech_time

  if not transcript.text:
    # Check if enough silence time has passed
    if time.time() - last_speech_time > SILENCE_THRESHOLD:
      ret, frame = cap.read()
      if ret:
        cv2.imwrite('memory_artifact.jpg', frame)
        print("\nSpeech finished - Photo captured!")
        last_speech_time = time.time()  # Reset timer
    return

  # Update the last speech time whenever we receive new text
  last_speech_time = time.time()

  if isinstance(transcript, aai.RealtimeFinalTranscript):
    print(transcript.text, end="\r\n")
  else:
    print(transcript.text, end="\r")

def on_error(error: aai.RealtimeError):
  "This function is called when the connection has been closed."
  print("An error occured:", error)

def on_close():
  "This function is called when the connection has been closed."
  print("Closing Session")
  cap.release()

transcriber = aai.RealtimeTranscriber(
  on_data=on_data,
  on_error=on_error,
  sample_rate=44_100,
  on_open=on_open, # optional
  on_close=on_close, # optional
)

# Start the connection
transcriber.connect()

# Open a microphone stream
microphone_stream = aai.extras.MicrophoneStream()
# Press CTRL+C to abort
transcriber.stream(microphone_stream)

transcriber.close()