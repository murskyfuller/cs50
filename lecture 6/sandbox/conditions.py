from cs50 import get_int

# Get x from user
x = get_int("x: ")
y = get_int("y: ")

# compare x and y
if x < y:
    print("x is less than y")
elif x > y:
    print("x is greater than y")
else:
    print("x is equal to y")