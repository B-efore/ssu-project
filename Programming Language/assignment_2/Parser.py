def ERR():
    return -987654321

def number():
    value = ERR()

    while len(form)!=0 and '0' <= form[0] and form[0] <= '9':
        if value==ERR():
            value = int(form[0])
        else:
            value *= 10
            value += int(form[0])

        form.pop(0)

    return value

def factor():
    value = ERR()
    minusCheck = 1

    if len(form)!=0 and form[0] == '-':
        form.pop(0)
        minusCheck = -1

    if len(form)!=0 and '0' <= form[0] and form[0] <= '9':
        value = number()

    elif len(form)!=0 and form[0] == '(':
        form.pop(0)
        value = expr()

        if len(form)!=0 and form[0] == ')':
            form.pop(0)
        else:
            return ERR()

    else:
        return ERR()

    return (value * minusCheck)

def term():
    value = factor()

    if value == ERR():
        return ERR()

    while len(form)!=0:

        if form[0]!='*' and form[0]!='/':
            break

        if form[0]=='*':
            form.pop(0)

            tmp = factor()

            if tmp==ERR():
                return ERR()

            value *= tmp

        elif form[0]=='/':
            form.pop(0)

            tmp = factor()

            if tmp==ERR():
                return ERR()

            value /= tmp

    return value

def expr():
    value = term()

    if value == ERR():
        return ERR()

    while len(form)!=0:
        if form[0]!='+' and form[0]!='-':
            break

        if form[0]=='+':
            form.pop(0)

            tmp = term()

            if tmp == ERR():
                return ERR()

            value += tmp

        elif form[0]=='-':
            form.pop(0)

            tmp = term()

            if tmp == ERR():
                return ERR()

            value -= tmp

    return value

while 1:
    stringInput = input(">> ")
    newInput = stringInput.replace(" ", "")
    form = list(newInput)

    solve = expr()

    if solve == ERR() or len(form)!=0:
        print("syntax error!")
        break

    print(solve)