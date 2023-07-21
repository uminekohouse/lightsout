
    
dI = [1, 0, -1, 0]
dJ = [0, 1, 0, -1]

filename= 'sample.txt'
"""
def setup():
    size(900,900)
    background(255)

def draw():
    react(30, 30, 30, 30)        
"""

def matrix_to_board(matrix):
    H = len(matrix)
    W = len(matrix[0])
    board = [[0 for _ in range(W+2)] for _ in range(H+2)]
    for i in range(H):
        for j in range(W):
            if (matrix[i][j] == 0):
                continue
            I = i+1
            J = j+1
            board[I][J] = board[I][J] ^ 1
            for k in range(4):
                nI = J+dI[k]
                nJ = I+dJ[k]
                if (nI < 0 or nI > H+1 or nJ < 0 or nJ > W+1):
                    continue
                board[nI][nJ] = board[nI][nJ] ^ 1
    return board


def print_board(board):
    H = len(board)
    W = len(board[0])
    for i in range(H):
        print(board[i])
        
def plot_board_state(H, W, board, matrix):
    size(900,900)
                
def input_text():
    lines = loadStrings(filename)
    index = 1
    for _ in range(int(lines[0])):
        H, W, N = map(int, lines[index].split())
        index += 1
        print(H, W, N)
        for _ in range(N):
            matrix = []
            for i in range(H):    
                matrix.append(list(map(int, lines[index+i].split())))
            board = matrix_to_board(matrix)
            print_board(board)    
            index += H
            plot_board_state(H, W, board, matrix)
            
input_text()
"""
for _ in range(Q):
    H = int(input())
    W = int(input())
    N = int(input()) 
    for _ in range(N):
        matrix = [list(map(int, input().split(" "))) for _ in range(H)]
        # board = [[0 for _ in range(W+1))] for _ in range(H+1)]
        board = matrix_to_board(matrix)
        print_board(board)
        """
        
