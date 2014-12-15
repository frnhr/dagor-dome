Dagor Dome
==========


IDE and code style guide
------------------------

### Code style guide ###

First, memorize this:   
**The Zen of Pyth.. Arduino! Arduino, yes.** And C, too.

    Beautiful is better than ugly.
    Explicit is better than implicit.
    Simple is better than complex.
    Complex is better than complicated.
    Flat is better than nested.
    Sparse is better than dense.
    Readability counts.
    Special cases aren't special enough to break the rules.
    Although practicality beats purity.
    Errors should never pass silently.
    Unless explicitly silenced.
    In the face of ambiguity, refuse the temptation to guess.
    There should be one-- and preferably only one --obvious way to do it.
    Although that way may not be obvious at first unless you're Dutch.
    Now is better than never.
    Although never is often better than *right* now.
    If the implementation is hard to explain, it's a bad idea.
    If the implementation is easy to explain, it may be a good idea.
    Namespaces are one honking great idea -- let's do more of those!
    
https://www.python.org/doc/humor/#the-zen-of-python



### C ⊂ C++ ###

"Arduino code" is C++, not (just) C. Even though a lot of Arduino sketches look like they are C-only, libraries are usually teeming with C++ features. 

In user code (i.e. the Arduino sketch) its ok to use C++ features, but always make sure  the code is readable for someone new to C++ as well as Arduino.

We are primarily working with hardware (registers, pins, etc), and this should be:
 * clearly visible in the code,
 * OR completely encapsulated (as most libraries do),
 * BUT NEVER something "in between".


##### Code style #####

###### TL;DR:

  * indent with 4 spaces
  * always use "{" and "}"
  * keep "{" inline everywhere except when declaring functions and classes - in those cases "{" should have a line of its own
  * cases:
    - variables: lowercase\_with_underscores
    - constants: UPPERCASE\_WITH_UDERSCORES
    - classes, types, structs: CamelCase
  * keep lines length in check! 80 chars is uber-strict, 100 is polite, 120 is ok, more is just bad. It just is. Really.
  
    

###### Details:

 * please refer to:
   - http://users.ece.cmu.edu/~eno/coding/CCodingStandard.html
   - https://www.kernel.org/doc/Documentation/CodingStyle
 * in that order, those two docs are not in agreement on everything
 

###### Global variables

Global variables are ok. Global classes and/or structs that replace several variables are even better.

###### Separation of concerns

One piece of code should only deal with one thing. The "piece" may be a file, a function, etc.

Different "pieces" should interact only via a well-defined interfaces - preferably state variables.



### Stino and Sublime Text 2 #########

**Stino** is a plugin for **Sublime Text 2** editor. It is simple to install and works more or less *outofthebox*.


#### Installation #####################

**Sublime Text 2** can be installed from http://www.sublimetext.com/2

**Stino** install instruction are localted here: https://sublime.wbond.net/installation#st2


### Eclipse

todo ... how to set it up?


### Why not the Arduino IDE ##############

It needs to be installed and set up, but it has several shortcomings that make it a substandard choice for non-trivial projects.

 * no code completion
 * global settings - when programming multiple boards settings need to be changed before each upload
 * poor code formatting
 * poor code coloring

 
## Code overview

todo ...  here come nice ASCII diagram of code and state buffers ...



