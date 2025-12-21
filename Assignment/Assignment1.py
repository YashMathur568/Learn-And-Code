import random

# Refactored Number Guessing Game
# Improvements made:
# Clear, meaningful names
# No global dependencies
# SRP respected
# Easier to test & extend
# Bug fixed (s parameter)

def is_valid_guess(user_input):
    """Checks if input is a number between 1 and 100"""
    return user_input.isdigit() and 1 <= int(user_input) <= 100


def get_user_guess():
    """Prompts user until a valid guess is entered"""
    while True:
        user_input = input("Guess a number between 1 and 100: ")
        if is_valid_guess(user_input):
            return int(user_input)
        print("Invalid input. Please enter a number between 1 and 100.")


def evaluate_guess(guess, target_number):
    """Compares guess with target number"""
    if guess < target_number:
        print("Too low. Guess again.")
        return False
    elif guess > target_number:
        print("Too high. Guess again.")
        return False
    return True


def play_number_guessing_game():
    target_number = random.randint(1, 100)
    guess_count = 0

    while True:
        guess = get_user_guess()
        guess_count += 1

        if evaluate_guess(guess, target_number):
            print(f"You guessed it in {guess_count} guesses!")
            break


play_number_guessing_game()
