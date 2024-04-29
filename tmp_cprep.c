#line 1 "nagamochi.c"
#include <stdlib.h>
#include <stdio.h>
#include "../include/tree-helper.h"
#include "../include/graph.h"
#include <string.h>
#include "../include/list.h"
#include "../include/blossom.h"
#include "../include/lemma7.h"


void print_edge_ls_fn(void* el){
    edge* e = ((edge_ls*)el)->e;
    printf("(%i,%i)",e->thisVertex,e->otherVertex);
}


edge_ls* edge_ls_copy(edge_ls* el){
    edge_ls* ret = NULL;

    for(;el; el=el->next){
        ret = l_add(ret, edge_ls_create(el->e));
    }
    return ret;
}

void print_edge_ls(edge_ls* el){
    l_print(el,print_edge_ls_fn);
}

edge_ls* edge_ls_create(edge* e){
    edge_ls* nn = malloc(sizeof(edge_ls));
    nn->e = e;
    nn->next = NULL;
    nn->prev = NULL;

    return nn;
}


swing_ls* create_swing(graph* t, int up, int down, edge* e){
    swing_ls* swing = malloc(sizeof(swing_ls));

    swing->up = up;
    swing->down = down;
    swing->p_up = get_parent(t,up);
    swing->p_down = get_parent(t,down);

    swing->e = e;

    swing->is_solo_edge = 0;
    swing->in_lower = 0;

    swing->next = NULL;
    swing->prev = NULL;
    swing->binding_edges = NULL;

    return swing;
}


chain_ls* create_chain(int u, int u2, int uk){
    chain_ls* chain = malloc(sizeof(chain_ls));
    chain->next = NULL;
    chain->prev = NULL;

    chain->u = u;
    chain->u2 = u2;
    chain->uk = uk;

    chain->ua = 0;

    chain->swings = NULL;
    chain->binding_edges = NULL;
    chain->swing_edges = NULL;

    chain->e_p = NULL;
}


void p_ch(void* chain){
    chain_ls* chain_ls = chain;
    printf("(u:%i uk:%i ua:%i), ", chain_ls->u, chain_ls->uk, chain_ls->ua);
}

void print_chain(chain_ls* chain_ls){
    l_print(chain_ls, p_ch);
}

void p_swing(void* swings){
    swing_ls* swing = swings;
    printf("(up:%i down:%i p_up:%i p_down:%i)\n ",swing->up, swing->down, swing->p_up, swing->p_down);
    printf("\t\tbinding_edges: "); print_edge_ls(swing->binding_edges); if(swing->is_solo_edge)printf(" solo-edge!"); printf("\n");
}

void print_swings(swing_ls* swings){
    l_print(swings,p_swing);
}


void p_ch_n_s(void* chain){
    chain_ls* chain_ls = chain;
    printf("chain(u:%i uk:%i ua:%i uo:%i)\n", chain_ls->u, chain_ls->uk, chain_ls->ua, chain_ls->e_p->otherVertex);
    printf("\tbinding_edges: "); print_edge_ls(chain_ls->binding_edges); printf("\n");
    if(chain_ls->swings){
        printf("\tswings: "); print_swings(chain_ls->swings); printf("\n"); fflush(stdout);
    }
}

void print_chain_and_swings(chain_ls* chain_ls){
    l_print(chain_ls, p_ch_n_s);
}

int_ls* branches(graph* t, int v){
    int_ls* d = descendants(t,v);
    int_ls* b = NULL;

    for(int_ls* cd = d; cd; cd=cd->next){
        int u = value(t,cd->value);
        if( is_branch(t,u)){
            b = ls_add(b,u);
        }
    }
    ls_free(d);
    return b;
}

chain_ls* find_chains(graph* t, int v){
    v = value(t,v);

    chain_ls* chains = NULL;

    int_ls* brnches = branches(t,v);

    for(int_ls* b = brnches; b; b = b->next){
        int u = value(t,b->value);
        int_ls* queue = children(t,u);
        int_ls* end_que = ls_last(queue);

        for(int_ls* cur_que = queue; cur_que; cur_que = cur_que->next){
            int cv = value(t,cur_que->value);
            if(is_leaf(t,cv))
                continue;

            if(is_branch(t,cv) || is_fringe(t,cv)){
                int u2 = cv;
                while(u!=get_parent(t,u2 = get_parent(t,u2)) && u2 != t->root);
                chain_ls* new_chain = create_chain(u,u2,cv);
                chains = l_add(chains,new_chain);
            }else{
                int_ls* next = children(t,cv);
                end_que = ls_last(ls_merge(end_que,next));
            }
        }
        ls_free(queue);
    }
    return chains;
}

int next_in_chain(graph* t, int v){
    v = value(t,v);

    if(is_branch(t,v))
        return 0;

    int_ls* kids = children(t,v);

    v = 0;
    for(int_ls* kid = kids; kid; kid = kid->next ){
        v = value(t,kid->value);
        if(!is_leaf(t,v)){
            break;
        }
    }

    ls_free(kids);
    return v;
}

int_ls* immediate_thorns(graph* t, int v){
    int_ls* kids = children(t,v);

    int_ls* kid = kids;
    for(; kid; kid = kid->next){
        if(!is_leaf(t,kid->value)){
            kid = ls_remove(kid);
            break;
        }
    }
    return ls_first(kid);
}


edge* upper_edge(graph* g, graph* t, chain_ls* chain){
    int u = value(g,chain->u);
    int uk = value(g,chain->uk);

    int u2 = value(g,chain->u2);

    int_ls* d_u2 = descendants(t, u2);


    for(int v = uk; v != u; v = get_parent(t,v)){
        int_ls* thorns_and_v = NULL;
        if(v != uk)
            thorns_and_v = ls_merge(immediate_thorns(t,v),ls_add(NULL,v));
        else
            thorns_and_v = ls_add(NULL,v);

        for(int_ls* tv = thorns_and_v; tv; tv = tv->next){
            for(edge* e = g->vert[v]->edge; e ; e = e->next){
                int other_v = value(g,e->otherVertex);
                if(!ls_contains(d_u2,other_v)){
                    ls_free(d_u2);

                    fflush(stdout);
                    return e;
                }
            }
        }
        ls_free(thorns_and_v);
    }
    ls_free(d_u2);
    return NULL;
}

void find_swings(graph* g, graph* t, chain_ls* chain){
    int u = chain->u;
    int uk = chain->uk;

    int d_ua = get_depth(t,chain->ua);

    int higher = 0;
    int lower = 0;
    for(int cu = chain->u2; cu && cu!=uk; cu = next_in_chain(t,cu)){


        int_ls* thrns = immediate_thorns(t,cu);
        int num_thrns = ls_size(thrns);
        if(num_thrns > 2){
            higher = 0;
        }
        else if(num_thrns <= 0){

        }
        else if(num_thrns == 2){
            higher = 0;
            int thrn1 = value(g,thrns->value);
            int thrn2 = value(g,thrns->next->value);

            edge* e = find_edge(g,thrn1,thrn2);
            if(e){
                swing_ls* new_sw = create_swing(t,thrn1,thrn2,e);
                int d_higher = get_depth(t,thrn1);
                int d_lower = get_depth(t,thrn2);
                if(d_ua<d_higher-1){
                      new_sw->in_lower = 1;
                }
                if(get_depth(t,thrn1) < get_depth(t,chain->ua) && get_depth(t,thrn2) > get_depth(t,get_parent(t,uk))){
                        new_sw->in_lower = 1;
                    }
                chain->swings = l_add(chain->swings,new_sw);
                chain->swing_edges = l_add(chain->swing_edges,edge_ls_create(e));
            }
        }
        else{
            if(!higher){
                higher = value(g,thrns->value);
            }
            else{
                int current_thorn = value(g,thrns->value);
                edge* e = find_edge(g,higher,current_thorn);

                if(e){
                    swing_ls* new_sw = create_swing(t,higher,current_thorn,e);
                    int d_higher = get_depth(t,higher);
                    int d_lower = get_depth(t,current_thorn);
                    if(d_ua<d_higher-1){
                        new_sw->in_lower = 1;
                    }
                    chain->swings = l_add(chain->swings,new_sw);
                    chain->swing_edges = l_add(chain->swing_edges,edge_ls_create(e));
                    higher = 0;
                }
                else{
                    higher = current_thorn;
                }
            }
        }
        ls_free(thrns);
    }
}

void find_binding_edges(graph* g, graph* t, chain_ls* chain){

    int ua = chain->ua;
    int uk = chain->uk;
    int ua_next = next_in_chain(t,ua);
      ua_next = ua_next?ua_next: ua;
    int_ls* ua_to_uk = NULL;
    if(ua_next){
        ua_to_uk = tree_path(t,ua_next,get_parent(t,uk));
    }

    for(swing_ls* swing = chain->swings; swing; swing = swing->next){
        int upg = swing->p_up;
        int dng = swing->p_down;

        int up_in_lower = !!ls_contains(ua_to_uk,upg);
        int down_in_lower = !!ls_contains(ua_to_uk,dng);

        if(up_in_lower && down_in_lower){
            int_ls* w_candidates = thorns(t,chain->u2);
            int_ls* l_upg = leaves(t,upg);
            w_candidates = ls_remove_list(w_candidates,l_upg);
            int_ls* y_candidates = descendants(t,upg);

            for(int_ls* c_w = w_candidates; c_w; c_w = c_w->next){
                int w = value(g,c_w->value);
                for(edge* e = g->vert[w]->edge; e; e = e->next){
                    int y = value(g,e->otherVertex);
                    if(ls_contains(y_candidates,y)){
                        int_ls* zg_not = ls_add(ls_add(ls_add(NULL, w),swing->up),swing->down);


                        int_ls* p1 = tree_path(t,dng,y);
                        int_ls* p1_l = NULL;
                        for(int_ls* p1c = p1; p1c; p1c = p1c->next){
                            p1_l = ls_merge(p1_l,immediate_thorns(t,p1c->value));
                        }
                        p1_l = ls_remove_list(p1_l,zg_not);

                        if(p1_l){
                            swing->binding_edges = l_add(swing->binding_edges,edge_ls_create(e));
                            if(!l_contains(chain->binding_edges, edge_match, e) && swing->in_lower){
                                chain->binding_edges = l_add(swing->binding_edges,edge_ls_create(e));
                            }
                            ls_free(p1);
                            ls_free(p1_l);
                            ls_free(zg_not);
                            continue;
                        }

                        int_ls* p2 = tree_path(t,get_parent(t,w),upg);
                        int_ls* p2_l = NULL;
                        for(int_ls* p2c = p2; p2c; p2c = p2c->next){
                            p2_l = ls_merge(p2_l,immediate_thorns(t,p2c->value));
                        }
                        p2_l = ls_remove_list(p2_l,zg_not);
                        if(ls_size(p2_l) >= 2){
                            swing->binding_edges = l_add(swing->binding_edges,edge_ls_create(e));
                            if(!l_contains(chain->binding_edges, edge_match, e) && swing->in_lower){
                                chain->binding_edges = l_add(swing->binding_edges,edge_ls_create(e));
                            }
                            ls_free(p1);
                            ls_free(p1_l);
                            ls_free(zg_not);
                            ls_free(p2);
                            ls_free(p2_l);
                            continue;
                        }
                        else if(p2_l){

                            int l = value(g,p2_l->value);
                            if(!graph_is_edge(g,l,w)){
                                swing->binding_edges = l_add(swing->binding_edges,edge_ls_create(e));
                                if(!l_contains(chain->binding_edges, edge_match, e)){
                                    chain->binding_edges = l_add(swing->binding_edges,edge_ls_create(e));
                                }
                                ls_free(p1);
                                ls_free(p1_l);
                                ls_free(zg_not);
                                ls_free(p2);
                                ls_free(p2_l);
                                continue;
                            }
                        }
                        ls_free(p1);
                        ls_free(p1_l);
                        ls_free(zg_not);
                        ls_free(p2);
                        ls_free(p2_l);

                    }
                }

            }
            if(swing->binding_edges == NULL && swing->in_lower){
                swing->is_solo_edge = 1;
            }
            ls_free(w_candidates);
            ls_free(l_upg);
            ls_free(y_candidates);
        }
    }
    ls_free(ua_to_uk);
}

void process_chains(graph* g, graph* t, chain_ls* chains){

    for(chain_ls* ch = chains; ch; ch = ch->next){
        edge* up_edge = upper_edge(g,t,ch);
        if(up_edge){
            ch->e_p = up_edge;
            ch->ua = value(g,up_edge->thisVertex);
        }
        find_swings(g,t,ch);
        find_binding_edges(g,t,ch);
    }
}


void edge_ls_free(edge_ls* el){

}

void chain_ls_free(chain_ls* P){

}

edge_ls* E(graph* g, graph* t,int_ls* x){
   edge_ls* ret = NULL;

   for(int_ls* cx = x;cx; cx = cx->next){
      int u = value(g,cx->value);

      for(edge* e = g->vert[u]->edge; e; e = e->next){
         int v = value(g,e->otherVertex);

         if(!ls_contains(x,v)){
            ret = l_add(ret,edge_ls_create(e));
         }
      }
   }
   return ret;
}

edge* high(graph* g, graph* t, int_ls* x){

    edge* highest = NULL;

    edge_ls* es = E(g,t,x);

    int least_depth = __INT_MAX__;

    for(edge_ls* ec = es; ec; ec = ec->next ){
        edge* e = ec->e;



        int u = value(g,e->otherVertex);
        int v = value(g,e->thisVertex);

        int lc = lca(t,u,v);
        int d_lc = get_depth(t,lc);

        if(d_lc < least_depth){
            highest = e;
            least_depth = d_lc;;
        }
        else if(d_lc = least_depth){
            highest = e;
        }
    }

    l_free(es);
    return highest;
}

int case1(graph *g, graph *t)
{
    trim_all_duplicates(g);
    trim_all_duplicates(t);
    int ret = 0;
    int_ls *fringe = fringes(t, t->root);
    int_ls *cur_fringe = fringe;
    while (cur_fringe)
    {
        int v = value(t, cur_fringe->value);
        if (l_closed(g, t, v))
        {
            int_ls* des = leaves(t,v);

            edge_ls* e_retain = blossom_algorithm(g,des);
            ret = l_size(e_retain);

            edge_ls* cur_e = e_retain;

            for(edge_ls* cur_e = e_retain; cur_e; cur_e = cur_e->next){
                int u1 = cur_e->e->thisVertex;
                int u2 = cur_e->e->otherVertex;

                retain_merge_trim(g,t,cur_e->e->thisVertex, cur_e->e->otherVertex);
            }



            int_ls* remaining_des = descendants(t,v);
            int_ls* cur_des = remaining_des;


            while(cur_des){
                int u = value(g,cur_des->value);
                if(u == v){
                    cur_des = cur_des->next;
                    continue;
                }


                int u2 = g->vert[u]->edge->otherVertex;

                if(u != value(g,u2)){
                    retain_merge_trim(g,t,u,u2);
                }




                cur_des = cur_des->next;
            }

            ls_free(des);
        }
        cur_fringe = cur_fringe->next;
    }
    ls_free(fringe);


    return ret;
}

int case2(graph *g, graph *t)
{



    int_ls *fringe = fringes(t, t->root);
    int_ls *cur_fringe = fringe;

    int ret = 0;
    while (cur_fringe)
    {



        int parent = value(t, cur_fringe->value);
        if (!l_closed(g, t, parent))
        {


            int_ls *kids = children(t, parent);
            int_ls *cur_kid = kids;
            while (cur_kid)
            {


                int cur_v = value(g, cur_kid->value);
                int triv = trivial(g, t, cur_kid->value);
                if (triv && (cur_v != value(g, parent)))
                {
#line 565 "nagamochi.c"
                    ret++;
                    retain_merge_trim(g, t, cur_v, triv);


                }
                cur_kid = cur_kid->next;
            }
            ls_free(kids);
        }
        cur_fringe = cur_fringe->next;
    }
    ls_free(fringe);


    return ret;
}

int case3(graph *g, graph *t)
{
#line 596 "nagamochi.c"
    int_ls *fringe = fringes(t, t->root);

    int_ls *cur_fringe = fringe;
    int ret = 0;
    while (cur_fringe)
    {
        int parent = value(t, cur_fringe->value);
        if (!l_closed(g, t, parent))
        {

            int_ls *kids = children(t, parent);

            if (ls_size(kids) == 3)
            {

                int_ls *iso = isolated(g, t, parent);

                if (iso == NULL)
                {
                    int_ls *cur_kid = kids;
                    while (cur_kid)
                    {

                        if (trivial(g, t, cur_kid->value))
                            break;
                        cur_kid = cur_kid->next;
                    }
                    if (!cur_kid)
                    {

                        ret++;
                        int par_of_par = get_parent(t, parent);
                        int_ls* tp = tree_path(t, parent, par_of_par);
                        merge_list(g, tp);
                        merge_list(t, tp);
                        remove_self_edges(g, parent);
                        remove_self_edges(t, parent);
                    }
                }
                ls_free(iso);
            }
            ls_free(kids);
        }
        cur_fringe = cur_fringe->next;
    }
    ls_free(fringe);


    fflush(stdout);

    return ret;
}

int case4(graph *g, graph *t)
{
#line 664 "nagamochi.c"
    int u = t->root;

    int ret = 0;
    int_ls *fringe = fringes(t, u);

    int_ls *cur_fri = fringe;

    while (cur_fri)
    {

        int f = value(g, cur_fri->value);
        int_ls *kids = children(t, f);

        if (kids->next && !kids->next->next)
        {

            int u1 = kids->value;
            int u2 = kids->next->value;
            if (graph_is_edge(g, u1, u2))
            {

                int cur_parent = get_parent(t, f);
                int n;
                while (cur_parent)
                {

                    int_ls *cur_kids = children(t, cur_parent);
                    if (cur_kids->next && !cur_kids->next->next)
                    {

                        int v = cur_parent;
                        cur_parent = 0;


                        int u3 = is_leaf(t, cur_kids->value);
                        u3 = u3 ? u3 : is_leaf(t, cur_kids->next->value);

                        if (u3)
                        {
                            if (!l_closed(g, t, v))
                            {

                                int u1_u3 = graph_is_edge(g, u1, u3);
                                int u2_u3 = graph_is_edge(g, u2, u3);

                                int unconnected = (u1_u3? 0:u1);
                                unconnected = unconnected ? unconnected : (u2_u3?0:u2);

                                int unconnected_connects_outside = 0;


                                edge* e = g->vert[unconnected]->edge;
                                int_ls* tree_verts = descendants(t,v);


                                if(!(u1_u3 && u2_u3)){

                                    while(e){
                                        int cur_e = value(g,e->otherVertex);

                                        if(!ls_contains(tree_verts,cur_e)){

                                            unconnected_connects_outside = 1;

                                            break;
                                        }
                                        e = e->next;
                                    }

                                }

                                if ((u1_u3 && u2_u3) || unconnected_connects_outside)
                                {
                                    int prnt = get_parent(t,v);

                                    int_ls* tp = tree_path(t, v, prnt);
                                    merge_list(g, tp);
                                    merge_list(t, tp);
                                    remove_self_edges(g, prnt);
                                    remove_self_edges(t, prnt);
                                    ret = 1;
                                }
                                else{

                                    retain_merge_trim(g,t,u1, u2);
                                    ret = 1;
                                }
                                ls_free(tree_verts);
                            }
                        }
                    }
                    ls_free(cur_kids);
                    if (cur_parent)
                    {
                        int np = get_parent(t, cur_parent);
                        cur_parent = np == cur_parent ? 0 : np;
                    }
                }
            }
        }
        ls_free(kids);
        cur_fri = cur_fri->next;
    }
    ls_free(fringe);

   fflush(stdout);
    return ret;
}




edge_ls* prime_edges(graph* g, graph* t, int v){
   int u = value(g, v);


    edge_ls* prime_edges = NULL;

   int_ls *fringe = fringes(t, u);
   int_ls *cur_fri = fringes(t, u);

   while (cur_fri)
   {
      int f = value(g, cur_fri->value);
      int_ls *kids = children(t, f);

      if (kids->next && !kids->next->next)
      {

         int u1 = kids->value;
         int u2 = kids->next->value;
         edge* p1 = NULL;
         if (p1 = find_edge(g,u1,u2))
         {
            prime_edges = l_add(prime_edges, edge_ls_create(p1));

            int cur_parent = get_parent(t, f);
            int n;
            while (cur_parent)
            {

               int_ls *cur_kids = children(t, cur_parent);
               if (cur_kids->next && !cur_kids->next->next)
               {

                  int p_fringe_candidate = cur_parent;
                  cur_parent = 0;


                  int u3 = is_leaf(t, cur_kids->value);
                  u3 = u3 ? u3 : is_leaf(t, cur_kids->next->value);

                  if (u3)
                  {
                    edge* p21 = NULL;
                    edge* p22 = NULL;
                     if ((p21 = find_edge(g, u1, u3)) && (p22 = find_edge(g, u2, u3)))
                     {

                        int_ls *desc_v = descendants(t, p_fringe_candidate);

                        edge *e = g->vert[u1]->edge;

                        int u1_or_u2_bad_edge = 0;

                        while (e)
                        {
                           int other_v = value(g, e->otherVertex);
                           void *c = ls_contains(desc_v, other_v);
                           if (!c)
                           {
                              u1_or_u2_bad_edge = 1;
                              break;
                           }
                           e = e->next;
                        }

                        e = g->vert[u2]->edge;

                        while (e)
                        {
                           int other_v = value(g, e->otherVertex);
                           void *c = ls_contains(desc_v, other_v);
                           if (!c)
                           {
                              u1_or_u2_bad_edge = 1;
                              break;
                           }
                           e = e->next;
                        }
                        if (!u1_or_u2_bad_edge)
                        {
                            prime_edges = l_add(prime_edges, edge_ls_create(p21));
                            prime_edges = l_add(prime_edges, edge_ls_create(p22));
                        }
                        ls_free(desc_v);
                     }
                  }
               }
               ls_free(cur_kids);
               if (cur_parent)
               {
                  int np = get_parent(t, cur_parent);
                  cur_parent = np == cur_parent ? 0 : np;
               }
            }
         }
      }
      ls_free(kids);
      cur_fri = cur_fri->next;
   }
   ls_free(fringe);
   return prime_edges;
}

edge_ls* prime_edges_type1(graph* g, graph* t, int v){
       int u = value(g, v);


    edge_ls* prime_edges = NULL;

   int_ls *fringe = fringes(t, u);
   int_ls *cur_fri = fringes(t, u);

   while (cur_fri)
   {
      int f = value(g, cur_fri->value);
      int_ls *kids = children(t, f);

      if (kids->next && !kids->next->next)
      {

         int u1 = kids->value;
         int u2 = kids->next->value;
         edge* p1 = NULL;
         if (p1 = find_edge(g,u1,u2))
         {
            prime_edges = l_add(prime_edges, edge_ls_create(p1));
         }
      }
      ls_free(kids);
      cur_fri = cur_fri->next;
   }
   ls_free(fringe);
   return prime_edges;
}

edge_ls* prime_edges_type2(graph* g, graph* t, int v){
       int u = value(g, v);


    edge_ls* prime_edges = NULL;

   int_ls *fringe = fringes(t, u);
   int_ls *cur_fri = fringes(t, u);

   while (cur_fri)
   {
      int f = value(g, cur_fri->value);
      int_ls *kids = children(t, f);

      if (kids->next && !kids->next->next)
      {

         int u1 = kids->value;
         int u2 = kids->next->value;
         edge* p1 = NULL;
         if (p1 = find_edge(g,u1,u2))
         {


            int cur_parent = get_parent(t, f);
            int n;
            while (cur_parent)
            {

               int_ls *cur_kids = children(t, cur_parent);
               if (cur_kids->next && !cur_kids->next->next)
               {

                  int p_fringe_candidate = cur_parent;
                  cur_parent = 0;


                  int u3 = is_leaf(t, cur_kids->value);
                  u3 = u3 ? u3 : is_leaf(t, cur_kids->next->value);

                  if (u3)
                  {
                    edge* p21 = NULL;
                    edge* p22 = NULL;
                     if ((p21 = find_edge(g, u1, u3)) && (p22 = find_edge(g, u2, u3)))
                     {

                        int_ls *desc_v = descendants(t, p_fringe_candidate);

                        edge *e = g->vert[u1]->edge;

                        int u1_or_u2_bad_edge = 0;

                        while (e)
                        {
                           int other_v = value(g, e->otherVertex);
                           void *c = ls_contains(desc_v, other_v);
                           if (!c)
                           {
                              u1_or_u2_bad_edge = 1;
                              break;
                           }
                           e = e->next;
                        }

                        e = g->vert[u2]->edge;

                        while (e)
                        {
                           int other_v = value(g, e->otherVertex);
                           void *c = ls_contains(desc_v, other_v);
                           if (!c)
                           {
                              u1_or_u2_bad_edge = 1;
                              break;
                           }
                           e = e->next;
                        }
                        if (!u1_or_u2_bad_edge)
                        {
                            prime_edges = l_add(prime_edges, edge_ls_create(p21));
                            prime_edges = l_add(prime_edges, edge_ls_create(p22));
                        }
                        ls_free(desc_v);
                     }
                  }
               }
               ls_free(cur_kids);
               if (cur_parent)
               {
                  int np = get_parent(t, cur_parent);
                  cur_parent = np == cur_parent ? 0 : np;
               }
            }
         }
      }
      ls_free(kids);
      cur_fri = cur_fri->next;
   }
   ls_free(fringe);
   return prime_edges;
}

edge_ls* leaf_edges(graph* g, graph* t, int v){
    v = value(t,v);

    edge_ls* leaf_edges = NULL;

    int_ls* leafs = leaves(t,v);

    for(int_ls* leaf = leafs; leaf; leaf = leaf->next){
        int cur_lf = value(g,leaf->value);

        int_ls* parent_ls = NULL;

        for(int cur_v = get_parent(t,cur_lf); cur_v != get_parent(t,v); cur_v = get_parent(t,cur_v)){
            parent_ls = ls_add(parent_ls,cur_v);
        }

        for(edge* e = g->vert[cur_lf]->edge; e ; e=e->next){
            int other_v = value(g,e->otherVertex);

            if(ls_contains(parent_ls,other_v)){
                leaf_edges = l_add(leaf_edges, edge_ls_create(e));
            }
        }
        ls_free(parent_ls);
    }

    ls_free(leafs);
    return leaf_edges;
}

edge_ls* leaf_to_leaf_edges(graph* g, graph*t, int v){
    v = value(t,v);

    edge_ls* leaf_edges = NULL;

    int_ls* leafs = leaves(t,v);

    for(int_ls* leaf = leafs; leaf; leaf = leaf->next){
        int cur_lf = value(g,leaf->value);

        for(edge* e = g->vert[cur_lf]->edge; e ; e=e->next){
            int other_v = value(g,e->otherVertex);

            if(is_leaf(t,other_v) && !edge_ls_contains(leaf_edges,e)){

                leaf_edges = l_add(leaf_edges, edge_ls_create(e));
            }
        }
    }
    ls_free(leafs);

    return leaf_edges;
}




void COVER(graph* g, graph* t, int v, chain_ls* P){

    edge_ls* F_leaf = leaf_edges(g,t,v);

    edge_ls* E_leaf = leaf_to_leaf_edges(g,t,v);


    edge_ls* E_prime1 = prime_edges_type1(g,t,v);

    edge_ls* E_prime2 = prime_edges_type2(g,t,v);

    edge_ls* E_prime = l_merge(edge_ls_copy(E_prime1), edge_ls_copy(E_prime2));
#line 1091 "nagamochi.c"
    edge_ls* E_bind = NULL;
    edge_ls* E_upper = NULL;
    edge_ls* E_swing = NULL;

    for(chain_ls* chain = P; chain; chain = chain->next){
        if(chain->binding_edges){
            E_bind = l_add(E_bind,edge_ls_create(chain->binding_edges->e));
        }
        if(chain->e_p){
            E_upper = l_add(E_upper,edge_ls_create(chain->e_p));
        }
    }



        int vi;
#line 1112 "nagamochi.c"
        int_ls* vs = leaves(t,v);
#line 1118 "nagamochi.c"
        edge_ls* e_mat = l_remove_ls(E_leaf,edge_ls_match,E_prime);

        edge_ls* m_star = blossom_algorithm2(g,vs,e_mat);

        int_ls* matched_v = NULL;

        for(edge_ls* m = m_star; m; m=m->next){
            int u1 = value(g,m->e->thisVertex);
            int u2 = value(g,m->e->otherVertex);

            matched_v = ls_add(matched_v,u1);
            matched_v = ls_add(matched_v,u2);
        }


        int_ls* unmatched_v = ls_copy(vs); unmatched_v = ls_remove_list(unmatched_v,matched_v);

        edge_ls* M1_s = NULL;
        edge_ls* M2_s = NULL;

        int_ls* newly_matched_v = NULL;


        for(edge_ls* e1 = E_prime1; e1; e1 = e1->next){
            int u1 = value(g,e1->e->thisVertex);
            int u2 = value(g,e1->e->otherVertex);

            int_ls* uu = ls_add(NULL,u1);
            uu = ls_add(uu,u2);

            if(ls_contains_any(unmatched_v,uu)){
                M1_s = l_add(M1_s, edge_ls_create(e1->e));
                newly_matched_v = ls_add(newly_matched_v,u1);
                newly_matched_v = ls_add(newly_matched_v,u2);
            }
            ls_free(uu);
        }

        for(edge_ls* e2 = E_prime2; e2; e2 = e2->next){
            int u1 = value(g,e2->e->thisVertex);
            int u2 = value(g,e2->e->otherVertex);

            int_ls* uu = ls_add(NULL,u1);
            uu = ls_add(uu,u2);

            if(ls_contains_any(unmatched_v,uu)){
                M2_s = l_add(M1_s, edge_ls_create(e2->e));
                newly_matched_v = ls_add(newly_matched_v,u1);
                newly_matched_v = ls_add(newly_matched_v,u2);
            }

            ls_free(uu);
        }



        edge_ls* m2_d = NULL;
        for(edge_ls* m = M1_s; m; m = m->next){
            m2_d = l_add(m2_d, l_contains(M2_s,edge_match_one, m->e));

            int u1 = value(g,m->e->thisVertex);
            int u2 = value(g,m->e->otherVertex);
        }

        unmatched_v = ls_remove_list(unmatched_v,newly_matched_v);


        int_ls* Wmmm = ls_copy(unmatched_v);
        Wmmm = ls_remove_list(Wmmm,newly_matched_v);

        edge_ls* ew = NULL;


        for(int_ls* wl = Wmmm; wl; wl = wl->next){
            int w = value(g,wl->value);

            int continue_flag = 0;


            edge* e = edge_create(w,w);
            edge_ls* potential_ew = E_bind;
            edge* cur_highest_ew = NULL;

            while(potential_ew = l_contains(potential_ew,edge_match_one,e)){
                int u1 = value(g,potential_ew->e->thisVertex);
                int u2 = value(g,potential_ew->e->otherVertex);

                int y = u1 ^ u2 ^ w;

                if(get_depth(t,y) < get_depth(t,get_parent(t,w))){
                    if(!cur_highest_ew){
                        cur_highest_ew = potential_ew->e;
                    }
                    else{
                        int ux1 = value(g,e->thisVertex);
                        int ux2 = value(g,e->otherVertex);
                        int y2 = w ^ ux1 ^ ux2;

                        if(get_depth(t,y) < get_depth(t,y2)){
                            cur_highest_ew = potential_ew->e;
                        }
                    }
                }
                potential_ew = potential_ew->next;
            }


            if(cur_highest_ew){
                free(e);
                ew = l_add(ew,edge_ls_create(cur_highest_ew));
                continue;
            }




            edge* new_ew = NULL;

            for(chain_ls* ch = P; ch; ch = ch->next){
                edge_ls* sw = ch->swing_edges;
                while(sw = l_contains(sw,edge_match_one,e)){
                    int u1 = value(g,sw->e->thisVertex);
                    int u2 = value(g,sw->e->otherVertex);

                    if(u2 == w){
                        int t = u2;
                        u2 = u1;
                        u1 = t;
                    }

                    if(get_depth(t,u1) <= get_depth(t,u2)){

                        new_ew = sw->e;

                    }
                    sw = sw->next;
                }
            }

            if(new_ew){
                free(e);
                ew = l_add(ew,edge_ls_create(new_ew));
                continue;
            }



            int_ls* ww = ls_add(NULL,w);

            new_ew = high(g,t,ww);
            free(e);
            ls_free(ww);
            ew = l_add(ew,edge_ls_create(new_ew));
        }



        edge_ls* p4_eg = NULL;

        for(edge_ls* cg = l_first(M1_s); cg; cg = cg->next){

            edge_ls* p2 = NULL;
            if(p2 = l_contains(M2_s,edge_match_one,cg->e)){
                int w = value(g,p2->e->thisVertex);
                int wd = value(g,p2->e->otherVertex);

                if(get_depth(t,wd) > get_depth(t,w)){
                    int t = w;
                    w = wd;
                    wd = t;
                }

                int_ls* desc = descendants(t,w);

                edge* new_eg = high(g,t,desc);
                ls_free(desc);
                p4_eg = l_add(p4_eg,edge_ls_create(new_eg));
            }
            else{
                int u = value(g,cg->e->thisVertex);
                int ud = value(g,cg->e->otherVertex);
                int pu = get_parent(t,u);
                int_ls* uupu = ls_add(NULL,u);
                uupu = ls_add(uupu,ud);
                uupu = ls_add(uupu,pu);

                edge* new_eg = high(g,t,uupu);
                ls_free(uupu);
                p4_eg = l_add(p4_eg,edge_ls_create(new_eg));
            }
        }

        edge_ls* xx[4] = {M1_s, m2_d, ew, p4_eg};
        for(int ppp = 0; ppp<4; ppp++){

            for(edge_ls* eee = xx[ppp]; eee; eee = eee->next){

            }
        }

        edge_ls* E1 = l_merge(M1_s,m2_d);
        E1 = l_merge(E1,l_merge(ew,p4_eg));

        for(edge_ls* ec = E1; ec; ec = ec->next){
#line 1326 "nagamochi.c"
            retain_merge_trim(g,t,ec->e->thisVertex, ec->e->otherVertex);
        }
#line 1334 "nagamochi.c"
        for(chain_ls* ch = P; ch; ch = ch->next){
            for(swing_ls* sw = ch->swings; sw; sw = sw->next){


                int sw_v = value(g,sw->e->thisVertex);
                int matches = 0;

                for(int_ls* m = matched_v; m; m = m->next){
                    int u = value(g,m->value);
                    if(u == sw_v){
                        matches++;
                    }
                }

                if(matches == 2){
                    edge* ret_e = sw->in_lower ? sw->binding_edges->e : ch->e_p;
                    int u = value(g,ret_e->thisVertex);
                    int v2 = value(g,ret_e->otherVertex);

                    int_ls* tp = tree_path(t,u,v2);

                    int components = 0;

                    int_ls* temp = NULL;
                    for(int_ls* t = tp; t; t = t->next){
                        for(int_ls* m = matched_v; m; m = m->next){
                            int cur_m = value(g,m->value);
                            if(cur_m == tp->value){
                                components++;
                                break;
                            }
                        }
                    }

                    if(components >= 3){
                        retain_merge_trim(g,t,u,v2);
                    }
                }
            }
        }




        for(chain_ls* ch = P; ch; ch = ch->next){
            if(!ch->e_p)
                continue;

            int x = ch->ua;

            int ua2 = value(g,ch->e_p->otherVertex);
            int ua1 = value(g,ch->e_p->thisVertex);

            if(ua1 == ua2)
                continue;

            int v1_matched = !!ls_contains(matched_v,ch->u);

            int num_upward = 0;
            int any_in_matching = 0;

            for(swing_ls* sw = ch->swings; sw; sw = sw->next){
                if(!sw->in_lower){
                    num_upward++;
                    int u1 = value(g,sw->e->thisVertex);
                    int u2 = value(g,sw->e->otherVertex);
                    if(ls_contains_2(matched_v,u1,u2)){
                        any_in_matching++;
                    }
                }
            }

            if(any_in_matching && num_upward >=2 || v1_matched && num_upward){


                edge_ls* uppers = NULL;

                for(chain_ls* chc = P; chc; chc = chc->next){
                    if(chc == ch)
                        continue;

                    int uhc1 = value(g,chc->e_p->thisVertex);
                    int uhc2 = value(g,chc->e_p->otherVertex);

                    if(uhc1 == uhc2)
                        continue;

                    int v1_matched_c = !!ls_contains(matched_v,chc->u);

                    int num_upward_c = 0;

                    for(swing_ls* sw = ch->swings; sw; sw = sw->next){
                        if(!sw->in_lower){
                            num_upward++;
                        }
                    }
                    if(num_upward>=2)
                        uppers = l_add(uppers,edge_ls_create(chc->e_p));

                }

                if(uppers){
                    uppers = l_add(uppers,edge_ls_create(ch->e_p));

                    for(edge_ls* ue = uppers; ue; ue = ue->next){
                        int v1 = value(g,ue->e->thisVertex);
                        int v2 = value(g,ue->e->otherVertex);

                        retain_merge_trim(g,t,v1,v2);
                    }

                    l_free(uppers);
                }

            }

        }





        int_ls* cur_desc = descendants(t,v);
        cur_desc = ls_first(ls_remove(ls_contains(cur_desc,v)));
        int_ls* lf_p = ls_merge(leaves(t,v),pseudo_fringes(g,t,v));

        cur_desc = ls_remove_list(cur_desc,lf_p);

        for(int_ls* cd = cur_desc; cd; cd = cd->next){
            int u = value(g,cd->value);
            int_ls* ul = ls_add(NULL,u);
            edge* he = high(g,t,ul);
            int u1 = value(g,he->thisVertex);
            int u2 = value(g,he->otherVertex);

            int uo = u ^ u1 ^ u2;

            retain_merge_trim(g,t,u,uo);

            ls_free(ul);
        }



        l_free(F_leaf);
        l_free(E_leaf);
        l_free(E_prime1);
        l_free(E_prime2);
        l_free(E_prime);

        ls_free(vs);
        l_free(m_star);
        ls_free(matched_v);
        ls_free(unmatched_v);
        l_free(M2_s);
        ls_free(newly_matched_v);
        ls_free(Wmmm);
        l_free(E1);
        ls_free(cur_desc);
        ls_free(lf_p);
}


int A3(graph* g, graph* t, chain_ls* P){
    for(chain_ls* ch = P; ch; ch=ch->next){
        for(swing_ls* sw = ch->swings; sw; sw = sw->next){
            if(sw->is_solo_edge){
                return 0;
            }
        }
    }
    return 1;
}


void lemma9(graph* g, graph* t, int v, chain_ls* P, double epsilon){

    int u_j = 0;
    edge* low_solo = NULL;
    int max_depth = 0;
    chain_ls* deep_chain = NULL;

    for(chain_ls* ch = P; ch; ch = ch->next){
        for(swing_ls* sw = ch->swings; sw; sw = sw->next){
            if(sw->is_solo_edge){
                int cur_depth = get_depth(t,sw->up);
                if(cur_depth > max_depth){
                    max_depth = cur_depth;
                    u_j = sw->up;
                    deep_chain = ch;
                    low_solo = sw->e;
                }
            }
        }
    }

    if(deep_chain == NULL){
        printf("you shouldnt have called lemma9\n");
        fflush(stdout);
    }

    double l = (4/epsilon) - 1;
    int_ls* leafs = leaves(t,u_j);
    int n_leaves = ls_size(leafs);

    if(n_leaves >= l){
        chain_ls* ch_u = find_chains(t,deep_chain->u);
        process_chains(g,t,ch_u);
        COVER(g,t,u_j,ch_u);
    }
    else{
        lemma7(g,t,u_j,epsilon);
    }



    u_j = value(t,u_j);

    int_ls* d = descendants(t,u_j);
    printf("LEMMA9: %i desc after reduction\n", l_size(d));



    edge* e_star = NULL;
    int w_star = 0;


    for(int u = get_parent(g,u_j); ;u = get_parent(t,u)){
        edge* e = find_edge(g,u,u_j);
        if(e){
            e_star = e;
            w_star = u;
        }
        if(value(g,u)== value(g,v) || u == value(t,t->root)){
            break;
        }
    }


    int z1 = 0;
    int z2 = 0;

    int z1_d = __INT_MAX__;
    int z2_d = __INT_MAX__;

    int_ls* th = thorns(t,v);

    for(int_ls* cur_th; cur_th; cur_th = cur_th->next){
        int th_v = value(g, cur_th->value);
        int dep = get_depth(t,th_v);
        if(dep < z1_d){
            z1 = th_v;
            z1_d = dep;
        }
    }
    for(int_ls* cur_th; cur_th; cur_th = cur_th->next){
        int th_v = value(g, cur_th->value);
        int dep = get_depth(t,th_v);
        if(dep < z2_d && z1 != th_v && th_v != u_j){
            z2 = th_v;
            z2_d = dep;
        }
    }


    if(!z2){
        if(e_star){
            retain_merge_trim(g, t, e_star->thisVertex, e_star->otherVertex);
        }
        retain_merge_trim(g, t, z1, u_j);
    }
    else if( get_depth(t, w_star) <= get_depth(t, z2)-1){
        retain_merge_trim(g, t, e_star->thisVertex, e_star->otherVertex);
        retain_merge_trim(g, t, z1, z2);
    }
    else{
        retain_merge_trim(g,t,z1, u_j);
        retain_merge_trim(g, t, z1, z2);
    }

}


edge* nagamochi(graph* g, graph* t, double epsilon)
{
    set_gm(g);
    set_tm(t);
    if(!t->root){
        set_root(t, 1);
    }

    while(l_size(children(t, t->root)) > 0){
        int any_cases = 1;
        while(any_cases){
            any_cases = 0;
            int c = 0;
            while(c = case1(g, t)){any_cases |= c;}
            while(c = case2(g, t)){any_cases |= c;}
            while(c = case3(g, t)){any_cases |= c;}
            while(c = case4(g, t)){any_cases |= c;}
        }

        trim_all_duplicates(g);
        trim_all_duplicates(t);

        int_ls* mlfc = minimally_lf_closed(g,t,t->root);

        if(mlfc){
            chain_ls* P = find_chains(t,mlfc->value);
            process_chains(g,t,P);

            if(A3(g,t,P)){
                COVER(g,t,mlfc->value, P);
            }
            else{
                lemma9(g,t,mlfc->value,P,epsilon);
            }
            chain_ls_free(P);
            ls_free(mlfc);
        }
        trim_all_duplicates(g);
        trim_all_duplicates(t);
    }
    return t->vert[0]->edge;
}
