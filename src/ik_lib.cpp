#pragma region Internal Includes

#include "ik_lib.h"

#pragma endregion

#pragma region Math

i64 ik_abs(i64 a)
{
	return (a < 0) * -a + (a >= 0) * a;
}

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

// Constants
const double LN2 = 0.6931471805599453; // ln(2)
const int NEWTON_MAX_IT = 100; // Maximum iterations for Newton's method
const double EPSILON = 1e-10; // Precision threshold
const int INFINITY = 1e9; // Infinity value

double ik_exp(double x)
{
    double sum = 1.0;
    double term = 1.0;
    for (int i = 1; i < 20; ++i)
    {
        term *= x / i;
        sum += term;
    }
    return sum;
}
double ik_log(double x)
{
    if (x <= 0) {
        return -INFINITY; // Logarithm undefined for non-positive values
    }

    // Normalize x to the range [0.5, 2)
    int exponent = 0;
    while (x > 2.0) {
        x *= 0.5;
        exponent++;
    }
    while (x < 0.5) {
        x *= 2.0;
        exponent--;
    }

    // Use a good initial guess for y = ln(x)
    double y = x - 1.0;
    double term = y;

    // Apply Newton's method
    for (int i = 0; i < NEWTON_MAX_IT; ++i) {
        double e_y = ik_exp(y);
        double diff = (x - e_y) / e_y;

        if (ik_abs(diff) < EPSILON) {
            break;
        }

        y += diff;
    }

    // Add back the log(2^exponent)
    return y + exponent * LN2;
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

bool ik_string_split(
    const ik_string* const string,
    ik_string* out_left,
    ik_string* out_right,
    const char delimiter)
{
    if (!string->cstring) return false;

    u64 delimiter_index = 0;
    while (string->cstring[delimiter_index] != delimiter)
    {
        // do nothing if the string has no delimiter
        if (string->size < delimiter_index)
            return false;

        ++delimiter_index;
    }

    ik_string_make_range(out_left, string->cstring, 0, delimiter_index);
    ik_string_make_range(out_right, string->cstring, delimiter_index+1, string->size);

    return true;
}
bool ik_string_split_at(
    const ik_string* const string,
    ik_string* out_left,
    ik_string* out_right,
    int index)
{
    if (!string->cstring || index > string->size) return false;

    u64 delimiter_index = 0;

    ik_string_make_range(out_left, string->cstring, 0, index);
    ik_string_make_range(out_right, string->cstring, index + 1, string->size);

    return true;
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
    memcpy(string->cstring, cstring+start, len);
    string->size = len;
}

void ik_string_destroy(ik_string *string)
{
    /*if(string->cstring != 0) 
        {SAFEDELETE(string->cstring);}
    else
    {
        printf("");
    }*/
    SAFEDELETE(string->cstring);
    string->size = 0;
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
    if(start > in->size - 1 || start < 0 || end > in->size - 1 || end < 0 || start > end) return;

    ik_string _new = { };
    ik_string_make_empty(&_new, in->size - (end - start) + strlen(replace) - 1);
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


int ik_string_contains_char(ik_string *in, char find)
{
    for (int i = 0; i < in->size; i++)
    {
        if(in->cstring[i] == find) return i;
    }
    return -1;
}


void ik_string_set(ik_string *in, ik_string *to)
{
    ik_string_destroy(in);

    in->cstring = to->cstring;
    in->size = to->size;
}

void ik_string_set_at(ik_string* in, int i, char to){
    if(in == 0) return;
    if(i < 0 || i >= in->size) return;

    in->cstring[i] = to;
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
void ik_string_remove_range(ik_string* in, int start, int finish) 
{
    if (start >= in->size || finish >= in->size || start > finish) return;
    ik_string _new = {};

    ik_string_make_empty(&_new, in->size - (finish + 1 - start));

    bool has_replaced = false;
    int j = 0;
    for (int i = 0; i < strlen(in->cstring) - (finish + 1 - start); i++)
    {
        if (i == start)
            j += (finish + 1 - start);
        (&_new)->cstring[i] = in->cstring[i + j];
    }
    ik_string_set(in, &_new);
}

/*HELPER FUNCTION, DO NOT USE*/
void print(ik_string* in, reserve_space_options reserve, int spaces, align_options align) {
    if (spaces < 0) return;
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

void get_subcolors(ik_string* in, ik_array *out) {
    //formatting validating colors
    ik_string working = { };
    ik_string_make(&working, in->cstring);

    ik_array found_exps = { };
    ik_array_make(&found_exps, sizeof(size_t), 10);
    ik_array_make(&found_exps, sizeof(size_t), 10);

    ik_get_expression_indexes('<', '>', working, &found_exps);

    size_t replacement_offset = 0;
    bool has_printed = false;

    int _old_end = -1; int _old_begin = 0;
    int begin, end;
    for (size_t i = 0; i < found_exps.size; i += 2)
    {
        begin = *(int*)ik_array_get(&found_exps, (u32)i);
        end = *(int*)ik_array_get(&found_exps, i + 1);
        
        if (end - begin == 2)
        {
            if (_old_end != -1 && begin - 1 != _old_end)
            {
                ik_string _curr = { };
                ik_string_make_range(&_curr, in->cstring, _old_begin, begin);
                ik_array_append(out, (void*)&_curr);
                _old_begin = begin;
            }
            _old_end = end;
        }
    }
    ik_string _curr = { };
    ik_string_make_range(&_curr, in->cstring, _old_begin, in->size);
    ik_array_append(out, (void*)&_curr);
}

void ik_print_string(ik_string* in, reserve_space_options reserve, int spaces, align_options align)
{
    //formatting validating colors
    ik_array formatted = { };
    ik_array_make(&formatted, 2 * sizeof(u64), 10);

    ik_array total_exps = { };
    ik_array_make(&total_exps, sizeof(size_t), 10);
    ik_get_expression_indexes('<', '>', *in, &total_exps);

    get_subcolors(in, &formatted);
    bool has_printed = false;
    int stringsize = 0;
    int used_spaces = 0;

    for (size_t i = 0; i < formatted.size; i++)
    {
        ik_string* _curr = (ik_string*)ik_array_get(&formatted, i);

        ik_array found_exps = { };
        ik_array_make(&found_exps, sizeof(size_t), 10);

        ik_get_expression_indexes('<', '>', *_curr, &found_exps);

        int f_code = 0x0007; //white
        int b_code = 0x0000; //black

        stringsize = in->size - 3 * (total_exps.size / 2);

        int start_overflow_middle = (spaces - stringsize) / 2;     //for align_midle
        int end_overflow_middle = (spaces - stringsize + 1) / 2;       //for align_midle
        int overflow_left_right = (spaces - stringsize);           //for align left and right

        for (size_t j = 0; j < found_exps.size; j += 2)
        {
            char option = _curr->cstring[1 + 3 * j / 2];
#ifdef _WIN32

#pragma region CLUSTERFUCK

#define black 0x0000
#define blue 0x0001
#define green 0x0002
#define red 0x0004
#define intensity 0x0008
#define white (blue | green | red)





            if (option == 97) f_code = white;
            else if (option == 98) f_code = black;
            else if (option == 99) f_code = red;
            else if (option == 100) f_code = green;
            else if (option == 101) f_code = red | green;
            else if (option == 102) f_code = blue;
            else if (option == 103) f_code = blue | red;
            else if (option == 104) f_code = blue | green;
            else if (option == 105) f_code = white; //theres no light gray
            else if (option == 106) f_code = white | black;
            else if (option == 107) f_code = red | intensity;
            else if (option == 108) f_code = green | intensity;
            else if (option == 109) f_code = red | green | intensity;
            else if (option == 110) f_code = blue | intensity;
            else if (option == 111) f_code = blue | red | intensity;
            else if (option == 112) f_code = blue | green | intensity;
            else if (option == 113) f_code = white;

            else if (option == 65) b_code = black << 4;
            else if (option == 66) b_code = black << 4;
            else if (option == 67) b_code = red << 4;
            else if (option == 68) b_code = green << 4;
            else if (option == 69) b_code = (red | green) << 4;
            else if (option == 70) b_code = blue << 4;
            else if (option == 71) b_code = (blue | red) << 4;
            else if (option == 72) b_code = (blue | green) << 4;
            else if (option == 73) b_code = white << 4; //theres no light gray
            else if (option == 74) b_code = (white | black) << 4;
            else if (option == 75) b_code = (red | intensity) << 4;
            else if (option == 76) b_code = (green | intensity) << 4;
            else if (option == 77) b_code = (red | green | intensity) << 4;
            else if (option == 78) b_code = (blue | intensity) << 4;
            else if (option == 79) b_code = (blue | red | intensity) << 4;
            else if (option == 80) b_code = (blue | red | intensity) << 4;
            else if (option == 81) b_code = white << 4;



#pragma endregion

#else

            int num = 0;
            int begin = 3 * j;
            int end = 2 + 3 * j;

            if (option >= 97 && option < 114) { //textcolor
                if (option == 97) num = option - 58;
                else if (option < 106) num = option - 68;
                else if (option < 114) num = option - 16;
            }
            else if (option >= 65 && option < 82) { //background
                if (option == 65) num = option - 16;
                else if (option < 74) num = option - 26;
                else if (option < 82) num = option + 26;
            }
            if (num != 0) {
                char code[8];
                sprintf(code, "\033[%im", num);
                ik_string_replace_index
                (
                    in,
                    begin + replacement_offset,
                    end + replacement_offset, code
                );
                if (num >= 100) replacement_offset += (3);
                else replacement_offset += (2);
        }

#endif //_WIN32
    }

#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, f_code + b_code);
        if (found_exps.size != 0) {
            size_t end = *(size_t*)ik_array_get(&found_exps, found_exps.size - 1);

            ik_string_remove_range(_curr, 0, end);
        }
        if (reserve == dont_reserve) print(_curr, reserve, 0, align_left);

        if (align == align_middle) {
            if (reserve == reserve_cut) {
                if (i == 0) {
                    if (start_overflow_middle < 0) {
                        print(_curr, reserve, ik_min(spaces, _curr->size), align_right);
                        used_spaces += ik_min(spaces, _curr->size);
                    }
                    else
                    {
                        print(_curr, reserve, _curr->size + start_overflow_middle, align_right);
                        used_spaces += _curr->size + start_overflow_middle;
                    }
                }
                else if (i == formatted.size - 1) {
                    if (used_spaces < spaces)
                        print(_curr, reserve, ik_min(spaces - used_spaces, _curr->size + end_overflow_middle), align_left);
                }
                else {
                    print(_curr, reserve, ik_min(spaces - used_spaces, _curr->size), align_left);
                    if (ik_min(spaces - used_spaces, _curr->size) >= 0)
                        used_spaces += ik_min(spaces - used_spaces, _curr->size);

                }
            }
            else if (reserve == reserve_overflow) {
                if (i == 0) {
                    print(_curr, reserve, ik_max(_curr->size + start_overflow_middle, _curr->size), align_right);
                }
                else if (i == formatted.size - 1) {
                    print(_curr, reserve, ik_min(spaces, _curr->size + end_overflow_middle), align_left);
                }
                else {
                    print(_curr, reserve, ik_min(spaces, _curr->size), align_left);
                }
            }
        }
        
        if (align == align_left) {
            if (reserve == reserve_cut) {
                if (i == 0) {
                    if (overflow_left_right < 0) {
                        print(_curr, reserve, ik_min(spaces, _curr->size), align_left);
                        used_spaces += ik_min(spaces, _curr->size);
                    }
                    else
                    {
                        print(_curr, reserve, _curr->size, align_left);
                        used_spaces += _curr->size;
                    }
                }
                else if (i == formatted.size - 1) {
                    if (used_spaces < spaces)
                        print(_curr, reserve, ik_min(spaces - used_spaces, _curr->size + overflow_left_right), align_left);
                }
                else {
                    print(_curr, reserve, ik_min(spaces - used_spaces, _curr->size), align_left);
                    if (ik_min(spaces - used_spaces, _curr->size) >= 0)
                        used_spaces += ik_min(spaces - used_spaces, _curr->size);

                }
            }
            else if (reserve == reserve_overflow) {
                if (i == 0) {
                    print(_curr, reserve, ik_max(_curr->size, _curr->size), align_left);
                }
                else if (i == formatted.size - 1) {
                    print(_curr, reserve, ik_min(spaces, _curr->size + ik_max(overflow_left_right, 0 )), align_left);
                }
                else {
                    print(_curr, reserve, ik_min(spaces, _curr->size), align_left);
                }
            }
        }
        if (align == align_right) {
            if (reserve == reserve_cut) {
                if (i == 0) {
                    if (overflow_left_right <= 0) {
                        print(_curr, reserve, ik_min(spaces, _curr->size), align_left);
                        used_spaces += ik_min(spaces, _curr->size);
                    }
                    else
                    {
                        print(_curr, reserve, overflow_left_right + _curr->size, align_right);
                        used_spaces += overflow_left_right + _curr->size;
                    }
                }
                else if (i == formatted.size - 1) {
                    if (used_spaces < spaces)
                        print(_curr, reserve, ik_min(spaces - used_spaces, _curr->size), align_left);
                }
                else {
                    print(_curr, reserve, ik_min(spaces - used_spaces, _curr->size), align_left);
                    if (ik_min(spaces - used_spaces, _curr->size) >= 0)
                        used_spaces += ik_min(spaces - used_spaces, _curr->size);

                }
            }
            else if (reserve == reserve_overflow) {
                if (i == 0) {
                    print(_curr, reserve, ik_max(_curr->size, _curr->size + ik_max(overflow_left_right, 0)), align_right);
                }
                else if (i == formatted.size - 1) {
                    print(_curr, reserve, ik_min(spaces, _curr->size), align_left);
                }
                else {
                    print(_curr, reserve, ik_min(spaces, _curr->size), align_left);
                }
            }
        }

        SetConsoleTextAttribute(hConsole, white);
#endif
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
    
    if(ik_string_contains_char(&in, beginning) == -1 || ik_string_contains_char(&in, end) == -1) return false;
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

void ik_array_make(ik_array *ik_array, u64 stride_size, u64 num_elements)
{
	if (0 == stride_size)
	{
        return;
	}

    ik_array->data = malloc(
        stride_size * num_elements);

	if (!ik_array->data)
	{
        ik_array->size = 0;
        ik_array->stride = stride_size;
        ik_array->capacity = 0;
		return;
	}

	memset(ik_array->data, 0, stride_size * num_elements);

    ik_array->size = 0;
    ik_array->stride = stride_size;
    ik_array->capacity = num_elements;
}

void ik_array_destroy(ik_array *ik_array)
{
    free(ik_array->data);
    ik_array->data = 0;

    ik_array->size = 0;
    ik_array->stride = 0;
    ik_array->capacity = 0;
}

void ik_array_append(ik_array* thisptr, void* object)
{
    if (thisptr->capacity <= thisptr->size)
    {
        ik_array_grow(thisptr, ik_log(2 + thisptr->size));
    }

    memcpy(
        ((byte *)thisptr->data) + (thisptr->size++) * thisptr->stride,
        object,
        thisptr->stride
    );
}

void ik_array_remove(ik_array *thisptr, u32 index)
{
    for (u32 i = index; i < thisptr->size - 1; i++)
    {
        ik_swap(
            ((byte *)thisptr->data) + (i * thisptr->stride),
            ((byte *)thisptr->data) + ((i + 1) * thisptr->stride),
            thisptr->stride);
    }
    thisptr->size--;
}

void ik_array_remove_fast(ik_array *thisptr, u32 index)
{
    ik_swap(
        ((byte *)thisptr->data) + (index * thisptr->stride),
        ((byte *)thisptr->data) + ((thisptr->size - 1) * thisptr->stride),
        thisptr->stride);
}

void ik_array_sort(ik_array *thisptr, compare_callback comparator, ik_array_sort_mode mode)
{
	if (!comparator || !thisptr->size)
		return;

    bool is_sorted = false;
    while (!is_sorted)
    {
        is_sorted = true;
        for (u32 i = 0; i < thisptr->size - 1; i++)
        {
            void *first_element = ik_array_get(thisptr, i);
            void *second_element = ik_array_get(thisptr, i + 1);

            bool comp = comparator(
                first_element,
                second_element);

            if (mode == ik_array_sort_mode::asc ? comp : !comp)
            {
                is_sorted = false;
                ik_swap(
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

    u32 new_capacity = (thisptr->capacity + size);
    void* new_data = malloc(new_capacity * thisptr->stride);
    if (!new_data)
    {
	    return;
    }

	thisptr->data = new_data;
    memset(
        (byte*)new_data + (thisptr->size * thisptr->stride),
        0,
        (new_capacity - thisptr->size) * thisptr->stride
    );

    memcpy(
        thisptr->data,
        temp,
        thisptr->size * thisptr->stride
    );

    thisptr->capacity = new_capacity;

    SAFEDELETE(temp);
}

void* ik_array_get(ik_array *thisptr, u32 i)
{
	if (i >= thisptr->size)
	{
		return ((byte*)thisptr->data) + (thisptr->size - 1) * thisptr->stride;
	}

    return ((byte *)thisptr->data) + i * thisptr->stride;
}

void ik_swap(void *src, void *dst, u64 size)
{
    void *temp = malloc(size);

	if (!temp)
	{
		return;
	}

    memcpy(temp, src, size);
    memcpy(src, dst, size);
    memcpy(dst, temp, size);

    SAFEDELETE(temp);
}

bool ik_array_contains(ik_array* thisptr, void* object, compare_callback comparator)
{
	for (u32 i = 0; i < thisptr->size; i++)
	{
		if (comparator && comparator(object, ik_array_get(thisptr, i)))
		{
			return true;
		}
		if (!comparator && memcmp(object, ik_array_get(thisptr, i), thisptr->stride) == 0)
		{
			return true;
		}
	}
	return false;
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

#pragma region Comparator

bool ik_compare_string(void* a, void* b)
{
    u64 upper_limit = ik_min(((ik_string*)a)->size, ((ik_string*)b)->size);
    ik_string _a;
    ik_string _b;

    memset(&_a, '\0', sizeof(ik_string));
    memset(&_b, '\0', sizeof(ik_string));

    ik_string_make(&_a, ((ik_string*)a)->cstring);
    ik_string_make(&_b, ((ik_string*)b)->cstring);

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
bool ik_compare_i32(void* a, void* b)
{
    i32 _a = *(i32*)a;
    i32 _b = *(i32*)b;

	return _a > _b;
}
bool ik_compare_u32(void* a, void* b)
{
	u32 _a = *(u32*)a;
	u32 _b = *(u32*)b;

	return _a > _b;
}
bool ik_compare_i64(void* a, void* b)
{
	i64 _a = *(i64*)a;
	i64 _b = *(i64*)b;

	return _a > _b;
}
bool ik_compare_u64(void* a, void* b)
{
	u64 _a = *(u64*)a;
	u64 _b = *(u64*)b;

	return _a > _b;
}
bool ik_compare_f32(void* a, void* b)
{
	float _a = *(float*)a;
	float _b = *(float*)b;

	return _a > _b;
}
bool ik_compare_f64(void* a, void* b)
{
	double _a = *(double*)a;
	double _b = *(double*)b;

	return _a > _b;
}
bool ik_compare_byte(void* a, void* b)
{
	byte _a = *(byte*)a;
	byte _b = *(byte*)b;

	return _a > _b;
}

#pragma endregion

#pragma region Screen
u8 SCREEN_WIDTH, SCREEN_HEIGHT = 0; 
char SCREEN_BACKGROUND = ' ';
ik_array SCREEN_BUFFER = {};

//helper functions
ik_string *GET_PIXEL(int x, int y) {
    return (ik_string*)ik_array_get(&SCREEN_BUFFER, y * SCREEN_WIDTH + x);
}
//end !helper functions


void ik_screen_init(u8 width, u8 height, char background){
    SCREEN_WIDTH = width;
    SCREEN_HEIGHT = height;
    SCREEN_BACKGROUND = background;
    ik_array_make(&SCREEN_BUFFER, 2 * sizeof(u64), height * width);
    ik_string curr = { };
    for (size_t i = 0; i < height * width; i++)
    {
        ik_string_make_empty(&curr, 1);
        ik_string_set_at(&curr, 0, SCREEN_BACKGROUND);
        ik_array_append(&SCREEN_BUFFER, &curr);
    }
    ik_clrscr();
    printf("\n");
}

void ik_screen_set_pixels(ik_array pixels){

    for (size_t i = 0; i < pixels.size; i++)
    {
        pixel *_curr = (pixel*)ik_array_get(&pixels, i);
        ik_screen_set_pixel(_curr->_x, _curr->_y, _curr->_char, _curr->_foreground, _curr->_background);
    }
}

void ik_screen_set_pixel(u8 x, u8 y, char to, color foreground, color background){
    if(x < 0 || x >= SCREEN_WIDTH) return;
    if(y < 0 || y >= SCREEN_HEIGHT) return;

    int fore = (int)foreground;
    int back = (int)background;
    char formatted[14];
    //format the string
    

    formatted[0] = '<'; formatted[3] = '<'; formatted[7] = '<'; formatted[10] = '<';
    formatted[2] = '>'; formatted[5] = '>'; formatted[9] = '>'; formatted[12] = '>';
    formatted[1] = (char)(fore + 97); formatted[8] = (char)(97) ;
    formatted[4] = (char)(back + 65); formatted[11] = (char)(65) ;
    formatted[6] = to;
    formatted[13] = '\0';
    ik_string *ref = GET_PIXEL(x, y);
    ik_string_replace_index(ref, 0, 0, (char*)formatted);
}
void ik_screen_print(){
    ik_cursor_hide();
    //ik_clrscr();
    ik_move_cursor_up(SCREEN_HEIGHT);
    void* arr = (void*)SCREEN_BUFFER.data;
    fflush(stdout);
    for (size_t y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (size_t x = 0; x < SCREEN_WIDTH; x++)
        {
            ik_string *_this = GET_PIXEL(x, y);
            ik_print_string(_this, reserve_space_options::dont_reserve, 1, align_options::align_left);
        }
        ik_move_cursor_down(1);
        ik_move_cursor_left(SCREEN_WIDTH);
        fflush(stdout);
    }
    ik_cursor_show();
    //fflush(stdout);
}
void ik_screen_clear_screen(){
    for (size_t y = 0; y < SCREEN_HEIGHT; y++)
    {
        for (size_t x = 0; x < SCREEN_WIDTH; x++)
        {
            ik_string *curr = GET_PIXEL(x, y);
            ik_string _new = { };
            ik_string_make_empty(&_new, 1);
            ik_string_set_at(&_new, 0, SCREEN_BACKGROUND);
            ik_string_set(curr, &_new);
        }        
    }
}


#pragma endregion

#pragma region Sleep

void ik_sleep(u64 milliseconds)
{
#ifdef _WIN32
	Sleep(milliseconds);
#else
	usleep(milliseconds * 1000);
#endif
}

#pragma endregion

#pragma region Input

ik_input_type INPUT_TYPE = stream;
ik_array input_events;

void ik_init_input() {
    input_events = { };
    ik_array_make(&input_events, 3 * sizeof(bool), 25); //-65!!
    ik_input a = { };
    ik_array_append(&input_events, &a);
    for (size_t i = 0; i <= 25; i++)
    {
        a = { false, false, false };
        ik_array_append(&input_events, &a);
    }
}
void ik_set_input_type(ik_input_type type) {
    INPUT_TYPE = type;
}

void ik_update_input() {
    if (INPUT_TYPE != keyboardhit) return;

    for (size_t ch = 0; ch <= 25; ch++)
    {
        ik_input* p = (ik_input*)ik_array_get(&input_events, ch);
        if (GetAsyncKeyState(ch + 65))
        {

            if (p->pressed && !p->held) { //is pressed
                p->pressed = false;
                p->held = true;
                p->released = false;
            }
            else if (!p->pressed && !p->held) { //is held
                p->pressed = true;
                p->held = false;
                p->released = false;
            }
        }
        else if (p->held || p->pressed) {
            p->pressed = false;
            p->held = false;
            p->released = true;
        }
        else if (p->released) {
            p->pressed = false;
            p->held = false;
            p->released = false;
        }
        // if (p->pressed) printf("\n[%c]: Pressed.              ", ch + 65);
        // else if (p->held) printf("\n[%c]: Held.               ", ch + 65);
        // else if (p->released) printf("\n[%c]: Released.       ", ch + 65);
        // else printf("\n[%c]:                                  ", ch + 65);
    }
}

bool ik_get_key_state(char ch, ik_key_state state) {
    ik_input* p = (ik_input*)ik_array_get(&input_events, ch - 65);

    switch (state)
    {
        case pressed:
            return p->pressed;
        case held:
            return p->held;
        case released:
            return p->released;
        default:
            break;
    }
}

#pragma endregion

