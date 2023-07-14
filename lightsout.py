from tkinter import *
from tkinter import ttk
import time
import copy
from random import randrange
from PIL import Image, ImageTk


N = 30

root = Tk()
root.title("lightsout")
root.geometry("900x1600")
HEIGHT = 900
WIDTH = 900
frame1 = Frame(root, height=HEIGHT, width=WIDTH)
frame2 = Frame(root, height=900, width=600)
frame1.place(x=0, y=0)
frame2.place(x=WIDTH, y=0)

button_size = HEIGHT//N
img1 = Image.open("image1.png")
img2 = Image.open("image2.png")
img1 = img1.resize((button_size, button_size))
img2 = img2.resize((button_size, button_size))
img1 = ImageTk.PhotoImage(img1)
img2 = ImageTk.PhotoImage(img2)

dh = (-1, 0, 1, 0)
dw = (0, 1, 0, -1)

board_state = [[0 for _ in range(N)] for _ in range(N)]
pieces = [[None for _ in range(N)] for _ in range(N)]
pieces = [[Button(frame1, image=img1, width=3, border=0, command=lambda h=h,
                  w=w: cmd(h, w)) for w in range(N)] for h in range(N)]
solve_pieces = [[0 for _ in range(N)] for _ in range(N)]


def change_img(h, w):
    global board_state
    board_state[h][w] ^= 1
    pieces[h][w]["image"] = img1 if not board_state[h][w] else img2


def cmd(h, w):
    change_img(h, w)
    for i in range(4):
        next_h = h+dh[i]
        next_w = w+dw[i]
        if (next_h < 0 or next_h >= N or next_w < 0 or next_w >= N):
            continue
        change_img(next_h, next_w)


for i in range(N):
    for j in range(N):
        pieces[i][j].place(y=i*HEIGHT/N, x=j*WIDTH/N,
                           width=button_size, height=button_size)


def convert_1d_to_2d(l, cols):
    return [l[i:i + cols] for i in range(0, len(l), cols)]


def swap_row(A, i, j):
    tmp = A[i]
    A[i] = A[j]
    A[j] = tmp
    return A


def get_stair_matrix(A):
    n = len(A)
    B = [[0 for _ in range(n)] for _ in range(n)]
    for i in range(n):
        B[i][i] = 1

    for i in range(n):
        if not A[i][i]:
            for j in range(i+1, n):
                if A[j][i]:
                    A = swap_row(A, i, j)
                    B = swap_row(B, i, j)
                    break
        for j in range(i+1, n):
            if A[j][i]:
                for k in range(n):
                    A[j][k] ^= A[i][k]
                    B[j][k] ^= B[i][k]
    for i in range(n-1, -1, -1):
        if not A[i][i]:
            continue
        for j in range(i-1, -1, -1):
            if not A[j][i]:
                continue
            for k in range(n):
                A[j][k] ^= A[i][k]
                B[j][k] ^= B[i][k]
    return A, B


def matrix_times_vector(A, x):
    n = len(A)
    res = [0 for _ in range(n)]
    for i in range(n):
        for j in range(n):
            res[i] ^= A[i][j]*x[j]
    return res


def matrix_times_matrix(A, B):
    n = len(A)
    C = [[0 for _ in range(n)] for _ in range(n)]
    for i in range(n):
        for j in range(n):
            for k in range(n):
                C[i][j] ^= A[i][k]*B[k][j]
    return C


# 階段行列を入れて自由度を得る
def get_corank(A):
    n = len(A)
    for i in range(n):
        ok = True
        for j in A[i]:
            if not j:
                ok = False
                break
        if (ok):
            return m-i
    return 0


# 階段行列を入れてね
def get_kernel_vectors(A):
    n = len(A)
    sets = [[0 for _ in range(n)]]
    corank = get_corank(A)

    kernel_base_vectors = []

    for i in range(n-corank, n):
        v = [0 for _ in range(n)]
        for j in range(0, i-1):
            if (A[j][i]):
                v[j] = 1
        v[i] = 1
        kernel_base_vectors.append(v)

    for v in kernel_base_vectors:
        sets_tmp = []
        for s in sets:
            sets_tmp.append([i+j for i, j in zip(s, v)])
            sets_tmp.append(s)
        sets = sets_tmp
    return sets


def can_solve(A, B, x):
    corank = get_corank(A)
    v = matrix_times_vector(B, x)
    if corank:
        for i in range(N*N-corank, N):
            if x[i]:
                return False
    return True


def get_shortest(A, B, x):
    sets = get_kernel_vectors(A)
    cnt = N*N
    shortest = x
    # print(x)
    for v in sets:
        tmp_cnt = 0
        u = [i ^ j for i, j in zip(x, v)]
        for k in u:
            if k:
                tmp_cnt += 1
        if (tmp_cnt < cnt):
            cnt = tmp_cnt
            shortest = u

    return cnt, shortest


def solve():
    global solve_pieces
    A = [[0 for _ in range(N*N)] for _ in range(N*N)]
    for i in range(N*N):
        A[i][i] = 1
        if (i % N != 0):
            A[i][i-1] = 1
        if ((i+1) % N != 0):
            A[i][i+1] = 1
        if (i+N < N*N):
            A[i][i+N] = 1
        if (i-N >= 0):
            A[i][i-N] = 1
    A, B = get_stair_matrix(A)
    b = [0 for _ in range(N*N)]
    for i in range(N):
        for j in range(N):
            b[i*N+j] = board_state[i][j]
    # for i in A: print(i)
    if (can_solve(A, B, b)):
        x = matrix_times_vector(B, b)
        # print(x)
        cnt, shortest = get_shortest(A, B, x)
        print(cnt)
        print(shortest)

    for i in range(N):
        for j in range(N):
            solve_pieces[i][j] = shortest[i*N+j]
    solve_draw()


def random():
    global board_state
    for i in range(N):
        for j in range(N):
            tmp = randrange(100)
            if (tmp % 2):
                cmd(i, j)


title = Label(frame2, text="LIGHTSOUT", font=("family", 85, "bold", "italic"))
title.place(x=0, y=0)

frame3 = Frame(frame2, height=480, width=480)
frame3.place(x=0, y=350)
canvas = Canvas(frame3, background="#fff", height=480, width=480)
canvas.pack()
for i in range(N+1):
    canvas.create_line(480/N*i, 0, 480/N*i, 480, width=3)
    canvas.create_line(0, 480/N*i, 480, 480/N*i, width=3)


def solve_draw():
    for i in range(N):
        for j in range(N):
            if (solve_pieces[i][j]):
                canvas.create_oval(480/N*j+5, 480/N*i+5, 480/N*j+480 /
                                   N-5, 480/N*i+480/N-5, fill="green", outline="green")


reset = Button(frame2, text="RESET",  height=1, width=6, font=("family", 30))
start = Button(frame2, text="START",  height=1, width=6, font=("family", 30))
random = Button(frame2, text="RANDOM",  command=random,
                height=1, width=6, font=("family", 30))
solve = Button(frame2, text="SOLVE",  command=solve,
               height=1, width=6, font=("family", 30))
reset.place(x=0, y=100)
start.place(x=150, y=100)
random.place(x=300, y=100)
solve.place(x=0, y=150)

s = Spinbox(
    frame2,
    textvariable=N,   # 変数
    from_=0,          # 下限値
    to=60,              # 上限値
    increment=1,        # 増減ステップ
    width=2,
    font=("", 30),
)
s.place(x=70, y=200)

size = Label(frame2, text="SIZE", height=1, width=3, font=("family", 30))
size.place(x=0, y=200)

root.mainloop()
