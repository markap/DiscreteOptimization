// Animation of prim's algorithm to find a minimum spanning tree
// unvisited part is displayed in yellow (default), completed nodes and edges in blue,
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
#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/list.h>

#include "control.h" // Control window (adjusting speed etc.)

#define WAIT 0.3

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
    using leda::node_pq;
    using leda::edge_array;
    using leda::node_partition;

    void prim(graph &g, GraphWin &gw, node &start_node, int &weight_sum) {

        node_pq<int> prio_queue(g);
        node_array<int> tree_nodes(g, 0);


        node_array<edge> node_edge_array(g);
        
        int counter = 0;


        edge_array<int> edge_weight(g);

        edge e;
        forall_edges(e, g) {
            string s = gw.get_user_label(e);
            leda::string_istream I(s);
            I >> edge_weight[e];

        }

        // first: add all neighbours of startnode, weight of edge
        // node -> edge

        tree_nodes[start_node] = 1;

        gw.set_color(start_node, blue); 
        gw.set_user_label(start_node, string("%d", counter++));
        control_wait(WAIT);


        edge ed;
        forall_inout_edges(ed, start_node) {
           node n = g.opposite(start_node, ed); 

           gw.set_color(n, red);
           gw.set_color(ed, red);
           control_wait(WAIT);

           node_edge_array[n] = ed; 
           prio_queue.insert(n, edge_weight[ed]);
        }

        
        do {
            node min_node = prio_queue.del_min();
            tree_nodes[min_node] = 1;

            gw.set_user_label(min_node, string("%d", counter++));
            gw.set_color(min_node, blue);
            gw.set_color(node_edge_array[min_node], blue);

            weight_sum += edge_weight[node_edge_array[min_node]];


            control_wait(WAIT);

            edge ed;
            forall_inout_edges(ed, min_node) {
                node n = g.opposite(min_node, ed);
            
                if (tree_nodes[n] == 0) { // node is not already a tree node
                    if (node_edge_array[n] != NULL and edge_weight[node_edge_array[n]] > edge_weight[ed]) {
                        p("update");
                        gw.set_color(node_edge_array[n], green);
                        gw.set_color(ed, red);
                        p(edge_weight[ed]);
                        node_edge_array[n] = ed;
                        prio_queue.decrease_p(n, edge_weight[ed]);
                    } else if (node_edge_array[n] == NULL) { // wenn nicht bereits im spanning tree
                        p("insert");
                        p(edge_weight[ed]);
                        node_edge_array[n] = ed;
                        prio_queue.insert(n, edge_weight[ed]);
                        gw.set_color(n, red);
                        gw.set_color(ed, red);
                    } else {
                        gw.set_color(ed, green);
                    }

                    control_wait(WAIT);
                }
            }
        } while(!prio_queue.empty());

        // min node ausw�hlen
        // nachbarn betrachten
        // neu einf�gen oder aktualisieren - R�ckw�rtsnode ist schon drinnen!!!
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

    node_array<int> dfsnum(g, -1);

    // jetzt lassen wir den Benutzer mit der Maus einen unbesuchten Knoten
    while ((v = gw.read_node()) == NULL || dfsnum[v] >= 0) ;

    int weight_sum = 0;
    prim(g, gw, v, weight_sum);

    gw.acknowledge(string("Weight is %d !", weight_sum)); // Dialogbox anzeigen und best�tigen lassen
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
