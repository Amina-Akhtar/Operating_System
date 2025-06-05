#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

bool decrypt(const char *input_file, const char *output_file) 
{
    int in_file = open(input_file, O_RDONLY);
    int out_file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (in_file < 0 || out_file < 0) 
    {
        perror("Error opening files!");
        if (in_file >= 0) 
            close(in_file);
        if (out_file >= 0) 
            close(out_file);
        return false;
    }
    char file_data;
    while (read(in_file, &file_data, 1) > 0) 
    {
        if (isalpha(file_data)) 
        {
            if (file_data >= 'A' && file_data <= 'Z') 
            {
                file_data = (file_data - 'A' - 1 + 26) % 26 + 'A';
            } 
            else if (file_data >= 'a' && file_data <= 'z') 
            {
                file_data = (file_data - 'a' - 1 + 26) % 26 + 'a';
            }
        }
        write(out_file, &file_data, 1);
    }
    close(in_file);
    close(out_file);
    return true;
}

int validity(char file_data, int index) 
{
    if (index == 3 || index == 6) 
    {
        return (file_data == '-');
    } 
    else 
    {
        return isdigit(file_data);
    }
}

bool redact(const char *input_file, const char *output_file) 
{
    int in_file = open(input_file, O_RDONLY);
    int out_file = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (in_file < 0 || out_file < 0) 
    {
        perror("Error opening files");
        if (in_file >= 0) 
            close(in_file);
        if (out_file >= 0)  
            close(out_file);
        return false;
    }
    char file_data;
    char buffer[12];
    int index = 0;
    while (read(in_file, &file_data, 1) > 0) 
    {
        if (isdigit(file_data) || file_data == '-') 
        {
            buffer[index++] = file_data;
            if (index == 11) 
            {
                int valid = 1;
                for (int i = 0; i < 11; i++) 
                {
                    if (!validity(buffer[i], i)) 
                    {
                        valid = 0;
                        break;
                    }
                }
                if (valid) 
                {
                    write(out_file, "[REDACTED]", 10);
                }
                else 
                {
                    write(out_file, buffer, 11);
                }
                index = 0;
            }
        } 
        else 
        {
            if (index > 0) 
            {
                int valid = 1;
                for (int i = 0; i < index; i++) 
                {
                    if (!validity(buffer[i], i)) 
                    {
                        valid = 0;
                        break;
                    }
                }
                if (valid) 
                {
                    write(out_file, "[REDACTED]", 10);
                } 
                else 
                {
                    write(out_file, buffer, index);
                }
                index = 0;
            }
            write(out_file, &file_data, 1);
        }
    }

    if (index > 0) 
    {
        int valid = 1;
        for (int i = 0; i < index; i++) 
        {
            if (!validity(buffer[i], i)) 
            {
                valid = 0;
                break;
            }
        }
        if (valid) 
        {
            write(out_file, "[REDACTED]", 10);
        }
        else 
        {
            write(out_file, buffer, index);
        }
    }
    close(in_file);
    close(out_file);
    return true;
}

int main() 
{
    char input_file[256];
    char output_file[256];
    int choice;

    printf("Enter the name of input file: ");
    scanf("%255s", input_file);
    printf("Enter the name of output file: ");
    scanf("%255s", output_file);
    printf("Select Transformation choice:\n");
    printf("1. Decrypt\n");
    printf("2. Redact\n");
    scanf("%d", &choice);

    while (choice < 1 || choice > 2) 
    {
        printf("Invalid choice!\n");
        printf("1. Decrypt\n");
        printf("2. Redact\n");
        scanf("%d", &choice);
    }

    pid_t process = fork();

    if (process < 0)
    {
        perror("Fork failed!");
        return 1;
    }

    if (process == 0) // Child process
    {
        if (choice == 1) 
        {
            if (!decrypt(input_file, output_file)) 
            {
                fprintf(stderr, "Transformation failed!\n");
                exit(1);
            }
        }
        else if (choice == 2) 
        {
            if (!redact(input_file, output_file)) 
            {
                fprintf(stderr, "Transformation failed!\n");
                exit(1);
            }
        }
        exit(0); 
    } 
    else // Parent process
    {
        int status;
        if (waitpid(process, &status, 0) == -1) 
        {
            perror("Error waiting for child process");
            return 1;
        }
            int exit_status = WEXITSTATUS(status);
            if (exit_status == 0) 
            {
                printf("Transformation completed!\n");
            }
    }

    return 0;
}

