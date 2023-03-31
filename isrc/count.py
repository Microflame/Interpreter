def add(n):
    return n + 1

i = 0
while i < 500000:
    i = add(i)

print(i)