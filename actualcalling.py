from twilio.rest import Client
from dotenv import load_dotenv
import os
import boto3

load_dotenv()

account_sid = os.getenv("TWILIO_ACCOUNT_SID")
auth_token = os.getenv("TWILIO_AUTH_TOKEN")
twilio_number = os.getenv("TWILIO_PHONE_NUMBER")
aws_access_key = os.getenv("AWS_ACCESS_KEY_ID")
aws_secret_key = os.getenv("AWS_SECRET_ACCESS_KEY")

# Initialize S3 client
s3_client = boto3.client(
    's3',
    aws_access_key_id=aws_access_key,
    aws_secret_access_key=aws_secret_key
)

client = Client(account_sid, auth_token)

def upload_audio_to_s3():
    bucket_name = 'treehacksbucketeer'
    file_name = 'output.mp3'
    
    # Upload the file
    s3_client.upload_file(file_name, bucket_name, file_name)
    
    # Generate the URL for the uploaded file
    url = f"https://treehacksbucketeer.s3.us-east-1.amazonaws.com/output.mp3"
    
    call_emergency_services()
    return url

def call_emergency_services():
    call = client.calls.create(
        to="+16472896168",
        from_=twilio_number,
        url=upload_audio_to_s3()  # Use the S3 URL here
    )
    print(f"Calling ...")
    print(f"Call SID: {call.sid}")
    
if __name__ == "__main__":
    upload_audio_to_s3()  # Comment this out temporarily


