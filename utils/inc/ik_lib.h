#ifndef __IK_LIB_H__
#define __IK_LIB_H__

#pragma region External Includes

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#pragma endregion

#pragma region Constants

#define FLT_MIN 0.001f

#pragma endregion

#pragma region Types

typedef char*                   string;

typedef signed char             i8;
typedef signed short            i16;
typedef signed int              i32;
typedef signed long long        i64;

typedef unsigned char           u8;
typedef unsigned short          u16;
typedef unsigned int            u32;
typedef unsigned long long      u64;

typedef unsigned char           byte;

#ifndef __cplusplus
    typedef _Bool bool;
#   define false   0
#   define true    1
#endif //!__cplusplus

#ifdef DOUBLE_PRECISION
    typedef float               real;
#else
    typedef double              real;
#endif //!DOUBLE_PRECISION

#pragma endregion

#pragma region Structs, Other Typedefs

typedef bool (*compare_callback)(void *a, void *b);

typedef struct {
  char *cstring;
  u64 size;
} ik_string;

typedef struct
{
  void *data;
  u64 stride;
  u64 size;
  u64 capacity;
  compare_callback comparison;
} ik_array;

typedef struct
{
    u32 state_array[624];
    i32 state_index;
} ik_random;

#pragma endregion

#pragma region Clear Screen Logic

#ifdef _WIN32
    #include <conio.h>
    #define ik_clrscr() clrscr()
#else
    #define ik_clrscr() printf("\e[1;1H\e[2J")
#endif

#pragma endregion

#pragma region Console Cursor stuff

#ifdef _WIN32
    #TODO
#else
    #define ik_move_cursor_up(x) printf("\033[%sA", x)
    #define ik_move_cursor_down(x) printf("\033[%sB", x)
    #define ik_move_cursor_right(x) printf("\033[%sC", x)
    #define ik_move_cursor_left(x) printf("\033[%sD", x)
    #define ik_cursor_save_pos() printf("\033[s")
    #define ik_cursor_load_pos() printf("\033[u")

#endif

#pragma endregion

#pragma region Safe Delete

#define SAFEDELETE(x)   \
if (x != __null)        \
{                       \
    free(x);            \
    x = __null;         \
}

#pragma endregion

#pragma region Math

/**
* @brief min Function
* @param[in] a First Number
* @param[in] b Second Number
* @return the smaller Number
*/
extern i64 ik_min(i64 a, i64 b);

/**
* @brief max Function
* @param[in] a First Number
* @param[in] b Second Number
* @return the bigger Number
*/
extern i64 ik_max(i64 a, i64 b);

/**
 * @brief remaps a value from one range to another
 * @param[in] start_min the current range start
 * @param[in] start_max the current range end
 * @param[in] res_min the new range start
 * @param[in] res_max the new range end
 * @param[in, out] value the value to remap
 */
extern void ik_remap(i64 start_min, i64 start_max, i64 res_min, i64 res_max, i64* value);
#pragma endregion 

#pragma region Utility

/**
* @brief to lower case conversion
* @param[in] cstring a c-style string that is converted
*/
extern void ik_to_lower(char *cstring);

/**
* @brief checks if the input is a real number
* @param[in] cstring a c-style string that is checked
*/
extern bool ik_is_numeric(char *cstring);

/**
* @brief checks if the input is an integer
* @param[in] cstring a c-style string that is checked
*/
extern bool ik_is_int(char *cstring);

/**
* @brief rounds the input tp an integer
* @param[in] in_num the number to be converted
* @param[in,out] out_num the number that will store the result
*/
extern void ik_round_to_int(float in_num, int *out_num);

/**
* @brief checks if the input contains a specific character
* @param[in] cstring a c-style string that is checked
*/
extern bool ik_contains(char *cstring, char _char);

/**
 * @deprecated use ik_read_string() instead.
 */
extern bool ik_read_input(char* buffer, i32 bufferLength);

#define stringify(a) stringify_(a)
#define stringify_(a) #a

#define generate_params(funcname, ...)    \
typedef struct                            \
{                                         \
 ...                                      \
} stringify(funcname)_params;             \

typedef void(*measure_callback)(void*);
extern void ik_measure_time (char* name, void* params, measure_callback cb);
#pragma endregion

#pragma region String

typedef enum {
  align_left,
  align_right,
  align_middle
} align_options;

typedef enum {
  reserve_overflow,
  reserve_cut,
  dont_reserve
} reserve_space_options;

typedef enum {
    _string,
    _float,
    _int
} type_options;

/**
 * @brief Creates a string from a c-style string
 * @param[in,out] string the string object to be created
 * @param[in] cstring the c-style string to be managed by the string object
 * @note This string has its own managed memory. Call ik_string_destroy when you're done with it!
 */
extern void ik_string_make(ik_string* string, const char* cstring);

/**
 * @brief Creates a string from a c-style string
 * @param[in,out] string the string object to be created
 * @param[in] charcount the maximum length of the string
 * @note This string has its own managed memory. Call ik_string_destroy() when you're done with it!
 */
extern void ik_string_make_empty(ik_string* string, u64 charcount);

/**
 * @brief Destroys a string after usage
 * @param[in,out] string the string object to be deleted
 * @note after calling this, you can re-initialise this string again with ik_string_make()
 */
extern void ik_string_destroy(ik_string* string);

/**
 * @brief comparing function for strings
 * @param[in] a first string to compare
 * @param[in] b second string to compare
 * @return true if a is coming before b
 */
extern bool ik_string_compare(void* a, void* b);

/**
 * @brief a replace function for strings
 * @param[in,out] in the string to manipulate
 * @param[in] find the string to be replaced
 * @param[in] replace the replacement string 
 */
extern void ik_string_replace(ik_string* in, char* find, char* replace);

/**
 * @brief a replace function for strings
 * @param[in,out] in the string to manipulate
 * @param[in] start the start index
 * @param[in] end the end index
 * @param[in] replace the replacement string 
 */
extern void ik_string_replace_index(ik_string* in, int start, int end, char* replace);

/**
 * @brief checks if a string contains another string
 * @param[in] in the string to check
 * @param[in] find the string to find
 * @returns the starting index if the string to find is found, -1 if not
 */
extern int ik_string_contains(ik_string* in, char* find);

/**
 * @brief sets on string to another
 * @param[in, out] in the string to manipulate
 * @param[in] to the reference string
 */
extern void ik_string_set(ik_string* in, ik_string* to);

/**
 * @brief removes a string from another string, same as calling ik_string_replace(in, find, "")
 * @param[in, out] in the string to manipulate
 * @param[in] to the string to remove
 */
extern void ik_string_remove(ik_string* in, char* find);

/**
* @brief printing function for ik_string
* @param[in] in the string to print out
* @param[in] reserve options to reserve printing space for the string
* @param[in] spaces the amount of spaces to reserve
* @param[in] align options to align the string in the reserve
* @note (ONLY ON LINUX ATM!) you can also use color codes here. Use <(color)> to
* set the color. Valid colors are: 
* @note a - Default
* @note b - Black
* @note c - Dark Red
* @note d - Dark Green
* @note e - Dark Yellow
* @note f - Dark Blue
* @note g - Dark Magenta
* @note h - Dark Cyan
* @note i - Light Gray
* @note j - Dark Gray
* @note k - Red
* @note l - Green
* @note m - Orange
* @note n - Blue
* @note o - Magenta
* @note p - Cyan
* @note q - White
* @note Capitalize the letter if you want to change the Background instead.
* this is stackable, so if you do <c><R>, you get a dark red text with a white background
*/
extern void ik_print_string(ik_string *in, reserve_space_options reserve, int spaces, align_options align);

/**
* @brief reading an input into an ik_string.
* @return true if successful, false if not
* @param[in] string the string to parse input to
* @param[in] max_len maximum length of the input
* @param[in] type specify which type the input should match
* @param[in] return_code 1 if successful, 2 if input is to long, 3 if type missmatch
*/
extern bool ik_read_string(ik_string* string, int max_len, type_options type, int* return_code);

/**
 * @brief gets all occuruncies of an expression
 * @return true if theres atleast one occurency, false if not
 * @param[in] beginning the opening part of the expression 
 * @param[in] end the closing part of the expression
 * @param[in] in the string to check
 * @param[out] out the resulting indexes
 * @note even indexes mark the start of an expression, odd ones the end
 */
extern bool ik_get_expression_indexes(char beginning, char end, ik_string in, ik_array* out);

#pragma endregion

#pragma region Array

typedef enum
{
    asc, /**< Ascending */
    desc /**< Descending */
} ik_array_sort_mode;

/**
 * @brief Creates a dynamically growing array from the given parameters
 * @param[in,out] array is the array base memory to be managed
 * @param[in] stride_size is the block size of an object within the array
 * @param[in] num_elements represents the desired number of elements to be preallocated
 * @param[in] comparison is an additional user defined ptr-sized comparison function callback
 * @note This function creates memory on the heap. Call ik_array_destroy to free mem at the end of its life-time.
 */
extern void ik_array_make(ik_array *ik_array, u64 stride_size, u64 num_elements, compare_callback comparison);

/**
 * @brief Destroys the data from a given array.
 * @param[in,out] array is the array to be destroyed
 * @note This does not delete the object itself. It just removes all managed heap memory data and clears their assoc. vars.
 */
extern void ik_array_destroy(ik_array* ik_array);
/**
 * @brief Appends to the given array another element.
 * @param[in,out] array is the array to append to
 * @param[in] object is the array to append to
 * @note If there's no more memory available within the array, it grows linearily by 10 elements.
 */
extern void ik_array_append(ik_array* thisptr, void *object);

/**
 * @brief Removes an element from the given array
 * @param[in,out] array the array to remove an element from
 * @param[in] index the index of the element to be removed
 * @note O(n) linear removal at worst. Use when order matters.
 */
extern void ik_array_remove(ik_array* thisptr, u32 index);

/**
 * @brief Removes an element from the given array quickly
 * @param[in,out] array the array to remove an element from
 * @param[in] index the index of the element to be removed
 * @note O(1) constant removal. Use when performance matters but order does not.
 */
extern void ik_array_remove_fast(ik_array* thisptr, u32 index);

/**
 * @brief Sorts an array using bubble sort and a sorting mode
 * @param[in,out] array the array to be sorted
 * @param[in] mode is the mode of sorting (ascending or descending)
 * @note O(n*n) squared time sorting at worst.
 */
extern void ik_array_sort(ik_array* thisptr, ik_array_sort_mode mode);

/**
 * @brief grows the array's capacity by allocating more memory to the array
 * @param[in,out] thisptr the array to grow
 * @param[in] size the amount to grow the array 
 */
extern void ik_array_grow(ik_array * thisptr, u64 size);

/**
 * @brief Gets an element from the array
 * @param[in,out] array the array to be sorted
 * @param[in] index is the index of the element to be retrieved
 */
extern void* ik_array_get(ik_array* thisptr, u32 i);

/**
 * @brief swaps two elements of the array
 * @param[in,out] src the first element
 * @param[in,out] dst the second element
 * @param[in] size the memory size of one element
 */
extern void swap(void *src, void *dst, u64 size);

#pragma endregion

#pragma region Parser

/**
 * @brief find the first index of a comma
 * @param[in] text the string to look in
 * @returns the index of the comma or -1 if there is none
 * @note this is really just a helper function. use ik_string_contains() instead
 */
extern i32 ik_parser_comma_index(char *text);

/**
 * @brief get the length of the decimal part of a number 
 * @param[in] text the string to look in
 * @returns the length of the decimal part
 * @note this does not validate if the string is a number
 */
extern u64 ik_parser_decimal_count(char *text, i32 comma_index);

/**
 * @brief parsing a string to an integer
 * @param[in] text the text to parse
 * @param[in,out] out_number the number to parse to
 * @returns true if successful, false if noty
 */
extern bool ik_parser_parse_as_int(ik_string *text, int *out_number);

/**
 * @brief parsing a string to an float
 * @param[in] text the text to parse
 * @param[in,out] out_number the number to parse to
 * @returns true if successful, false if noty
 */
extern bool ik_parser_parse_as_float(ik_string *text, float *out_number);

#pragma endregion

#pragma region Random

extern void ik_random_init(ik_random* state, u32 seed);

extern void ik_random_next(ik_random* state, i32* out);

#pragma endregion

#endif //!__IK_LIB_H__