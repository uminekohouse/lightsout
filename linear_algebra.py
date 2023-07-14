import copy
def createAugmentedMatrix(A, b):
    #拡大係数行列の作成
    augmented_matrix = copy.deepcopy(A)
    for i in range(len(A)):
        augmented_matrix[i].append(copy.deepcopy(b[i][0]))
    for i in augmented_matrix: print(i)    
    return  augmented_matrix

def select_pivot(augmented_matrix, i):
    k = i
    m = len(augmented_matrix)
    val = copy.deepcopy(augmented_matrix[i][i])
    for j in range(i, m):
        tmp = copy.deepcopy(augmented_matrix[j][i])
        if int(tmp) < 0: tmp *= -1 
        if int(val) < int(tmp):
            val = tmp
            k = j
    if k != i:
        augmented_matrix[i], augmented_matrix[k] = augmented_matrix[k], augmented_matrix[i]



def SimultaneousEquationsSolver(A,b):
    #連立方程式を解く
    augmented_matrix = createAugmentedMatrix(A,b)
    m = len(augmented_matrix)
    n = len(augmented_matrix[0])
    # 前身消去
    for i in range(m):
        select_pivot(augmented_matrix, i)
        if int(augmented_matrix[i][i]) == 0:
            ok = True
            for j in range(i,m):
                for k in range(k,n):
                    if int(augmented_matrix[j][k]) != 0: ok = False
            if ok:
                m = i
                break
            return 
        x = copy.deepcopy(augmented_matrix[i][i])
        for j in range(i, n):
            augmented_matrix[i][j] /= x
        for j in range(i+1, m):
            y = copy.deepcopy(augmented_matrix[j][i])
            for k in range(i, n):
                augmented_matrix[j][k] -= augmented_matrix[i][k] * y 
    for i in range(m-1, 0, -1):
        val = n-1
        for j in range(n):
            if(int(augmented_matrix[i][j]!=0)):
                val = j
                break

        for j in range(i-1,-1,-1):       
            y = copy.deepcopy(augmented_matrix[j][val])
            for k in range(val, n):
                augmented_matrix[j][k] -= augmented_matrix[i][k]*y
    res = []
    for i in augmented_matrix:
        print(i)
        res.append(i[n-1])
    return res    
