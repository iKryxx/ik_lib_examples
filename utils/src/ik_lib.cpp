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

#pragma endregion

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
            j += strlen(find);
        (&_new)->cstring[i] = in->cstring[i + j];
    }
    ik_string_set(in, &_new);
}

void ik_print_string(ik_string *in, reserve_space_options reserve, int spaces, align_options align)
{
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
    char buffer[max_len];
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
    return false;
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