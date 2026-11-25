# Lisp-to-Python Transpiler (In Progress)

**Name:** Makenzie Hale  
**Programming Language:** C++  

## Project Summary
This project implements a Lisp-to-Python transpiler in C++. The transpiler reads Lisp-style S-expressions and generates equivalent Python source code. 
The S-expression reader and parser are adapted from my standalone C++ Lisp interpreter.

The transpiler supports arithmetic operations, list operations, predicates, Boolean operations, conditionals, variables, quoting, function definitions, 
function calls, anonymous functions, and explicit output using print.

Lisp source files can be translated into Python files and the generated Python can be executed automatically. The output produced by the Python program 
is saved to an output file. The transpiler also includes an interactive mode that displays the generated Python for individual Lisp expressions and 
allows the user to choose whether to execute them.

### Syntax

The transpiler accepts Lisp-style prefix notation, where the operation or function name comes before its arguments inside parentheses.

The interpreter uses Lisp-style prefix notation, where the operation or function name comes before its arguments inside parentheses.

Examples:
```text
(add 2 3)
(mul 4 5)
(cons 'a '(b c))
(foo 2 3)
```

S-expressions can be quoted using `'` to prevent them from being evaluated.
```text
'(a b c)
'hello
```

Variables can be stored using `set`:
```text
(set x 10)
```

User-defined functions can be defined using `def`, followed by the function name, argument list, and body:
```text
(def foo (a b) (add a b))
(foo 2 3)
```

Anonymous functions can also be created and called:
```text
((function (x) (add x 1)) 5)
```

Output can be explicitly produced using `print`:
```text
(print(square 5))
```



For example:
```text
(def square (x) (mul x x))
(set x 5)
(print (square x))
```

is translated to:
```text
def square(x):
    return(x * x)

x = 5

print(square(x))
```

### Build Instructions

### Transpiler
Compile the transpiler:
```bash
g++ src/main.cpp src/sexpr.cpp src/transpiler.cpp -o transpiler
```

### Unit Tests
Compile the unit tests:
```bash
g++ tests/unit_test_transpiler.cpp src/sexpr.cpp src/transpiler.cpp -o transpiler_test
```

## Run Instructions
### File Mode
Run the transpiler using a Lisp source file:
```bash
./transpiler example.lisp
```

The transpiler creates a Python source file:
    
    example.py

The generated Python file is then executed and its output is saved to:

    example.out

The output file contains anything explicitly printed by the generated Python program.

### Interactive Mode
Run the transpiler without an input file:
```bash
./transpiler
```

Enter Lisp expressions individually. The transpiler displays the equivalent Python code and asks whether the generated expresssion should be executed.

Example:
```text
Lisp to Python Transpiler
Enter Lisp expressions (type quit to exit):

(add 2 3)
Python: (2 + 3)
Execute? (y/n): y
5
```
Enter `quit` to exit interactive mode.

### Unit Tests
Run the unit tests:
```bash
./transpiler_test
```

Run the unit tests and save the output:
```bash
./transpiler_test > tests/unit_test_output.txt
```

## Testing
All testing files can be found in the `tests/` directory.

`unit_test_transpiler.cpp` - contains unit tests for individual Lisp-to-Python translations, file transpilation, generated Python execution, and somplete transpiled programs. It reports PASS or FAIL for each test case and provides a test report with the total number of passed and failed tests.

`unit_test_output.txt` - contains the saved output from the unit tests.


## Directory Structure
```text
lisp-python-transpiler/
├── README.md
├── src/
│   ├── main.cpp
│   ├── sexpr.cpp
│   ├── sexpr.h
│   ├── transpiler.cpp
│   └── transpiler.h
└── tests/
    ├── unit_test_transpiler.cpp
    └── unit_test_output.txt
```

## Limitations
Dotted pairs are not currenly supported by the Python translation. Proper Lisp lists are translated into Python lists.