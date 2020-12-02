from cs50 import get_string

# instantiate the name variable
name = ""

# keep asking user for their name
# until they enter something
while not name:
    name = get_string("Name: ")

# print the name
print(f"hello, {name}")