# test.h

* [EmptyStructDecl](#Struct-EmptyStructDecl)
* [EnumClassDecl](#Enum-EnumClassDecl)
    * [Constant_1](#Enum Constant-EnumClassDecl::Constant_1)
    * [Constant_2](#Enum Constant-EnumClassDecl::Constant_2)
    * [Constant_3](#Enum Constant-EnumClassDecl::Constant_3)
    * [Last_Constant](#Enum Constant-EnumClassDecl::Last_Constant)
* [StructDeclA](#Struct-StructDeclA)
* [AllInOneLineEnum](#Enum-ns::AllInOneLineEnum)
    * [Constant_A](#Enum Constant-ns::AllInOneLineEnum::Constant_A)
    * [Constant_B](#Enum Constant-ns::AllInOneLineEnum::Constant_B)
* [BaseClassDecl](#Class-ns::BaseClassDecl)
    * [private_function_with_comment](#Function-ns::BaseClassDecl::private_function_with_comment)
    * [template_func](#Function-ns::BaseClassDecl::template_func)
    * [template_func2](#Function-ns::BaseClassDecl::template_func2)
    * [virtual_func](#Function-ns::BaseClassDecl::virtual_func)
* [ClassDecl](#Class-ns::ClassDecl)
* [StructDeclB](#Struct-ns::StructDeclB)
    * [StructDeclB](#Constructor-ns::StructDeclB::StructDeclB)
    * [member_func](#Function-ns::StructDeclB::member_func)
* [StructDeclD](#Struct-ns::StructDeclD)
* [free_function](#Function-ns::free_function)
* [free_function_2](#Function-ns::free_function_2)
* [StructDeclC](#Struct-ns::ns2::StructDeclC)
* [u32](#Typedef-ns::u32)
* [u64](#Type Alias-ns::u64)

---

## Struct `EmptyStructDecl`

```cpp
struct EmptyStructDecl {
};
```

---

## Enum `EnumClassDecl`

```cpp
enum class EnumClassDecl {
    Constant_1,
    Constant_2,
    Constant_3,
    Last_Constant = Constant_3
};
```
### Enum Constant `EnumClassDecl::Last_Constant`

```cpp
Last_Constant = Constant_3
```
Same value as Constant_3


---

## Struct `StructDeclA`

```cpp
struct StructDeclA {
    std::string name;

    std::vector<uint64_t> items;
};
```

---

## Enum `ns::AllInOneLineEnum`

```cpp
enum AllInOneLineEnum { Constant_A, Constant_B };
```
This enum is written in one line

## Class `ns::BaseClassDecl`

```cpp
class BaseClassDecl {
public:
    virtual void virtual_func(const std::unique_ptr<StructDeclB> &decl_b) const;

    virtual ~BaseClassDecl() = default;

    void member_func(int x, int y) noexcept;

    template<typename T>
    void template_func() const;

    template<typename... Args>
    std::string
    template_func2(const std::vector<std::shared_ptr> &long_variable_name1,
                   const std::vector<std::shared_ptr> &long_variable_name2);
};
```
### Function `ns::BaseClassDecl::template_func`

```cpp
template<typename T>
void template_func() const;
```
Some template function in BaseClassDecl

### Function `ns::BaseClassDecl::template_func2`

```cpp
template<typename... Args>
std::string
template_func2(const std::vector<std::shared_ptr> &long_variable_name1,
               const std::vector<std::shared_ptr> &long_variable_name2);
```
A template function that takes up many lines

### Function `ns::BaseClassDecl::virtual_func`

```cpp
virtual void virtual_func(const std::unique_ptr<StructDeclB> &decl_b) const;
```
A *virtual* function in BaseClassDecl

    // This is a comment in a code block

See the example above.


---

## Class `ns::ClassDecl`

```cpp
class ClassDecl : public BaseClassDecl {
public:
    int documented_member;

    std::vector<std::string> vector_of_string;

    int this_one_will_not;
};
```
#### Member Variables
* `documented_member`  This member of ClassDecl is documented
* `vector_of_string`  This will also show up


---

## Struct `ns::StructDeclB`

```cpp
struct StructDeclB {
    float x, y, z;

    StructDeclB(float x, float y, float z);

    static inline StructDeclB member_func(float x, float y);
};
```
### Constructor `ns::StructDeclB::StructDeclB`

```cpp
StructDeclB(float x, float y, float z);
```
Constructor for StructDeclB

### Function `ns::StructDeclB::member_func`

```cpp
static inline StructDeclB member_func(float x, float y);
```
Static inline member function in StructDeclB


---

## Struct `ns::StructDeclD`

```cpp
struct StructDeclD {
    union;
};
```
### Union `ns::StructDeclD::`

```cpp
union {
    u64 var64;

    u32 var32;
};
```

---

## Function `ns::free_function`

```cpp
int free_function();
```
Some function in namespace ns

## Function `ns::free_function_2`

```cpp
int free_function_2();
```
Another function declaration in namespace ns

## Struct `ns::ns2::StructDeclC`

```cpp
struct StructDeclC {
    int x;

    int y;
};
```
A struct in a nested namespace


---

## Typedef `ns::u32`

```cpp
typedef uint32_t u32;
```
A typedef

## Type Alias `ns::u64`

```cpp
using u64 = long long;
```
A type alias
