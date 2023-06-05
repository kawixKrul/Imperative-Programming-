#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_STR_LEN 64

typedef struct Vector {
    void *data;
    size_t element_size;
    size_t size;
    size_t capacity;
} Vector;

typedef struct{
    void *data;
    size_t size;
} Vector_elem;

typedef struct Person {
    int age;
    char first_name[MAX_STR_LEN];
    char last_name[MAX_STR_LEN];
} Person;

typedef int(*cmp_ptr)(const void*, const void*);
typedef int(*predicate_ptr)(void*);
typedef void(*read_ptr)(void*);
typedef void(*print_ptr)(const void*);

// Allocate vector to initial capacity (block_size elements),
// Set element_size, size (to 0), capacity
void init_vector(Vector *vector, size_t block_size, size_t element_size){
    vector->data = malloc(element_size*block_size);
    vector->capacity = block_size;
    vector->size = 0;
    vector->element_size = element_size;
}

// If new_capacity is greater than the current capacity,
// new storage is allocated, otherwise the function does nothing.
void reserve(Vector *vector, size_t new_capacity){
    if(new_capacity <= vector->capacity)
        return;
    size_t target = 2*(vector->capacity);
    while(target < new_capacity)
        target <<= 1;

    vector->data = realloc(vector->data, target*(vector->element_size));
    vector->capacity = target;
}

size_t ptr_diff(void *start, void *end){
    return (size_t)end - (size_t)start;
}

void *ptr_add(void *start, size_t offset){
    char *s = (char*)start;
    return s+offset;
}


//Funkcja może przeskoczyć rozmiar wektora, używać do dodawania elementów
Vector_elem at_unsafe(Vector *vector, size_t ix){
    Vector_elem e;
    e.data = ptr_add(vector->data, ix*(vector->element_size));
    e.size = vector->element_size;
    return e;
}

Vector_elem at(Vector *vector, size_t ix){
    if(ix >= vector->size)
        abort();
    return at_unsafe(vector, ix);
}

// Resizes the vector to contain new_size elements.
// If the current size is greater than new_size, the container is
// reduced to its first new_size elements.

// If the current size is less than new_size,
// additional zero-initialized elements are appended
void resize(Vector *vector, size_t new_size){
    if(vector->size == new_size)
        return;

    reserve(vector, new_size);

    if(vector->size < new_size){
        void *start = at_unsafe(vector, vector->size).data;
        void *end = at_unsafe(vector, new_size).data;
        memset(start, 0, ptr_diff(start, end));
    }
    vector->size = new_size;
}

// Add element to the end of the vector
void push_back(Vector *vector, void *value){
    reserve(vector, (vector->size)+1);
    void *last_element = at_unsafe(vector, vector->size).data;
    memcpy(last_element, value, vector->element_size);
    (vector->size)++;
}

// Remove all elements from the vector
void clear(Vector *vector){
    vector->size = 0;
}

// Insert new element at index (0 <= index <= size) position
void insert(Vector *vector, size_t index, void *value){
    if(index > vector->size)
        return;

    reserve(vector, vector->size+1);
    void *new_elem = at_unsafe(vector, index).data;
    void *last_elem = at_unsafe(vector, vector->size).data;
    memmove(ptr_add(new_elem, vector->element_size), new_elem, ptr_diff(new_elem, last_elem));
    memcpy(new_elem, value, vector->element_size);
    (vector->size)++;
}

// Erase element at position index
void erase(Vector *vector, size_t index){
    if(index >= vector->size)
        return;

    void *del_elem = at(vector, index).data;
    memmove(del_elem, ptr_add(del_elem, vector->element_size), (vector->size-1-index)*(vector->element_size));
    (vector->size)--;
}

// Erase all elements that compare equal to value from the container
void erase_value(Vector *vector, void *value, cmp_ptr cmp){
    for(int i = vector->size-1; i >= 0; i--){
        if(cmp(at(vector, i).data, value) == 0)
            erase(vector, i);
    }
}

// Erase all elements that satisfy the predicate from the vector
void erase_if(Vector *vector, int (*predicate)(void *)){
    for(int i = vector->size-1; i >= 0; i--){
        if(predicate(at(vector, i).data)){
            erase(vector, i);
        }
    }
}

// Request the removal of unused capacity
void shrink_to_fit(Vector *vector){
    size_t new_capacity = vector->size;
    if(new_capacity >= vector->capacity)
        return;

    vector->capacity = new_capacity;
    vector->data = realloc(vector->data, new_capacity*(vector->element_size));
}

// integer comparator
int int_cmp(const void *v1, const void *v2){
    int a = *(int*)v1;
    int b = *(int*)v2;

    return a-b;
}

// char comparator
int char_cmp(const void *v1, const void *v2){
    char a = *(char*)v1;
    char b = *(char*)v2;

    return a-b;
}

// Person comparator:
// Sort according to age (decreasing)
// When ages equal compare first name and then last name
int person_cmp(const void *p1, const void *p2){
    Person a = *(Person*)p1;
    Person b = *(Person*)p2;

    if(a.age == b.age){
        int cmp_first = strcmp(a.first_name, b.first_name);
        if(cmp_first == 0)
            return strcmp(a.last_name, b.last_name);
        return cmp_first;
    }
    return b.age - a.age;
}

// predicate: check if number is even
int is_even(void *value){
    int num = *(int*)value;

    if(!(num%2))
        return 1;
    return 0;
}

// predicate: check if char is a vowel
int is_vowel(void *value){
    char vowels[12] = "aeiouyAEIOUY";

    char c = *(char*)value;

    for(int i = 0; i < 12; i++){
        if(c == vowels[i])
            return 1;
    }
    return 0;
}

// predicate: check if person is older than 25
int is_older_than_25(void *person){
    Person p = *(Person*)person;

    if(p.age > 25)
        return 1;
    return 0;
}

// print integer value
void print_int(const void *v){
    printf("%d ", *(int*)v);
}

// print char value
void print_char(const void *v){
    printf("%c ", *(char*)v);
}

// print structure Person
void print_person(const void *v){
    Person p = *(Person*)v;
    printf("%d %s %s\n", p.age, p.first_name, p.last_name);
}

// print capacity of the vector and its elements
void print_vector(Vector *vector, print_ptr print){
    void *ptr = vector->data;
    printf("%zu\n", vector->capacity);
    for(int i = vector->size-1; i >= 0; i--){
        print(ptr);
        ptr += vector->element_size;
    }
    printf("\n");
}

// read int value
void read_int(void* value){
    int *addr = (int*)value;
    scanf("%d", addr);
}

// read char value
void read_char(void* value){
    char *addr = (char*)value;
    scanf(" %c", addr);
}

// read struct Person
void read_person(void* value){
    Person *p = (Person*)value;
    scanf(" %d", &p->age);
    scanf(" %s", p->first_name);
    scanf(" %s", p->last_name);
}

void vector_test(Vector *vector, size_t block_size, size_t elem_size, int n, read_ptr read,
                 cmp_ptr cmp, predicate_ptr predicate, print_ptr print) {
    init_vector(vector, block_size, elem_size);
    void *v = malloc(vector->element_size);
    size_t index, size;
    for (int i = 0; i < n; ++i) {
        char op;
        scanf(" %c", &op);
        switch (op) {
            case 'p': // push_back
                read(v);
                push_back(vector, v);
                break;
            case 'i': // insert
                scanf("%zu", &index);
                read(v);
                insert(vector, index, v);
                break;
            case 'e': // erase
                scanf("%zu", &index);
                erase(vector, index);
                break;
            case 'v': // erase
                read(v);
                erase_value(vector, v, cmp);
                break;
            case 'd': // erase (predicate)
                erase_if(vector, predicate);
                break;
            case 'r': // resize
                scanf("%zu", &size);
                resize(vector, size);
                break;
            case 'c': // clear
                clear(vector);
                break;
            case 'f': // shrink
                shrink_to_fit(vector);
                break;
            case 's': // sort
                qsort(vector->data, vector->size,
                      vector->element_size, cmp);
                break;
            default:
                printf("No such operation: %c\n", op);
                break;
        }
    }
    print_vector(vector, print);
    free(vector->data);
    free(v);
}

int main(void) {
    int to_do, n;
    Vector vector_int, vector_char, vector_person;

    scanf("%d%d", &to_do, &n);

    switch (to_do) {
        case 1:
            vector_test(&vector_int, 4, sizeof(int), n, read_int, int_cmp,
                        is_even, print_int);
            break;
        case 2:
            vector_test(&vector_char, 2, sizeof(char), n, read_char, char_cmp,
                        is_vowel, print_char);
            break;
        case 3:
            vector_test(&vector_person, 2, sizeof(Person), n, read_person,
                        person_cmp, is_older_than_25, print_person);
            break;
        default:
            printf("Nothing to do for %d\n", to_do);
            break;
    }

    return 0;
}
