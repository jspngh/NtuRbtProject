import speech_recognition as sr
from difflib import SequenceMatcher
import time


def heartbeat():
    with open('last_callback.txt', 'w') as f:
        f.write(time.strftime("%-S", time.localtime()) + "\n")

def find_best_match(phrase, sentences):
    similarities = [similarity(phrase, s) for s in sentences]
    max_similarity = max(similarities)
    return sentences[similarities.index(max_similarity)], max_similarity

def read_sentences(file_name):
    return [line.rstrip('\n') for line in open(file_name)]

def similarity(a, b):
    return SequenceMatcher(None, a, b).ratio()

def phrase_callback(model, audio):
    heartbeat()
    try:
        phrase = model.recognize_google(audio)
        best_match, score = find_best_match(phrase, sentences)
        if score < 0.6:
            best_match = ""

        print "score:           ", score
        print "your sentence:   ", phrase
        print "best match:      ", best_match
    except:
        pass

def main():
    heartbeat()
    model = sr.Recognizer()
    source = sr.Microphone()
    model.operation_timeout = 5
    model.listen_in_background(source, phrase_callback, phrase_time_limit = 4)

    while True:
        try:
            pass
        except KeyboardInterrupt:
            break

# main
sentences = read_sentences('sentences.txt')
main()
