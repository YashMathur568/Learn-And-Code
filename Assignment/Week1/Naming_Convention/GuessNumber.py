import random


def is_valid_guess(guess_text):
    return guess_text.isdigit() and 1 <= int(guess_text) <= 100


def main():
    secret_number = random.randint(1, 100)
    is_number_guessed = False
    guess_count = 0

    user_guess_text = input("Guess a number between 1 and 100: ")

    while not is_number_guessed:

        if not is_valid_guess(user_guess_text):
            user_guess_text = input(
                "Invalid input. Please enter a number between 1 and 100: "
            )
            continue

        guess_count += 1
        user_guess_number = int(user_guess_text)

        if user_guess_number < secret_number:
            user_guess_text = input("Too low. Guess again: ")
        elif user_guess_number > secret_number:
            user_guess_text = input("Too high. Guess again: ")
        else:
            print("You guessed it in", guess_count, "guesses!")
            is_number_guessed = True


main()
