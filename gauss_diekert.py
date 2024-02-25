# For given t, generates Diekert's graph in dot format for concurrent Gauss algorithm.
# The graph is saved to gauss_diekert.dot.

t = 8

############################################

from graphviz import Digraph

A_colors = ["#ccccff", "#8080ff", "#3333ff", "#0000cc", "#000066"]
B_colors = ["#99ff99", "#1aff1a", "#009900", "#006600", "#003300"]
C_colors = ["#ff9999", "#ff4d4d", "#990000", "#660000", "#330000"]

A = []
for i in range(1, t):
    for j in range(i + 1, t + 1):
        if i <= 5:
            color = A_colors[i - 1]
        else:
            color = "white"
        A += [((i, j), f"A{i}{j}", color)]

B = []
for i in range(1, t):
    if i <= 5:
        color = B_colors[i - 1]
    else:
        color = "white"
    for j in range(i, t + 2):
        for k in range(i + 1, t + 1):
            B += [((i, j, k), f"B{i}{j}{k}", color)]

C = []
for i in range(1, t):
    if i <= 5:
        color = C_colors[i - 1]
    else:
        color = "white"
    for j in range(i, t + 2):
        for k in range(i + 1, t + 1):
            C += [((i, j, k), f"C{i}{j}{k}", color)]

##### nodes #####
G = Digraph(name="Graf Diekerta")
for coords, label, color in A + B + C:
    G.node(label, style="filled", fillcolor=color)

##### edges #####
# D1
for (i, j), A_label, _ in A:
    for (l, m, n), B_label, _ in B:
        if i == l and j == n:
            G.edge(A_label, B_label)

# D2
for (i, j, k), B_label, _ in B:
    for (l, m, n), C_label, _ in C:
        if i == l and j == m and k == n:
            G.edge(B_label, C_label)

# D3
for (i, j, k), C_label, _ in C:
    for (l, m), A_label, _ in A:
        if i == l - 1 and j == l and (k == l or k == m):
            G.edge(C_label, A_label)

# D4
for (i, j, k), C_label, _ in C:
    for (l, m, n), B_label, _ in B:
        if l != m and i == l - 1 and j == m and k == l:
            G.edge(C_label, B_label)

# D5
for (i, j, k), C1_label, _ in C:
    for (l, m, n), C2_label, _ in C:
        if l != m and i == l - 1 and j == m and k == n:
            G.edge(C1_label, C2_label)


G.save(filename="gauss_diekert.dot")
