#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POINTER_SIZE 8
enum {_TODO, _WAIT, _DONE};

typedef struct {
    int TodoNum;
    int WaitNum;
} TODOHeader;

typedef struct {
    int Stage;
    char Desc[100];
    int Type;
} Mission;

void
GreetingMessage()
{
    // TODO: Welcome Messages
    printf("Welcome to todoer!\n");
    return;
}

void
FillHeader(FILE *Config)
{
    TODOHeader Head;

    Head.TodoNum = 0;
    Head.WaitNum = 0;

    fwrite(&Head , sizeof(TODOHeader), sizeof(Head), Config);
    return;
}

FILE *
GetConfig()
{
    // TODO: Deal with file path
    char const *path = "/home/wheatdog/.todo";
    FILE *Config = fopen(path, "rb+");

    if (!Config)
    {
        printf("Create %s ...\n", path);
        Config = fopen(path, "wb+");
        FillHeader(Config);
    }

    return Config;
}

int
GetCommand(char *arg)
{
    char *Command[] = {"ls", "new", "edit"};
    for (int count = 0; count < (sizeof(Command) / POINTER_SIZE); count++)
    {
        if (strcmp(arg, Command[count]) == 0)
        {
            printf("%s\n", Command[count]);
            return count;
        }
    }
    printf("no such command\n");
    exit(-1);
}

int 
main(int argc, char *argv[])
{

    //GreetingMessage();
    
    // TODO: deal with different commands
    FILE *Config = GetConfig();

    int Command = GetCommand(argv[1]);

    printf("%d\n", Command);

    TODOHeader Header;
    fread(&Header, sizeof(TODOHeader), 1, Config);   

    rewind(Config);
    fwrite(&Header , sizeof(TODOHeader), sizeof(Header), Config);

    fclose(Config);
    
    // TODO: Time Remain
    time_t t = time(NULL);
    struct tm *local = localtime(&t);
    char str[100];
    strftime(str,80,"%G %m %H %n", local);
    printf("%s", str);

    return 0;
}
