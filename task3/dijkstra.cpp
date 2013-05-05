// Animation of disjktra's algorithm to find the shortest paths in a graph for an ssc- problem
// unvisited part is displayed in yellow (default), completed nodes and edges belonging to shortest pathsin blue
//start node (selected by user) in green
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

#define WAIT 1

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

    // iterative dijkstra algorithm to find the shortest paths
    // parameter:
    //   graph g: the graph as reference
    //   GraphWin gw: the graph windows as reference
    //   node start_node: the starting node
 void dijkstra(graph &g, GraphWin &gw, node &start_node) {

    node_pq<double> prio_queue(g); // priority queue for nodes

    node_array<node> from(g, NULL); // from Node-Array to store the parent node of each node; can be updated dynamically
    node_array<edge> from_edge(g);  // from Edge-Array to store the parent edge of each node; can be updated dynamically

    node_array<double> distance(g, -1); // Distance double-Array to store the current shortest distance to a particular node; default value -1
        


    // get the edge weight from the user label
    edge_array<double> edge_weight(g); // contains the edge weights for all edges in the graph

    edge e;
    forall_edges(e, g) { // insert the edge from the user labels
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];
    }

    
    distance[start_node] = 0; // set distance to start node to 0
    prio_queue.insert(start_node, 0); // insert start node in priority queue

    gw.set_color(start_node, red); // color node in red
    control_wait(WAIT); // wait

    while (!prio_queue.empty()) { // while unprocessed nodes are in queue, proceed with conducting the algorith
        
        node current_node = prio_queue.del_min(); // pop the node with least distance from all completed nodes
        if (current_node == start_node) {
            gw.set_color(current_node, green); // start node is depicted in green
        } else {

            gw.set_color(current_node, blue); // all other nodes being finished in blue

            gw.set_color(from_edge[current_node], blue);// processed edges are colored in blue
        }


        control_wait(WAIT);

        // add node to done set
        edge e;
        forall_out_edges(e, current_node) { // process all neighbor nodes
            
            node n = g.opposite(current_node, e);

            double d = distance[current_node] + edge_weight[e]; // calculate distance from current node
            if (distance[n] == -1) { // if node has not been inserted into priority queue yet, insert it
                //insert
                p("insert");
                prio_queue.insert(n, d); // insert node in prio queue with respective distance
                distance[n] = d; // save distance
                from[n] = current_node; // save parent node
                from_edge[n] = e;//save parent edge

                gw.set_color(n, red);// nodes in priority queue, still being processed are depicted in red
                gw.set_color(e, red);// as well as their corresponding edges
                p(distance[n]);
                p(edge_weight[e]);
                gw.set_user_label(n, string("%.0f", d)); // show current distance in node visible for user during runtime
                control_wait(WAIT);
                
            } else if (distance[n] > d) {// if node has already been inserted in priority queue
            //with a distance higher than the distance from current node then update its priority value
                p("update");
                prio_queue.decrease_p(n, d);// update priority value to lower distance
                distance[n] = d; // update distance
                gw.set_user_label(n, string("%.0f", d)); // update distance shown in node as user label
                gw.set_color(e, red); // color node in red as it is still being processed
                gw.set_color(from_edge[n], green); // color previous parent edge in green as it has already been processed
                // but is not an edge which is part of a shortest path
                from[n] = current_node; // update parent node
                from_edge[n] = e;// update parent edge

                control_wait(WAIT);// wait 1.0 sec
                    
            } 

        }
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

    dijkstra(g, gw, v);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
