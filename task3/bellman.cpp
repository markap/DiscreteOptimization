// Animation of prim's algorithm to find a minimum spanning tree
// unvisited part is displayed in yellow (default), completed nodes and edges in blue,
// nodes and their corresponding edges in the priority queue in red
// edges which were regarded as minimum edge but are not after an upate anymore are displayed in green
// as well as edges which are not minimum when you try to add them
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

#define WAIT 0

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


void mark_cycle(GraphWin &gw, graph &g, node &n, node_array<node> &from, node_array<edge> &edge_for_node) {

    node cycle_begin = n;
    gw.set_color(edge_for_node[cycle_begin], orange); 
    gw.set_color(cycle_begin, orange);
     

    node next;
    node last_node;
    while ((next = from[n]) != cycle_begin) {
        if (next == NULL) {
           break;
        }
        last_node = next;
        if (edge_for_node[next] != NULL) {
           gw.set_color(edge_for_node[next], orange); 
        }
        gw.set_color(next, orange);
        n = next;
    }

    // we loop n-1 times, so we need to find the single edge btw the cycle_begin node
    // and the last node (only if the circle occurs at the end of our bellman
    // ford algorithm - not if we found the circle with early detection)
    if (edge_for_node[last_node] == NULL) {
        p("no early termination");
        edge e;
        forall_out_edges(e, cycle_begin) {
            if (last_node == g.opposite(cycle_begin, e)) {
                gw.set_color(e, orange);
            }
        }
    }

    gw.acknowledge("cycle");
}

void bellman(graph &g, GraphWin &gw, node &start_node) {

    queue<node> fifo_queue; 

    node_array<node> from(g, NULL);

    node_array<int> node_in_queue(g, 0);
    node_array<edge> edge_for_node(g);

    node_array<double> distance(g, numeric_limits<double>::infinity());

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

    
    distance[start_node] = 0;
    gw.set_user_label(start_node, string("%.1f", +0.0));
    fifo_queue.append(start_node);
    node_in_queue[start_node] = 1;

    gw.set_color(start_node, red);
    control_wait(WAIT);

    int current_queue_size;
    int node_length = g.all_nodes().length();
    p(node_length);
    for (int i = 1; i < node_length; i++) {

        gw.message(string("Phase %d", i));
        current_queue_size = fifo_queue.size();
        // early termination
        if (current_queue_size == 0) {
            p("queue is empty!");
            break;
        }
        int j = 0;
        while (j < current_queue_size && !fifo_queue.empty()) {
            j++;
        
            node current_node = fifo_queue.pop();
            node_in_queue[current_node] = 0;
            gw.set_color(current_node, green); 



            control_wait(WAIT);

            edge e;
            forall_out_edges(e, current_node) {
            
                node n = g.opposite(current_node, e);

                double d = distance[current_node] + edge_weight[e];
                if (d < distance[n]) {
                    if (edge_for_node[n] != NULL) {
                        gw.set_color(edge_for_node[n], green);
                    }
                    edge_for_node[n] = e;
                    distance[n] = d;
                    from[n] = current_node;
                    node_map<int> from_map = from_chain[current_node];
                    if (from_map[n] == 1) {
                        p("early termination");
                        mark_cycle(gw, g, n, from, edge_for_node);
                        return;
                    }
                    from_map[current_node] = 1;
                    from_chain[n] = from_map;

                    // fuege nur ein wenn nicht schon in queue
                    if (node_in_queue[n] == 0) {
                        fifo_queue.append(n);
                        node_in_queue[n] = 1;
                        gw.set_color(n, red);

                    }


                    gw.set_color(e, red);
                    gw.set_user_label(n, string("%.1f", d));
                    control_wait(WAIT);
                
                } else {
                    gw.set_color(e, green);
                }

            }
            gw.set_color(current_node, blue);
            if (edge_for_node[current_node] != NULL) {
                gw.set_color(edge_for_node[current_node], blue);
            }
            control_wait(WAIT);
        }
    }

    if (!fifo_queue.empty()) {
        node n = fifo_queue.pop();
        mark_cycle(gw, g, n, from, edge_for_node);
    }

}


    // Main program
int main(int argc, char *argv[]) {
    p("los gehts");
    // Create window for illustrating the graph with size 800 x 600 
    GraphWin gw(800, 600);
    gw.display(); // Display window on the screen
    create_control(); // Display control window
    gw.set_directed(true); // use undirected graph presentation
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

    // jetzt lassen wir den Benutzer mit der Maus einen unbesuchten Knoten
    while ((v = gw.read_node()) == NULL || dfsnum[v] >= 0) ;

    bellman(g, gw, v);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
