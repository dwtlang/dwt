[![Actions Status](https://github.com/dwtlang/dwt/workflows/performance profile/badge.svg)](https://github.com/dwtlang/dwt/actions) [![Actions Status](https://github.com/dwtlang/dwt/workflows/compact profile/badge.svg)](https://github.com/dwtlang/dwt/actions)

# Dwt
A simple scripting language called Dwt (rhymes with "put"), which is Welsh for a dinky thing that is cute and small in stature.

Dwt is written in modern C++ and has no external dependencies beyond the C++ standard library.

Dwt was inspired by the charming book "Crafting Interpreters" by Bob Nystrom who helped to lift the veil on a subject I had wrongly considered witchcraft and wizardry and way beyond my abilities. See https://craftinginterpreters.com/

## Feaures
* A multi-pass, multi-threaded optimising compiler
* A fast and efficient bytecode interpreter
* Support for functions, closures, lambdas, objects, maps, lists, and unicode strings

## Examples
### Function
```
fun fibonacci(n) {
  if n <= 1 {
    return 1
  }
  return self(n - 1) + self(n - 2)
}

print fibonacci(28)
```
### Lambda
```
print λ(28) |n| {
    if n <= 1 {
        return 1
    }
    return self(n - 1) + self(n - 2)
}
```
## Building
Dwt has been tested on Linux and FreeBSD on x86_64 and ARM and can be built using GNU Make:

```
$ make release
```
or
```
$ make debug
```
## Confession
Dwt is still a work in progress so it has plenty of rough edges, the syntax is still subject to change, and there is no standard library yet. Users looking for a stable scripting language to embed in their project should look elsewhere for now but if you are interested in this stuff and fancy getting involved there is a lot of low hanging fruit on the development side of things and your help will be very welcome!
