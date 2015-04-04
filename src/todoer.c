#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POINTER_SIZE 8
enum {_TODO, _WAIT, _DONE};
enum {_ls, _add, _edit, _rm};

typedef struct {
    int TotalNum;
    int TodoNum;
    int WaitNum;
} TODOHeader;

typedef struct {
    char ID[7];
    char Deadline[20];
    char Desc[100];
    int Stage;
    char Type[20];
} Mission;

typedef struct {
    int MainCommand;
    int OptionNum;
    char **Option;
} Command;

void
InitHeader(FILE *Config)
{
    TODOHeader Head;

    Head.TotalNum = 0;
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
        InitHeader(Config);
    }

    TODOHeader Header;
    rewind(Config);
    fread(&Header, sizeof(TODOHeader), 1, Config);   
    fclose(Config);
    return Header;
}

int
GetMainCommand(char *arg)
{
    char *Command[] = {"ls", "add", "edit", "rm"};
    for (int count = 0; count < (sizeof(Command) / POINTER_SIZE); count++)
    {
        if (strcmp(arg, Command[count]) == 0)
        {
            return count;
        }
    }
    printf("error: no such command\n");
    exit(-1);
}

Command
GetCommand(int argc, char **argv)
{
    Command Input;
    Input.MainCommand = GetMainCommand(argv[2]);
    Input.OptionNum = argc - 3;
    Input.Option = (char **)malloc(Input.OptionNum * sizeof(char *)); 

    for (int OptionCount = 0;
            OptionCount < Input.OptionNum;
            OptionCount++)
    {
        Input.Option[OptionCount] = (char *)malloc((strlen(argv[3 + OptionCount]) + 1) * 
                                                   sizeof(char));
        strcpy(Input.Option[OptionCount], argv[3 + OptionCount]);
    }

    return Input;
}

void
FreeCommandOption(Command *Input)
{
    for (int OptionCount = 0;
            OptionCount < Input->OptionNum;
            OptionCount++)
    {
        free(Input->Option[OptionCount]);
    }

    free(Input->Option);
    return;
}

void
SetID(char *ID)
{
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    char str[20];
    strftime(str,80,"%g-%m-%d-%H-%M-%S", local);

    char table[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int length = strlen(table);

    srand (time(NULL));

    char *token;
    token = strtok(str,"-");
    while (token != NULL)
    {
        int shift = rand() % length;
        sprintf(ID++,"%c", table[(shift + atoi(token)) % length]);
        token = strtok (NULL, "-");
    }

    ID[6] = '\0';

    return;
}

void
DoCommand(char const *Path, Command *Input, TODOHeader *Header)
{
    switch(Input->MainCommand)
    {
        case _ls:
        {
            FILE *TodoFile = fopen(Path, "rb");

            fseek(TodoFile, sizeof(TODOHeader), SEEK_SET);

            Mission TodoObj;

            if (!Header->TodoNum)
            {
                printf("There isn't any thing in TODOer! HURRAY!\n");
            }
            else
            {
                printf("%-9s%-15s%-30s%-10s%-10s\n", "ID",
                                                     "Deadline", 
                                                     "Description",
                                                     "Stage",
                                                     "Type");
                for (int MissionCount = 0;
                     MissionCount < Header->TotalNum;
                     MissionCount++)
                {
                    fread(&TodoObj, sizeof(TodoObj), 1, TodoFile);
                    printf("%-9s%-15s%-30s%-10d%-10s\n", TodoObj.ID, TodoObj.Deadline, TodoObj.Desc, TodoObj.Stage, TodoObj.Type);
                }
            }

            fclose(TodoFile);

        } break;

        case _add:
        {
            FILE *TodoFile = fopen(Path, "ab");
            Mission TodoObj;

            // TODO: Generate unique ID
            SetID(TodoObj.ID);

            printf("Description: \t");
            fgets(TodoObj.Desc, 100, stdin);
            TodoObj.Desc[strlen(TodoObj.Desc) - 1] = '\0';

            printf("Deadline: \t");
            fgets(TodoObj.Deadline, 20, stdin);
            TodoObj.Deadline[strlen(TodoObj.Deadline) - 1] = '\0';

            printf("Type: \t\t");
            fgets(TodoObj.Type, 20, stdin);
            TodoObj.Type[strlen(TodoObj.Type) - 1] = '\0';

            TodoObj.Stage = _TODO;

            Header->TotalNum++;
            Header->TodoNum++;

            fwrite(&TodoObj, sizeof(TodoObj), 1, TodoFile);
            fclose(TodoFile);

        } break;

        case _edit:
        {
        } break;

        case _rm:
        {
            if (!Input->OptionNum)
            {
                printf("error: need at least one more argument as ID to be removed\n");
                printf("usage: todoer <path-to-todo> rm <ID>\n");
                exit(-1);
            }

            if (!(Header->TodoNum + Header->WaitNum))
            {
                printf("error: nothing can delete in TODOer\n");
                exit(-1);
            }

            for (int OptionCount = 0;
                 OptionCount < Input->OptionNum;
                 OptionCount++)
            {
                // TODO: Load the TodoObjs from file
                FILE *TodoFile = fopen(Path, "rb");

                fseek(TodoFile, sizeof(TODOHeader), SEEK_SET);

                Mission Buf[Header->TotalNum];

                fread(Buf, sizeof(Mission), Header->TotalNum, TodoFile);

                fclose(TodoFile);
                
                // TODO: find the match TodoObj
                int Found = 0;
                int FoundMissionOrder;
                for (int MissionCount = 0;
                     MissionCount < Header->TodoNum && !Found;
                     MissionCount++)
                {
                    if (strcmp(Input->Option[OptionCount], Buf[MissionCount].ID) == 0)
                    {
                        Found = 1;
                        FoundMissionOrder = MissionCount;
                    }

                }

                if (!Found)
                {
                    printf("error: ID '%s' doesn't exist, need a valid ID\n", 
                           Input->Option[OptionCount]);
                }
                else
                {
                    // TODO: delete it and recreate a todo file
                    
                    FILE *TodoFile = fopen(Path, "wb");

                    Header->TotalNum--;
                    Header->TodoNum--;
                    fwrite(Header , sizeof(TODOHeader), 1, TodoFile);
                    fwrite(Buf , sizeof(Mission), FoundMissionOrder, TodoFile);
                    fwrite(Buf + FoundMissionOrder + 1 , sizeof(Mission),
                           Header->TotalNum - FoundMissionOrder, TodoFile);

                    fclose(TodoFile);
                }


            }

        } break;
    }
}

void
HeaderUpdate(char *Path, TODOHeader *Header)
{
    // TODO: Header Update
    FILE *TodoFile = fopen(Path, "rb+");
    fwrite(Header , sizeof(TODOHeader), 1, TodoFile);
    fclose(TodoFile);
}

int 
main(int argc, char *argv[])
{

    // TODO: Deal with different commands
    if (argc == 1)
    {
        printf("usage: todoer <path-to-todo> <command> [<option>]\n");
        exit(-1);
    }
    else if (argc == 2)
    {
        char *Path = argv[1];
        TODOHeader Header = GetConfig(Path);
        printf("usage: todoer <path-to-todo> <command> [<option>]\n");

    }
    else
    {
        char *Path = argv[1];
        TODOHeader Header = GetConfig(Path);

        Command Input = GetCommand(argc, argv);
        DoCommand(Path, &Input, &Header);
        FreeCommandOption(&Input);

        HeaderUpdate(Path, &Header);

    }

    return 0;
}

// TODO: Time Remain
#if 0
        time_t t = time(NULL);
        struct tm *local = localtime(&t);
        char str[100];
        strftime(str,80,"%G %m %H %n", local);
        printf("%s", str);
#endif
