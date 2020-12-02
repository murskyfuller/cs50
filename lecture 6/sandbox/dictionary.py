words = set()

def load(dictionary):
    # open the file
    file = open(dictionary, "r")

    # each word on its own line
    for line in file:
        words.add(line.lower().rstrip("\n"))

    file.close()

    return True

def check(word):
    return word.lower() in words

def size():
    return len(words)

def unload():
    return True