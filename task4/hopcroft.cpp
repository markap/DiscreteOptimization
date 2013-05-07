//
//
// Animation of Hopcraft-Karp algortihm to find a maximum matching in bipartite graphs
//
// -> all nodes are depicted in yellow by default
// -> at the beginning of each dfs or bfs, nodes are displayed in orange, edges in black
// -> free nodes are depicted with a bigger border width
// -> matched edges are illustrated with a bigger width
//
// -> bfs: nodes within the queue and its edges are red
// -> bfs: current node and current edge is green
// -> bfs: completed nodes and edges are blue
//
// -> dfs: completed nodes and edges are blue
// -> dfs: current edge is green
//
// -> all matched edges and all nodes are depicted in violet at the end
//

#include <iostream>
#include <math.h>
#include <climits>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/color.h>
#include <LEDA/system/basic.h>
#include <LEDA/core/queue.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/node_map.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/list.h>

#include "control.h" // Control window (adjusting speed etc.)

#define WAIT 0.7

#define p(str) ( std::cout << str << std::endl ) // print helper

using leda::graph;
using leda::node;
using leda::edge;
using leda::node_array;
using leda::user_label;
using leda::GraphWin;
using leda::red;
using leda::blue;
using leda::orange;
using leda::green;
using leda::yellow;
using leda::black;
using leda::violet;
using leda::string;
using leda::queue;
using leda::list;
using leda::node_pq;
using leda::node_map;
using leda::edge_array;
using leda::node_partition;
using std::numeric_limits;


// computational upper bound
// 
// parameters:
//   int maximum_matching: the maximum matching for the current bipartite graph
//   int current_matching: the current matching
double upper_bound(int maximum_matching, int current_matching) {

    double x = current_matching / (maximum_matching - current_matching);
    double y = floor(x);
    return (2 * y) + 1;
}


// the depth first search
// parameters:
//   GraphWin gw: the window as reference
//   graph g: the graph as a reference
//   node v: current node
//   int round: a flag to find out if v is a element of set 1 or set 2
//   node_array<int> level: the level-node-relation
//   node_array<int> free: stores whether node is free
//   edge<int> matching: stores whether edge is matched
//   node parent: the parent of v
//   int current_matching: the current matching
int dfs(GraphWin &gw, graph &g, node &v, int round, node_array<int> &level, node_array<int> &free, edge_array<int> &matching, node &parent, int &current_matching) {

    int next_round = round + 1;

    int invert;  // stores whether we should invert the path or not (boolean)

    // current node is blue
    gw.set_color(v, blue);
    control_wait(WAIT);

    // check all edges of v
    edge e;
    forall_inout_edges(e, v) {
        node opposite_node = g.opposite(v, e);
        // dont check parent node
        if (opposite_node == parent) {
            p("dont check parent");
            continue;
        }
        gw.set_color(e, green);
        control_wait(WAIT);

        if ((round % 2) == 0) { // node v is from vertex set 1
            if (matching[e] == 0) { // unmatched
                if (level[opposite_node] == level[v] + 1) { // level must be greater 1

                    gw.set_color(e, blue);
                    gw.set_color(opposite_node, blue);
                    control_wait(WAIT);

                    // check for augmented path
                    if (free[opposite_node] == 1) {
                        level[opposite_node] = -1;
                        gw.set_color(opposite_node, blue);
                        gw.set_user_label(opposite_node, string("%d", level[opposite_node]));

                        level[v] = -1;
                        gw.set_user_label(v, string("%d", level[v]));
                        
                        // invert path
                        if (matching[e] == 0) {
                            gw.set_color(e, blue);
                            free[opposite_node] = 0;
                            gw.set_border_width(opposite_node, 1);
                            free[v] = 0;
                            gw.set_width(e, 10);
                            gw.set_border_width(v, 1);
                            matching[e] = 1;
                            current_matching++;
                        } else {
                            matching[e] = 0; 
                            gw.set_width(e, 2);
                            current_matching--;
                        }
                        control_wait(WAIT);
                        return 1;
                    } else { // no augmented path yet
                        // call dfs for next node
                        invert = dfs(gw, g, opposite_node, next_round, level, free, matching, v, current_matching);
                        if (invert == 1) { // invert path
                            if (matching[e] == 0) {
                                free[opposite_node] = 0;

                                gw.set_border_width(opposite_node, 1);
                                gw.set_width(e, 10);
                                free[v] = 0;
                                gw.set_border_width(v, 1);
                                matching[e] = 1;
                                current_matching++;
                            } else {
                                gw.set_width(e, 1);
                                matching[e] = 0; 
                                current_matching--;
                            }
                            
                            level[v] = -1;
                            gw.set_user_label(v, string("%d", level[v]));
                            control_wait(WAIT);
                            return 1;
   
                        }
                    }
                }

            }
        } else { // v is a element of vertex set 2
            if (matching[e] == 1) { // matched
                if (level[opposite_node] == level[v] + 1) { // level must be greater 1

                    gw.set_color(e, blue);
                    gw.set_color(opposite_node, blue);
                    control_wait(WAIT);

                    // call dfs for opposite_node
                    invert = dfs(gw, g, opposite_node, next_round, level, free, matching, v, current_matching); }

                    if (invert == 1) {
                        // invert path
                        if (matching[e] == 0) {
                            free[opposite_node] = 0;

                            gw.set_border_width(opposite_node, 5);
                            free[v] = 0;
                            gw.set_border_width(v, 10);
                            gw.set_width(e, 5);
                            matching[e] = 1;
                            current_matching++;
                        } else {
                            matching[e] = 0; 
                            gw.set_width(e, 1);
                            current_matching--;
                        }
                        level[v] = -1;
                        gw.set_user_label(v, string("%d", level[v]));
                        control_wait(WAIT);
                        return 1;
   
                 } 
            }

        }
    }

    level[v] = -1;
    gw.set_user_label(v, string("%d", level[v]));
    return 0;

}


// the main function of the hopcroft karp algorithm
// executes the simultaneous bfs and the recursive dfs
//
// parameters:
//   graph g: the graph as reference
//   GraphWin gw: the window as reference
//
void hopcroft(graph &g, GraphWin &gw) {

    list<node> v_set_1; // vertex set 1
    list<node> v_set_2; // vertex set 2

    node_array<int> level(g, -1); // init every level with -1 at start
    edge_array<int> matching(g, 0); // in the beginning all edges are unmatched
    node_array<int> free(g, 1); // in the beginning all nodes are free
    node_array<edge> bfs_edge_to_node(g, NULL); // stores the relation edge-node for bfs
    node_array<node> bfs_from(g, NULL); // stores the bfs from node for bfs

    queue<node> fifo_queue;

    int nodes_count = 0; // counts all nodes of the graph
    
    // split up the nodes into vertex set 1 and set 2 depending on the user label
    node n;
    forall_nodes(n, g) { 
        string s = gw.get_user_label(n);
        if (s == string("1")) {
            v_set_1.append(n); 
        } else if (s == string("2")) {
            v_set_2.append(n);
        }

        gw.set_user_label(n, string("-1"));
        gw.set_border_width(n, 5);
        nodes_count++;
    }

    int maximum_matching = nodes_count / 2;
    int current_matching = 0;
    int upper_b; // the upper bound of a shortest augmenting path

    control_wait(WAIT);
    int set_index = 0; // determines whether the current node in the bfs is from vertex set 1 or 2

    while (true) { // the outer while loop

        // start a new bfs round
        if (fifo_queue.empty()) {
            p("max");
            p(maximum_matching);
            p("current");
            p(current_matching);
            if (maximum_matching == current_matching) {
                p("early termination");
                break;
            }
            upper_b = upper_bound(maximum_matching, current_matching);
            gw.message(string("Upper bound: %d - Breadth First Search", upper_b));

            // init
            level.use_node_data(g, -1);
            bfs_from.use_node_data(g, NULL);
            bfs_from.use_node_data(g, NULL);

            // just for animation
            forall_nodes(n, g) {
                gw.set_color(n, orange);
                gw.set_user_label(n, string("%d", level[n]));
            }
            edge ed;
            forall_edges(ed, g) {
                gw.set_color(ed, black);
            }
            control_wait(WAIT);

            // add all free nodes from vertex set 1 to the queue
            forall(n, v_set_1) {
                if (free[n] == 1) {
                    fifo_queue.append(n);
                    level[n] = 0;
                    gw.set_user_label(n, string("0"));
                    gw.set_color(n, red);
                }
            }
            control_wait(WAIT);
        }


        // start the bfs for the nodes which are in the queue in the beginning
        int fifo_queue_length = fifo_queue.length();
        int free_v2_elem_in_list = 0;
        for (int i = 0; i < fifo_queue_length; i++) {

            node current_node = fifo_queue.pop();
            gw.set_color(current_node, green);
            p("neuer knoten");
            control_wait(WAIT);


            // check all edges of current node
            edge e;
            forall_inout_edges(e, current_node) {
                p("und die nächste edge");

                node opposite_node = g.opposite(current_node, e); 
                if (bfs_from[current_node] == opposite_node) {
                    p("dont check parent");
                    continue;
                }

                gw.set_color(e, green);
                control_wait(WAIT);
                if ((set_index % 2) == 0) { // nodes out of v_set_1
                    p("d 1");
                    if (matching[e] == 0) {  // edge is unmatched
                    p("d 2");
                        if (level[opposite_node] == -1) {  // node is unvisited so add it to the fifo_queue
                            p("d3");
                            level[opposite_node] = level[current_node] + 1;
                            fifo_queue.append(opposite_node);

                            gw.set_user_label(opposite_node, string("%d", level[opposite_node]));
                            gw.set_color(opposite_node, red);
                            gw.set_color(e, red);
                            bfs_edge_to_node[opposite_node] = e;
                            bfs_from[opposite_node] = current_node;
                            control_wait(WAIT);

                            // check for free nodes to start later dfs
                            if (free[opposite_node] == 1) {
                                p("free");
                                free_v2_elem_in_list = 1;
                            }
                        }
                    }    

                } else { // nodes out of v_set_2
                    p(" e 1");
                    if (matching[e] == 1) {  // edge is matched
                        p("e 2");
                        if (level[opposite_node] == -1) {  // node is unvisited so add it to the fifo_queue
                            p("e 3");

                            fifo_queue.append(opposite_node);
                            level[opposite_node] = level[current_node] + 1;

                            gw.set_user_label(opposite_node,string("%d", level[opposite_node]));
                            gw.set_color(opposite_node, red);
                            bfs_edge_to_node[opposite_node] = e;
                            bfs_from[opposite_node] = current_node;
                            gw.set_color(e, red);
                            control_wait(WAIT);

                        }
                    } 

                }
            }

            gw.set_color(current_node, blue);
            if (bfs_edge_to_node[current_node] != NULL) {
                gw.set_color(bfs_edge_to_node[current_node], blue);
            }
            control_wait(WAIT);

        }
        set_index++;

        // stop if fifo_queue is empty
        if (fifo_queue.empty()) {
            p("queue is empty - done ");
            break;
        }

        // there is a free node which is element of vertex set 2 -> start dfs
        if (free_v2_elem_in_list == 1) {
            p("dfs");
            gw.message(string("Upper bound: %d - Depth First Search", upper_b));


            // just for animation
            forall_nodes(n, g) {
                gw.set_color(n, orange);
            }
            edge ed;
            forall_edges(ed, g) {
                gw.set_color(ed, black);
            }
            control_wait(WAIT);


            // start dfs for all free nodes of vertex set 1
            node v;
            forall(v, v_set_1) {
                if (free[v] == 1) {
                    int round = 0;
                    p("start dfs");
                    dfs(gw, g, v, round, level, free, matching, v, current_matching);
                    p("end dfs");
                    control_wait(WAIT);
                }
            }
            set_index = 0; 
            fifo_queue.clear(); // clear fifo_queue to enable new bfs

        }
        p("ende");



    }

    // animate the matching edges in one color in the end
    edge edg;
    forall_edges(edg, g) {

        if (matching[edg] == 1) {
            gw.set_color(edg, violet);

        }
    }

    node no;
    forall_nodes(no, g) {
        gw.set_color(no, violet);
    }
    

}


    // Main program
int main(int argc, char *argv[]) {
    p("los gehts");
    // Create window for illustrating the graph with size 800 x 600 
    GraphWin gw(800, 600);
    gw.display(); // Display window on the screen
    create_control(); // Display control window
    gw.set_directed(false); // use undirected graph presentation
    if (argc > 1)    // falls Name als Parameter, Graph laden
        gw.read(argv[1]);

    gw.edit();   // switch to edit mode until user presses 'done'

    // jetzt holen wir uns den Graphen, den der Benutzer eingegeben oder geladen hat
    graph &g = gw.get_graph();

    if (g.number_of_nodes() == 0) {  // Ende, wenn der Graph leer ist.
        gw.close(); destroy_control();
        exit(1);
    }

    // Jetzt deklarieren wir ein Feld, das jedem Knoten eine Nummer zuordnet,
    // und initialisieren es mit "-1"
    node_array<int> bfsnum(g, -1);


    // Nun zeigen wir fuer alle Knoten den bfsnum-Wert als User-Label an
    // sowie initialisieren den Graphen gelb.
    node v;
    forall_nodes(v, g) {
        gw.set_label_type(v, user_label);    // User-Label anzeigen (statt Index-Label)
        gw.set_color(v, yellow);
    }
    edge e;
    forall_edges(e, g)
        gw.set_color(e, yellow);


    hopcroft(g, gw);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
