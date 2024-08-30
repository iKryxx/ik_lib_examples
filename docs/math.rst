===================================
Math
===================================
WIP

A list of usefull functions regarding math

ik_min
=======
**Description:**
A function that returns the smaller number between a and b

**Definition**

.. code-block:: CPP
  
  extern i64 ik_min(i64 a, i64 b);

**Usage / Examples**

.. code-block:: CPP
   
   int a = 4, b = 5;
   printf("%i", ik_min(a, b));
   // >>> 4

ik_max
=======
**Description:**
A function that returns the bigger number between a and b

**Definition**

.. code-block:: CPP
  
  extern i64 ik_max(i64 a, i64 b);

**Usage / Examples**

.. code-block:: CPP
   
   int a = 4, b = 5;
   printf("%i", ik_min(a, b));
   // >>> 5

ik_remap
=======
**Description:**
A function that remaps a value from one range to another

**Definition**

.. code-block:: CPP
  
  extern void ik_remap(i64 start_min, i64 start_max, i64 res_min, i64 res_max, i64* value);

**Usage / Examples**

.. code-block:: CPP
   
   int a = 50;
   int remap = ik_remap(0, 100, 0, 200, a);
   printf("%i", remap);
   // >>> 100

