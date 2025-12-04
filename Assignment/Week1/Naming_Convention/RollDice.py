import random


def roll_dice(max_sides):
    dice_value = random.randint(1, max_sides)
    return dice_value


def main():
    number_of_sides = 6
    keep_rolling = True

    while keep_rolling:
        user_choice = input("Ready to roll? Enter Q to Quit: ")

        if user_choice.lower() != "q":
            rolled_number = roll_dice(number_of_sides)
            print("You have rolled a", rolled_number)
        else:
            keep_rolling = False


main()
