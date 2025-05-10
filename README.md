<div align="center">

# lain

<image src="docs/lain.png" width="128" />

[![MIT License](https://img.shields.io/badge/License-Apache_2.0-red?style=for-the-badge&logo=googledocs&logoColor=white)](LICENSE)
[![C++](https://img.shields.io/badge/Implement-C++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)](#)
![Target](https://img.shields.io/badge/Target-LLVM-green?style=for-the-badge&logo=amazonec2&logoColor=white)

Cautiously modern approach to systems programming.

</div>

## Design

> **[IMPORTANT]** lain programming language is in active development/design and is subject to change.

Lain is designed to combine the convenience of ease of use syntax such as templating and type inference, without including any runtime overhead associated with modern languages like garbage collection and 'virtual methods'. The compiler allows for stricter checking than C, though is not ridiculously pedantic and restrictive like rust. Despite diverging significantly syntactically from C, it is designed to remain trivially transpilable to it.

```lain
# fancy literals
var localhost = 127.0.0.1;
var red = #ff0000;

# explicit compile time declarations
comp u8 bytes[] = {
    1, 4, 6, 7, 9
    3, 5, 6, 6, 8
};

# inline template definitions
fun arr_len (comp <T> arr[]) : uint {
    return sizeof(arr) / sizeof(T);
}
```

### Memory Safety

<p align="justify">
Lain provides the user with the `unique` keyword, which is a pointer declaration modifier that can be applied for heap allocated pointers. A pointer marked unique will automatically free itself upon leaving the scope it was declared in, including early exits from `return`, `break`, etc. To prevent dangling pointers from accessing the memory after it's been freed, unique pointers are protected from having their value assigned elsewhere. 
</p>

```lain
fun no_leak_here () {
    unique u8 *ptr = new u8(1000);

    u8 *x = ptr;    # compile error: cannot share memory

    if (global_variable) {
        # free is implicitly called before return
        return;
    }

    use_ptr(ptr);   # ok if function parameter is also marked unique

    # free is called implicitly on scope exit
}
```
<p align="justify">
Lain also keeps track of the size of memory from both stack and heap allocations and will throw a compiler error in a provably improper access. These protections may be propogated outside of the context where the memory is defined through a properly defined function prototype, with no additional runtime overhead. For example lain's `strncpy` will behave like it's `cstdlib` counterpart, except the prototype states n should be <= the size of the memory of both the pointer arguments; if lain can prove one of the pointers doesnt have enough memory a compiler error will be thrown.
</p>

```lain
fun lain_safety (uint n) {
    var data = new u8(100);
    data[n+1] = 0;                  # error: out of bounds

    strncpy(data, GLOBAL_STR, n+1); # error: out of bounds

    free data;
    data[0] = 0;                    # error: use after free
}
```

### Enums

Enums are already incredibly useful in C, but can be a hassle to cleanly work with; working with them often results in a lot of duplicate code. In lain, they have been expanded to be far more powerful then just aliases to integers.

```lain
enum TokenType {
    Identifier  = "identifier";
    Integer     = "integer";
    String      = "string";
};
```

Other than the implicit unique integer each enum entry is assigned, lain allows any number of other compile time values to also be assigned to it. The compiler will construct perfect hash tables for converting between the enumerated type at runtime, or will automatically convert at compile time if possible. The most common use case for this is to assign a string value to the enum, allowing for easy printing.

Another common usecase for enums is for bit flags, where it is not trivial for the user to have the flags automatically assigned unique bits without some metaprogramming.

```lain
enum TokenCategory : bitflags {
    None = 0;
    Separator;                          # assigned 0b0001
    Operator;                           # assigned 0b0010
    Keyword;                            # assigned 0b0100
}
```

### Metaprogramming

As lain was initially envisioned as c with some minor preprocessor time syntax extensions, metaprogramming is a very important part of the language. The goal of the langauge is to provide a more powerful, semantically responsive, and syntacticaly pleasing alternative to c-style macros. One ability of lain's function-style macros is to allow the user to constrain its parameters without including manual static asserts. Macros are also able to return values, much like the gcc+clang `({})` extension.

```lain
struct list { 
    int value;
    list *next;
};

macro list_next (list *l) {
    l = l.next;
}

fun iter_linked_list (list *l) {
    while (l) {
        list_next(l);
    }
}
```