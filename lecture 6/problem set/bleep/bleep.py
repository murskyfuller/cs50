from cs50 import get_string
from sys import argv, exit


class TrieNode:
    def __init__(self, is_word=False):
        self.is_word = is_word
        self.children = {}

    def child(self, key):
        if key not in self.children:
            return None

        return self.children[key]

    def set_child(self, key, value):
        if not isinstance(value, TrieNode):
            print("Trie node children must be an instance of TrieNode")
            return

        self.children[key] = value


class Trie(TrieNode):
    def __init__(self):
        super().__init__()
        self._is_loaded = False

    # returns the is_loaded prop of the Trie instance
    def is_loaded(self):
        return self._is_loaded

    # sets the is_loaded prop of the Trie instance
    # to the provided value
    def set_loaded(self, value):
        # enforce boolean type
        if not isinstance(value, bool):
            print("Value for is_loaded should be a boolean")
            return

        self._is_loaded = value

    # moves through the Trie, character by
    # character, adding nodes as necessary
    # and marking the last as being a word
    def add_word(self, word):
        trav = self

        # iterate to the last relevant node
        for c in word:
            if trav.child(c) is None:
                new_node = TrieNode()
                trav.set_child(c, new_node)
            trav = trav.child(c)

        # mark it as a word
        if not trav.is_word:
            trav.is_word = True

    # moves through the Trie to find the end
    # node and determine if the provided word
    # exists
    def has_word(self, word):
        trav = self

        lower_case = word.lower()

        for c in lower_case:
            trav = trav.child(c)

            if trav is None:
                return False

        return trav.is_word


root = Trie()


def main():
    # validate the args
    is_valid = validate_argv()

    if not is_valid:
        exit(1)

    # build the trie
    try:
        build_trie()
        if not root.is_loaded():
            print("Failed to load Trie")
            exit(1)
    except FileNotFoundError:
        print(f"Error: dictionary not found at '{argv[1]}'")
        exit(1)

    message = get_string("What message would you like to censor? ")
    censored = censor(message)
    print(censored)


def censor(string):
    words = string.split()

    for ix, word in enumerate(words):
        if root.has_word(word):
            # build the censored string
            censored_word = ""
            for c in word:
                censored_word += "*"
            words[ix] = censored_word

    return " ".join(words)


def build_trie():
    with open(argv[1]) as dict:
        for line in dict:
            root.add_word(line.strip())

        root.set_loaded(True)


def validate_argv():
    result = True

    if len(argv) != 2:
        print("Bleep usage: python ./bleep.py <path to dictionary>")
        result = False

    return result


if __name__ == "__main__":
    main()
