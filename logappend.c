/* 
 * File:   logappend.c
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
        logarray[i].name = (char *) malloc(sizeof(char) * 1000);
        strcpy(logarray[i].name, name);    
        i++;
    }
    return i;
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
    fprintf(stderr, "security error");
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
    if (argc < 3)
        normalExit(); // parameter number if too low


    //check if -B is given
    if (BatchMode(argc, argv))
    {
        if (argc != 3)
            normalExit();

        //batch mode
        int switchflag = -1;
        char* batch_file_path = NULL;
        while ((switchflag = getopt(argc, argv, "B:")) != -1)
        {
            switch (switchflag)
            {
                case 'B':
                    batch_file_path = optarg;
                    break;
                default:
                    normalExit();
                    break;
            }
        }


        FILE* batchfile = fopen(batch_file_path, "r");
        if (batchfile == NULL) normalExit();

        char line[1000];
        char** batch_argv = NULL;

        batch_argv = (char **) malloc(sizeof (char*) * 20);
        int i;
        for (i = 0; i < 20; i++)
        {
            batch_argv[i] = (char*) malloc(sizeof (char) * 100);
        }

        //reading the batch file line by line
        while (EOF != fscanf(batchfile, "%[^\n]\n", line))
        {
            //allocating arguments
            //number of allowed arguments 20
            //length of each argument is 100
            for (i = 0; i < 20; i++)
            {
                memset(batch_argv[i], '\0', sizeof (char) * 100);
            }

            int batch_argc = tokenizing(line, batch_argv);
            batch_handler(batch_argc, batch_argv);
        }


    } else
    {
        //normal mode
        int aflag = -1, lflag = -1, roomid = -1, timestamp = -1;
        char* token = NULL, * employeename = NULL, *guestname = NULL, *logfilename = NULL;

        int switchflag = -1;
        //reading options
        while ((switchflag = getopt(argc, argv, "T:K:E:G:ALR:")) != -1)
        {
            switch (switchflag)
            {
                case 'K':
                    if (!checkToken(optarg)) normalExit();
                    token = optarg;
                    break;
                case 'T':
                    if (!checkNumber(optarg)) normalExit();
                    timestamp = atoi(optarg);
                    if (timestamp < 0) normalExit();
                    break;
                case 'E':
                    if (!checkName(optarg)) normalExit();
                    employeename = optarg;
                    break;
                case 'G':
                    if (!checkName(optarg)) normalExit();
                    guestname = optarg;
                    break;
                case 'A':
                    aflag = true;
                    break;
                case 'L':
                    lflag = true;
                    break;
                case 'R':
                    if (!checkNumber(optarg)) normalExit();
                    roomid = atoi(optarg);
                    if (roomid < 0)normalExit();
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
        if (timestamp == -1 || token == NULL || logfilename == NULL)
            normalExit();
        else if ((aflag == true && lflag == true) ||
                (aflag == -1 && lflag == -1))
            normalExit();
        else if ((employeename != NULL && guestname != NULL) ||
                (employeename == NULL && guestname == NULL))
            normalExit();

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

            //old time and type and name
            char *old_name;
            int old_where, old_type, old_time;

            old_time = timestamp;

            old_type = EMPLOYEETYPE, old_name = employeename;
            if (employeename == NULL)
                old_type = GUESTTYPE, old_name = guestname;

            old_where = OUTGALARY;

            int i;
            for (i = 0; i < log_count; i++)
            {
                if (old_time <= logarray[i].time)
                    normalExitWithEncryption(logfilename, token);

                if (stringEquals(logarray[i].name, old_name) &&
                        logarray[i].type == old_type)
                    old_where = logarray[i].where;

                else if (stringEquals(logarray[i].name, old_name) &&
                        logarray[i].type != old_type)
                    normalExitWithEncryption(logfilename, token);
            }

            //applying room logic
            //room
            if (isRoom(old_where) && aflag == true)
                normalExitWithEncryption(logfilename, token);
            if (isRoom(old_where) && lflag == true && old_where != roomid)
                normalExitWithEncryption(logfilename, token);

            //outofgalary
            if (old_where == OUTGALARY && lflag == true)
                normalExitWithEncryption(logfilename, token);
            if (old_where == OUTGALARY && aflag == true && roomid != -1)
                normalExitWithEncryption(logfilename, token);

            //in galary
            if (old_where == INGALARY && lflag == true && roomid != -1)
                normalExitWithEncryption(logfilename, token);
            if (old_where == INGALARY && aflag == true && roomid == -1)
                normalExitWithEncryption(logfilename, token);

            int new_where = -1;
            if (isRoom(old_where) && lflag == true)
                new_where = INGALARY;
            if (old_where == OUTGALARY)
                new_where = INGALARY;
            if (old_where == INGALARY && lflag == true)
                new_where = OUTGALARY;
            if (old_where == INGALARY && aflag == true)
                new_where = roomid;

            logarray[log_count].name = old_name;
            logarray[log_count].type = old_type;
            logarray[log_count].time = old_time;
            logarray[log_count].where = new_where;
            log_count++;

            FILE* output = fopen(logfilename, "w");
            if (output == NULL) normalExitWithEncryption(logfilename, token);

            write_log_array(output, log_count);

            fclose(output);

            //encrypt file
            execution_argv[0] = logfilename;
            execution_argv[1] = token;
            if (executeCommand("encrypt.sh", 2, execution_argv) != 0)
                normalExitWithEncryption(logfilename, token);

        } else
        {
            //new file
            //old time and type and name
            char *old_name;
            int old_where, old_type, old_time;

            old_time = timestamp;

            old_type = EMPLOYEETYPE, old_name = employeename;
            if (employeename == NULL)
                old_type = GUESTTYPE, old_name = guestname;

            old_where = OUTGALARY;

            if (roomid != -1 || lflag == true)
                normalExit();
            int log_count = 0;
            //file not found
            logarray[log_count].name = old_name;
            logarray[log_count].type = old_type;
            logarray[log_count].time = old_time;
            logarray[log_count].where = INGALARY;
            log_count++;


            FILE* output = fopen(logfilename, "w");
            if (output == NULL) normalExit();

            write_log_array(output, log_count);

            fclose(output);


            //encrypt file
            execution_argv[0] = logfilename;
            execution_argv[1] = token;
            if (executeCommand("encrypt.sh", 2, execution_argv) != 0)
                normalExit();
        }
    }
    return (EXIT_SUCCESS);
}

int batch_handler(int argc, char** argv)
{
    //checking
    if (argc < 3)// parameter number if too low
    {
        printf("invalid");
        return 255;
    }

    //check if -B is given
    if (BatchMode(argc, argv))
    {
        printf("invalid");
        return 255;
    } else
    {
        //normal mode
        int aflag = -1, lflag = -1, roomid = -1, timestamp = -1;
        char* token = NULL, * employeename = NULL, *guestname = NULL, *logfilename = NULL;

        int switchflag = -1;
        //reading options
        while ((switchflag = getopt(argc, argv, "T:K:E:G:ALR:")) != -1)
        {
            switch (switchflag)
            {
                case 'K':
                    if (!checkToken(optarg))
                    {
                        printf("invalid");
                        return 255;
                    }
                    token = optarg;
                    break;
                case 'T':
                    if (!checkNumber(optarg))
                    {
                        printf("invalid");
                        return 255;
                    }
                    timestamp = atoi(optarg);
                    if (timestamp < 0)
                    {
                        printf("invalid");
                        return 255;
                    }
                    break;
                case 'E':
                    if (!checkName(optarg))
                    {
                        printf("invalid");
                        return 255;
                    }
                    employeename = optarg;
                    break;
                case 'G':
                    if (!checkName(optarg))
                    {
                        printf("invalid");
                        return 255;
                    }
                    guestname = optarg;
                    break;
                case 'A':
                    aflag = true;
                    break;
                case 'L':
                    lflag = true;
                    break;
                case 'R':
                    if (!checkNumber(optarg))
                    {
                        printf("invalid");
                        return 255;
                    }
                    roomid = atoi(optarg);
                    if (roomid < 0)
                    {
                        printf("invalid");
                        return 255;
                    }
                    break;
                default:
                    printf("invalid");
                    return 255;
            }
        }
        if (argc - optind != 1)
        {

            printf("invalid");
            return 255;
        } else
        {
            if (!checkLogName(argv[optind]))
            {

                printf("invalid");
                return 255;
            };
            logfilename = argv[optind];
        }

        //here to check program syntax sanity
        if (timestamp == -1 || token == NULL || logfilename == NULL)
        {
            printf("invalid");
            return 255;
        } else if ((aflag == true && lflag == true) ||
                (aflag == -1 && lflag == -1))
        {
            printf("invalid");
            return 255;
        } else if ((employeename != NULL && guestname != NULL) ||
                (employeename == NULL && guestname == NULL))
        {
            printf("invalid");
            return 255;
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
            {
                fprintf(stderr, "security error");
                return 255;
            }

            //reading from the file time, type, where, name
            FILE* input = fopen(logfilename, "r");
            if (input == NULL)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }
            int log_count = fill_log_array(input);
            fclose(input);

            //old time and type and name
            char *old_name;
            int old_where, old_type, old_time;

            old_time = timestamp;

            old_type = EMPLOYEETYPE, old_name = employeename;
            if (employeename == NULL)
                old_type = GUESTTYPE, old_name = guestname;

            old_where = OUTGALARY;

            int i;
            for (i = 0; i < log_count; i++)
            {
                if (old_time <= logarray[i].time)
                {
                    printf("invalid");
                    execution_argv[0] = logfilename;
                    execution_argv[1] = token;
                    executeCommand("encrypt.sh", 2, execution_argv);
                    return 255;
                }

                if (stringEquals(logarray[i].name, old_name) &&
                        logarray[i].type == old_type)
                    old_where = logarray[i].where;

                else if (stringEquals(logarray[i].name, old_name) &&
                        logarray[i].type != old_type)
                {
                    printf("invalid");
                    execution_argv[0] = logfilename;
                    execution_argv[1] = token;
                    executeCommand("encrypt.sh", 2, execution_argv);
                    return 255;
                }
            }

            //applying room logic
            //room
            if (isRoom(old_where) && aflag == true)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }
            if (isRoom(old_where) && lflag == true && old_where != roomid)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }

            //outofgalary
            if (old_where == OUTGALARY && lflag == true)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }
            if (old_where == OUTGALARY && aflag == true && roomid != -1)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }

            //in galary
            if (old_where == INGALARY && lflag == true && roomid != -1)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }
            if (old_where == INGALARY && aflag == true && roomid == -1)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }

            int new_where = -1;
            if (isRoom(old_where) && lflag == true)
                new_where = INGALARY;
            if (old_where == OUTGALARY)
                new_where = INGALARY;
            if (old_where == INGALARY && lflag == true)
                new_where = OUTGALARY;
            if (old_where == INGALARY && aflag == true)
                new_where = roomid;

            logarray[log_count].name = old_name;
            logarray[log_count].type = old_type;
            logarray[log_count].time = old_time;
            logarray[log_count].where = new_where;
            log_count++;

            FILE* output = fopen(logfilename, "w");
            if (output == NULL)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }

            write_log_array(output, log_count);

            fclose(output);

            //encrypt file
            execution_argv[0] = logfilename;
            execution_argv[1] = token;
            if (executeCommand("encrypt.sh", 2, execution_argv) != 0)
            {
                printf("invalid");
                execution_argv[0] = logfilename;
                execution_argv[1] = token;
                executeCommand("encrypt.sh", 2, execution_argv);
                return 255;
            }

        } else
        {
            //new file
            //old time and type and name
            char *old_name;
            int old_where, old_type, old_time;

            old_time = timestamp;

            old_type = EMPLOYEETYPE, old_name = employeename;
            if (employeename == NULL)
                old_type = GUESTTYPE, old_name = guestname;

            old_where = OUTGALARY;

            if (roomid != -1 || lflag == true)
            {
                printf("invalid");
                return 255;
            }
            int log_count = 0;
            //file not found
            logarray[log_count].name = old_name;
            logarray[log_count].type = old_type;
            logarray[log_count].time = old_time;
            logarray[log_count].where = INGALARY;
            log_count++;


            FILE* output = fopen(logfilename, "w");
            if (output == NULL)
            {
                printf("invalid");
                return 255;
            }

            write_log_array(output, log_count);

            fclose(output);


            //encrypt file
            execution_argv[0] = logfilename;
            execution_argv[1] = token;
            if (executeCommand("encrypt.sh", 2, execution_argv) != 0)
            {
                printf("invalid");
                return 255;
            }
        }
    }
    return 0;
}
