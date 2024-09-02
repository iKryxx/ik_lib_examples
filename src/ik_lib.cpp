#pragma region Internal Includes

#include "ik_lib.h"

#pragma endregion

#pragma region Math

i64 ik_min(i64 a, i64 b)
{
    return (a < b) * a + (b <= a) * b;
}

i64 ik_max(i64 a, i64 b)
{
    return (a > b) * a + (b >= a) * b;
}

void ik_remap(i64 start_min, i64 start_max, i64 res_min, i64 res_max, i64* value) {
    *value = res_max + (*value - start_min) * (res_max - res_min) / (start_max - start_min);
}

#pragma endregion

#ifdef _WIN32
COORD cursor_coords;
#endif

#pragma region Utility

void ik_to_lower(char *cstring)
{
    if (0 == cstring)
        return;

    for (int i = 0; i < strlen(cstring); i++)
    {
        cstring[i] += (cstring[i] >= 65 && cstring[i] <= 90) * 32;
    }
}

bool ik_is_numeric(char *cstring)
{
    bool has_comma = false;
    for (int i = 0; i < strlen(cstring); i++)
    {

        if (i == 0 && (cstring[i] == ',' || cstring[i] == '.'))
            return false;
        if (i != 0 && cstring[i] == '\n')
            continue;
        if ((cstring[i] == ',' || cstring[i] == '.'))
        {
            if (has_comma)
                return false;
            has_comma = true;
            continue;
        }
        if (cstring[i] < '0' || cstring[i] > '9')
            return false;
    }
    return true;
}

bool ik_is_int(char *cstring)
{
    return ik_is_numeric(cstring) && (!ik_contains(cstring, ',') && !ik_contains(cstring, '.'));
}

void ik_round_to_int(float in_num, int *out_num)
{
    *out_num = (int)in_num;
}


bool ik_contains(char *cstring, char _char)
{
    for (int i = 0; i < strlen(cstring); i++)
    {
        if (cstring[i] == _char)
            return true;
    }
    return false;
}

bool ik_read_input(char *buffer, i32 bufferLength)
{
    fgets(buffer, bufferLength + 1, stdin);

    if (strlen(buffer) == bufferLength)
        return false;
    return true;
}


void ik_measure_time (char* name, void* params, measure_callback cb) {
    clock_t t;
    ik_string stripe, profiling, time = { };
    ik_string_make(&stripe, "<a>------------------------------------------------------------\n");
    char* _p = (char*)malloc(strlen(name) + 16);
    sprintf(_p, "[%s] <k>Profiling...\n", name);

    ik_string_make(&profiling, _p);


    ik_print_string(&stripe, reserve_space_options::dont_reserve, 0, align_options::align_left);
    ik_print_string(&profiling, reserve_space_options::dont_reserve, 0, align_options::align_left);
    ik_print_string(&stripe, reserve_space_options::dont_reserve, 0, align_options::align_left);
    
    ik_cursor_save_pos();
    ik_move_cursor_up(2);
    char con[5];
    sprintf(con, "%lli", (strlen(name) + 16));
    ik_move_cursor_right(atoi(con));
    fflush(stdout);
    
    ///////////////////////
    t = clock();
    cb(params);
    t = clock() - t;
    ///////////////////////
    
    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b");
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    char _t[30];
    sprintf(_t, "<l>took %.4fs\n", time_taken);
    ik_string_make(&time, _t);

    ik_print_string(&time, reserve_space_options::reserve_cut, 60 - (int)strlen(name) + 4, align_options::align_right);
    ik_cursor_load_pos();
    printf("\n");

    free(_p);
}



#pragma endregion

#pragma region String

void ik_string_make(ik_string* string, const char* cstring)
{
    if (0 == string || 0 == cstring)
    {
        return;
    }

    u64 len = strlen(cstring);
    string->cstring = (char *)malloc(len + sizeof(char));

    if (!string->cstring)
    {
        return;
    }

    memset(string->cstring, '\0', sizeof(char) + len);
    memcpy(string->cstring, cstring, len);
    string->size = len;
}

void ik_string_make_empty(ik_string *string, u64 charcount)
{
    if (0 == string)
    {
        return;
    }
    string->cstring = (char *)malloc(charcount + sizeof(char));

    if (!string->cstring)
    {
        return;
    }

    memset(string->cstring, '\0', sizeof(char) + charcount);
    string->size = charcount;
}

void ik_string_make_range(ik_string* string, const char* cstring, u64 start, u64 end)
{
    if (0 == string || 0 == cstring || end < start)
    {
        return;
    }

    u64 len = (end - start);
    string->cstring = (char*)malloc(len + sizeof(char));

    if (!string->cstring)
    {
        return;
    }

    memset(string->cstring, '\0', sizeof(char) + len);
    memcpy(string->cstring, cstring, len);
    string->size = len;
}

void ik_string_destroy(ik_string *string)
{
    SAFEDELETE(string->cstring);
    string->size = 0;
}

bool ik_string_compare(void *a, void *b)
{
    u64 upper_limit = ik_min(((ik_string *)a)->size, ((ik_string *)b)->size);
    ik_string _a;
    ik_string _b;

    memset(&_a, '\0', sizeof(ik_string));
    memset(&_b, '\0', sizeof(ik_string));

    ik_string_make(&_a, ((ik_string *)a)->cstring);
    ik_string_make(&_b, ((ik_string *)b)->cstring);

    ik_to_lower(_a.cstring);
    ik_to_lower(_b.cstring);

    for (int i = 0; i < upper_limit; i++)
    {
        if (_a.cstring[i] == _b.cstring[i])
        {
            continue;
        }

        bool ret = _a.cstring[i] > _b.cstring[i];
        ik_string_destroy(&_a);
        ik_string_destroy(&_b);

        return ret;
    }
    return false;
}

void ik_string_replace(ik_string *in, char *find, char *replace)
{
    if (strlen(replace) == 0)
    {
        ik_string_remove(in, find);
        return;
    }
    int index = ik_string_contains(in, find);
    if (index == -1)
        return;

    ik_string _new = {};

    ik_string_make_empty(&_new, in->size - strlen(find) + strlen(replace));

    bool has_replaced = false;
    for (int i = 0; i < _new.size; i++)
    {
        if (i >= index && i < index + strlen(replace))
        {
            (&_new)->cstring[i] = *(replace + i - index);
            has_replaced = true;
        }
        else
        {
            // if it has replaced the word, it offsets the index to continue with the rest of the original
            (&_new)->cstring[i] = in->cstring[i - (has_replaced ? strlen(replace) - strlen(find) : 0)];
        }
    }
    ik_string_set(in, &_new);
}

void ik_string_replace_index(ik_string* in, int start, int end, char* replace){
    if(start > in->size - 2 || start < 0 || end > in->size - 1 || end < 0 || start >= end) return;

    ik_string _new = { };
    ik_string_make_empty(&_new, in->size - (end - start) + strlen(replace));
    bool has_replaced = false;
    for (int i = 0; i < _new.size; i++)
    {
        if (i >= start && i < start + strlen(replace))
        {
            (&_new)->cstring[i] = *(replace + i - start);
            has_replaced = true;
        }
        else
        {
            // if it has replaced the word, it offsets the index to continue with the rest of the original
            (&_new)->cstring[i] = in->cstring[i - (has_replaced ? strlen(replace) - (end - start + 1) : 0)];
        }
    }
    ik_string_set(in, &_new);

}

int ik_string_contains(ik_string *in, char *find)
{
    for (int i = 0; i < in->size; i++)
    {
        bool contains = true;
        for (int j = 0; j < strlen(find); j++)
        {
            if (in->cstring[i + j] != find[j])
            {
                contains = false;
                break;
            }
        }
        if (contains)
            return i;
    }
    return -1;
}

void ik_string_set(ik_string *in, ik_string *to)
{
    ik_string_destroy(in);

    in->cstring = to->cstring;
    in->size = to->size;
}

void ik_string_remove(ik_string *in, char *find)
{
    int index = ik_string_contains(in, find);
    if (index == -1)
        return;

    ik_string _new = {};

    ik_string_make_empty(&_new, in->size - strlen(find));

    bool has_replaced = false;
    int j = 0;
    for (int i = 0; i < strlen(in->cstring) - strlen(find); i++)
    {
        if (i == index)
            j += (int)strlen(find);
        (&_new)->cstring[i] = in->cstring[i + j];
    }
    ik_string_set(in, &_new);
}

void ik_print_string(ik_string *in, reserve_space_options reserve, int spaces, align_options align)
{
    //formatting validating colors
    ik_array found_exps = { };
    ik_array_make(&found_exps, sizeof(size_t), 10, NULL);
    ik_get_expression_indexes('<', '>', *in, &found_exps);
    ik_array valid_exps = { };
    ik_array_make(&valid_exps, sizeof(size_t), 10, NULL);

    size_t replacement_offset = 0;
    for (size_t i = 0; i < found_exps.size; i += 2)
    {
        int begin = *(int*)ik_array_get(&found_exps, (u32)i);
        int end = *(int*)ik_array_get(&found_exps, i + 1);
        if(end - begin == 2)
        {
            char option = (*in).cstring[end-1 + replacement_offset];
            int num = 0;

            //offsets: 
            //    a / A: -58 / -16 (==97 / ==65)
            //b-i / B-I: -68 / -26 (<104 / <72)
            //h-o / H-O: -14 / +28 (<112 / <80)

            if(option >= 97 && option < 114){ //textcolor
                if(option == 97) num = option - 58;
                else if(option < 106) num = option - 68;
                else if(option < 114) num = option - 16; 
            }
            else if(option >= 65 && option < 82){ //background
                if(option == 65) num = option - 16;
                else if(option < 74) num = option - 26;
                else if(option < 82) num = option + 26;
            }
            if(num != 0) {
                char code[8];
                sprintf(code, "\033[%im", num);
                ik_string_replace_index
                (
                    in, 
                    begin + replacement_offset, 
                    end + replacement_offset, code
                );                
                if(num >= 100) replacement_offset += (3);
                else replacement_offset += (2);
            }
        }
    }
    

    //printing logic
    if (reserve == reserve_space_options::dont_reserve)
    {
        printf("%s", in->cstring);
        return;
    }
    int max_len = 0;
    int start_spaces = 0;
    if (reserve == reserve_space_options::reserve_cut)
        max_len = spaces;
    if (reserve == reserve_space_options::reserve_overflow)
        max_len = ik_max(spaces, in->size);
    if (align == align_options::align_left)
        start_spaces = 0;
    if (align == align_options::align_middle)
        start_spaces = ik_max((int)((max_len - in->size) / 2), 0);
    if (align == align_options::align_right)
        start_spaces = ik_max(max_len - in->size, 0);

    int start_space_counter = start_spaces;
    for (int i = 0; i < max_len; i++)
    {
        if (start_space_counter != 0)
        {
            printf(" ");
            start_space_counter--;
            continue;
        }
        int string_offset = i - start_spaces;
        if (string_offset < in->size)
        {
            printf("%c", in->cstring[string_offset]);
        }
        else
        {
            printf(" ");
        }
    }
}

bool ik_read_string(ik_string *string, int max_len, type_options type, int *return_code)
{
    char Newline[] = "\n";
    char* buffer = (char*)malloc(max_len);
    fgets(buffer, max_len + 1, stdin);

    if (strlen(buffer) == max_len)
    {
        *return_code == 2;
        return false;
    }
    if (type == type_options::_string)
    {
        ik_string_make(string, buffer);
        ik_string_remove(string, Newline);
        *return_code = 1;
        return true;
    }
    else if (type == type_options::_float && ik_is_numeric(buffer))
    {
        ik_string_make(string, buffer);
        ik_string_remove(string, Newline);
        *return_code = 1;
        return true;
    }
    else if (type == type_options::_int && ik_is_int(buffer))
    {
        ik_string_make(string, buffer);
        ik_string_remove(string, Newline);
        *return_code = 1;
        return true;
    }
    // error codes
    *return_code = 3;

    free(buffer);
    return false;
}


bool ik_get_expression_indexes(char beginning, char end, ik_string in, ik_array* out){
    bool found_start = false;
    bool has_expression = false;
    int current_index = 0;
    for (size_t i = 0; i < in.size; i++)
    {
        if(in.cstring[i] == beginning){
            if(found_start) current_index = i;
            if(!found_start){ found_start = true; current_index = i; } 
        }
        if(in.cstring[i] == end){
            if(!found_start) continue;
            if(found_start) { ik_array_append(out, &current_index); ik_array_append(out, &i); found_start = false; has_expression = true;}
        }
    }
    return has_expression;
}

#pragma endregion

#pragma region Array

void ik_array_make(ik_array *ik_array, u64 stride_size, u64 num_elements, compare_callback comparison)
{
    ik_array->data = malloc(
        stride_size * num_elements);

    ik_array->size = 0;
    ik_array->stride = stride_size;
    ik_array->capacity = num_elements;
    ik_array->comparison = comparison;
}

void ik_array_destroy(ik_array *ik_array)
{
    free(ik_array->data);
    ik_array->data = 0;

    ik_array->size = 0;
    ik_array->stride = 0;
    ik_array->capacity = 0;
    ik_array->comparison = 0;
}

void ik_array_append(ik_array *thisptr, void *object)
{
    if (thisptr->capacity <= thisptr->size)
    {
        ik_array_grow(thisptr, 10);
    }

    memcpy(
        ((byte *)thisptr->data) + (thisptr->size++) * thisptr->stride,
        object,
        thisptr->stride);
}

void ik_array_remove(ik_array *thisptr, u32 index)
{
    for (u32 i = index; i < thisptr->size - 1; i++)
    {
        swap(
            ((byte *)thisptr->data) + (i * thisptr->stride),
            ((byte *)thisptr->data) + ((i + 1) * thisptr->stride),
            thisptr->stride);
    }
    thisptr->size--;
}

void ik_array_remove_fast(ik_array *thisptr, u32 index)
{
    swap(
        ((byte *)thisptr->data) + (index * thisptr->stride),
        ((byte *)thisptr->data) + ((thisptr->size - 1) * thisptr->stride),
        thisptr->stride);
}

void ik_array_sort(ik_array *thisptr, ik_array_sort_mode mode)
{
    bool is_sorted = false;
    while (!is_sorted)
    {
        is_sorted = true;
        for (u32 i = 0; i < thisptr->size - 1; i++)
        {
            void *first_element = ik_array_get(thisptr, i);
            void *second_element = ik_array_get(thisptr, i + 1);

            bool comp = thisptr->comparison(
                first_element,
                second_element);

            if (mode == ik_array_sort_mode::asc ? comp : !comp)
            {
                is_sorted = false;
                swap(
                    first_element,
                    second_element,
                    thisptr->stride);
            }
        }
    }
}

void ik_array_grow(ik_array *thisptr, u64 size)
{
    void *temp = thisptr->data;

    thisptr->data = malloc(
        (thisptr->capacity + size) * thisptr->stride);
    memcpy(
        thisptr->data,
        temp,
        thisptr->size * thisptr->stride);

    thisptr->capacity = thisptr->capacity + size;

    SAFEDELETE(temp);
}

void *ik_array_get(ik_array *thisptr, u32 i)
{
    return ((byte *)thisptr->data) + i * thisptr->stride;
}

void swap(void *src, void *dst, u64 size)
{
    void *temp = malloc(size);

    memcpy(temp, src, size);
    memcpy(src, dst, size);
    memcpy(dst, temp, size);

    SAFEDELETE(temp);
}

#pragma endregion

#pragma region Parsers

i32 ik_parser_comma_index(char *text)
{

    for (i32 i = 0; i < strlen(text); i++)
    {
        if (text[i] == ',')
        {
            return i;
        }
    }
    return -1;
}

u64 ik_parser_decimal_count(char *text, i32 comma_index)
{
    return strlen(text) - 1 - comma_index;
}

bool ik_parser_parse_as_int(ik_string *text, int *out_number)
{
    *out_number = 0;

    // check if text is null
    if (text == 0)
    {
        return 0;
    }

    ik_string fixed_comma = {};
    ik_string_make(&fixed_comma, text->cstring);
    ik_string comma = { };
    ik_string_make(&comma, ",");
    ik_string dot = { };
    ik_string_make(&dot, ".");
    ik_string_replace(&fixed_comma, (&comma)->cstring, (&dot)->cstring);

    if (ik_is_numeric(text->cstring))
        ik_round_to_int(atof(fixed_comma.cstring), out_number);
    else
        return false;
    return true;
}

bool ik_parser_parse_as_float(ik_string *text, float *out_number)
{
    *out_number = 0;

    // check if text is null
    if (text == 0)
    {
        return 0;
    }

    ik_string fixed_comma = {};
    ik_string_make(&fixed_comma, text->cstring);

    ik_string comma = { };
    ik_string_make(&comma, ",");
    ik_string dot = { };
    ik_string_make(&dot, ".");
    ik_string_replace(&fixed_comma, (&comma)->cstring, (&dot)->cstring);


    if (!ik_is_numeric(text->cstring))
        return false;
    float out = (float)atof(fixed_comma.cstring);
    *out_number = out;
    return true;
}

#pragma endregion

#pragma region Random

void ik_random_init(ik_random* state, u32 seed) 
{
    u32* state_array = &(state->state_array[0]);
    
    state_array[0] = seed; // suggested initial seed = 19650218UL
    
    for (int i=1; i<624; i++)
    {
        seed = 1812433253UL * (seed ^ (seed >> (32-2))) + i; 
        state_array[i] = seed; 
    }
    
    state->state_index = 0;
}


void ik_random_next(ik_random* state, i32* out)
{
    u32* state_array = &(state->state_array[0]);
    
    int k = state->state_index;      
    int j = k - (624-1);               
    if (j < 0) j += 624;               

    u32 x = (state_array[k] & (0xffffffffUL << 31)) | (state_array[j] & (0xffffffffUL >> (32-31)));
    
    u32 xA = x >> 1;
    if (x & 0x00000001UL) xA ^= 0x9908b0dfUL;
    
    j = k - (624-397);
    if (j < 0) j += 624;
    
    x = state_array[j] ^ xA;
    state_array[k++] = x;
    
    if (k >= 624) k = 0;
    state->state_index = k;
    
    u32 y = x ^ (x >> 11);
            y = y ^ ((y << 7) & 0x9d2c5680UL);
            y = y ^ ((y << 15) & 0xefc60000UL);
    u32 z = y ^ (y >> 18);
    
    *out = z; 
}

#pragma endregion