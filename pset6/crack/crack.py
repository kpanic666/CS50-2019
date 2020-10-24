from sys import argv
from crypt import crypt
import string
# Newbie code, there are huge space for optimisation


def main():
    error = False
    ihash = ""

    if len(argv) == 2:
        # Check that input Hash consist only from [a-zA-Z0-9./]
        # and it is 13-character in lenght
        ihash = str(argv[1])
        for c in ihash:
            if len(ihash) != 13 or (c.isalnum() == False and
                                    c != '.' and c != '/'):
                error = True
                break

        # If hash is fine. Try to crack the password
        if error == False:
            result, findPwd = bruteforce(ihash, "A")

            if result == True:
                print(findPwd)
            else:
                exit("Failed. Cant crack the hash")
    else:
        error = True

    if error == True:
        exit("Usage: python crack.py hash")


# Increment password from "A" to "ZZZZZ"
def inc_pwd(inPwd):
    pwdInd = 0
    switchNextLetter = True
    pwd_list = [[] for i in range(5)]

    # Because strings in Python is immutable i find a newbie way to resolve this
    for i in range(len(inPwd)):
        pwd_list[i] = inPwd[i]

    # Iterate over all letters in password string. From left to right
    while switchNextLetter == True and pwdInd < 5:
        switchNextLetter = False

        if pwd_list[pwdInd] < 'Z':
            pwd_list[pwdInd] = chr(ord(pwd_list[pwdInd]) + 1)
        elif pwd_list[pwdInd] < 'a':
            pwd_list[pwdInd] = 'a'
        elif pwd_list[pwdInd] < 'z':
            pwd_list[pwdInd] = chr(ord(pwd_list[pwdInd]) + 1)
        else:
            pwd_list[pwdInd] = 'A'
            switchNextLetter = True
            pwdInd += 1
            if pwd_list[pwdInd] == []:
                pwd_list[pwdInd] = 'A'
                break

    inPwd = ""
    for c in pwd_list:
        if c != []:
            inPwd = inPwd + c
    return inPwd


# Calc the hash, take new pass if it fails
def bruteforce(inHash, spwd):
    end_pwd = "zzzzz"

    while True:
        outHash = crypt(spwd, inHash[:2])
        # print(spwd)
        if outHash == inHash:
            return True, spwd
        elif spwd != end_pwd:
            spwd = inc_pwd(spwd)
        else:
            return False


if __name__ == "__main__":
    main()