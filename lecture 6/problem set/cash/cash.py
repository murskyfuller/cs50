from cs50 import get_float
from math import floor


# define the value of the coins that
# can be returned
coins = [25, 10, 5, 1]


def main():
    v = change_owed()
    print(calc_coins(v))


# get the amount of change owed from the user
def change_owed():
    v = None

    # prompt user until they enter a valid value
    while not v or v < 0:
        v = get_float("Change owed: ")

    return int(v * 100)


# calculate the minimum number of coins needed
# to meet the required change
def calc_coins(change):
    current = change
    count = 0

    # keep working until all change due is accounted for
    for c in coins:
        if current == 0:
            break

        num = floor(current / c)
        count += num  # add to running total
        current -= num * c  # subtract the dollar value

    return count


if __name__ == "__main__":
    main()