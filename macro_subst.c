#include "header.h"
#include <ctype.h>

int is_id(char c)
{
    return isalnum((unsigned char)c) || c == '_';
}

/* Replace function macro parameters with actual arguments */
void replace_params(char *value, char params[10][50],
                    char args[10][200], int n, char *result)
{
    int i, j, k, found;
    char word[100];

    result[0] = '\0';

    for(i = 0; value[i] != '\0'; )
    {
        if(isalpha((unsigned char)value[i]) || value[i] == '_')
        {
            j = 0;

            while(is_id(value[i]))
                word[j++] = value[i++];

            word[j] = '\0';
            found = 0;

            for(k = 0; k < n; k++)
            {
                if(strcmp(word, params[k]) == 0)
                {
                    strcat(result, args[k]);
                    found = 1;
                    break;
                }
            }

            if(!found)
                strcat(result, word);
        }
        else
        {
            j = strlen(result);
            result[j] = value[i++];
            result[j + 1] = '\0';
        }
    }
}

int get_args(char *p, char args[10][200], char **end)
{
    int count = 0, depth = 0;
    char *start;

    p++;
    start = p;

    while(*p)
    {
        if(*p == '(')
            depth++;

        else if(*p == ')')
        {
            if(depth == 0)
            {
                strncpy(args[count], start, p - start);
                args[count][p - start] = '\0';

                count++;
                *end = p + 1;
                return count;
            }

            depth--;
        }

        else if(*p == ',' && depth == 0)
        {
            strncpy(args[count], start, p - start);
            args[count][p - start] = '\0';

            count++;
            start = p + 1;
        }

        p++;
    }

    return -1;
}
/* Expand macros in one line */
void expand_line(char *line, char macro[100][100],
                 char val[100][300], int func[],
                 char params[100][10][50], int pc[])
{
    char result[5000] = "";
    char args[10][200];
    char replacement[2000];
    char word[100];
    char *p, *q, *end;
    int i, j, len, count, found;

    p = line;

    while(*p)
    {
        if(isalpha((unsigned char)*p) || *p == '_')
        {
            q = p;
            j = 0;

            while(is_id(*q))
            {
                if(j < 99)
                    word[j++] = *q;

                q++;
            }

            word[j] = '\0';
            found = 0;

            for(i = 0; i < 100; i++)
            {
                if(macro[i][0] == '\0')
                    break;

                if(strcmp(word, macro[i]) == 0)
                {
                    if(func[i] == 0)
                    {
                        if(!is_id(*q))
                        {
                            strcat(result, val[i]);
                            p = q;
                            found = 1;
                            break;
                        }
                    }
                    else
                    {
                        end = q;

                        while(*end == ' ' || *end == '\t')
                            end++;

                        if(*end == '(')
                        {
                            count = get_args(end, args, &end);

                            if(count == pc[i])
                            {
                                for(j = 0; j < count; j++)
                                {
                                    char *s = args[j];
                                    while(*s == ' ' || *s == '\t')
                                        s++;

                                    if(s != args[j])
                                        memmove(args[j], s, strlen(s) + 1);

                                    len = strlen(args[j]);

                                    while(len > 0 &&
                                         (args[j][len-1] == ' ' ||
                                          args[j][len-1] == '\t'))
                                        args[j][--len] = '\0';
                                }

                                replace_params(val[i], params[i],
                                               args, count, replacement);

                                strcat(result, replacement);
                                p = end;
                                found = 1;
                                break;
                            }
                        }
                    }
                }
            }

            if(!found)
            {
                len = strlen(result);
                strncat(result, p, q - p);
                p = q;
            }
        }
        else
        {
            len = strlen(result);
            result[len] = *p++;
            result[len + 1] = '\0';
        }
    }

    strcpy(line, result);
}

void macro_subst(char *file)
{
    FILE *fp, *out;
    char line[1000], name[100];
    char output[150];
    char macro[100][100] = {{0}};
    char val[100][300];
    char params[100][10][50];
    int func[100] = {0};
    int pc[100] = {0};
    int count = 0;
    int i, j, len;
    char *p;

    fp = fopen(file, "r");

    if(fp == NULL)
    {
        printf("File not found\n");
        return;
    }

    snprintf(output, sizeof(output), "%s.tmp", file);

    out = fopen(output, "w");

    if(out == NULL)
    {
        printf("Cannot create output file\n");
        fclose(fp);
        return;
    }

    while(fgets(line, sizeof(line), fp))
    {
        if(strncmp(line, "#define", 7) == 0 &&
           (line[7] == ' ' || line[7] == '\t'))
        {
            p = line + 7;

            while(*p == ' ' || *p == '\t')
                p++;

            i = 0;

            while(is_id(*p) && i < 99)
                name[i++] = *p++;

            name[i] = '\0';

            if(i == 0 || count >= 100)
                continue;

            strcpy(macro[count], name);

            /* Function-like macro */
            if(*p == '(')
            {
                func[count] = 1;
                p++;

                while(*p && *p != ')')
                {
                    while(*p == ' ' || *p == '\t' || *p == ',')
                        p++;

                    if(*p == ')')
                        break;

                    j = 0;

                    while(is_id(*p) && j < 49)
                        params[count][pc[count]][j++] = *p++;

                    params[count][pc[count]][j] = '\0';

                    if(j > 0 && pc[count] < 10)
                        pc[count]++;
                    else
                        break;
                }

                if(*p == ')')
                    p++;
            }

            while(*p == ' ' || *p == '\t')
                p++;

            strcpy(val[count], p);

            len = strlen(val[count]);

            while(len > 0 &&
                 (val[count][len-1] == '\n' ||
                  val[count][len-1] == '\r'))
                val[count][--len] = '\0';

            count++;
        }
        else
        {
            expand_line(line, macro, val, func, params, pc);
            fputs(line, out);
        }
    }

    fclose(fp);
    fclose(out);

    remove(file);
    rename(output, file);

    printf("Macro substitution completed: %s\n", file);
}
