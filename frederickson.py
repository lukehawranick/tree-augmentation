import networkx as nx
import matplotlib.pyplot as plt
import treegenerator as tg
import random
import numpy as np
import time

def frederickson(T, L):
    S = T.copy()

    r = random.choice([r for r in T.nodes() if T.degree(r) == 1])

    diT = nx.reverse(nx.bfs_tree(T, r))

    A = nx.DiGraph()
    for (u, v) in L.edges():
        A.add_edge(u, v, weight=1)
        A.add_edge(v, u, weight=1)

    for (u, v) in diT.edges():
        A.add_edge(u, v, weight=0)
        A.add_edge(v, u, weight=A.number_of_nodes() + 1)

    for u in A.nodes():
        if A.has_edge(u, r):
            A.remove_edge(u, r)

    Arb = nx.minimum_spanning_arborescence(A)

    for (u, v) in Arb.edges():
        S.add_edge(u, v)

    return len(S.edges) - len(T.edges)


def main():
    T = tg.random_tree(10)
    L = tg.generate_links(T, 0.5)
    st = time.time()
    linksAdded = frederickson(T, L)
    et = time.time()
    print("Frederickson:")
    print(f"Links Added: {linksAdded}, Time: {et-st}")


if __name__ == '__main__':
    main()
