# python3 fib(40) runs in ~ 23 s.
# pypy3 fib(40) runs in ~ 1.9 s.
# Interp fib(40) runs in ~ 69 s.

def fib(n):
    if n < 2:
        return n
    return fib(n - 1) + fib(n - 2)

print(fib(35))
