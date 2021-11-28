# python3 fib(40) runs in ~ 45.8 s.
# pypy3 fib(40) runs in ~ 2.86 s.

def fib(**args):
    n = args['n']
    if n < 2:
        return n
    return fib(n=n - 1) + fib(n=n - 2)

print(fib(n=40))
