# Recursive Descent Parser – User Manual

## Overview
This program implements a **recursive descent parser** for a small, expression-based
programming language. The parser is written in C++ and follows a top-down,
function-per-grammar-rule design.

The language supports:
- A required `begin ... end.` program structure
- Assignment statements
- Arithmetic expressions with proper operator precedence
- Identifiers and integer literals
- Parenthesized expressions
- Line comments starting with `~`

The parser assumes a **single lookahead token** and is designed to mirror the
grammar rules directly in code.

---

## Building the Program
To compile the program, run:

```bash
make
```  
  
This will compile compiler.cpp and produce an executable named main.  
  
## Running the Program  
To run the parser, use:  
```  
./main <test-file>  
```  
   
Example:  
```  
./main ./tests/a1  
```  
  
Alternatively, use the Makefile helper:  
```  
make run FILE=./tests/a1  
```  
  
# Fail vs Pass
Pass will state:  
```
Parsing completed successfully.  
```  
  
Fail will state:  
```  
Error: Program must start with 'begin'  
NextToken: 11  
NextChar: 
  
Lexeme: began  
```  
