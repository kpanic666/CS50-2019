#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <crypt.h>

#define ALLOWED_NUM_ARGS 2
#define ALLOWED_HASH_SIZE 13
#define PWD_LENGHT 5

int bruteforce(const string inHash, string startPwd);
void inc_pwd(string inPwd);

int main(int argc, string argv[])
{
    bool error = false;

    // Check numbers of command line arguments. We must receive only 1 argument
    // with the hash
    if (argc == ALLOWED_NUM_ARGS)
    {
        const int keyArgInd = ALLOWED_NUM_ARGS - 1;

        // Check that input Hash consist only from [a-zA-Z0-9./]
        // and it is 13-character in lenght
        for (int i = 0, n = strlen(argv[keyArgInd]); i < n; i++)
        {
            if (n != ALLOWED_HASH_SIZE || (isalnum(argv[keyArgInd][i]) == 0 &&
                argv[keyArgInd][i] != '.' && argv[keyArgInd][i] != '/'))
            {
                error = true;
                break;
            }
        }

        // If hash is fine. Try to crack the password
        if (error == false)
        {
            // 1. We will try to use dictionary attack
            //string genHash = crypt("AaAa", salt);
            //printf("%s\n", genHash);

            // 2. If dictionary attack didn't help us - than bruteforce.
            char startPwd[PWD_LENGHT + 1] = {'A'};

            int result = bruteforce(argv[keyArgInd], startPwd);
            //printf("result: %i\n", result);
            if (result == 0)
            {
                printf("%s\n", startPwd);
                return 0;
            }
            else
            {
                printf("Failed! Not found password for hash: %s\n", argv[keyArgInd]);
                return 2;
            }
        }
    }
    else
    {
        error = true;
    }

    if (error == true)
    {
        printf("Usage: ./crack hash\n");
        return 1;
    }
}

int bruteforce(const string inHash, string startPwd)
{
    char salt[2] = {inHash[0], inHash[1]};
    char endPwd[PWD_LENGHT + 1];
    string outHash;

    // Fill the endPwd. It will be 'zzzz'... Depends on how much in PWD_LENGHT
    for (int i = 0; i < PWD_LENGHT; i++)
    {
        endPwd[i] = 'z';
    }

    //printf("Starting bruteforce the password from '%s' to '%s':\n", startPwd, endPwd);

    do
    {
        //printf("'%s' -", startPwd);
        outHash = crypt(startPwd, salt);

        if (strcmp(outHash, inHash) == 0)
        {
            //printf("Success\n");
            return 0;
        }
        else if (strcmp(startPwd, endPwd) != 0)
        {
            //printf("Failed\n");
            inc_pwd(startPwd);
        }
        else
        {
            //printf("Failed\n");
            return 1;
        }
    }
    while (true);
}

void inc_pwd(string inPwd)
{
    int pwdInd = 0;
    bool switchNextLetter;

    do
    {
        switchNextLetter = false;

        if (inPwd[pwdInd] < 'Z')
        {
            inPwd[pwdInd]++;
        }
        else if (inPwd[pwdInd] < 'a')
        {
            inPwd[pwdInd] = 'a';
        }
        else if (inPwd[pwdInd] < 'z')
        {
            inPwd[pwdInd]++;
        }
        else
        {
            inPwd[pwdInd] = 'A';
            switchNextLetter = true;
            pwdInd++;
            if (inPwd[pwdInd] < 'A')
            {
                inPwd[pwdInd] = 'A';
                break;
            }
        }
    }
    while (switchNextLetter == true && pwdInd < PWD_LENGHT);
}