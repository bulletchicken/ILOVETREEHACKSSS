import os
from dotenv import load_dotenv
import uuid
from elevenlabs import VoiceSettings
from elevenlabs.client import ElevenLabs
import asyncio
load_dotenv()
ELEVENLABS_API_KEY = os.getenv("ELEVENLABS_API_KEY")
if not ELEVENLABS_API_KEY:
    raise ValueError("ELEVENLABS_API_KEY environment variable is not set")

client = ElevenLabs(api_key=ELEVENLABS_API_KEY)


async def text_to_speech(text: str) -> str:
    # Calling the text_to_speech conversion API with detailed parameters
    response = client.text_to_speech.convert(
        voice_id="nPczCjzI2devNBz1zQrb", # Adam pre-made voice
        output_format="mp3_22050_32",
        text=text,
        model_id="eleven_flash_v2_5", # use the turbo model for low latency
        voice_settings=VoiceSettings(
            stability=1.0,
            similarity_boost=1.0,
            style=0.9,
            use_speaker_boost=True,
        ),
    )

    # uncomment the line below to play the audio back
    # play(response)

    # Generating a unique file name for the output MP3 file
    save_file_path = f"spotify/spotted.mp3"

    # Writing the audio to a file
    with open(save_file_path, "wb") as f:
        for chunk in response:
            if chunk:
                f.write(chunk)

    print(f"{save_file_path}: A new audio file was saved successfully!")

    # Return the path of the saved audio file
    

if __name__ == "__main__":
    asyncio.run(text_to_speech("I have spotted someone! Meow meow meow..."))