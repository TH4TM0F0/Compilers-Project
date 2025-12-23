# Compiler Project Documentation  
**Lexical Analysis, Parsing, Semantic Processing, and Intermediate Code Generation**

---

## 1. Project Overview

This project implements a **custom C-like programming language compiler** using a traditional **multi-phase compiler architecture**. The compiler translates high-level source code into an **intermediate representation (IR)** based on **quadruples**, and then lowers this representation into a **pseudo-assembly language**.

The design follows standard compiler construction principles and is divided into the following phases:

1. **Lexical Analysis**  
   The source code is scanned and converted into a stream of tokens using **Flex**.

2. **Syntax Analysis**  
   Tokens are parsed using **Bison** to validate grammatical correctness and construct semantic actions.

3. **Semantic Analysis**  
   - Type checking  
   - Symbol table management  
   - Scope validation  
   - Function and parameter consistency  

4. **Intermediate Representation (IR) Generation**  
   The compiler generates **quadruples** to represent program semantics in a structured, machine-independent form.

5. **Intermediate Assembly Generation**  
   Quadruples are translated into a readable **pseudo-assembly language**, bridging the gap between semantic IR and machine-level code.

The compiler is modular, extensible, and designed for clarity, correctness, and maintainability.

---

## 2. Tools and Technologies Used

| Component | Technology |
|---------|------------|
| Programming Language | **C** |
| Lexical Analyzer | **Flex** |
| Parser Generator | **Bison** |
| Build System | **Makefile (GCC)** |
| Intermediate Representation | **Quadruples** |
| Backend Output | **Pseudo-Assembly Language** |

---

## 3. Project Structure

```
Include/
 ├── Assembler.h
 ├── ErrorHandler.h
 ├── Parameter.h
 ├── Quadruple.h
 ├── SymbolTable.h
 ├── Utils.h
 └── WarningHandler.h

Source/
 ├── Assembler.c
 ├── ErrorHandler.c
 ├── Parameter.c
 ├── Quadruple.c
 ├── SymbolTable.c
 ├── Utils.c
 └── WarningHandler.c

lexer.l
parser.y
makefile
```

---

## 4. Lexical Tokens Specification

### 4.1 Keywords

| Token | Description |
|------|-------------|
| int | Integer data type |
| float | Floating-point data type |
| bool | Boolean data type |
| char | Character data type |
| string | String data type |
| void | Function with no return |
| if | Conditional branching |
| else | Alternative conditional branch |
| while | Loop construct |
| for | Iterative loop |
| repeat | Post-test loop |
| until | Loop termination condition |
| switch | Multi-branch selection |
| case | Switch case |
| default | Switch default case |
| break | Loop or switch termination |
| continue | Skip to next loop iteration |
| return | Function return |
| function | Function declaration |
| print | Output statement |

---

### 4.2 Identifiers and Literals

| Token | Description |
|------|-------------|
| IDENTIFIER | Variable or function name |
| INT_VALUE | Integer literal |
| FLOAT_VALUE | Floating-point literal |
| BOOL_VALUE | true or false |
| CHAR_VALUE | Character literal |
| STRING_VALUE | String literal |

---

### 4.3 Operators

#### Arithmetic Operators
| Operator | Meaning |
|--------|---------|
| + | Addition |
| - | Subtraction |
| * | Multiplication |
| / | Division |
| % | Modulus |
| ** | Exponentiation |

#### Comparison Operators
| Operator | Meaning |
|--------|---------|
| == | Equal |
| != | Not equal |
| < | Less than |
| > | Greater than |
| <= | Less than or equal |
| >= | Greater than or equal |

#### Logical Operators
| Operator | Meaning |
|--------|---------|
| and | Logical AND |
| or | Logical OR |
| not | Logical NOT |

---

## 5. Quadruple Intermediate Representation

### 5.1 Quadruple Format

```
(OPERATOR, ARG1, ARG2, RESULT)
```

---

## 6. Quadruple Instruction Set

### Assignment
| Quadruple | Description |
|----------|-------------|
| (=, x, NONE, y) | Assign x to y |

### Arithmetic
| OpType | Meaning |
|------|---------|
| ADD | Addition |
| SUB | Subtraction |
| MUL | Multiplication |
| DIV | Division |
| MOD | Modulus |
| EXP | Exponentiation |

### Comparison
| OpType | Meaning |
|------|---------|
| EQ | Equal |
| NEQ | Not Equal |
| LT | Less Than |
| GT | Greater Than |
| LTE | Less Than or Equal |
| GTE | Greater Than or Equal |

### Logical
| OpType | Meaning |
|------|---------|
| AND | Logical AND |
| OR | Logical OR |
| NOT | Logical NOT |

### Control Flow
| OpType | Description |
|------|-------------|
| LABEL | Label declaration |
| GOTO | Unconditional jump |
| IF_GOTO | Jump if condition true |
| IFFALSE | Jump if condition false |

### Functions
| OpType | Description |
|------|-------------|
| PARAM | Push argument |
| CALL | Call function |
| RETURN | Return from function |

---

## 7. Pseudo-Assembly Language

General Syntax:
```
INSTRUCTION RESULT, ARG1, ARG2
```

Example:
```asm
MOV x, 5
ADD t1, x, 10
MOV EAX, t1
RET
```

---

## 8. High-Level Language to Quadruples Mapping

### Example
```c
int x = 5;
```
```
(=, 5, NONE, x)
```

---

## 9. Summary

This project demonstrates a complete compiler pipeline from lexical analysis to intermediate assembly generation. Quadruples provide a clean and extensible semantic representation, enabling structured code generation and future optimization.