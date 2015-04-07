#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define POINTER_SIZE 8
enum {_TODO, _WAIT, _DONE};
enum {_ls, _add, _edit, _rm};

// TODO: Change to use argp API for command line arguments

typedef struct {
    int TotalNum;
    int TodoNum;
    int WaitNum;
} TODOHeader;

typedef struct {

    // TODO: Maybe I need to add an id
    
    char Deadline[21];
    char Desc[101];
    int Stage;
    char Type[21];
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

void
HeaderUpdate(char const *Path, TODOHeader *Header)
{
    // TODO: Header Update
    FILE *TodoFile = fopen(Path, "rb+");
    fwrite(Header , sizeof(TODOHeader), 1, TodoFile);
    fclose(TodoFile);
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
    char *Command[] = {"ls", "add", "edit", "-r"};
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

#if 0
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
#endif

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

            if (!Header->TotalNum)
            {
                printf("\nThere isn't any thing in TODOer! HURRAY!\n\n");
            }
            else
            {
                printf("\nTotal: \t%2d\nTodo: \t%2d\nWait: \t%2d\n\n", Header->TotalNum, 
                                                            Header->TodoNum, 
                                                            Header->WaitNum);
                char StageTable[][5] = {"T", "W", "D"};
                for (int MissionCount = 0;
                     MissionCount < Header->TotalNum;
                     MissionCount++)
                {
                    fread(&TodoObj, sizeof(TodoObj), 1, TodoFile);
                    printf("%2d - [%s] (%s) %s\n", MissionCount + 1,
                                                   StageTable[TodoObj.Stage],
                                                   TodoObj.Deadline, 
                                                   TodoObj.Desc);
                }
            }

            printf("\n");
            fclose(TodoFile);

        } break;

        case _add:
        {
            FILE *TodoFile = fopen(Path, "ab");
            Mission TodoObj;


            printf("\n");
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

            printf("\n");

            Header->TotalNum++;
            Header->TodoNum++;

            fwrite(&TodoObj, sizeof(TodoObj), 1, TodoFile);
            fclose(TodoFile);

            HeaderUpdate(Path, Header);

        } break;

        case _edit:
        {
        } break;

        case _rm:
        {

            // NOTE: if TODOer is empty
            if (!(Header->TodoNum + Header->WaitNum))
            {
                printf("error: nothing can delete in TODOer\n");
                exit(-1);
            }

            // NOTE: if user do not enter the id to delete
            if (!Input->OptionNum)
            {
                printf("error: need at least one more argument as ID to be removed\n");
                printf("usage: todoer <path-to-todo> rm <ID>\n");
                exit(-1);
            }


            // TODO: Load the TodoObjs from file
            FILE *TodoFile = fopen(Path, "rb");

            fseek(TodoFile, sizeof(TODOHeader), SEEK_SET);

            Mission *Buf = (Mission *)malloc(Header->TotalNum * sizeof(Mission));
            int *WantDelete = (int *)malloc(Header->TotalNum * sizeof(int));

            for (int i = 0; i < Header->TotalNum; i++)
                WantDelete[i] = 0;

            fread(Buf, sizeof(Mission), Header->TotalNum, TodoFile);

            fclose(TodoFile);

            int validIdNum = 0;
            for (int IdCount = 0;
                 IdCount < Input->OptionNum;
                 IdCount++)
            {
                // NOTE: change argument to int
                int IDToDelete = atoi(Input->Option[IdCount]);

                // NOTE: check whether argument is valid or not
                if (IDToDelete > Header->TotalNum)
                {
                    printf("error: %s is invalid.\n", Input->Option[IdCount]);
                    continue;
                }

                validIdNum++;
                WantDelete[IDToDelete - 1] = 1;
            }

            // TODO: delete it/them and recreate a todo file
            
            Mission *BufToFile = 
                (Mission *)malloc((Header->TotalNum - validIdNum) * sizeof(Mission));
            int MissionToFileCount = 0;
            for (int MissionCount = 0;
                 MissionCount < Header->TotalNum;
                 MissionCount++)
            {
                if (WantDelete[MissionCount])
                {
                    continue;
                }

                memcpy(BufToFile + (MissionToFileCount++),
                       Buf + MissionCount,
                       sizeof(Mission));
            }

            Header->TotalNum -= validIdNum;
            // TODO: handle waitnum
            Header->TodoNum -= validIdNum;

            TodoFile = fopen(Path, "wb");
            fwrite(Header, sizeof(TODOHeader), 1, TodoFile);
            fwrite(BufToFile, sizeof(Mission), MissionToFileCount, TodoFile);
            fclose(TodoFile);

            free(Buf);

        } break;
    }
    FreeCommandOption(Input);
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
        // TODO: Show list of Todo if no command
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
