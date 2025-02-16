from twilio.rest import Client
from dotenv import load_dotenv
import os
import boto3
import asyncio
# Load environment variables
load_dotenv()

# Get Twilio credentials
account_sid = os.getenv("TWILIO_ACCOUNT_SID")
auth_token = os.getenv("TWILIO_AUTH_TOKEN")
twilio_number = os.getenv("TWILIO_PHONE_NUMBER")
my_phone_number = os.getenv("MY_PHONE_NUMBER")

# Get AWS credentials
aws_access_key = os.getenv("AWS_ACCESS_KEY_ID")
aws_secret_key = os.getenv("AWS_SECRET_ACCESS_KEY")



# Validate that credentials exist
if not all([account_sid, auth_token, twilio_number, aws_access_key, aws_secret_key]):
    raise ValueError("Missing required environment variables. Check your .env file.")

# Initialize S3 client
s3_client = boto3.client(
    's3',
    aws_access_key_id=aws_access_key,
    aws_secret_access_key=aws_secret_key
)

# Initialize Twilio client
client = Client(account_sid, auth_token)

async def upload_audio_to_s3():
    """Uploads an audio file to S3 and returns its public URL."""
    bucket_name = 'treehacksbucketeer'
    file_name = 'output.mp3'
    
    # Ensure the file exists before uploading
    if not os.path.exists(file_name):
        raise FileNotFoundError(f"Error: {file_name} not found.")
    
    try:
        # Upload the file to S3 with public-read ACL
        s3_client.upload_file(
            file_name, 
            bucket_name, 
            file_name,
            ExtraArgs={'ACL': 'public-read'}
        )
    except Exception as e:
        print(f"Error uploading to S3: {e}")
        return None

async def call_emergency_services():

    
    try:
        call = client.calls.create(
                    twiml=f'<?xml version="1.0" encoding="UTF-8"?><Response><Play>https://api.twilio.com/cowbell.mp3</Play></Response>',
                    to=my_phone_number,
                    from_=twilio_number
                )
        print(f"Calling emergency services...")
        print(f"Call SID: {call.sid}")
    except Exception as e:
        print(f"Error making the call: {e}")
        
if __name__ == "__main__":
    asyncio.run(call_emergency_services())