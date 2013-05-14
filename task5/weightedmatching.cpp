
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
#include <LEDA/geo/point.h>

#include "control.h" // Control window (adjusting speed etc.)

#define WAIT 0.7 

#define p(str) ( std::cout << str << std::endl ) // print helper

using leda::graph;
using leda::node;
using leda::edge;
using leda::point;
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
using leda::edge_map;
using leda::edge_array;
using leda::node_partition;
using std::numeric_limits;
using leda::gw_edge_dir;


int dijkstra(graph &g, GraphWin &gw, node &start_node, node &target_node, edge_array<double> &edge_weight, edge_array<int> &matching) {


    node_pq<double> prio_queue(g); // priority queue for nodes

    node_array<node> from(g, NULL); // from Node-Array to store the parent node of each node; can be updated dynamically
    node_array<edge> from_edge(g);  // from Edge-Array to store the parent edge of each node; can be updated dynamically

    node_array<double> distance(g, -1); // Distance double-Array to store the current shortest distance to a particular node; default value -1
        

    
    distance[start_node] = 0; // set distance to start node to 0
    gw.set_user_label(start_node, string("0")); // show current distance in node visible for user during runtime
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
                p(d);
                gw.set_user_label(n, string("%.0f", d)); // show current distance in node visible for user during runtime
                gw.set_color(n, orange);
                gw.redraw();
                control_wait(WAIT);
                
            } else if (distance[n] > d) {// if node has already been inserted in priority queue
            //with a distance higher than the distance from current node then update its priority value
                p("update");
                prio_queue.decrease_p(n, d);// update priority value to lower distance
                distance[n] = d; // update distance
                p(distance[n]);
                p(edge_weight[e]);
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

    if (distance[target_node] == -1) {
        return 0;
    }

    
    edge_array<int> in_path(g, 0);
    node next_node = target_node;
    node last_node;
    edge next_edge;
    gw.set_color(from[target_node], orange);
    gw.set_color(from[from[target_node]], orange);
    gw.set_color(from[from[from[target_node]]], orange);
    while ((next_node = from[next_node]) != start_node) {
        next_edge = from_edge[next_node];
        in_path[next_edge] = 1;
        g.rev_edge(next_edge);
        edge_weight[next_edge] = 0;
        gw.set_user_label(next_edge, string("%.1f", edge_weight[next_edge]));
        gw.update_graph();
        gw.set_color(next_edge, orange);

        last_node = next_node;

        if (matching[next_edge] == 0) {
            matching[next_edge] = 1;
            gw.set_width(next_edge, 10);    
        } else {
            matching[next_edge] = 0;
            gw.set_width(next_edge, 1);
        }
        control_wait(WAIT);
    }

    gw.del_edge(from_edge[target_node]);
    gw.del_edge(from_edge[last_node]);

    gw.acknowledge("update costs");
    edge ed;
    forall_edges(ed, g) {
        if (in_path[ed] == 0) {
            node source = g.source(ed);
            node target = g.target(ed);
            edge_weight[ed] = edge_weight[ed] + distance[source] - distance[target];
            gw.set_user_label(ed, string("%.1f", edge_weight[ed]));

        }
    }

    gw.redraw();

    return 1;
}




void weightedmatching(graph &g, GraphWin &gw) {

    list<node> v_set_1; // vertex set 1
    list<node> v_set_2; // vertex set 2

    edge_map<double> edge_weight(g);


    int nodes_count = 0; // counts all nodes of the graph
    
    // split up the nodes into vertex set 1 and set 2 depending on the user label
    point dummy(0.0, 0.0);
    node source_node = gw.new_node(dummy);
    node target_node = gw.new_node(dummy);

    gw.set_label_type(source_node, user_label);   
    gw.set_label_type(target_node, user_label);  

    double v1_min = numeric_limits<double>::infinity();
    double v1_max = 0;
    double v1_height;
    double v2_min = numeric_limits<double>::infinity();
    double v2_max = 0;
    double v2_height;


    node n;
    forall_nodes(n, g) { 
        string s = gw.get_user_label(n);
        point p1 = gw.get_position(n);
        edge e;
        if (s == string("1")) {
            v1_height = p1.ycoord();
            if (p1.xcoord() > v1_max) v1_max = p1.xcoord();
            if (p1.xcoord() < v1_min) v1_min = p1.xcoord();
            v_set_1.append(n); 
            e = gw.new_edge(source_node, n);
            edge_weight[e] = 0.0;
            gw.set_user_label(e, string("%.1f", edge_weight[e]));

        } else if (s == string("2")) {
            v2_height = p1.ycoord();
            if (p1.xcoord() > v2_max) v2_max = p1.xcoord();
            if (p1.xcoord() < v2_min) v2_min = p1.xcoord();
            v_set_2.append(n);
            e = gw.new_edge(n, target_node);
            edge_weight[e] = 0.0;
            gw.set_user_label(e, string("%.1f", edge_weight[e]));
        }

        nodes_count++;
    }

    point p_start((v1_max + v1_min) / 2, v1_height + 1.7);
    gw.set_position(source_node, p_start);
    point p_target((v2_max + v2_min) / 2, v2_height - 1.7);
    gw.set_position(target_node, p_target);



    edge e;
    double min_weight = numeric_limits<double>::infinity();
    forall_edges(e, g) { // insert the edge from the user labels
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];

        if (edge_weight[e] < min_weight) min_weight = edge_weight[e];

        gw.set_direction(e, leda::directed_edge);
    }

    gw.acknowledge("add const");

    if (min_weight < 0) {
        forall_edges(e, g) {
            edge_weight[e] = edge_weight[e] - min_weight;
            gw.set_user_label(e, string("%.1f", edge_weight[e]));
        }
    }

    gw.redraw();


    edge_array<int> matching(g, 0);

    p("out nodes");
    p(g.outdeg(source_node));
    while (1) {
        gw.acknowledge("dijstra");
        int done = dijkstra(g, gw, source_node, target_node, edge_weight, matching);
        if (done == 0) {
            break;
        }
    }


    gw.acknowledge("end m");

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


    weightedmatching(g, gw);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufr�umen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
