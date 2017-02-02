

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 80
#define VAR_ATTRIBUTE "__attribute__ ((section (\".rodata_noremap\")))"
#define TAB_STRING    "    "

const char tab[] = TAB_STRING; 

int file_exists(
    const char* file_name
)
{
    int response = 0;
    FILE* stream;
    stream = fopen(file_name, "rb");

    if(stream != NULL)
    {
        response = 1;
        fclose(stream);
    }
    return response;
}

char* get_file_name(
    char* buffer,
    int length,
    char* file_name,
    int strip_ext
)
{
    char* tmp;


    file_name = strncpy(buffer, file_name, length);

    tmp = strtok(file_name, "\\/");
    while(NULL != tmp)
    {
        file_name = tmp;
        tmp = strtok(NULL, "\\/");
    }
    if(strip_ext)
    {
        file_name = strtok(file_name, ".");
    }

    return strncpy(buffer, file_name, length);
}


int main(
    int argc, 
    char**argv
)
{
    int current_file;

    if(argc > 2)
    {
        unsigned char c;
        char buffer[256];
        char* prefix = argv[1]; 
        char** files = &(argv[2]);
        int line_pos;
        int file_length, file_pos;

        for(current_file = 0; current_file < (argc - 2); current_file ++)
        {
            FILE* stream;

            /* Open file for reading */
            stream = fopen(files[current_file], "rb");

            /* If file was openned successfully */
            if(stream != NULL)
            {
                get_file_name(buffer, 256, files[current_file], 1);
                printf("%s unsigned char %s_%i_%s[] = \n{\n",
                    VAR_ATTRIBUTE, prefix, current_file, buffer);

                /* Get the file length */
                fseek(stream, 0L, SEEK_END);
                file_length = ftell(stream);
                fseek(stream, 0L, SEEK_SET); 

                line_pos = 0;
                for(file_pos = 0; file_pos < (file_length - 1); file_pos ++)
                {
                    if(0 == line_pos)
                    {                        
                        line_pos += printf(tab);
                    }

                    fread(&c, 1, 1, stream);
                    line_pos += printf("0x%02x, ", (unsigned int)c);                   

                    if(line_pos >= (MAX_LINE_LENGTH - 6))
                    {
                        line_pos = 0;
                        printf("\n");
                    }
                }

                fread(&c, 1, 1, stream);
                line_pos += printf("0x%02x", (unsigned int)c);
                printf("\n};\n\n");
        
                fclose(stream);
            }
        }

        /* Print File names */
        printf("char* %s_names[] = \n{\n", prefix);
        for(current_file = 0; current_file < (argc - 2); current_file ++)
        {
            get_file_name(buffer, 256, files[current_file], 0);
            printf("%s\"%s\"", tab, buffer);

            if(current_file < (argc - 3))
            {
                printf(",\n");
            }   
        }       

        printf("\n};\n\n");

        /* Generate Array of Files */
        printf("unsigned char* %s_contents[] = \n{\n", prefix);
        for(current_file = 0; current_file < (argc - 2); current_file ++)
        {
            get_file_name(buffer, 256, files[current_file], 1);
            printf("%s%s_%i_%s", tab, prefix, current_file, buffer);

            if(current_file < (argc - 3))
            {
                printf(",\n");
            }            
        }
        printf("\n};\n\n");

        /* Generate Array File sizes */
        printf("unsigned int %s_file_sizes[] = \n{\n", prefix);
        for(current_file = 0; current_file < (argc - 2); current_file ++)
        {
            get_file_name(buffer, 256, files[current_file], 1);
            printf("%ssizeof(%s_%i_%s) / sizeof(unsigned char)", tab, prefix, current_file, buffer);

            if(current_file < (argc - 3))
            {
                printf(",\n");
            }            
        }
        printf("\n};\n\n");

        /* Generate Number of files */
        printf("unsigned int %s_count = %u;\n\n", prefix, (unsigned int)(argc - 2));


        return 0;
    }
    else
    {
        return 1;
    }
}