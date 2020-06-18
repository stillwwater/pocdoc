# vec.h

* [Vec3](#Struct-ne::Vec3)
    * [lerp](#Function-ne::Vec3::lerp)
    * [normalized](#Function-ne::Vec3::normalized)
    * [random_lambertian](#Function-ne::Vec3::random_lambertian)
    * [reflect](#Function-ne::Vec3::reflect)
    * [refract](#Function-ne::Vec3::refract)
    * [scale](#Function-ne::Vec3::scale)

---

## Struct `ne::Vec3`

```cpp
struct Vec3 {
    static const Vec3 zero;

    static const Vec3 one;

    static const Vec3 Up;

    static const Vec3 Left;

    static const Vec3 Right;

    static const Vec3 Down;

    static const Vec3 Forward;

    float x, y, z;

    Vec3() : x(0.0f), y(0.0f), z(0.0f) {};

    float length() const;

    float length_sqr() const;

    Vec3 normalized() const;

    static inline float dot(const Vec3 &a, const Vec3 &b);

    static inline Vec3 cross(const Vec3 &a, const Vec3 &b);

    static inline Vec3 scale(const Vec3 &a, const Vec3 &b);

    static inline Vec3 lerp(const Vec3 &a, const Vec3 &b, float t);

    static inline Vec3 reflect(const Vec3 &v, const Vec3 &normal);

    static inline Vec3 refract(const Vec3 &v, const Vec3 &n, float etai_etat);

    static inline Vec3 random_lambertian();

    static inline Vec3 random_in_unit_sphere();

    static inline Vec3 random_in_unit_circle();

    static inline Vec3 random_in_hemisphere(const Vec3 &normal);

    Vec3 operator-() const;

    bool operator==(const Vec3 &v) const;

    bool operator!=(const Vec3 &v) const;
};
```
### Function `ne::Vec3::lerp`

```cpp
static inline Vec3 lerp(const Vec3 &a, const Vec3 &b, float t);
```
Lineraly interpolate between two vectors

### Function `ne::Vec3::normalized`

```cpp
Vec3 normalized() const;
```
Returns the normalized vector to length 1

### Function `ne::Vec3::random_lambertian`

```cpp
static inline Vec3 random_lambertian();
```
Returns a vector of length 1 with a random direction

### Function `ne::Vec3::reflect`

```cpp
static inline Vec3 reflect(const Vec3 &v, const Vec3 &normal);
```
Reflect a vector to normal

### Function `ne::Vec3::refract`

```cpp
static inline Vec3 refract(const Vec3 &v, const Vec3 &n, float etai_etat);
```
Refract a vector on normal

### Function `ne::Vec3::scale`

```cpp
static inline Vec3 scale(const Vec3 &a, const Vec3 &b);
```
Multiply each component of one vector with another


---

