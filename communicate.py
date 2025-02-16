import os
from openai import OpenAI
import base64
from dotenv import load_dotenv
from voice import text_to_speech
from actualcalling import call_emergency_services
from actualcalling import upload_audio_to_s3
load_dotenv()  # Load environment variables from .env file

# Now you can access variables like this:
api_key = os.getenv('OPENAI_API_KEY')

def encode_image(image_path):
    with open(image_path, "rb") as image_file:
        return base64.b64encode(image_file.read()).decode('utf-8')

client = OpenAI(api_key=api_key)

async def analyze_scene(prompt, image_path):
    base64_image = encode_image(image_path)

    # Create async client
    completion = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[
            {
                # You are an AI assistant helping with emergency rescue operations. Analyze the image and provide detailed information about: 1) Number of people detected 2) Description of each person's position, condition, and clothing 3) Analysis of the room/environment
                "role": "system",
                "content": prompt
            },
            {
                "role": "user",
                "content": [
                    {
                        "type": "image_url",
                        "image_url": {
                        "url": f"data:image/jpeg;base64,{base64_image}"
                        }
                    }
                ]
            }
        ],
        max_tokens=500
    )

    # all to send a police report
    with open('baramemory.txt', 'r') as f:
        memory_text = f.read()
    await text_to_speech(completion.choices[0].message.content + " " + memory_text)
    #add scrapybara answers to this report.
    await upload_audio_to_s3()
    await call_emergency_services()
    return completion.choices[0].message.content

# Example usage:
# scene_analysis = analyze_scene("path_to_captured_frame.jpg")
# print(scene_analysis)