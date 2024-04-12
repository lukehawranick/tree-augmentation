import pulp as lp
import networkx as nx
import time
import datetime
import tracemalloc
import treegenerator as tg

def cutlp(T, L):
    tree = T.copy()
    links = L.copy()
    link_list = list(links.edges)
    edge_list = list(tree.edges)
    paths = {(i, j): nx.shortest_path(tree, i, j) for (i, j) in link_list}
    cover = {(i, j): [(u, v) for (u, v) in link_list if i in paths[(u, v)] and j in paths[(u, v)]] for (i, j) in edge_list}

    model = lp.LpProblem(name="tap-problem", sense=lp.LpMinimize)

    # decision variables
    X = {(i, j): lp.LpVariable(name=f"x_({i},{j})", cat="Integer", lowBound=0, upBound=1) for (i, j) in link_list}

    # objective function
    model += lp.lpSum(X.values())

    # constraints
    for (i, j) in edge_list:
        model += (lp.lpSum([x for (u, v), x in X.items() if (u, v) in cover[(i, j)]]) >= 1, f"e_({i},{j})")

    status = model.solve(lp.CPLEX_CMD(msg=False, timeLimit=1800))

    links_added = 0
    for (u, v), x in X.items():
        if x.value() == 1:
            links_added = links_added + 1

    return links_added


def main():
    file = open("exact.txt", "w")
    file.write(f"{datetime.datetime.now()}\n")
    file.write(f"test, size, density, tree, edges, time, memory\n")

    sizes = [10]
    densities = [0.1, 0.5, 0.8]
    trees = ["path", "star", "starlike", "caterpillar", "lobster", "random"]
    treeFunc = [tg.path_tree, tg.star_tree, tg.starlike_tree, tg.caterpillar_tree, tg.lobster_tree, tg.random_tree]
    iterations = range(3)
    for s in sizes:
        for d in densities:
            for idx, tree in enumerate(trees):
                for i in iterations:
                    T = treeFunc[idx](s)
                    L = tg.generate_links(T, d)

                    st = time.time()
                    tracemalloc.start()
                    exact_edges = cutlp(T, L)
                    exact_time = time.time() - st
                    current, exact_mem = tracemalloc.get_traced_memory()

                    file.write(f"{i+1}, {s}, {d}, {tree}, {exact_edges}, {exact_time}, {exact_mem}\n")

    file.close()


if __name__ == "__main__":
    main()