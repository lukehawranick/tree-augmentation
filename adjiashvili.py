import networkx as nx

def adjiashvili(T,L, epsilon):

    gamma = 168/(epsilon**2)

    # TODO: find OPT(B) for each B = gamma-bundle (union of gamma-many paths)
        # split B into paths by splitting on vertices of deg >= 3
        # iterate over 3^k^2 possibilities of pathA-pathB links (k-many leaves on B)
            # finish covering each path with internal links (interval covering problem solvable in linear time)
            # obtain a count for this guess

        # OPT(B) is lowest count

    # TODO: formulate bundle LP = cut LP with additional OPT constraints

    # TODO: round the LP solution using the easy 3-step algo

def contract(tree, link, links):
    """
    Contract the edges covered by a given link.\n
    Options:\n 
             links - the link set to update. Possible to pass links: None\n
    """
    global root_node

    toContract = []

    # determine the set of edges covered by the link
    shadow = nx.shortest_path(tree, link[0], link[1])
    #print("link", link) if not_copy and update_map else None
    # perpare each edge for contraction
    for i in range(len(shadow)-1):
        toContract.append([shadow[i],shadow[i+1]]) if [shadow[i],shadow[i+1]] not in toContract and [shadow[i+1],shadow[i]] not in toContract else None

    # contract each edge covered by the link
    while(not len(toContract) == 0):

        # update the root node if necessary
        if toContract[0][1] == root_node:
            root_node = toContract[0][0]

        # contract the first edge in the list
        nx.contracted_edge(tree,tuple(toContract[0]),self_loops=False, copy=False)

        # add the tree edge to L for the sake of contracting to see the resulting link configuration
        if links is not None and tuple(toContract[0]) not in links.edges() and tuple(toContract[0])[::-1] not in links.edges():
            links.add_edge(*tuple(toContract[0]))
        nx.contracted_edge(links,tuple(toContract[0]),self_loops=False, copy=False) if links is not None else None

        # if any other edge in our list is adjacent to the "dest" node of contraction, re-index it
        for toFix in toContract[1:]:
            if(toFix[0] == toContract[0][1]):
                toFix[0] = toContract[0][0]
            elif(toFix[1] == toContract[0][1]):
                toFix[1] = toContract[0][0]

        # remove the edge we contracted from our list, 
        toContract.remove(toContract[0])