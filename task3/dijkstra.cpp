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
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/list.h>

#include "control.h" // Control window (adjusting speed etc.)

#define WAIT 3

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

    // iterative prim algorithm to find a minimal spanning tree
    // parameter:
    //   graph g: the graph as reference
    //   GraphWin gw: the graph windows as reference
    //   node start_node: the starting node
    //   int weight_sum: the sum of all edges of the spanning tree
void dijkstra(graph &g, GraphWin &gw, node &start_node) {

    node_pq<double> prio_queue(g); // priority queue for nodes

    node_array<node> from(g, NULL);
    node_array<edge> from_edge(g);

    node_array<double> distance(g, -1);
        


    // get the edge weight from the user label
    edge_array<double> edge_weight(g);

    edge e;
    forall_edges(e, g) { // not relevant for runtime
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];
    }

    
    distance[start_node] = 0;
    prio_queue.insert(start_node, 0);

    gw.set_color(start_node, red);
    control_wait(WAIT);

    while (!prio_queue.empty()) {
        
        node current_node = prio_queue.del_min();
        if (current_node == start_node) {
            gw.set_color(current_node, green); 
        } else {

            gw.set_color(current_node, blue);

            gw.set_color(from_edge[current_node], blue);
        }


        control_wait(WAIT);

        // add node to done set
        edge e;
        forall_out_edges(e, current_node) {
            
            node n = g.opposite(current_node, e);

            double d = distance[current_node] + edge_weight[e];
            if (distance[n] == -1) {
                //insert
                p("insert");
                prio_queue.insert(n, d);
                distance[n] = d;
                from[n] = current_node;
                from_edge[n] = e;

                gw.set_color(n, red);
                gw.set_color(e, red);
                p(distance[n]);
                p(edge_weight[e]);
                //gw.set_user_label(n, string("%s -> %.0f", gw.get_user_label(n), d));
                gw.set_user_label(n, string("%.0f", d));
                control_wait(WAIT);
                
            } else if (distance[n] > d) {
                // update
                p("update");
                prio_queue.decrease_p(n, d);
                distance[n] = d;
                //gw.set_user_label(n, string("%s -> %.0f", gw.get_user_label(n), distance[n]));
                gw.set_user_label(n, string("%.0f", d));
                gw.set_color(e, red);
                gw.set_color(from_edge[n], green);
                from[n] = current_node;
                from_edge[n] = e;

                control_wait(WAIT);
                    
            } 

        }
    }

    /**
    // fragen ob das von der laufzeit ok ist
    // ist aber eigtl nur markieren ...
    node n;
    forall_nodes(n, g) {
        node next;
        while ((next = from[n]) != NULL) {
           gw.set_color(from_edge[n], blue); 
           n = next;
        }
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

    dijkstra(g, gw, v);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
