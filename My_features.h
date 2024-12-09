#ifndef DRA_HEADER
#define DRA_HEADER
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <sys/stat.h>





#ifndef NDEBUG


    #define my_assert(condition)                                                                    \
        if ((condition) == true) {                                                                  \
            printf ("Assertaion Error : "#condition" , file  %s , line %d" , __FILE__ , __LINE__);  \
            abort ();                                                                               \
        }                                                                                           \

#else

    #define my_assert(condition)

#endif


typedef int (*My_Compare) (const void* point1 , const void* point2);


struct Text {
    size_t size ;
    int num_lines ;
    char * text_Onegin;
};

struct Inf_Lines {
    const char* str;
    size_t len;
};

struct Arr_Structs {
    struct Inf_Lines* arr_structs;
};


enum indicators_progress { // индикаторы выполнениия
    complete_value = 0 ,
    incomplete_value = 1 ,
};

enum indicators_compare {  // индикаторы сравнения
    str1_longer_str2 = 1 ,
    str2_longer_str1 = -1 ,
    str1_equals_str2 = 3802
};

enum Sort_Type {           // индикаторы типа сортировки
    SORT_ALPHABETICALLY = 1,
    SORT_BY_RHYME = 2 ,
    INCRE = 1 ,
    DECRE = -1
};

enum Error_fstat {
    error_fstat = -1
};

const float RAND_MAX_F = RAND_MAX;


const char* const yellow          = "\033[3;33;40m";
const char* const red             = "\033[3;31;40m";
const char* const blue_dark       = "\033[3;34;40m";
const char* const blue            = "\033[3;36;40m";
const char* const blue_dark_flash = "\033[5;34;40m";
const char* const green           = "\033[3;32;40m";
const char* const end             = "\033[0m";

int Calculate_Size_File (FILE* fp , struct Text* Onegin);
int Calculate_Num_Lines_File (FILE* fp , struct Text* Onegin);
char* Converting_Text_to_Arr (Text* Onegin , FILE* fp);
FILE* Open_File (const char* filename_read);
int Write_Sort_Files (const char* filename_write , struct Inf_Lines* arr_structs , Text* Onegin);
struct Inf_Lines* Filling_Arr_Structs (Text* Onegin , Arr_Structs* arr);
void* Bubble_Sort (void* arr , Text* Onegin , int el_size , int type_compare , My_Compare My_compare );
void My_qsort (void* data , int el_size ,  int start , int finish , My_Compare My_compare , int type_compare);
int My_Compare_Str_Alphabetically (const void* point1, const void* point2);
int My_Compare_Str_Rhyme (const void* point1, const void* point2);
int Compare_Int (const void* point1 , const void* point2);
int Process_Onegin (Text* Onegin , int* sort_type);
_Bool Check_Characters (char sym);
void print_usage ();
void Print_Array_Structs (struct Inf_Lines* arr_structs, size_t num_lines);
void Swap (void* point1 , void* point2 , int el_size);
int Random_in_Range (int min , int max);



#endif
