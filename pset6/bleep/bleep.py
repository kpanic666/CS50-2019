from cs50 import get_string
from sys import argv


def main():
    # Check that we have only one parameter with banned words file name
    if len(argv) != 2:
        exit("Usage: python bleep.py dictionary")

    # Get the user input to check it for ban words
    in_words = get_string("What message would you like to censor?\n")

    # Open up the file with banned words
    ban_words = set()
    ban_file = open(argv[1], "r")

    # Move all words to set
    for word in ban_file:
        ban_words.add(word.rstrip("\n"))
    ban_file.close

    # Split sense in to the words, hash words from ban and print it
    for in_word in in_words.split():
        if in_word.lower() in ban_words:
            print("*" * len(in_word) + " ", end="")
        else:
            print(in_word + " ", end="")
    print()


if __name__ == "__main__":
    main()
