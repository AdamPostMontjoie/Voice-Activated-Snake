import vosk
import pyaudio
import json
import serial


# Here I have downloaded this model to my PC, extracted the files 
# and saved it in local directory
# Set the model path
model_path = "vosk-model-small-en-us-0.15"
# Initialize the model with model-path
model = vosk.Model(model_path)

accepted_words = '["up", "down","left" ,"right", "terminate", "[unk]"]'

#if you don't want to download the model, just mention "lang" argument 
#in vosk.Model() and it will download the right  model, here the language is 
#US-English
#model = vosk.Model(lang="en-us")
# Create a recognizer
esp = serial.Serial('/dev/tty.usbserial-1440',115200)

rec = vosk.KaldiRecognizer(model, 16000,accepted_words)
# Open the microphone stream
p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16,
                channels=1,
                rate=16000,
                input=True,
                frames_per_buffer=8192)
# Specify the path for the output text file
output_file_path = "recognized_text.txt"
# Oen a text file in write mode using a 'with' block
print("Listening for speech. Say 'Terminate' to stop.")
    # Start streaming and recognize speech
while True:
        data = stream.read(4096)#read in chunks of 4096 bytes
        if rec.AcceptWaveform(data):#accept waveform of input voice
            # Parse the JSON result and get the recognized text
            result = json.loads(rec.Result())
            recognized_text = result['text']
            
            # Write recognized text to the file
            print(recognized_text)
            esp.write(recognized_text.encode('utf-8'))
            # Check for the termination keyword
            if "terminate" in recognized_text.lower():
                print("Termination keyword detected. Stopping...")
                break
# Stop and close the stream
stream.stop_stream()
stream.close()

# Terminate the PyAudio object
p.terminate()