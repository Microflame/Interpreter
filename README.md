# About

This is my implementation of the first half of https://craftinginterpreters.com/ but in C++.

# Build

Tested under gcc-9:

```
mkdir build && cd build
cmake ..
make
```

# Run Example

```
./src/Interp ../example.inp
```

should result in result:

```
==== print() examples ====
This "escaped_string" contains \
61
==== while loop example ====
i = 1 / 5
i = 2 / 5
i = 3 / 5
i = 4 / 5
i = 5 / 5
==== closure example ====
counter result = 1
counter result = 2
counter result = 3
==== inheritance example ====
A method
B method
```

see `example.inp` to better understand what is happening.
