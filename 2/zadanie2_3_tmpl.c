#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "limits.h"
#define CBUFF_SIZE  52
#define QUEUE_SIZE 52

double queue_push(int val, int queue[], int in);
int queue_pop(int how_many, int queue[]);
int queue_state(void);
void queue_print(int queue[], int in);
double cbuff_push(int cli_nr, int *len, int out, int cbuff[]);
int cbuff_pop(int *len, int *out, int cbuff[]);
void cbuff_print(int len, int out, int cbuff[]);
int card_val(int card);
int conflict(int a, int b);
void rand_permutation(int n, int tab[]);
int rand_from_interval(int a, int b);
void print_card(int card);

int main(void) {
    int seed, version, conflicts;
    scanf("%d%d%d", &seed, &version, &conflicts);
    srand(seed);
    int deckA[CBUFF_SIZE], deckB[CBUFF_SIZE], deck[CBUFF_SIZE];
    int lenA = CBUFF_SIZE/2, outA = 0 , lenB = CBUFF_SIZE/2, outB = 0;
    rand_permutation(CBUFF_SIZE, deck);
    for (int i = 0; i < CBUFF_SIZE/2; ++i) {
        deckA[i] = deck[i];
        deckB[i] = deck[i+CBUFF_SIZE/2];
    }
    int i = 0;
    if (version){
        while (conflicts != i){
            //cbuff_print(lenA, outA, deckA);
            //cbuff_print(lenB, outB, deckB);
            int a = cbuff_pop(&lenA, &outA, deckA);
            int b = cbuff_pop(&lenB, &outB, deckB);
            int cA = card_val(a);
            int cB = card_val(b);
            if(cA>cB){
                cbuff_push(a, &lenA, outA, deckA);
                cbuff_push(b, &lenA, outA, deckA);
            }
            else if (cB>cA) {
                cbuff_push(b, &lenB, outB, deckB);
                cbuff_push(a, &lenB, outB, deckB);
            }
            else {
                cbuff_push(a, &lenA, outA, deckA);
                cbuff_push(b, &lenB, outB, deckB);
            }
            if (lenA == 0) {
                printf("3 ");
                cbuff_print(lenB, outB, deckB);
                return 0;
            }
            if (lenB == 0){
                printf("2 %d", ++i);
                return 0;
            }
            i += 1;
        }
        printf("0 %d %d", lenA, lenB);
    }
    else {
        int war_qA[QUEUE_SIZE] = {0};
        int war_qB[QUEUE_SIZE] = {0};
        while(conflicts != i){
            int a = cbuff_pop(&lenA, &outA, deckA);
            int b = cbuff_pop(&lenB, &outB, deckB);
            int cA = card_val(a);
            int cB = card_val(b);
            if(cA>cB){
                cbuff_push(a, &lenA, outA, deckA);
                cbuff_push(b, &lenA, outA, deckA);
            }
            else if (cB>cA) {
                cbuff_push(b, &lenB, outB, deckB);
                cbuff_push(a, &lenB, outB, deckB);
            }
            else {
                int j = 0;
                queue_push(a, war_qA, j);
                queue_push(b, war_qB, j);
                while(1) {
                    j += 1;
                    a = cbuff_pop(&lenA, &outA, deckA);
                    b = cbuff_pop(&lenB, &outB, deckB);
                    if (a == -1 || b == -1 || lenB == 0){
                        return 0;
                    }
                    cA = card_val(a);
                    cB = card_val(b);
                    queue_push(a, war_qA, j);
                    queue_push(b, war_qB, j);
                    if (j%2 == 0) {
                        i += 1;
                        if (cA > cB) {
                            for (int k = 0; k <= j; ++k) {
                                a = queue_pop(1, war_qA);
                                cbuff_push(a, &lenA, outA, deckA);
                            }
                            for (int k = 0; k <= j; ++k) {
                                b = queue_pop(1, war_qB);
                                cbuff_push(b, &lenA, outA, deckA);
                            }
                            break;
                        }
                        else if (cB>cA) {
                            for (int k = 0; k <= j; ++k) {
                                b = queue_pop(1, war_qB);
                                cbuff_push(b, &lenB, outB, deckB);
                            }
                            for (int k = 0; k <= j; ++k) {
                                a = queue_pop(1, war_qA);
                                cbuff_push(a, &lenB, outB, deckB);
                            }
                            break;
                        }
                    }
                }
            }
            if (lenA == 0) {
                printf("3 ");
                cbuff_print(lenB, outB, deckB);
                return 0;
            }
            if (lenB == 0){
                printf("2 %d", ++i);
                return 0;
            }
            i += 1;
        }
        printf("0 %d %d", lenA, lenB);
    }

    return 0;
}



double cbuff_push(int cli_nr, int *len, int out, int cbuff[]) {
    if (*len >= CBUFF_SIZE)
        return INFINITY;
    cbuff[(out+*len)%CBUFF_SIZE] = cli_nr;
    *len += 1;
    return 0;
}

int cbuff_pop(int *len, int *out, int cbuff[]) {
    if (len == 0)
        return -1;
    *out += 1;
    *len -= 1;
    return cbuff[*out-1];
}

int cbuff_state(int len) {
    return len;
}

void cbuff_print(int len, int out, int cbuff[]) {
    for (int i = out; i < len + out; ++i) {
        printf("%d ",cbuff[i%CBUFF_SIZE]);
    }
    printf("\n");
}

void rand_permutation(int n, int tab[]) {
    if (n<0 || n>100)
        return;
    for (int i = 0; i < n; ++i) {
        tab[i] = i;
    }
    for (int i = 0; i < n-1; ++i) {
        int k = rand_from_interval(i, n-1);
        int temp = tab[i];
        tab[i] = tab[k];
        tab[k] = temp;
    }
}

int rand_from_interval(int a, int b) {
    if (a>b)
        return INT_MIN;
    if (b-a > RAND_MAX)
        return INT_MAX;
    if (a==b)
        return a;
    return rand()%(b-a+1) + a;
}

int card_val(int card){
    return floor(card/4);
}

int conflict(int a, int b){
    a = card_val(a);
    b = card_val(b);
    if (a>b)
        return 1;
    else if (b<a)
        return -1;
    else
        return 0;
}

void print_card(int card){
    char *cards[] = {"2", "3", "4", "5", "6","7", "8", "9", "10", "J", "Q", "K", "A"};
    printf("%s ", cards[card_val(card)]);
}

double queue_push(int val, int queue[], int in) {
    int flag = 1;
    queue[in] = val;
    if (in > QUEUE_SIZE)
        in = QUEUE_SIZE;
    if (!flag)
        return INFINITY;
    else
        return 0;
}

int queue_pop(int how_many, int queue[]) {
    if(how_many > QUEUE_SIZE){
        return -1;
    }
    else{
        int res = queue[0];
        for (int i = 0; i < QUEUE_SIZE-1; ++i) {
            queue[i] = queue[i+1];
        }
        return res;
    }
}

void queue_print(int queue[], int in) {
    for (int i = 0; i < in; ++i) {
        printf("%d ", queue[i]);
    }
}
