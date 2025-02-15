import os
from openai import OpenAI
import base64
from dotenv import load_dotenv

load_dotenv()  # Load environment variables from .env file

# Now you can access variables like this:
api_key = os.getenv('OPENAI_API_KEY')

def encode_image(image_path):
    with open(image_path, "rb") as image_file:
        return base64.b64encode(image_file.read()).decode('utf-8')

client = OpenAI(api_key=api_key)

def analyze_scene(image_path):
    base64_image = encode_image(image_path)

    completion = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[
            {
                # You are an AI assistant helping with emergency rescue operations. Analyze the image and provide detailed information about: 1) Number of people detected 2) Description of each person's position, condition, and clothing 3) Analysis of the room/environment
                "role": "system",
                "content": "Quickly summarize the scene in 2-3 sentences. Describe 1) Number of people 2) Any details about the person's age, gender, whether they look injured or distressed, and ignore their clothing 3) A quick guess of the room they are in."
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

    return completion.choices[0].message.content

# Example usage:
# scene_analysis = analyze_scene("path_to_captured_frame.jpg")
# print(scene_analysis)