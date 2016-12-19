import speech_recognition as sr
from difflib import SequenceMatcher

def find_best_match(phrase, sentences):
    similarities = [similarity(phrase, s) for s in sentences]
    return sentences[similarities.index(max(similarities))]

def read_sentences(file_name):
    return [line.rstrip('\n') for line in open(file_name)]

def similarity(a, b):
    return SequenceMatcher(None, a, b).ratio()

def phrase_callback(model, audio):
    try:
        phrase = model.recognize_sphinx(audio)
        best_match = find_best_match(phrase, sentences)
        print "your sentence:   ", phrase
        print "best match:      ", best_match
    except LookupError:
        print "Could not understand audio"

def main():
    model = sr.Recognizer()
    source = sr.Microphone()
    model.listen_in_background(source, phrase_callback)

    while True:
        try:
            pass
        except KeyboardInterrupt:
            break

# main
sentences = read_sentences('sentences.txt')
main()
