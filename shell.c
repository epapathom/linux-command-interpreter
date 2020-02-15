#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_LEN 512

char line[BUFFER_LEN];                   // Get command line.
char store_line[BUFFER_LEN][BUFFER_LEN]; // Store lines in 2D array.
char *argv[100];                         // User command.
char *temp[100];                         // Temp user command.
char *path = "/bin/";                    // Set path at bin.
char progpath[20];                       // Full file path.
FILE *fp;                                // Open batch file.
int k = 0;                               // Count lines in batch file.
int count = 0;                           // Count commands in the line.

int categorize();
int execute();

int main(int argc_bat, char *argv_bat[])
{
    if (argv_bat[1] != NULL) // If there is a batch file as an arguement.
    {
        fp = fopen(argv_bat[1], "r"); // Open batch file.
        if (fp == NULL)               // If batch file is empty, exit.
        {
            exit(1);
        }
        while (fgets(store_line[k], sizeof(store_line[k]), fp) != NULL) // Get lines and store them in the 2D array.
        {
            strcpy(line, store_line[k]); // Copy the line to specific variable.
            size_t ln = strlen(line);    // Get line's length.
            if (line[ln - 1] == '\n')    // Delete new lines (\n).
            {
                line[ln - 2] = '\0';
            }
            categorize(); // Categorize commands.
            k++;
        }
        fclose(fp);
    }
    else
    {
        while (1)
        {
            printf("papathomas_8692> "); // Print the shell prompt.

            if (!fgets(line, BUFFER_LEN, stdin)) // Get the command(s) and put it/them in line.
            {
                exit(0); // If user hits CTRL+D, exit.
            }

            size_t length = strlen(line); // Delete new lines (\n).
            if (line[length - 1] == '\n')
            {
                line[length - 1] = '\0';
            }
            categorize(); // Categorize commands.
        }
    }
}

int categorize() // This function categorizes the commands of the line.
{
    if (strcmp(line, "quit") == 0) // Check if command is quit.
    {
        exit(0);
    }

    char *temp_command;
    char *ret;
    count = 0;

    if ((ret = strchr(line, ';')) != NULL) // Check if there is ';' in the line.
    {
        temp_command = strtok(line, ";");
        while (temp_command != NULL) // Split commands into separate commands when divided with semicolons.
        {
            temp[count] = temp_command;
            temp_command = strtok(NULL, ";");
            count++;
        }
        execute(count); // Execute commands that are divided by ';'.
    }

    else if ((ret = strchr(line, '&')) != NULL) // Check if there is '&&' in the line.
    {
        temp_command = strtok(line, "&&");
        while (temp_command != NULL) // Split commands into separate commands when divided with '&&'.
        {
            temp[count] = temp_command;
            temp_command = strtok(NULL, "&&");
            count++;
        }
        execute(count); // Execute commands that are divided by '&&'.
    }

    else // If there is only one command in the line.
    {
        temp_command = strtok(line, ";");
        while (temp_command != NULL)
        {
            temp[count] = temp_command;
            temp_command = strtok(NULL, ";");
            count++;
        }
        execute(count); // Execute a single command.
    }
}

int execute(int argc) // This function executes the commands of the line.
{
    for (int k = 0; k < argc; k++)
    {
        char *token;                  // Split selected command into separate strings.
        token = strtok(temp[k], " "); // A command is selected from the various commands inputed.
        int i = 0;
        while (token != NULL)
        {
            argv[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        argv[i] = NULL; // Set last value to NULL for execvp.

        strcpy(progpath, path);    // Copy /bin/ to file path.
        strcat(progpath, argv[0]); // Add program to path.

        for (i = 0; i < strlen(progpath); i++) // Delete newline.
        {
            if (progpath[i] == '\n')
            {
                progpath[i] = '\0';
            }
        }
        int pid = fork(); // Fork child.

        if (pid == 0) // Child.
        {
            execvp(progpath, argv);
            fprintf(stderr, "The command you inserted doesn't exist.\n");
        }
        else // Parent.
        {
            wait(NULL);
        }
    }
}