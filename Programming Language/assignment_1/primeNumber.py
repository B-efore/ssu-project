import math
import time

def GCD(a, b):
    if a > b:
        i = b
    else:
        i = a

    while i > 0:
        if a % i == 0 and b % i == 0: break
        i -= 1

    return i


def merge(list1, list2):
    tmp = list()

    while (len(list1) > 0) and (len(list2) > 0):
        if list1[0] <= list2[0]:
            tmp.append(list1.pop(0))
        else:
            tmp.append(list2.pop(0))

    if (len(list1)>0):
        tmp += list1

    if (len(list2)>0):
        tmp += list2

    return tmp


def mergeSort(num_list):

    if len(num_list) <= 1:
        return num_list

    mid = len(num_list) // 2
    left = mergeSort(num_list[:mid])
    right = mergeSort(num_list[mid:])
    merged = merge(left, right)
    return merged

def primeNumber(x, p):

    p[0] = 0
    p[1] = 0

    for i in range(2, int(math.sqrt(x))+1):
        if p[i]==1:
            for j in range(i*2, x+1, i):
                p[j] = 0

    return p

def prime(a, b, p):

    num = 0

    while a<=b:
        if p[a]==1:
            num += 1

        a += 1

    return num

while 1:
    n = int(input(">> Input the number of numbers to process: "))
    if (2<=n) and (n<=30):
        break

print(">> Input the numbers to be processed:")

num_list = list(map(int, input().split()))


start = time.time()

gcd = GCD(num_list[0], num_list[1])
for i in range(2, n):
    gcd = GCD(gcd, num_list[i])

print("GCD of input numbers is", gcd)

sortedArr = mergeSort(num_list)
p = [1 for i in range(100001)]
p = primeNumber(sortedArr[len(sortedArr)-1], p)

for i in range(0, n-1, 1):
    if sortedArr[i] != sortedArr[i+1]:
        print("Number of prime numbers between ",
              sortedArr[i], ",", sortedArr[i+1], ":", prime(sortedArr[i], sortedArr[i+1], p))

end = time.time()
print("Total execution time using Python is", (end-start), "seconds!")
