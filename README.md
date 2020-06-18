# piece of cake documentation

A minimal documentation generator for C++ written using libclang.

The tool generates markdown file from each header file by extracting comments directly above declarations, as such documentation comments may use markdown syntax. A table of contents is optionally included.

Declarations in the resulting markdown are taken directly from the source file as is, and so the original formatting of the source is preserved.

## Minimal Example
For more complete examples see [test.h.md](./test/test.h.md) and [vec.h.md](./test/vec.h.md)

```math.h```

```cpp
// A 3D float vector.
struct Vector {
    float x, y, z;
    
    // Returns a normalized *copy* of the vector.
    Vector normalized() const;
};
```

``math.h.md``

> # math.h
>
> * [Vector](#Struct-Vector)
>     * [normalized](#Function-Vector::normalized)
>
> ---
>
> ## Struct `Vector`
>
> ```cpp
> struct Vector {
> 	float x, y, z;
>     
>     Vector normalized() const;
> };
> ```
>
> A 3D float vector.
>
> ### Function `Vector::normalized`
>
> ```cpp
> Vector normalized() const;
> ```
>
> Returns a normalized *copy* of the vector.

## Limitations

For starters C style comments (`/*comment*/`)  are ignored, use `//` , `//!  ` or `///` instead.

The are no documentation “commands” in the form of `\command some string`, if you need that sort of thing consider [doxygen](https://www.doxygen.nl/index.html) or [standardese](https://github.com/standardese/standardese/) instead.