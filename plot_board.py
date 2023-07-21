import sys
import tkinter

root = tkinter.Tk()
root.title(u"lightsout")
root.geometry("900x900")


dI = [1, 0, -1, 0]
dJ = [0, 1, 0, -1]


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
                if (nI < 0 or nI > H or nJ < 0 or nJ > W):
                    continue
                board[nI][nJ] = board[nI][nJ] ^ 1
    return board


def print_board(board):
    H = len(board)
    W = len(board[0])
    for i in range(H):
        print(board[i])


def print_board_state(H, W, board, matrix):
    canvas = tkinter.Canvas(
        root,
        width=900,
        height=900,
        bg='white'
    )
    canvas.pack()


Q = int(input())
for _ in range(Q):
    H, W, N = map(int, input().split())
    for _ in range(N):
        matrix = [list(map(int, input().split(" "))) for _ in range(H)]
        # board = [[0 for _ in range(W+1))] for _ in range(H+1)]
        board = matrix_to_board(matrix)
        print_board(board)
        print_board_state(H, W, board, matrix)

root.mainloop()
