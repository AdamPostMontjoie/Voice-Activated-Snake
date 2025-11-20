import vosk
import pyaudio 
import json
import serial


try:
    esp = serial.Serial('/dev/tty.usbserial-1430',baudrate=115200)
except Exception as e:
    print(e)
# Set the model path
model_path = "vosk-model-small-en-us-0.15"
# Initialize the model with model-path
model = vosk.Model(model_path)


#in vosk.Model() and it will download the right  model, here the language is 
#US-English
#model = vosk.Model(lang="en-us")
allowed_words = '["terminate", "start", "up", "down", "left", "right", "[unk]"]'
rec = vosk.KaldiRecognizer(model, 16000,allowed_words)

# microphone stream
p = pyaudio.PyAudio()
stream = p.open(format=pyaudio.paInt16,
                channels=1,
                rate=16000,
                input=True,
                frames_per_buffer=1024)
# 

# Open a text file in write mode using a 'with' block
print("Listening for speech. Say 'Terminate' to stop.")
    # Start streaming and recognize speech
while True:
        data = stream.read(1024, exception_on_overflow=False)
        if rec.AcceptWaveform(data):#accept waveform of input voice
            # Parse the JSON result and get the recognized text
            result = json.loads(rec.Result())
            voice_command = result['text']
            esp.write(voice_command.encode('utf-8'))
            print(voice_command)
            
            # Check for the termination keyword
            if "terminate" in voice_command.lower():
                print("Termination keyword detected. Stopping...")
                break
# Stop and close the stream
stream.stop_stream()
stream.close()

# Terminate the PyAudio object
p.terminate()

