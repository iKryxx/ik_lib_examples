===================================
Constants
===================================

Constants provide the lib with fixed Variables,
so you don't have to remember each one.

FLT_MIN
=======
**Description:**
Specifies the maximum allowed precision Error

**Definition**

.. code-block:: CPP
  
  #define FLT_MIN 0.001f

**Usage / Examples**

This lets you compare two floats. Because of precision errors,
some calculations output wrong values 

.. code-block:: CPP
   
   bool paid_enough_money(float sum, float paid){
      return (sum - paid) < FLT_MIN
   }

