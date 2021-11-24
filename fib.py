# fib(40) runs in ~ 23 s.

import time
import sys

def fib(n):
    if n < 2:
        return n
    return fib(n - 1) + fib(n - 2)

s = time.time()
print(fib(40))
e = time.time()
print((e - s) * 1000)
