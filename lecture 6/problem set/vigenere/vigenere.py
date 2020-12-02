from sys import argv, exit
from re import compile
from cs50 import get_string


def main():
    # validate CLI args
    valid = validate_args()
    if not valid:
        exit(1)

    # get input from user
    plain = get_string("plaintext: ")

    # print the encoded input
    print(f"ciphertext: {encode(plain)}")


# encodes the provided plain text
def encode(plain):
    k = argv[1]
    k_len = len(k)
    k_ix = 0
    result = ""

    # iterate each character in the plain text
    for c in plain:
        new_char = encoded_char(c, k[k_ix])

        # use the encoded char or the original
        # if None was returned
        if new_char:
            result += new_char
            k_ix = (k_ix + 1) % k_len
        else:
            result += c

    return result


# encode the specific char based on its cipher character
def encoded_char(plain_char, cipher_char):
    a = ord('A')
    z = ord('Z')

    # if the provided character is an alphabetic character
    # encode it
    if plain_char.isalpha():
        shift = ord(cipher_char.upper()) - a
        new_char_ord = (ord(plain_char.upper()) + shift) % (z + 1)

        # if the range has been exceeded then
        # come back around to the beginning
        if new_char_ord < a:
            new_char_ord += a
        new_char = chr(new_char_ord)

        # preserve the original casing
        if plain_char.islower():
            new_char = new_char.lower()

        return new_char


# validates the CLI arguments
def validate_args():
    length = len(argv)

    # make sure number of args is valid
    if length is not 2:
        print("Usage: python vigenere.py <keyword>")
        return False

    # make sure the keyword is valid
    if not is_alpha_str(argv[1]):
        print("Keyword must be an alphabetic string")
        return False

    return True


# returns True if the provided string only contains alphabetic characters
def is_alpha_str(str):
    reg = compile('^[a-zA-Z]+$')
    return reg.match(str) is not None


if __name__ == "__main__":
    main()