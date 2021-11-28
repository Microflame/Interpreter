-- lua5.2 runs fib(40) in ~58 seconds
-- luajit runs fib(40) in ~10.2 seconds

function fib(args)
    local n = args.n
    if n < 2 then return n end
    return fib{n=n - 1} + fib{n=n - 2}
end

print(fib{n=40})
