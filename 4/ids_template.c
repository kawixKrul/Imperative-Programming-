#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2
#define IN_STRING 4
#define IN_ID 8

#define MAX_ID_LEN 64
#define MAX_IDS 1024

#define TEST 0  // 1 - dla testowania,  0 - dla automatycznej oceny

int index_cmp(const void*, const void*);
int cmp(const void*, const void*);

char tab[MAX_IDS][MAX_ID_LEN];

char *keywords[] = {
	"auto", "break", "case", "char",
	"const", "continue", "default", "do",
	"double", "else", "enum", "extern",
	"float", "for", "goto", "if",
	"int", "long", "register", "return",
	"short", "signed", "sizeof", "static",
	"struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while"
};

int find_idents(FILE *stream)
{
    /*int count = 0;
    int key_counts[32] = {0};
    char line[MAX_ID_LEN];

    while (fscanf(stream, "%s", line) != EOF) {
        char *token;
        token = strtok(line, " ,!?/\\{}[]()\"';|&!@#$%^*:.");
        while (token != NULL) {
            for (int i = 0; i < 32; ++i) {
                if (strcmp(keywords[i], token) == 0 && !key_counts[i]) {
                    key_counts[i] = 1;
                }
            }
            token = strtok(NULL, " ,!?/\\{}[]()\"';|&!@#$%^*:.");
        }
    }
    for (int i = 0; i < 32; ++i) {
        if (key_counts[i])
            count += 1;
    }
    return count;*/
    /*#define MAX_LINE 128
    char buffer[MAX_LINE];
    int in_comment = 0, j = 0;
    while (fgets(buffer, MAX_LINE, stream) != NULL) {
        if (!in_comment) {
            char *token;
            token = strtok(buffer, " ,!?/\\{}[]()\"';|&!@#$%^*:.");
            while (token != NULL) {
                int is_ident = 1;
                for (int i = 0; i < 32; ++i) {
                    is_ident -= !strcmp(token, keywords[i]);
                }
                if (is_ident) {
                    int unique = 1;
                    for (int i = 0; i < j; ++i) {
                        unique -= !strcmp(token, tab[j]);
                    }
                    if (unique) {
                        strcpy(tab[j], token);
                        j += 1;
                    }
                }
                token = strtok(NULL, " ,!?/\\{}[]()\"';|&!@#$%^*:.");
            }
        }
    }
    for (int i = 0; i < j; ++i) {
        printf("%s ", tab[i]);
    }
    return j;*/

    int c = 0;
    int d = 0;
    int x = 0;
    int y = 0;
    int podobne = 1;
    char ind[MAX_ID_LEN];
    for(int i=0;i<MAX_ID_LEN;i++)
        ind[i] = '\0';
    while( ( c = fgetc(stream) ) != EOF )
    {
        if( ( c >= (int)'a' && c <= (int)'z' ) || ( c >= (int)'A' && c <= (int)'Z' ) || c == (int)'_' )
        {
            ind[x] = (char)c;
            x++;
            while( ( c = fgetc(stream) ) != EOF )
            {
                if( ( c >= (int)'a' && c <= (int)'z' ) || ( c >= (int)'A' && c <= (int)'Z' ) || ( c >= (int)'0' && c <= (int)'9' ) ||  c == (int)'_' )
                {
                    ind[x] = (char)c;
                    x++;
                }
                else
                    break;
            }
            podobne = 0;
            for( int k = 0; k < 32; k++ )
            {
                if( x == strlen(keywords[k]) )
                {
                    podobne = 1;
                    for( int i = 0; i < x; i++ )
                    {
                        if( keywords[k][i] != ind[i] )
                        {
                            podobne = 0;
                            break;
                        }
                    }
                    if( podobne == 1 )
                        break;
                }
            }
            if( podobne == 0 )
            {
                for( int k = 0; k < y; k++)
                {
                    if(x == strlen(tab[k]))
                    {
                        podobne = 1;
                        for( int i = 0; i < x; i++ )
                        {
                            if( tab[k][i] != ind[i] )
                            {
                                podobne = 0;
                                break;
                            }
                        }
                        if( podobne == 1 )
                            break;
                    }
                }
                if( podobne == 0 )
                {
                    for( int i = 0; i < x; i++ )
                    {
                        tab[y][i] = ind[i];
                    }
                    y++;
                }
            }
            for( int i = 0; i < x; i++ )
            {
                ind[i] = '\0';
            }
            x = 0;
        }
        if( c == (int)'/' )
        {
            c = fgetc(stream);

            if( c == (int)'/' )
            {
                while( ( c = fgetc(stream)) != EOF )
                {
                    if( c == (int)'\n' )
                        break;
                }
            }
            else if( c == (int)'*' )
            {
                d = fgetc(stream);
                while( ( c = fgetc(stream)) != EOF )
                {
                    if( d == (int)'*' && c == (int)'/')
                        break;
                    d = c;
                }
            }
        }
        if( c == (int)'"' )
        {
            d = fgetc(stream);
            if( d != (int)'"')
            {
                while( ( c = fgetc(stream)) != EOF )
                {
                    if( d != (int)'\\' && c == (int)'"')
                        break;
                    d = c;
                }
            }
        }
        if( c == '\'' )
        {
            while( ( c = fgetc(stream)) != EOF )
            {
                if( c == '\'' )
                    break;
            }
        }
    }
    return y;



}

int cmp(const void* first_arg, const void* second_arg) {
	char *a = *(char**)first_arg;
	char *b = *(char**)second_arg;
	return strcmp(a, b);
}

int index_cmp(const void* first_arg, const void* second_arg) {
	int a = *(int*)first_arg;
	int b = *(int*)second_arg;
	return strcmp(tab[a], tab[b]);
}

int main(void) {
	char file_name[40];
	FILE *stream;

  if(TEST)  stream = stdin;
  else {
      scanf("%s",file_name);
      stream = fopen(file_name,"r");
      if(stream == NULL) {
          printf("fopen failed\n");
          return -1;
      }
  }
	printf("%d\n", find_idents(stream));
	return 0;
}

/*
 int c = 0;
    int d = 0;
    int x = 0;
    int y = 0;
    int podobne = 1;
    char ind[MAX_ID_LEN];
    for(int i=0;i<MAX_ID_LEN;i++)
        ind[i] = '\0';
    while( ( c = fgetc(stream) ) != EOF )
    {
        if( ( c >= (int)'a' && c <= (int)'z' ) || ( c >= (int)'A' && c <= (int)'Z' ) || c == (int)'_' )
        {
            ind[x] = (char)c;
            x++;
            while( ( c = fgetc(stream) ) != EOF )
            {
                if( ( c >= (int)'a' && c <= (int)'z' ) || ( c >= (int)'A' && c <= (int)'Z' ) || ( c >= (int)'0' && c <= (int)'9' ) ||  c == (int)'_' )
                {
                    ind[x] = (char)c;
                    x++;
                }
                else
                    break;
            }
            podobne = 0;
            for( int k = 0; k < 32; k++ )
            {
                if( x == strlen(keywords[k]) )
                {
                    podobne = 1;
                    for( int i = 0; i < x; i++ )
                    {
                        if( keywords[k][i] != ind[i] )
                        {
                            podobne = 0;
                            break;
                        }
                    }
                    if( podobne == 1 )
                        break;
                }
            }
            if( podobne == 0 )
            {
                for( int k = 0; k < y; k++)
                {
                    if(x == strlen(tab[k]))
                    {
                        podobne = 1;
                        for( int i = 0; i < x; i++ )
                        {
                            if( tab[k][i] != ind[i] )
                            {
                                podobne = 0;
                                break;
                            }
                        }
                        if( podobne == 1 )
                            break;
                    }
                }
                if( podobne == 0 )
                {
                    for( int i = 0; i < x; i++ )
                    {
                        tab[y][i] = ind[i];
                    }
                    y++;
                }
            }
            for( int i = 0; i < x; i++ )
            {
                ind[i] = '\0';
            }
            x = 0;
        }
        if( c == (int)'/' )
        {
            c = fgetc(stream);

            if( c == (int)'/' )
            {
                while( ( c = fgetc(stream)) != EOF )
                {
                    if( c == (int)'\n' )
                        break;
                }
            }
            else if( c == (int)'*' )
            {
                d = fgetc(stream);
                while( ( c = fgetc(stream)) != EOF )
                {
                    if( d == (int)'*' && c == (int)'/')
                        break;
                    d = c;
                }
            }
        }
        if( c == (int)'"' )
        {
            d = fgetc(stream);
            if( d != (int)'"')
            {
                while( ( c = fgetc(stream)) != EOF )
                {
                    if( d != (int)'\\' && c == (int)'"')
                        break;
                    d = c;
                }
            }
        }
        if( c == '\'' )
        {
            while( ( c = fgetc(stream)) != EOF )
            {
                if( c == '\'' )
                    break;
            }
        }
    }

    return y;


 */