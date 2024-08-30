===================================
Types
===================================

ik_lib redefines certain types to make your
code more readable

string
=======
**Description:**
A collection of characters into a single type

**Definition**

.. code-block:: CPP
  
  typedef char* string;

**Usage / Examples**

This lets you initialize a string 

.. code-block:: CPP
   
   string name = "John";
   //same as writing 'char* name = "John";'

i8
=======
**Description:**
A signed 8-Bit integer. The max value is 127, the min value is -128

**Definition**

.. code-block:: CPP
  
  typedef signed char i8;

**Usage / Examples**

This lets you initialize a signed 8-Bit integer

.. code-block:: CPP
   
   i8 foo = 127;
   //same as writing 'signed char foo = 127;'

i16
=======
**Description:**
A signed 16-Bit integer. The max value is 32767, the min value is -32768

**Definition**

.. code-block:: CPP
  
  typedef signed short i16;

**Usage / Examples**

This lets you initialize a signed 16-Bit integer

.. code-block:: CPP
   
   i16 foo = 32767;
   //same as writing 'signed short foo = 32767;'

i32
=======
**Description:**
A signed 32-Bit integer. The max value is 2147483647, the min value is -2147483648

**Definition**

.. code-block:: CPP
  
  typedef signed int i32;

**Usage / Examples**

This lets you initialize a signed 32-Bit integer

.. code-block:: CPP
   
   i32 foo = 2147483647;
   //same as writing 'signed int foo = 2147483647;'

i64
=======
**Description:**
A signed 64-Bit integer. The max value is 2e63 - 1, the min value is -2e63

**Definition**

.. code-block:: CPP
  
  typedef signed int i64;

**Usage / Examples**

This lets you initialize a signed 64-Bit integer

.. code-block:: CPP
   
   i64 foo = 9223372036854775807;
   //same as writing 'signed long long foo = 9223372036854775807;'

u8
=======
**Description:**
An unsigned 8-Bit integer. The max value is 255, the min value is 0

**Definition**

.. code-block:: CPP
  
  typedef unsigned char u8;

**Usage / Examples**

This lets you initialize an unsigned 8-Bit integer

.. code-block:: CPP
   
   u8 foo = 255;
   //same as writing 'unsigned char foo = 255;'

u16
=======
**Description:**
An unsigned 16-Bit integer. The max value is 65535, the min value is 0

**Definition**

.. code-block:: CPP
  
  typedef unsigned short u16;

**Usage / Examples**

This lets you initialize an unsigned 16-Bit integer

.. code-block:: CPP
   
   u16 foo = 65535;
   //same as writing 'unsigned short foo = 65535;'

u32
=======
**Description:**
A unsigned 32-Bit integer. The max value is 4294967295, the min value is 0

**Definition**

.. code-block:: CPP
  
  typedef unsigned int u32;

**Usage / Examples**

This lets you initialize an unsigned 32-Bit integer

.. code-block:: CPP
   
   u32 foo = 4294967295;
   //same as writing 'unsigned int foo = 4294967295;'

u64
=======
**Description:**
An unsigned 64-Bit integer. The max value is 2e64 - 1, the min value is 0

**Definition**

.. code-block:: CPP
  
  typedef unsigned int u64;

**Usage / Examples**

This lets you initialize an unsigned 64-Bit integer

.. code-block:: CPP
   
   u64 foo = 18446744073709551615;
   //same as writing 'unsigned long long foo = 18446744073709551615;'

bool
=======
**Description:**
A value that either holds 0 (false) or 1 (true)

**Definition**

This defines the bool type. Note that C++ already has
the bool type, hence the preprocessing check

.. code-block:: CPP
  
  #ifndef __cplusplus
    typedef _Bool bool;
  # define false   0
  # define true    1
  #endif //!__cplusplus

**Usage / Examples**

This lets you initialize a bool

.. code-block:: CPP
   
   bool foo = true;
   //same as writing '_Bool foo = 1;'

real
=======
**Description:**
A floating point number with maximum possible precision

**Definition**

This defines the real type based on your computers
architecture. if your OS is running on 64 bits, real 
has 64-Bit precision, if its running on 32 bits it 
has 32-Bit precision

.. code-block:: CPP
  
  #ifdef DOUBLE_PRECISION
    typedef float               real;
  #else
    typedef double              real;
  #endif //!DOUBLE_PRECISION

**Usage / Examples**

This lets you initialize a real

.. code-block:: CPP
   
   real foo = 1/3;
   //same as writing 'double foo = 1/3;' (64x)
   //or              'float foo = 1/3;' (32x)

compare_callback
=======
**Description:**
Callback function used to compare variables a and b. 
it returns true if a and b should be switched when 
sorting an array


**Definition**

.. code-block:: CPP
  
  typedef bool (*compare_callback)(void *a, void *b);

**Usage / Examples**

for integration, read Array_ 

This lets you initialize a compare function

.. code-block:: CPP
   
  bool compare_int(void* a, void* b){
    int* _a = (int *)a;
    int* _b = (int *)b;

    return *_a > *_b;
  }

  int main() {
    compare_callback c_int = compare_int;
    
    int a = 1, b = 4;
    printf("%i\n", c_int((void *)&a, (void *)&b));

    a = 3, b = 2;
    printf("%i\n", c_int((void *)&a, (void *)&b));
  }
  // >>> 0
  // >>> 1

.. _Array: array.html

