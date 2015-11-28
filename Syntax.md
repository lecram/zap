# Overview #

zap has a concise syntax, both in terms of representation and number of rules.

All reserved keywords are prefixed by a backslash:

| `\break` | `\cont` | `\def`   |
|:---------|:--------|:---------|
| `\del`   | `\elif` | `\else`  |
| `\if`    | `\ret`  | `\while` |

Comments start with a hash sign (`#`) and go to the end of line.

Expressions are made of literal values, identifiers and function calls:

```
42                          # A literal integer value.
"Foo, Bar and Baz."         # A literal string value.
[7 11 13]                   # A literal list value.
foo                         # An identifier.
+-abc_123                   # Another valid identifier.
bar()                       # A function call.
f(x 17)                     # A function call with two arguments.
+(*(a b) /(256 -(15 7)))    # Another valid expression.
```

Unlike in many languages, there's no special symbol for assignments in zap. An assignment is represented by a whitespace between an identifier and an expression:

```
x 30     # Now x has value 30.
y +(x 1) # Now y has value 31.
```

An expression can be assigned to multiple names:

```
x y z 0 # Only zero is evaluated, and assigned to x, y and z.
```

Parentheses are used to make assignments from list items:

```
(a b c) [0 1 2] # Now a is 0, b is 1 and c is 2.
```

Control flow structures and function definitions are delimited only by indentation (there's no curly braces or begin/end keywords):

```
\if ==(x 1)
    # The following code is executed only if x is 1.
    b *(b 3)
    foo(b)
\else
    # The following code is executed if x isn't 1.
    bar(x)
    b *(b 7)
# The following code is always executed.
baz()
\del b
```

# Literals #

A literal None value is always `NONE`.

A literal Boolean value is either `TRUE` or `FALSE`.

A literal Byte value is either a hexadecimal in the form `0x61` or a single quoted ASCII character in the form `'a'`.

A literal Integer value is any digit sequence, optionally prefixed with `-`.

A literal ByteArray value is any double quoted sequence of ASCII characters.

A literal List value is a whitespace-separated sequence of expressions enclosed in square brackets (e.g. `[foo 1337 bar()]`).

# Identifiers #

A zap identifier shouldn't start with a digit or a backslash. Also, it shouldn't contain any whitespace, parenthesis, bracket, brace, colon, dot, comma or quote.

# Sample Code #

The following code shows the general syntactic aspects of a zap program.

```
# Returns the floored square root of x.
\def sqrt(x)
    (r o n) [0 1 x]
    \while <=(o x)
        o <<(o 2)
    o >>(o 2)
    \while !=(o 0)
        \if >=(n +(r o))
            n -(n +(r o))
            r +(>>(r 1) o)
        \else
            r >>(r 1)
        o >>(o 2)
    \ret r

a sqrt(49)
s join([repr(a) "\n"] "")
print(s) # -> 7
```