from flask import Flask, request, jsonify
from flask_cors import CORS  # <--- Added this
import os
import psutil
import pyautogui

app = Flask(__name__)
CORS(app)  # <--- This allows the ESP32 page to talk to your PC

SECRET = "12345"

def check_key(req):
    return req.args.get("key") == SECRET

# ---------------- SYSTEM ----------------
@app.route('/lock')
def lock():
    if not check_key(request): return "Unauthorized", 403
    os.system("rundll32 user32.dll,LockWorkStation")
    return "OK"

@app.route('/shutdown')
def shutdown():
    if not check_key(request): return "Unauthorized", 403
    os.system("shutdown /s /t 1")
    return "OK"

@app.route('/restart')
def restart():
    if not check_key(request): return "Unauthorized", 403
    os.system("shutdown /r /t 1")
    return "OK"

# ---------------- MEDIA ----------------
@app.route('/media/play')
def play():
    if not check_key(request): return "Unauthorized", 403
    pyautogui.press('playpause')
    return "OK"

@app.route('/media/next')
def next_track():
    if not check_key(request): return "Unauthorized", 403
    pyautogui.press('nexttrack')
    return "OK"

@app.route('/media/prev')
def prev_track():
    if not check_key(request): return "Unauthorized", 403
    pyautogui.press('prevtrack')
    return "OK"

@app.route('/media/volup')
def vol_up():
    if not check_key(request): return "Unauthorized", 403
    pyautogui.press('volumeup')
    return "OK"

@app.route('/media/voldown')
def vol_down():
    if not check_key(request): return "Unauthorized", 403
    pyautogui.press('volumedown')
    return "OK"

@app.route('/media/mute')
def mute():
    if not check_key(request): return "Unauthorized", 403
    pyautogui.press('volumemute')
    return "OK"

# ---------------- FILE ----------------
@app.route('/save', methods=['POST'])
def save():
    if request.args.get("key") != SECRET:
        return "Unauthorized", 403
    data = request.json.get("text", "")
    with open("notes.txt", "a", encoding="utf-8") as f:
        f.write(data + "\n")
    return "Saved"

# ---------------- STATS ----------------
@app.route('/stats')
def stats():
    if not check_key(request): return "Unauthorized", 403
    # Use interval=0.1 to get an accurate CPU reading
    return jsonify({
        "cpu": psutil.cpu_percent(interval=0.1),
        "ram": psutil.virtual_memory().percent
    })

# ---------------- RUN ----------------
if __name__ == '__main__':
    # Ensure it's running on your local network
    app.run(host='0.0.0.0', port=5000)