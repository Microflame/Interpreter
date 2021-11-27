-- lua5.2 runs fib(40) in ~14.7 seconds
-- luajit runs fib(40) in ~0.9 seconds

function fib(n)
    if n < 2 then return n end
    return fib(n - 1) + fib(n - 2)
end

print(fib(40))
