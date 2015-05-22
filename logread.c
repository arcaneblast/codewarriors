
/* 
 * File:   logread.c
 * Author: root
 *
 * Created on May 19, 2015, 10:38 AM
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


//locations
#define INGALARY -988
#define OUTGALARY -989
#define EMPLOYEETYPE 943
#define GUESTTYPE 933

//definitions
#define EXECUTION_PATH "./bash_helpers/"
#define true 1
#define false 0
#define MAX_LOGS 10000
#define MAX_COMMAND_LENGTH 1000
#define MAX_COMMAND_PARAMETERS 100
/*
 * file format will be like the following
 * multiple lines each line has
 * time, type, where, name
 */

char * execution_argv[MAX_COMMAND_PARAMETERS];
//file methods
//file unit

struct logunit
{
    int time, type, where;
    char * name;
};

struct logunit logarray[MAX_LOGS];

int string_comparer(const void *p1, const void *p2)
{
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

int asc_comparer(const void *p1, const void *p2)
{
    int* num1 = (int*) p1;
    int* num2 = (int*) p2;
    return (*num1 - *num2);
}

int fill_log_array(FILE* input)
{
    int time, type, where;
    char name[1000];
    int i = 0;
    while (fscanf(input, "%d %d %d %s", &time, &type, &where, name) == 4)
    {
        logarray[i].time = time;
        logarray[i].type = type;
        logarray[i].where = where;
        logarray[i].name = (char *) malloc(sizeof (char) * 1000);
        strcpy(logarray[i].name, name);
        i++;
    }
    return i;
}

int get_guest_names_list(char **list, int log_count)
{
    int count = 0, i;

    for (i = 0; i < log_count; i++)
    {
        if (logarray[i].type = GUESTTYPE)
            strcpy(list[count++], logarray[i].name);
    }
    qsort(list, count, sizeof (char *), string_comparer);
    return count;
}

int get_employee_names_list(char **list, int log_count)
{
    int count = 0, i;

    for (i = 0; i < log_count; i++)
    {
        if (logarray[i].type = EMPLOYEETYPE)
            strcpy(list[count++], logarray[i].name);
    }
    qsort(list, count, sizeof (char *), string_comparer);
    return count;
}

int find_number_in_list(int x, int *list, int list_count)
{
    int i = 0;
    for (i = 0; i < list_count; i++)
        if (list[i] == x)
            return true;
    return false;
}

int get_room_list(int * list, int log_count)
{
    int i, count = 0;
    ;
    for (i = 0; i < log_count; i++)
    {
        if (isRoom(logarray[i].where) && !find_number_in_list(logarray[i].where,
                list, count))
            list[count++] = logarray[i].where;
    }
    qsort(list, count, sizeof (int), asc_comparer);
    return count;
}

int get_r_request(int * list, char* name, int type, int log_count)
{
    int i, count = 0;
    for (i = 0; i < log_count; i++)
    {
        if (stringEquals(logarray[i].name, name) && logarray[i].type == type &&
                isRoom(logarray[i].where))
            list[count++] = logarray[i].where;
    }
    return count;
}

int who_is_in_room(char ** list, int roomid, int log_count)
{
    int i, count = 0;
    for (i = 0; i < log_count; i++)
    {
        if (logarray[i].where == roomid)
            list[count++] = logarray[i].name;
    }
    qsort(list, count, sizeof (char *), string_comparer);
    return count;
}

void write_log_array(FILE* output, int count)
{
    int i;
    for (i = 0; i < count; i++)
        fprintf(output, "%d %d %d %s \n",
            logarray[i].time,
            logarray[i].type,
            logarray[i].where,
            logarray[i].name);
}
//program methods

int stringEquals(char* s1, char* s2)
{
    if (strlen(s1) != strlen(s2))
        return false;

    char* tmp1 = s1, *tmp2 = s2;
    while (*tmp1++ == *tmp2++ && *tmp1 != '\0' && *tmp2 != '\0');
    if (*tmp1 == '\0' || *tmp2 == '\0')
        return true;
    return false;
}

int executeCommand(char * command, int argc, char * argv[])
{
    char* finalcommand = (char*) malloc(MAX_COMMAND_LENGTH * sizeof (char));
    //resolving path
    char* path = EXECUTION_PATH;
    char* execution_path = EXECUTION_PATH;
    //char* tmp =realpath(execution_path, path);

    strcpy(finalcommand, path);
    strcpy(finalcommand + strlen(finalcommand), command);
    strcpy(finalcommand + strlen(finalcommand), " ");

    int i;
    for (i = 0; i < argc; i++)
    {
        strcpy(finalcommand + strlen(finalcommand), argv[i]);
        strcpy(finalcommand + strlen(finalcommand), " ");
    }
    return system(finalcommand);
}

void normalExit()
{
    printf("invalid");
    exit(255);
}

void normalExitWithEncryption(char * logfilepath, char* token)
{
    execution_argv[0] = logfilepath;
    execution_argv[1] = token;
    if (executeCommand("encrypt.sh", 2, execution_argv) != 0)
        normalExit();
    normalExit();
}

int isRoom(int roomid)
{
    if (roomid == INGALARY || roomid == OUTGALARY)
        return false;
    return true;
}

void securityExit()
{
    fprintf(stderr, "integrity violation");
    exit(255);
}

int BatchMode(int argc, char** argv)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        if (strlen(argv[i]) < 2) continue;
        if (argv[i][0] == '-' && argv[i][1] == 'B' && argc == 3)
        {
            return true;
        } else if (argv[i][0] == '-' && argv[i][1] == 'B')
        {

            normalExit();
        }
    }
    return false;
}

int checkToken(char * token)
{
    int i;
    for (i = 0; i < strlen(token); i++)
    {

        if (!isalnum(token[i]))
            return false;
    }
    return true;
}

int checkLogName(char * logpath)
{
    char * log = strrchr(logpath, '/');
    if (log == NULL)
        log = logpath;
    else
        log++;

    int i;
    for (i = 0; i < strlen(log); i++)
    {

        if (!isalnum(log[i]) && log[i] != '_')
            return false;
    }
    return true;
}

int checkName(char * name)
{
    int i;
    for (i = 0; i < strlen(name); i++)
    {

        if (!isalpha(name[i]))
            return false;
    }
    return true;
}

int checkNumber(char *num)
{
    int i;
    for (i = 0; i < strlen(num); i++)
    {

        if (!isdigit(num[i]))
            return false;
    }
    return true;
}

int tokenizing(char * line, char** argv_output)
{
    int i = 3;
    argv_output[0] = "command";
    argv_output[1] = "-K";
    argv_output[2] = "lol";
    while (*line != '\0')
    {

        while (isblank(*line) && line != '\0')
            line++;

        int k = 0;
        while (!isblank(*line) && *line != '\0')
            argv_output[i][k++] = *line++;

        i++;
        while (isblank(*line) && line != '\0')
            line++;
    }
    return i;
}

int main(int argc, char** argv)
{
    //checking
    //int argc = 11;
    //char* argv[] = {"logappend", "-K", "secret", "-T", "13" ,"-A" ,"-R",  "0", "-E" ,"John", "/root/Documents/NetBeansProjects/bibifi-final/dist/Debug/GNU-Linux-x86/log2"};
    //char* argv[] = {"logappend", "-B", "/root/Documents/NetBeansProjects/bibifi-final/dist/Debug/GNU-Linux-x86/hello"};
    if (argc < 5)
        normalExit(); // parameter number if too low



    //normal mode
    int sflag = 0, rflag = 0, tflag = 0, iflag = 0;
    char* token = NULL, * employeename = NULL, *guestname = NULL, *logfilename = NULL;

    int switchflag = -1;
    //reading options
    while ((switchflag = getopt(argc, argv, "K:SRTIE:G:")) != -1)
    {
        switch (switchflag)
        {
            case 'K':
                if (!checkToken(optarg)) normalExit();
                token = optarg;
                break;
            case 'S':
                sflag = true;
                break;
            case 'R':
                rflag = true;
                break;
            case 'T':
                tflag = true;
                break;
            case 'I':
                iflag = true;
                break;
            case 'E':
                if (!checkName(optarg)) normalExit();
                employeename = optarg;
                break;
            case 'G':
                if (!checkName(optarg)) normalExit();
                guestname = optarg;
                break;
            default:
                normalExit();
        }
    }
    if (argc - optind != 1)
        normalExit();
    else
    {
        if (!checkLogName(argv[optind])) normalExit();
        logfilename = argv[optind];
    }

    //here to check program syntax sanity
    int count_true = 0;
    if (sflag) count_true++;
    if (iflag) count_true++;
    if (rflag) count_true++;
    if (tflag) count_true++;

    if (count_true != 1) normalExit();
    if (employeename != NULL && guestname != NULL && rflag) normalExit();
    if (employeename == NULL && guestname == NULL && rflag) normalExit();
    if (sflag && (employeename != NULL || guestname != NULL)) normalExit();
    if (tflag || iflag)
    {
        printf("unimplemented");
        exit(0);
    }
    //checking if file exists
    //getting the resolved path
    execution_argv[0] = logfilename;
    if (executeCommand("file_exists.sh", 1, execution_argv) == 0) // file found
    {
        //decrypt file
        execution_argv[0] = logfilename;
        execution_argv[1] = token;
        if (executeCommand("decrypt.sh", 2, execution_argv) != 0)
            securityExit();

        //reading from the file time, type, where, name
        FILE* input = fopen(logfilename, "r");
        if (input == NULL) normalExitWithEncryption(logfilename, token);
        int log_count = fill_log_array(input);
        fclose(input);


        if (sflag)
        {
            char ** emplist = (char **) malloc(sizeof (char*) * 1000);
            char ** guestlist = (char **) malloc(sizeof (char*) * 1000);
            int i, emp_count = 0, guest_count = 0;
            for (i = 0; i < 1000; i++)
            {
                emplist[i] = (char*) malloc(sizeof (char) * 100);
                guestlist[i] = (char*) malloc(sizeof (char) * 100);
            }
            emp_count = get_employee_names_list(emplist, log_count);
            guest_count = get_guest_names_list(guestlist, log_count);

            if (emp_count == 0)
                printf("\n");
            else
            {
                for (i = 0; i < emp_count; i++)
                {
                    if (i == emp_count - 1)
                        printf("%s\n", emplist[i]);
                    else
                        printf("%s,", emplist[i]);
                }
            }

            if (guest_count == 0)
                printf("\n");
            else
            {
                for (i = 0; i < guest_count; i++)
                {
                    if (i == guest_count - 1)
                        printf("%s\n", guestlist[i]);
                    else
                        printf("%s,", guestlist[i]);
                }
            }
            int* room_ids = (int*) malloc(sizeof (int) * 100);
            int room_count = get_room_list(room_ids, log_count);
            int j;
            for (i = 0; i < room_count; i++)
            {
                int who_count = who_is_in_room(guestlist, room_ids[i], log_count);
                if (who_count != 0)
                    printf("%d: ");

                for (j = 0; j < who_count; j++)
                {
                    if (j == who_count - 1) printf("%s\n", guestlist[j]);
                    else printf("%s,", guestlist[j]);
                }
            }
        }

        if (rflag)
        {
            char *some_name;
            int some_type;
            some_type = EMPLOYEETYPE, some_name = employeename;
            if (employeename == NULL)
                some_type = GUESTTYPE, some_name = guestname;

            int* room_ids = (int*) malloc(sizeof (int) * 100);
            int room_count = get_r_request(room_ids, some_name, some_type, log_count);
            int index;
            for (index = 0; index < room_count; index++)
            {
                if (index == room_count - 1)
                    printf("%d", room_ids[index]);
                else
                    printf("%d,", room_ids[index]);
            }
            printf("\n");
        }


        //encrypt file
        execution_argv[0] = logfilename;
        execution_argv[1] = token;
        if (executeCommand("encrypt.sh", 2, execution_argv) != 0)
            normalExitWithEncryption(logfilename, token);

    } else
        normalExit();

    return (EXIT_SUCCESS);
}
