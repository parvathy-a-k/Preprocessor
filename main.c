#include"header.h"
int main(int argc,char **argv)
{
char temp[100];
char output[100];
char *ptr;
if(argc!=2)
{
printf("Usage:./mypreprocessor inputfile.c\n");
return 1;
}
strcpy(temp, argv[1]);
ptr = strrchr(temp, '.');
   if(ptr != NULL)
        strcpy(ptr, ".temp");
    else
        strcat(temp, ".temp");
strcpy(output, argv[1]);

   ptr = strrchr(output, '.');

    if(ptr != NULL)
        strcpy(ptr, ".i");
    else
        strcat(output, ".i");

comment_removal(argv[1]);
file_inclusion(temp);
macro_subst(output);
return 0;
}

