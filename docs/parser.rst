===================================
Parser
===================================

A collection of functions for parsing

ik_parser_parse_as_int
=======
**Description:**
A function that parses an ik_string to an int

**Definition**

.. code-block:: CPP
  
  extern bool ik_parser_parse_as_int(ik_string *text, int *out_number);

**Usage / Examples**

.. code-block:: CPP
   
   ik_string str = { };
   ik_string_make(&str, "7300");

   int a = 0;
   ik_parser_parse_as_int(&str, &a);
   a -= 7000;

   printf("%i", a);
   // >>> 300

ik_parser_parse_as_float
=======
**Description:**
A function that parses an ik_string to an float

**Definition**

.. code-block:: CPP
  
  extern bool ik_parser_parse_as_float(ik_string *text, float *out_number);

**Usage / Examples**

.. code-block:: CPP
   
   ik_string str = { };
   ik_string_make(&str, "31.2123");

   float a = 0;
   ik_parser_parse_as_float(&str, &a);
   a -= 30;

   printf("%.4f", a);
   // >>> 1.2123

