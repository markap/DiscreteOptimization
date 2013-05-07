// Animation of bellman - ford algorithm to find shortest path in a graph
// unvisited part is displayed in yellow (default), completed nodes and edges in blue,
// nodes in the priority queue in red
// the current node popped from the queue is marked in green
// already handled edges with longer paths are marked in green
// possible cycles are marked in orange
#include <iostream>
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

#define WAIT 2

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
using leda::string;
using leda::queue;
using leda::list;
using leda::node_pq;
using leda::node_map;
using leda::edge_array;
using leda::node_partition;
using std::numeric_limits;


// @todo dfs rückwärtskante
// @todo länge des pfades


int dfs(GraphWin &gw, graph &g, node &v, int round, node_array<int> &level, node_array<int> &free, edge_array<int> &matching) {

    int next_round = round + 1;

    int invert;

    gw.set_color(v, blue);
    control_wait(WAIT);

    edge e;
    forall_inout_edges(e, v) {
        gw.set_color(e, green);
        control_wait(WAIT);

        if ((round % 2) == 0) {
            if (matching[e] == 0) { // unmatched
                node opposite_node = g.opposite(v, e);
                if (level[opposite_node] == level[v] + 1) { // level must be greater 1

                    gw.set_color(e, blue);
                    gw.set_color(opposite_node, blue);
                    control_wait(WAIT);

                    if (free[opposite_node] == 1) {
                        level[opposite_node] = -1;
                        gw.set_color(opposite_node, blue);
                        gw.set_user_label(opposite_node, string("%d", level[opposite_node]));

                        level[v] = -1;
                        gw.set_user_label(v, string("%d", level[v]));
                        
                        // invertiere
                        if (matching[e] == 0) {
                            gw.set_color(e, blue);
                            free[opposite_node] = 0;
                            gw.set_border_width(opposite_node, 1);
                            free[v] = 0;
                            gw.set_width(e, 10);
                            gw.set_border_width(v, 1);
                            matching[e] = 1;
                        } else {
                            matching[e] = 0; 
                            gw.set_width(e, 2);
                        }
                        control_wait(WAIT);
                        return 1;
                    } else {
                        invert = dfs(gw, g, opposite_node, next_round, level, free, matching);
                        if (invert == 1) {
                            if (matching[e] == 0) {
                                free[opposite_node] = 0;

                                gw.set_border_width(opposite_node, 1);
                                gw.set_width(e, 10);
                                free[v] = 0;
                                gw.set_border_width(v, 1);
                                matching[e] = 1;
                            } else {
                                gw.set_width(e, 1);
                                matching[e] = 0; 
                            }
                            level[v] = -1;
                            gw.set_user_label(v, string("%d", level[v]));
                            control_wait(WAIT);
                            return 1;
   
                        }
                    }
                }

            }
        } else {
            if (matching[e] == 1) { // matched
                node opposite_node = g.opposite(v, e);
                if (level[opposite_node] == level[v] + 1) { // level must be greater 1

                    gw.set_color(e, blue);
                    gw.set_color(opposite_node, blue);
                    control_wait(WAIT);

                    invert = dfs(gw, g, opposite_node, next_round, level, free, matching); }
                    if (invert == 1) {
                        if (matching[e] == 0) {
                            free[opposite_node] = 0;

                            gw.set_border_width(opposite_node, 5);
                            free[v] = 0;
                            gw.set_border_width(v, 10);
                            gw.set_width(e, 5);
                            matching[e] = 1;
                        } else {
                            matching[e] = 0; 
                            gw.set_width(e, 1);
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

void hopcroft(graph &g, GraphWin &gw) {

    list<node> v_set_1;
    list<node> v_set_2;

    node_array<int> level(g, -1);
    edge_array<int> matching(g, 0);
    node_array<int> free(g, 1);
    node_array<edge> bfs_edge_to_node(g, NULL);

    queue<node> fifo_queue;

    int nodes_count = 0;

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

    control_wait(WAIT);
    int set_index = 0;

    while (true) {

        if (fifo_queue.empty()) {
            gw.acknowledge("BFS init");
            level.use_node_data(g, -1);

            // just for animation
            forall_nodes(n, g) {
                gw.set_color(n, yellow);
                gw.set_user_label(n, string("%d", level[n]));
            }
            edge ed;
            forall_edges(ed, g) {
                gw.set_color(ed, yellow);
            }
            control_wait(WAIT);


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


        int fifo_queue_length = fifo_queue.length();
        int free_v2_elem_in_list = 0;
        for (int i = 0; i < fifo_queue_length; i++) {

            node current_node = fifo_queue.pop();
            gw.set_color(current_node, green);
            p("neuer knoten");
            control_wait(WAIT);


            edge e;
            forall_inout_edges(e, current_node) {
                p("und die nächste edge");
                gw.set_color(e, green);
                control_wait(WAIT);
                if ((set_index % 2) == 0) { // nodes out of v_set_1
                    p("d 1");
                    if (matching[e] == 0) {  // edge is unmatched
                    p("d 2");
                        node opposite_node = g.opposite(current_node, e); 
                        if (level[opposite_node] == -1) {
                            p("d3");
                            level[opposite_node] = level[current_node] + 1;
                            fifo_queue.append(opposite_node);

                            gw.set_user_label(opposite_node, string("%d", level[opposite_node]));
                            gw.set_color(opposite_node, red);
                            gw.set_color(e, red);
                            bfs_edge_to_node[opposite_node] = e;
                            control_wait(WAIT);

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
                        node opposite_node = g.opposite(current_node, e); 
                        if (level[opposite_node] == -1) {
                            p("e 3");

                            fifo_queue.append(opposite_node);
                            level[opposite_node] = level[current_node] + 1;

                            gw.set_user_label(opposite_node,string("%d", level[opposite_node]));
                            gw.set_color(opposite_node, red);
                            bfs_edge_to_node[opposite_node] = e;
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

        if (fifo_queue.empty()) {
            p("queue is empty - done ");
            break;
        }

        if (free_v2_elem_in_list == 1) {
            p("dfs");
            gw.acknowledge("search for free nodes in v1");


            // just for animation
            forall_nodes(n, g) {
                gw.set_color(n, yellow);
            }
            edge ed;
            forall_edges(ed, g) {
                gw.set_color(ed, yellow);
            }
            control_wait(WAIT);


            node v;
            forall(v, v_set_1) {
                if (free[v] == 1) {
                    int round = 0;
                    p("start dfs");
                    dfs(gw, g, v, round, level, free, matching);
                    p("end dfs");
                    control_wait(WAIT);
                }
            }
            set_index = 0; // check
            fifo_queue.clear();

        }
        p("ende");



    }

    







/**
    queue<node> fifo_queue; 

    node_array<node> from(g, NULL);

    node_array<int> node_in_queue(g, 0);
    node_array<edge> edge_for_node(g);

    // a distance array with relation: node -> infinity
    node_array<double> distance(g, numeric_limits<double>::infinity());

    // the from chain to allow early detection
    node_array<node_map<int> > from_chain(g);
    node v;
    forall(v, g.all_nodes()) {
        node_map<int> tmp(g);
        from_chain[v] = tmp;
    }
        


    // get the edge weight from the user label
    edge_array<double> edge_weight(g);

    edge e;
    forall_edges(e, g) { 
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];
    }

    
    // handle the start node
    distance[start_node] = 0;
    gw.set_user_label(start_node, string("%.1f", +0.0));
    fifo_queue.append(start_node);
    node_in_queue[start_node] = 1;

    gw.set_color(start_node, red);
    control_wait(WAIT);

    int current_queue_size;
    int node_length = g.all_nodes().length();
    p(node_length);
    // the outer loop loops n-1 times
    for (int i = 1; i < node_length; i++) {

        gw.message(string("Phase %d", i));
        current_queue_size = fifo_queue.size();
        // early termination
        if (current_queue_size == 0) {
            p("queue is empty!");
            break;
        }
        // loop through all elements of fifo_queue at the beginning of while loop
        int j = 0;
        while (j < current_queue_size && !fifo_queue.empty()) {
            j++;
        
            node current_node = fifo_queue.pop();
            node_in_queue[current_node] = 0;
            gw.set_color(current_node, green); 



            control_wait(WAIT);

            // check all the outgoing edges
            edge e;
            forall_out_edges(e, current_node) {
            
                node n = g.opposite(current_node, e);

                double d = distance[current_node] + edge_weight[e];
                if (d < distance[n]) { // update if distance is smaller
                    if (edge_for_node[n] != NULL) {
                        gw.set_color(edge_for_node[n], green);
                    }
                    edge_for_node[n] = e;
                    distance[n] = d;
                    from[n] = current_node;

                    // check for early termination
                    node_map<int> from_map = from_chain[current_node];
                    if (from_map[n] == 1) {
                        p("early termination");
                        mark_cycle(gw, g, n, from, edge_for_node);
                        return;
                    }
                    // add node to the from_chain
                    from_map[current_node] = 1;
                    from_chain[n] = from_map;

                    // add node only if not already in queue
                    if (node_in_queue[n] == 0) {
                        fifo_queue.append(n);
                        node_in_queue[n] = 1;
                        gw.set_color(n, red);

                    }


                    gw.set_color(e, red);
                    gw.set_user_label(n, string("%.1f", d));
                    control_wait(WAIT);
                
                } else { // no smaller distance
                    gw.set_color(e, green);
                }

            }

            // node is done: mark it blue
            gw.set_color(current_node, blue);
            if (edge_for_node[current_node] != NULL) {
                gw.set_color(edge_for_node[current_node], blue);
            }
            control_wait(WAIT);
        }
    }

    // if fifo_queue is not empty after n-1 rounds, there is a cycle
    if (!fifo_queue.empty()) {
        node n = fifo_queue.pop();
        mark_cycle(gw, g, n, from, edge_for_node);
    }

*/
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

    node_array<int> dfsnum(g, -1);

    hopcroft(g, gw);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
