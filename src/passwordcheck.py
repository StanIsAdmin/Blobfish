

def main():
    pwd_maxVal = 32                 #Values in range [1, 32] --> FINAL PWD LENGHT
    pwd_maxTempVal = pwd_maxVal*10  #Temp values in range [1, 320] -->LIMIT TO PWD-TEXT LENGHT
    pwd_values = []                 #Will contain final values
    
    with open('pwd.txt', 'r') as pwd_file:
        pwd_tempVal = [ord(char) for char in pwd_file.readline()] #One line passwords
    if len(pwd_tempVal) < pwd_maxVal//4: #Not secure enough
        return("Error: password contains less than 8 characters")
    if len(pwd_tempVal) > pwd_maxTempVal: #We use maxVal*10 possible temp values!
        return("Error: password contains more than 320 characters")
    
    for v in pwd_tempVal: #We make the values unique
        v_temp = makeUnique(v, pwd_values, pwd_maxTempVal)
        pwd_values.append(v_temp)#A new list makes the average change more
    
    if len(pwd_values) != pwd_maxVal: #We adjust the lenght of the values list
        adjustLenght(pwd_values, pwd_maxVal-len(pwd_values), pwd_maxTempVal)
    
    normalize(pwd_values, pwd_maxVal)
    print(len(pwd_values))
    print(' '.join(list(map(str, pwd_values))))
    return pwd_values

def makeUnique(v, values, tempMax):
    try:
        average = sum(values)//len(values) + 1
    except:
        average = 1
    supp = 0
    while v in values:
        v = (v*average + supp)%(tempMax)
        supp += 1 #Supplementary value makes all values possible and avoids useless iterations
    return v

def adjustLenght(pwd_values, sizeDiff, tempMax):
    if sizeDiff > 0:#If we need to ADD values (not to use anymore)
        for i in range(sizeDiff):
            v_new = pwd_values[i] + pwd_values[-1]
            v_new = makeUnique(v_new, pwd_values, tempMax)
            pwd_values.append(v_new)
    else:           #If we need to REMOVE values
        i = 0       
        while len(pwd_values) > 32:
            v_new = pwd_values[i] + pwd_values[-1]
            v_new = makeUnique(v_new, pwd_values, tempMax)
            pwd_values[i] = v_new
            del pwd_values[-1] #NNNNNNNNNNNNNNNNNNNNNNEED TO INCREASI I-> TEST RANGE ERRORS
            
def normalize(pwd_values, pwd_maxVal):
    maxCount = -pwd_maxVal #Counts from -maxVal to -1 and gives its value to index of current max.
    for i in range(pwd_maxVal):
        #We look for each maximum, and give them an increasingly small (in abs) negative value
        pwd_values[pwd_values.index(max(pwd_values))] = maxCount
        maxCount += 1
    for i in range(pwd_maxVal):
        pwd_values[i] *= -1
            
if __name__=="__main__":
    main()