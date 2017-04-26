/* htable-main.c
 * COSC242 Assignment
 * Group 10 - Nadja Jury, Alex McKirdy, Mika Smith
 */

/* All required imports.*/
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "mylib.h"
#include "htable.h"


/* Prints out a statement stating a word and how many times it apears.
 * @param freq is the amount of times a word appears.
 * @param word is the given word that is counted. 
 */
static void print_info(int freq, char *word){
    fprintf(stdout, "%-4d %s\n", freq, word);
}

/* Finds the next highest prime than the given number.
 * @param the given number.
 * @return either a 1 if it's not a prime number, or 0 if it is.
 */
static int find_prime(int num){
    //Initalising variables
    int count=2;
    while(count <=sqrt(num)){
        if(num % count ==0){
            return 0;
        }else{
            count++;
        }
    }
    return 1; 
}


/* Removes punctuation and transforms string in to all lowercase.
 * @param str is the given string to edit.
 * @return the transformed string.
 */
static char *strlwr(char *str){
    /*Initalising variables*/
    char * result = emalloc(sizeof *str);
    int i, result_index=0;
    int len = (int)strlen(str);
    
    /*Removes non-alpha characters.*/
    for(i=0; i < len; i++){
        if(!ispunct(str[i])){
            result[result_index]=tolower(str[i]);
            result_index++;
        }
    }

    return result;
}


/* The main method. Includes all command line argument cases.
 * @param argc
 * @param argv
 */
int main(int argc, char **argv) {
    /*Initalising variables*/
    int table_size=113;
    htable h= htable_new(table_size, LINEAR_P);
    char word[256];
    const char *optstring ="t:dc:eps:h";
    char option;
    FILE *infile;
    int spell_count=0, num_shots=10;
    clock_t start, end;
    double fill_time, search_time;
    hashing_t hash_choice = LINEAR_P;
    bool spell_check= 0, p_chosen=0;

    
    while((option=getopt(argc, argv, optstring)) != EOF){       
        switch(option){
            case 't':
                table_size=atoi(optarg)+1;
                while(table_size>0){
                    if(find_prime(table_size)==1){
                        break;
                    }else{
                        table_size++;
                    }
                }
                break;
            case 'd':
                /*Choose double hashing.*/
                hash_choice = DOUBLE_H;   
                break;
            case 'c':
                spell_check= 1;
                infile = fopen(optarg, "r");
                break;               
            case 'e':
                htable_print_entire_table(h);
                break;
            case 'h':
                printf("Performs various operations using a hash table.");
                printf(" By default, words are read from stdin and added to");
                printf(" the hash table, before being printed out alongside");
                printf(" their \nfrequencies to stdout.\n\n");
                printf("-c FILENAME   Check spelling of words in FILENAME");
                printf(" using words from stdin as \ndictionary. Print unknown");
                printf(" words to stdout, timing info & count to stderr");
                printf(" (ignore -p)\n");
                printf("-d            Use double hashing (linear probing is");
                printf(" the default)\n-e            Display entire");
                printf(" contents of hash table on stderr\n-p            ");
                printf("Print stats info instead of frequencies & words\n");
                printf("-s SNAPSHOTS  Show SNAPSHOTS stats snapshots (if -p");
                printf(" is used)\n-t TABLESIZE  Use the first prime >=");
                printf(" TABLESIZE as htable size\n\n-h             Display");
                printf(" this message");
                break;          
            case 'p':
                p_chosen=1;
                /*htable_print_stats(h, stdout, 10);;*/
                break;
            case 's':
                num_shots=atoi(optarg);            
                break;
            default:
                printf("Default!");
                break;
        }
      
    }


    if(spell_check==1){
        h= htable_new(table_size, hash_choice);
        start=clock();
        while(getword(word, sizeof word, stdin) != EOF){
            htable_insert(h, strlwr(word));
        }
        end=clock();

        fill_time = ((end-start) / (double)CLOCKS_PER_SEC); 

        start=clock();
        while(1==fscanf(infile,"%s", word)){
            if(htable_search(h, strlwr(word))==0){
                printf("%s\n", strlwr(word));
                spell_count++;
            }
        }
        end= clock();
        search_time = ((end-start) / (double)CLOCKS_PER_SEC); 
        fprintf(stderr,"Fill time     : %1.6f\n", fill_time);
        fprintf(stderr, "Search time   : %1.6f\n", search_time);
        fprintf(stderr,"Unknown words = %d\n", spell_count);
                
        fclose(infile);
    }else{
        h= htable_new(table_size, hash_choice);
        while(getword(word, sizeof word, stdin) != EOF){
            htable_insert(h, strlwr(word));
        }
        /*pass print_info to htable_print.*/
        htable_print(h, print_info);             

    }

    if(p_chosen == 1){
        htable_print_stats(h, stdout, num_shots);
    }
    
    htable_free(h);

    return EXIT_SUCCESS;

}
