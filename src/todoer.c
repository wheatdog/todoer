#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POINTER_SIZE 8
enum {_TODO, _WAIT, _DONE};
enum {_ls, _new, _edit};

typedef struct {
    int TodoNum;
    int WaitNum;
} TODOHeader;

typedef struct {
    int Order;
    char Deadline[20];
    char Desc[100];
    int Stage;
    char Type[20];
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

    fwrite(&Head , sizeof(TODOHeader), 1, Config);
    return;
}

TODOHeader
GetConfig(char const *path)
{
    // TODO: Deal with file path
    FILE *Config = fopen(path, "rb+");

    if (!Config)
    {
        printf("Create %s ...\n", path);
        Config = fopen(path, "wb+");
        FillHeader(Config);
    }

    TODOHeader Header;
    rewind(Config);
    fread(&Header, sizeof(TODOHeader), 1, Config);   
    fclose(Config);
    return Header;
}

int
GetCommand(char *arg)
{
    char *Command[] = {"ls", "new", "edit"};
    for (int count = 0; count < (sizeof(Command) / POINTER_SIZE); count++)
    {
        if (strcmp(arg, Command[count]) == 0)
        {
            return count;
        }
    }
    printf("no such command\n");
    exit(-1);
}

void
PrintTodo(Mission *todo)
{
    printf("Order = %d\n", todo->Order);
    printf("Deadline = %s\n", todo->Deadline);
    printf("Desc = %s\n", todo->Desc);
    printf("Stage = %d\n", todo->Stage);
    printf("Type = %s\n", todo->Type);
    return;
}

int 
main(int argc, char *argv[])
{

    //GreetingMessage();
    
    // TODO: Let path become argv[1]
    char const *path = "/home/wheatdog/.todo";

    TODOHeader Header = GetConfig(path);

    // TODO: Deal with different commands
    if (argc == 1)
    {
        printf("Please insert arguments.\n");
        exit(-1);
    }
    else
    {
        int Command = GetCommand(argv[1]);

        switch(Command)
        {
            case _ls:
            {
                FILE *TodoFile = fopen(path, "rb");

                fseek(TodoFile, 
                      sizeof(TODOHeader),
                      SEEK_SET);

                Mission TodoObj;

                if (!Header.TodoNum)
                {
                    printf("There isn't any thing in TODOer! HURRAY!\n");
                }
                else
                {
                    printf("%-9s%-10s%-30s%-10s%-10s\n", "Order", "Deadline", 
                                                         "Description", "Stage", "Type");
                    for (int MissionCount = 0;
                         MissionCount < Header.TodoNum + Header.WaitNum;
                         MissionCount++)
                    {
                            fread(&TodoObj, sizeof(TodoObj), 1, TodoFile);
                            //PrintTodo(&TodoObj);
                            printf("#%04d    %-10s%-30s%-10d%-10s\n", TodoObj.Order, TodoObj.Deadline, TodoObj.Desc, TodoObj.Stage, TodoObj.Type);
                    }
                }

                fclose(TodoFile);
            } break;

            case _new:
            {

                FILE *TodoFile = fopen(path, "ab");

                Mission TodoObj;

                TodoObj.Order = (Header.TodoNum + Header.WaitNum);

                printf("Description: \t");
                fgets (TodoObj.Desc, 100, stdin);
                TodoObj.Desc[strlen(TodoObj.Desc) - 1] = '\0';

                printf("Deadline: \t");
                fgets (TodoObj.Deadline, 20, stdin);
                TodoObj.Deadline[strlen(TodoObj.Deadline) - 1] = '\0';

                printf("Type: \t\t");
                fgets (TodoObj.Type, 20, stdin);
                TodoObj.Type[strlen(TodoObj.Type) - 1] = '\0';

                TodoObj.Stage = _TODO;
                
                Header.TodoNum++;

                fwrite(&TodoObj, sizeof(TodoObj), 1, TodoFile);

                fclose(TodoFile);

            } break;

            case _edit:
            {
                printf("%d\n", _edit);
            } break;

        }

        // TODO: Header Update
        FILE *TodoFile = fopen(path, "rb+");
        fwrite(&Header , sizeof(TODOHeader), 1, TodoFile);
        fclose(TodoFile);

        // TODO: Time Remain
#if 0
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        char str[100];
        strftime(str,80,"%G %m %H %n", local);
        printf("%s", str);
#endif
    }

    return 0;
}
