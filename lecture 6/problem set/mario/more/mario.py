from cs50 import get_int


def main():
    h = get_height()
    print_pyramid(h)


# get the desired pyramid height from the user
def get_height():
    # prompt the user until they enter a valid integer
    v = None
    while not v or v < 1 or v > 8:
        v = get_int("Pyramid height (1 - 8): ")

    return v


# prints the pyramid of the desired height to the console
def print_pyramid(height):
    # iterate over the height to print each line
    for i in range(height):
        line = ""
        spaces = height - i - 1

        for j in range(height):
            if (j < spaces):
                line += " "
            else:
                line += "#"
        line += " "

        print(f"{line}{reverse(line)}")


# returns the characters of the string in reverse order
# and strips the extra space at the end
def reverse(string):
    return string[::-1].rstrip()


if __name__ == "__main__":
    main()