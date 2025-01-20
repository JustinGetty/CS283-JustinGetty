#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);
int  count_words(char *, int, int);
void reverse_string(char *, int);
void word_print(char *, int);

int setup_buff(char *buff, char *user_str, int len){
    int i = 0;
    int j = 0;
    int prev_whitespace = 1;

    while (*user_str != '\0' && i < len) {
        if (*user_str == ' ' || *user_str == '\t') {
            if (!prev_whitespace) {
                *(buff + j) = ' ';
                j++;
            }
            prev_whitespace = 1;
        } else {
            *(buff + j) = *user_str;
            j++;
            prev_whitespace = 0;
        }
        user_str++;
        i++;

        if (j >= len) {
            return -1;
        }
    }

    while (j < len) {
        *(buff + j) = '.';
        j++;
    }

    return i; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i = 0; i < len; i++){
        putchar(*(buff + i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int buff_len, int str_len){
    int word_count = 0;
    int in_word = 0;

    for (int i = 0; i < str_len; i++) {
        char c = *(buff + i);
        if (c == ' ') {
            if (in_word) {
                word_count++;
                in_word = 0;
            }
        } else {
            in_word = 1;
        }
    }
    if (in_word) {
        word_count++;
    }

    return word_count;
}

void reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
    printf("Reversed String: ");
    for (int i = 0; i < str_len; i++) {
        putchar(*(buff + i));
    }
    putchar('\n');
}

void word_print(char *buff, int str_len) {
    int word_index = 1;
    int word_length = 0;
    char *word_start = buff;

    printf("Word Print\n----------\n");

    for (int i = 0; i <= str_len; i++) {
        char c = *(buff + i);

        if (c == ' ' || c == '.' || c == '\0') { 
            if (word_length > 0) {
                printf("%d. ", word_index++);
                for (char *ch = word_start; ch < word_start + word_length; ch++) {
                    putchar(*ch);
                }
                printf(" (%d)\n", word_length);
                word_length = 0;
            }
            word_start = buff + i + 1; 
        } else {
            word_length++;
        }
    }
}


int main(int argc, char *argv[]){

    char *buff;             //placeholder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
    // The check ensures argv[1] exists and begins with a dash ('-').
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //TODO:  #2 Document the purpose of the if statement below
    // Ensures the required input string argument is provided.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = malloc(BUFFER_SZ);
    if (buff == NULL) {
        printf("Error: Memory allocation failed\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff); // Free memory before exiting
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            reverse_string(buff, user_str_len);
            break;

        case 'w':
            word_print(buff, user_str_len);
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Why provide both the pointer and the length?
// Providing both ensures functions can operate within the buffer's allocated size,
// preventing potential buffer overruns or access beyond valid memory regions.
