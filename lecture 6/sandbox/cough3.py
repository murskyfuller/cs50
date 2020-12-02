from cs50 import get_int

def main():
    n = get_int("n: ")
    cough(n)

def cough(n):
    for i in range(n):
        print("cough")

if __name__ == "__main__":
    main()