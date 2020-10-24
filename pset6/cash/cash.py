from cs50 import get_float

n = -1.00
while n < 0:
    n = get_float("Change owed: ")

# Define some data
answer = 0
coins = [0.25, 0.10, 0.05, 0.01]

# Some checks and calculation
for coin in coins:
    answer = answer + int(n // coin)
    n = round(n % coin, 2)
    if n == 0:
        break

print(answer)