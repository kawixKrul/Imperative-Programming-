#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_DIGRAMS (LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR)

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

#define TEST 0   // 1 dla testowania, 0 dla automatycznej oceny

int count[MAX_DIGRAMS] = { 0 };

// function to be used as comparator to qsort (compares counters and thes sorts
// alphabetically when equal)
int cmp (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	if (count[va] == count[vb]) return va - vb; // sort alphabetically if counts equal
	return count[vb] - count[va];
}

// function to be used as comparator to qsort for digrams (compares counters and
// thes sorts alphabetically when equal)
int cmp_di (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	// sort according to second char if counts and the first char equal
	if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
	// sort according to first char if counts equal
	if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
	return count[vb] - count[va];
}

// count number of lines (nl), number of words (nw) and number of characters
// (nc) in the text read from stream
void wc(int *nl, int *nw, int *nc, FILE *stream){
    *nw = 0;
    *nl = 0;
    *nc = 0;
    int ch, in_word = 0;
    while ((ch = fgetc(stream)) != EOF) {
        *nc += 1;
        if (ch == ' ' || ch == '\t') {
            in_word = 0;
        }
        else if (ch == NEWLINE) {
            *nl += 1;
            in_word = 0;
        }
        else {
            if (!in_word) {
                *nw += 1;
                in_word = 1;
            }
        }
    }
}

struct CharachterCount{
    int charachter;
    int count;
};

int char_cmp(const void *a, const void *b){
    struct CharachterCount ca = *(struct CharachterCount*)a;
    struct CharachterCount cb = *(struct CharachterCount*)b;
    return cb.count - ca.count;
}
// count how many times each character from [FIRST_CHAR, LAST_CHAR) occurs
// in the text read from stream. Sort chars according to their respective
// cardinalities (decreasing order). Set n_char and cnt to the char_no - th char
// in the sorted list and its cardinality respectively
void char_count(int char_no, int *n_char, int *cnt, FILE *stream){
    struct CharachterCount char_count[MAX_CHARS];
    int c;
    for (int i = 0; i < MAX_CHARS; ++i) {
        char_count[i].charachter = FIRST_CHAR+i;
        char_count[i].count = 0;
    }
    while ((c = fgetc(stream)) != EOF) {
        if (c >= FIRST_CHAR && c <= LAST_CHAR) {
            char_count[c - FIRST_CHAR].count += 1;
        }
    }
//    for (int i = 0; i < MAX_CHARS; ++i) {
//        printf("%c %d", char_count[i].charachter, char_count[i].count);
//    }
    qsort(char_count,MAX_CHARS,sizeof(struct CharachterCount),char_cmp);

    *n_char = char_count[char_no-1].charachter;
    *cnt = char_count[char_no-1].count;
}

// count how many times each digram (a pair of characters, from [FIRST_CHAR,
// LAST_CHAR) each) occurs in the text read from stream. Sort digrams according
// to their respective cardinalities (decreasing order). Set digram[0] and
// digram[1] to the first and the second char in the digram_no - th digram_char
// in the sorted list. Set digram[2] to its cardinality.
#define MAX_L 128
void digram_count(int digram_no, int digram[], FILE *stream){
    char buffer[MAX_L];

    while(fgets(buffer, MAX_L, stream) != NULL){
        for(int i = 1; buffer[i - 1] != '\0' && buffer[i] != '\0'; ++i){
            if(buffer[i]>=FIRST_CHAR && buffer[i]<LAST_CHAR && buffer[i-1]>=FIRST_CHAR && buffer[i-1]<LAST_CHAR){
                count[((int)buffer[i-1] - FIRST_CHAR) * MAX_CHARS + ((int)buffer[i] - FIRST_CHAR)] += 1;
            }
        }
    }

    int index[MAX_DIGRAMS];
    for(int i = 0; i < MAX_DIGRAMS; ++i) {
        index[i] = i;
    }

    qsort(index, MAX_DIGRAMS, sizeof(int), cmp_di);
    digram[0] = index[digram_no - 1] / MAX_CHARS + FIRST_CHAR;
    digram[1] = index[digram_no - 1] % MAX_CHARS + FIRST_CHAR;
    digram[2] = count[index[digram_no - 1]];
}

// Count block and line comments in the text read from stream. Set
// line_comment_counter and block_comment_counter accordingly
void find_comments(int *line_comment_counter, int *block_comment_counter, FILE *stream){
    *line_comment_counter = 0;
    *block_comment_counter = 0;
    char buffer[MAX_L];
    int in_comment = 0, j = 0, bl_com_st, bl_com_end;
    while (fgets(buffer, MAX_L, stream) != NULL){
        j += 1;
        for (int i = 1; buffer[i-1] != '\0' && buffer[i] != '\0'; ++i) {
            if (buffer[i-1] == '/' && buffer[i] == '/' && !in_comment){
                *line_comment_counter += 1;
                break;
            }
            else if (!in_comment){
                if(buffer[i-1] == '/' && buffer[i] == '*') {
                    in_comment = 1;
                    bl_com_st = j;
                    bl_com_end = i-1;
                    *block_comment_counter += 1;
                }
            }
            else {
                if (buffer[i-1] == '*' && buffer[i] == '/'){
                    if (j != bl_com_st) {
                        in_comment = 0;
                    }
                    else {
                        if (i-bl_com_end > 2){
                            in_comment = 0;
                        }
                    }
                }
            }
        }
    }
}

#define MAX_LINE 128

int read_line() {
	char line[MAX_LINE];
	int n;

	fgets (line, MAX_LINE, stdin); // to get the whole line
	sscanf (line, "%d", &n);
	return n;
}

int main(void) {
	int to_do;
	int nl, nw, nc, char_no, n_char, cnt;
	int line_comment_counter, block_comment_counter;
	int digram[3];

	char file_name[40];
	FILE *stream;

	if(TEST) printf("Wpisz nr zadania ");
    to_do = read_line();      //    scanf ("%d", &to_do);
    if(TEST)  stream = stdin;
    else {
        scanf("%s",file_name);  read_line();
        stream = fopen(file_name,"r");
        if(stream == NULL) {
            printf("fopen failed\n");
            return -1;
        }
    }

//	to_do = read_line();
	switch (to_do) {
		case 1: // wc()
			wc (&nl, &nw, &nc, stream);
			printf("%d %d %d\n", nl, nw, nc);
			break;
		case 2: // char_count()
      if(TEST) printf("Wpisz numer znaku ");
      char_no = read_line();  //    scanf("%d",&char_no);
			char_count(char_no, &n_char, &cnt, stream);
			printf("%c %d\n", n_char, cnt);
			break;
		case 3: // digram_count()
      if(TEST) printf("Wpisz numer digramu ");
      char_no = read_line();   //  scanf("%d",&char_no);
			digram_count(char_no, digram, stream);
			printf("%c%c %d\n", digram[0], digram[1], digram[2]);
			break;
		case 4:
			find_comments(&line_comment_counter, &block_comment_counter, stream);
			printf("%d %d\n", block_comment_counter, line_comment_counter);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}
