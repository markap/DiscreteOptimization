// Animation of a breadth-first-search in an undirected graph
// unvisited part is displayed in yellow (default), completed nodes in blue,
// nodes still being processed are displayed in red (elements of the search queue)
// current node is depicted in green
// edges belonging to search tree are firstly displayed in red, after processing eventually in blue
// backward (additional) edges in green

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
using leda::green;
using leda::yellow;
using leda::string;
using leda::queue;
using leda::list;
using leda::p_queue;
using leda::edge_array;
using leda::pq_item;
using leda::node_partition;

void kruskal(graph &g, GraphWin &gw) {

    p_queue<int, edge> prio_queue;

    node_partition partition = node_partition(g);

    list<node> all_nodes; 
    
    node n;
    forall (n, g.all_nodes()) {
       all_nodes.append(n); 
    }




    edge_array<int> edge_weight(g);

    edge e;
    forall_edges(e, g) {
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];

        prio_queue.insert(edge_weight[e], e);
        p(edge_weight[e]);
    }

    p("####################");

    do {
        pq_item it = prio_queue.find_min();
        edge working_edge = prio_queue[it];
        prio_queue.del_min();


        node source_node = g.source(working_edge);
        node target_node = g.target(working_edge);


        gw.set_color(working_edge, red);
        gw.set_color(source_node, red);
        gw.set_color(target_node, red);

        control_wait(WAIT);

        if (partition.same_block(source_node, target_node) == 0) {
            partition.union_blocks(source_node, target_node);

            gw.set_color(working_edge, blue);
            gw.set_color(source_node, blue);
            gw.set_color(target_node, blue);

            all_nodes.remove(target_node); 
            all_nodes.remove(source_node); 
        } else {

            gw.set_color(working_edge, green);

        }

        p(edge_weight[working_edge]);
    } while (!all_nodes.empty());
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

    gw.acknowledge("Ready!"); // Dialogbox anzeigen und bestätigen lassen
    kruskal(g, gw);

    gw.acknowledge("Ready!"); // Dialogbox anzeigen und bestätigen lassen
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufräumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
