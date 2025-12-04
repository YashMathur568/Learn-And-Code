def calculate_armstrong_sum(input_number):
     # Initializing Sum and Number of Digits
    digit_power_sum = 0
    digit_count = 0

    # Calculating Number of individual digits
    temp_number = input_number
    while temp_number > 0:
        digit_count += 1
        temp_number //= 10

    # Finding Armstrong Number
    temp_number = input_number
    while temp_number > 0:
        digit = temp_number % 10
        digit_power_sum += digit ** digit_count
        temp_number //= 10

    return digit_power_sum


# End of Function

# User Input

user_entered_number = int(
    input("\nPlease enter the number to check for Armstrong: ")
)

if user_entered_number == calculate_armstrong_sum(user_entered_number):
    print("\n", user_entered_number, "is an Armstrong number.\n")
else:
    print("\n", user_entered_number, "is NOT an Armstrong number.\n")
