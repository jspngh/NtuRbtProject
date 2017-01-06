import speech_recognition as sr
from difflib import SequenceMatcher
import time
import pandas as pd
import socket

time_cmd_send = None

def send_action(command):
    global time_cmd_send
    time_cmd_send = time.time()
    # HOST = 'localhost'
    # PORT = 50007
    # s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # s.connect((HOST, PORT))
    # s.sendall(str(command))
    # s.close()

def heartbeat():
    with open('last_callback.txt', 'w') as f:
        f.write(time.strftime("%-S", time.localtime()) + "\n")

def find_best_match(phrase):
    similarities = [similarity(phrase, s) for s in sentences['phrase']]
    max_similarity = max(similarities)
    return similarities.index(max_similarity), max_similarity

def read_sentences(file_name):
    return [line.rstrip('\n') for line in open(file_name)]

def similarity(a, b):
    return SequenceMatcher(None, a, b).ratio()

def phrase_callback(model, audio):
    heartbeat()
    print "in callback"
    if time_cmd_send is not None and time.time() - time_cmd_send < 5:
        print "busy..."
        return
    try:
        phrase = model.recognize_google(audio)
        index_best_match, score = find_best_match(phrase)
        if score > 0.6:
            print "score:           ", score
            print "your sentence:   ", phrase
            print "best command:    ", sentences['command'][index_best_match]
            send_action(sentences['command_id'][index_best_match])
        else:
            print "score too low..."
    except:
        print "bu dong"
        pass

def main():
    heartbeat()
    model = sr.Recognizer()
    model.energy_threshold = 4000
    source = sr.Microphone()
    model.operation_timeout = 5
    model.listen_in_background(source, phrase_callback, phrase_time_limit = 4)

    while True:
        try:
            pass
        except KeyboardInterrupt:
            break

# main
sentences = pd.read_csv("./sentences.txt")
print sentences
# send_action("")
main()
