// Animation of kruskal's algorithm to find a minimal spanning tree in a tree
// unvisited part is displayed in yellow (default), completed nodes and edges in blue,
// nodes still being processed are displayed in red,
// and edges leading to a cycle are displayed in green
#include <iostream>
#include <climits>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/color.h>
#include <LEDA/system/basic.h>
#include <LEDA/core/queue.h>
#include <LEDA/core/p_queue.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/list.h>

#include "control.h" // Control window (adjusting speed etc.)

#define WAIT 0.5

#define p(str) ( std::cout << str << std::endl ) // print helper

using leda::graph;
using leda::node;
using leda::edge;
using leda::node_array;
using leda::user_label;
using leda::GraphWin;
using leda::red;
using leda::blue;
using leda::green;
using leda::yellow;
using leda::string;
using leda::queue;
using leda::list;
using leda::p_queue;
using leda::edge_array;
using leda::pq_item;
using leda::node_partition;

// iterative kruskal algorithm to find a minimal spanning tree
// parameter:
//   graph g: the graph as reference
//   GraphWin gw: the graph windows as reference
//   int weight_sum: the sum of all edges of the spanning tree
void kruskal(graph &g, GraphWin &gw, int &weight_sum) {

    p_queue<int, edge> prio_queue; // priority queue for edges

    node_partition partition = node_partition(g); // graph's node partition

    int counter = 0; // node number counter
    
    // get the weight of the edges from the user labels
    // store them in the edge_weight array
    // and add the edges to the priority queue
    edge_array<int> edge_weight(g);
    edge e;
    forall_edges(e, g) { 
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];

        prio_queue.insert(edge_weight[e], e); 
    }
    


    // create the mst
    do { 
        // find the edge with minimum weight
        pq_item it = prio_queue.find_min(); 
        edge working_edge = prio_queue[it]; 
        prio_queue.del_min(); 

        // search the nodes to this edge
        node source_node = g.source(working_edge); 
        node target_node = g.target(working_edge); 


        gw.set_color(working_edge, red);
        // mark nodes only red if not already blue
        if (gw.get_color(source_node) != blue) {
            gw.set_color(source_node, red);
        }
        if (gw.get_color(target_node) != blue) {
            gw.set_color(target_node, red);
        }

        control_wait(WAIT);

        // check if the two nodes are in different partitions
        if (partition.same_block(source_node, target_node) == 0) { 
            partition.union_blocks(source_node, target_node); // runtime to be identified

            gw.set_color(working_edge, blue);
            weight_sum += edge_weight[working_edge]; 

            // mark nodes only if not already marked
            if (gw.get_color(target_node) != blue) {
                gw.set_user_label(target_node, string("%d", counter++));
                gw.set_color(target_node, blue);
            }

            if (gw.get_color(source_node) != blue) {
                gw.set_user_label(source_node, string("%d", counter++));
                gw.set_color(source_node, blue);
            }

        } else { // the nodes are already in the same partition which leads to a cycle

            gw.set_color(working_edge, green);

        }


    } while (partition.number_of_blocks() != 1); 
    // loop as long as all partition blocks are connected
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
        gw.set_user_label(v, string("%d", bfsnum[v])); // User-Label auf bfsnum[v] setzen
        gw.set_color(v, yellow);
    }
    edge e;
    forall_edges(e, g)
        gw.set_color(e, yellow);

    gw.acknowledge("Start ?"); // Dialogbox anzeigen und best�tigen lassen

    int weight_sum  = 0;
    kruskal(g, gw, weight_sum);

    gw.acknowledge(string("Weight is %d !", weight_sum)); // Dialogbox anzeigen und best�tigen lassen
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
