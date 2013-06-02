/*Illustration of weightedmatching

In the "Options" Menu the user can switch between a minimum and maximum searching by clicking on "Minimum Weight" or "Maximum weight" respectively before starting the algorithm
By default this is set to minimum.

For the illustration of the algorithm two Graphwin windows are used:

1)The first one showing the appliance of dijkstra's algorithm and inverting the minimum/maximum paths
 
 visited part is displayed in yellow (default), completed nodes and edges belonging to shortest paths in blue,
 nodes and their corresponding edges in the priority queue in red
 edges which were regarded as minimum/maximum edge but are not after an update anymore are displayed in green
 as well as edges which are not minimum/maximum when you try to add them
 The shortest/longest path is colored in orange


2)The second one depicting the progress of the found matchings in the graph
Bold edges are matched, while thin edges are unmatched, nodes and edges are colored in purple




*/
#include <iostream>
#include <math.h>
#include <climits>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graph/graph_misc.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/color.h>
#include <LEDA/system/basic.h>
#include <LEDA/core/queue.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/node_map.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/list.h>
#include <LEDA/geo/point.h>

#include "control.h" // Control window (adjusting speed etc.)

#define WAIT 0

int max = 0;
int transformed = 0;
int running = 0;

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
using leda::cyan;
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
using leda::gw_font_type;
using leda::roman_font;
using leda::bold_font;


// Runs modified dijsktra's algorithm and path invertion
//// parameter:
    //   graph g: the graph as reference
    //   GraphWin gw: the graph windows as reference
    //   node start_node: the starting node
    // 	 node target_node: the target node
    //   int number_of_edges_matched: the number of edges in matching
    // 	 edge_array edge_weight: contains all edge weights and changes them upon inverting a path
    // 	 edge_array<int> &matching: contains 0 if edge is unmatched and 1 if edge is matched
    //	 edge_array<double> &inmutual_weight: contains all origin edge weights to compute weight count
    // 	 double &weight_count: current weight count
    //	 GraphWin &gw2: second graphwin window 
    //	 node_array<node> &gw2_nodes:  mapping nodes from graph to nodes of graph window 2
    //	 edge_map<edge> &gw2_edges:	mapping edges from graph to edges of graph window 2
int dijkstra(graph &g, GraphWin &gw, node &start_node, node &target_node,int &number_of_edges_matched,  edge_array<double> &edge_weight, edge_array<int> &matching, edge_array<double> &inmutual_weight, double &weight_count, leda::GRAPH<node, edge> &h, GraphWin &gw2) {
    if (max ==0){
        gw.message(string("Now Dijkstra is searching for shortest path."));
        gw2.message(string("Now Dijkstra is searching for shortest path."));
	}
    if (max == 1){
	    gw.message(string("Now Dijskra is searching for longest path."));
	    gw2.message(string("Now Dijkstra is searching for longest path."));
    }

//inital node and edge coloring in yellow before each run of dijsktra
    node ndx;
    forall_nodes(ndx, g) {
        gw.set_color(ndx, yellow);
    }
    edge edg;
    forall_edges(edg, g) {
        gw.set_color(edg, cyan);
    }


    node_pq<double> prio_queue(g); // priority queue for nodes

    node_array<node> from(g, NULL); // from Node-Array to store the parent node of each node; can be updated dynamically
    node_array<edge> from_edge(g);  // from Edge-Array to store the parent edge of each node; can be updated dynamically

    node_array<double> distance(g, -1); // Distance double-Array to store the current shortest distance to a particular node; default value -1
    if (transformed == 0 && max == 1) {

		//Transform all edge weights to obtain exclusively positive edge weights -> Dijsktra is appliccable
        edge ex;
        double min_weight = numeric_limits<double>::infinity();
        forall_edges(ex, g) {
            double new_weight = edge_weight[ex] * -1; // multiply all edge weights with -1
            edge_weight[ex] = new_weight;

            if (new_weight < min_weight) {
                min_weight = new_weight;
            }
        }

        forall_edges(ex, g) {
            edge_weight[ex] = edge_weight[ex] - min_weight; // subtract the least edge weight from all edge and thus obtain solely positive edge weights
        }
        transformed = 1;
    }
        

    
    distance[start_node] = 0; // set distance to start node to 0
    gw.set_user_label(start_node, string("0")); // show current distance in node visible for user during runtime
    prio_queue.insert(start_node, 0); // insert start node in priority queue

    gw.set_color(start_node, red); // color node in red
   // control_wait(WAIT); // wait

    while (!prio_queue.empty()) { // while unprocessed nodes are in queue, proceed with conducting the algorithm
        
        node current_node = prio_queue.del_min(); // pop the node with least distance from all completed nodes
        if (current_node == start_node) {
           // gw.set_color(current_node, green); // start node is depicted in green
        } else {

            //gw.set_color(current_node, blue); // all other nodes being finished in blue

            //gw.set_color(from_edge[current_node], blue);// processed edges are colored in blue
        }


      //  control_wait(WAIT);

        // add node to done set
        edge e;
        forall_out_edges(e, current_node) { // process all neighbor nodes
            
            node n = g.opposite(current_node, e);

            double d = distance[current_node] + edge_weight[e]; // calculate distance from current node
            if (distance[n] == -1) { // if node has not been inserted into priority queue yet, insert it
                //insert
               // p("insert");
                prio_queue.insert(n, d); // insert node in prio queue with respective distance
                distance[n] = d; // save distance
                from[n] = current_node; // save parent node
                from_edge[n] = e;//save parent edge

              //  gw.set_color(n, red);// nodes in priority queue, still being processed are depicted in red
               // gw.set_color(e, red);// as well as their corresponding edges
               // p(distance[n]);
              //  p(edge_weight[e]);
              //  p(d);
              //  gw.set_user_label(n, string("%.0f", d)); // show current distance in node visible for user during runtime
                gw.redraw();
              //  control_wait(WAIT);
                
            } else if (distance[n] > d) {// if node has already been inserted in priority queue
            //with a distance higher than the distance from current node then update its priority value
                p("update");
                prio_queue.decrease_p(n, d);// update priority value to lower distance
                distance[n] = d; // update distance
                p(distance[n]);
                p(edge_weight[e]);
               // gw.set_user_label(n, string("%.0f", d)); // update distance shown in node as user label
               // gw.set_color(e, red); // color node in red as it is still being processed
               // gw.set_color(from_edge[n], green); // color previous parent edge in green as it has already been processed
                // but is not an edge which is part of a shortest path
                from[n] = current_node; // update parent node
                from_edge[n] = e;// update parent edge

                //control_wait(WAIT);// wait 1.0 sec
                    
            } 

        }
    }

    if (distance[target_node] == -1) {
        return 0;
    }
    int augmenting_path_length=0;
    for (node nodeiterator = target_node;nodeiterator!=start_node;nodeiterator=from[nodeiterator]){
          augmenting_path_length+=inmutual_weight[from_edge[nodeiterator]];
	  p(string("New augmenting path length is %d", augmenting_path_length));
}

    // Inverts the path
    if (max ==0){ 
        gw.message(string("Now shortest augmenting path is being inverted. Its length is %d", augmenting_path_length));
        gw2.message(string("Now shortest augmenting  path is being inverted. Its length is %d", augmenting_path_length));
    } else {
        gw.message(string ("Now longest augmenting path is being inverted. Its length is %d", augmenting_path_length));
        gw2.message(string ("Now longest augmenting path is being inverted. Its length is %d", augmenting_path_length));
}
    control_wait(WAIT); // wait 
    edge_array<int> in_path(g, 0); // shortest/longest edge path 
    node next_node = target_node;
 
    //node last_node;
    edge next_edge;
   
	// Path will be colored in orange
    //gw.set_color(from[target_node], orange); // Depict target node in orange
    //gw.set_color(from[from[target_node]], orange); // Color the predecessor in orange
    //gw.set_color(from[from[from[target_node]]], orange);
    while (from[(next_node = from[next_node])] != start_node) { // loop through the path
        next_edge = from_edge[next_node]; 
		//gw.set_color(from[from[from[target_node]]], orange);
        in_path[next_edge] = 1;
        g.rev_edge(next_edge);// reverses edge
        edge_weight[next_edge] = 0; // set edge weights on path to 0
        gw.set_user_label(next_edge, string("%.1f", edge_weight[next_edge])); // Print out the new edge weights as user labels
        gw.update_graph();
        gw.set_color(next_edge, orange);

        
		//Reverse matchings
         control_wait(WAIT);
	    if (matching[next_edge] == 0) {
            matching[next_edge] = 1;
	        number_of_edges_matched++;
	    //gw.message(string("Current number of edges matched: %d", number_of_edges_matched));
            //if (h[next_edge] != NULL) {
                gw2.set_width(next_edge, 10);
                gw2.set_user_label(next_edge, string("%s", gw2.get_user_label(next_edge)));
                gw2.redraw();
            //} 

            p("add w");
            p(inmutual_weight[next_edge]);
            weight_count += inmutual_weight[next_edge]; //Adjust the weight of current minimum/maximum matching
        } else {
            matching[next_edge] = 0;
	        number_of_edges_matched--;
	    //gw.message(string("Current number of edges matched: %d", number_of_edges_matched));
            //if (h[next_edge] != NULL) {
                gw2.set_width(next_edge, 1);
                gw2.set_user_label(next_edge, string("%s", gw2.get_user_label(next_edge)));
            //}

            weight_count -= inmutual_weight[next_edge];//Adjust the weight of current minimum/maximum matching
            p("remove w");
            p(inmutual_weight[next_edge]);
        }
        control_wait(WAIT);
    }

    control_wait(WAIT); 
    gw.del_edge(from_edge[target_node]); // delete last edge on path
    gw.del_edge(from_edge[next_node]); // delete first edge on path

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

// Runs weightedmatching algorithm and dijkstra preparation (subtracting least weight from every edge if necessary)
//// parameter:
    //   graph g: the graph as reference
    //   GraphWin gw: the graph windows as reference
    //	 node_array<node> &gw2_nodes:  mapping nodes from graph to nodes of graph window 2
    //	 edge_map<edge> &gw2_edges:	mapping edges from graph to edges of graph window 2


void weightedmatching(graph &g, GraphWin &gw, leda::GRAPH<node, edge> &h, GraphWin &gw2) {

    list<node> v_set_1; // vertex set 1
    list<node> v_set_2; // vertex set 2

    gw.set_flush(false);


    int nodes_count = 0; // counts all nodes of the graph
    
    // split up the nodes into vertex set 1 and set 2 depending on the user label
    point dummy(0.0, 0.0);
    node source_node = gw.new_node(dummy);
    node target_node = gw.new_node(dummy);

    gw.set_label_type(source_node, user_label);   
    gw.set_label_type(target_node, user_label);  
    
    int number_of_edges_matched = 0;
	
	// Find node which is on the right (v1_max), and on the left (v1_min) in vertex set 1
    double v1_min = numeric_limits<double>::infinity();
    double v1_max = 0;
    double v1_height;
	// Find node which is on the right (v2_max), and on the left (v2_min) in vertex set 2
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
            if (p1.xcoord() > v1_max) v1_max = p1.xcoord(); // x coordinate of current node on the very right in v1
            if (p1.xcoord() < v1_min) v1_min = p1.xcoord();	// x coordinate of current node on the very left in v1
            v_set_1.append(n); // append node to vertex set 1
            e = gw.new_edge(source_node, n);// connect source node with all nodes in v1
            //edge_weight[e] = 0.0; // edge weight on these connections is to be set to 0
            //gw2_edges[e] = NULL;
            gw.set_user_label(e, string("0"));

        } else if (s == string("2")) {
            v2_height = p1.ycoord();
            if (p1.xcoord() > v2_max) v2_max = p1.xcoord();
            if (p1.xcoord() < v2_min) v2_min = p1.xcoord();
            v_set_2.append(n);
            e = gw.new_edge(n, target_node); // connect target node with all nodes in v2
            //edge_weight[e] = 0.0;// edge weight on these connections is to be set to 0
            //gw2_edges[e] = NULL;
            gw.set_user_label(e, string("0"));
        }

        nodes_count++;
    }

    point p_start((v1_max + v1_min) / 2, v1_height + 5);
    gw.set_position(source_node, p_start); // position source node in the middle of all nodes of v1 and slightly on top
    point p_target((v2_max + v2_min) / 2, v2_height - 5);
    gw.set_position(target_node, p_target);// position target node in the middle of all nodes of v2 and slightly underneath

    gw.redraw();
    gw.set_flush(true);

    gw.zoom_graph(); // zoom out to assure that all nodes are visible in the graph window

    


    edge e;
    edge_array<double> edge_weight(g);
    double min_weight = numeric_limits<double>::infinity();
    forall_edges(e, g) { // insert the edge from the user labels
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> edge_weight[e];

        if (edge_weight[e] < min_weight) min_weight = edge_weight[e]; // find min weight in all edge

        gw.set_direction(e, leda::directed_edge); // set edges directed
    }

    edge_array<double> inmutual_weight = edge_weight;


    if (min_weight < 0) { // if one or more edges contain negative edge weights 
        forall_edges(e, g) {
            edge_weight[e] = edge_weight[e] - min_weight; // subtract the least weight
            gw.set_user_label(e, string("%.1f", edge_weight[e]));
        }
    }
	// all edge weights will now be positive and dijkstra appliccable
    gw.redraw();


    edge_array<int> matching(g, 0);// matched edges
    double weight_count = 0; // weight count of current matching with minimum/maximum weight

    p("out nodes");
    p(g.outdeg(source_node));
    while (1) {
        int done = dijkstra(g, gw, source_node, target_node,number_of_edges_matched, edge_weight, matching, inmutual_weight, weight_count, h, gw2);

        p("weight_count");
        p(weight_count);
        if (max == 1) { // on both graphwin windows print out maximum weight count
            gw.message(string("maximum weight is %.1f. Number of edges matched is %d.", weight_count, number_of_edges_matched));
            gw2.message(string("maximum weight is %.1f. Number of edges matched is %d.", weight_count, number_of_edges_matched));
        } else if (max == 0) {// on both graphwin windows print out minimum weight count
            
            gw.message(string("minimum weight is %.1f. Number of edges matched is %d.", weight_count, number_of_edges_matched));
            gw2.message(string("minimum weight is %.1f. Number of edges matched is %d.", weight_count, number_of_edges_matched));
        }
        if (done == 0) { // end algorithm

            edge edx;
            p("sum is");
            forall_edges(edx, g) {
                if (matching[edx] == 1) {
                    p(gw2.get_user_label(edx));
                }
            }
    
            break;
        }
    }
}
// to change between maximum and minimum search before starting the algorithm
void changeToMax(GraphWin &gw) {
    if (running == 0) {
        max = 1;
        gw.message("maximum weight");
    }
}

void changeToMin(GraphWin &gw) {
    if (running == 0) {
        max = 0;
        gw.message("minimum weight");
    }
}



// Main program
int main(int argc, char *argv[]) {



    // Create window for illustrating the graph with size 800 x 600 
    GraphWin gw(800, 600);
    gw.display(); // Display window on the screen
    gw.message("minimum weight");
    create_control(); // Display control window
    gw.set_directed(true); // use undirected graph presentation

    gw.add_separator(24);// add a separator
    gw.add_simple_call(changeToMax, string("maximum weight"), 24); // implementing option to change to maximum search
    gw.add_simple_call(changeToMin, string("minimum weight"), 24);// implementing option to change to minimum search

    if (argc > 1) {    // falls Name als Parameter, Graph laden
        gw.read(argv[1]);
    }

    gw.message("You can switch between min/max weight using the Options Menu - Default is minimum");


    gw.edit();   // switch to edit mode until user presses 'done'

    // jetzt holen wir uns den Graphen, den der Benutzer eingegeben oder geladen hat
    graph &g = gw.get_graph();

    if (g.number_of_nodes() == 0) {  // Ende, wenn der Graph leer ist.
        gw.close(); destroy_control();
        exit(1);
    }
	// Zweites Graphwin window
    leda::GRAPH<node, edge> h;
    CopyGraph(h, g);
    p("h has");
    p(h.number_of_nodes());
    GraphWin gw2(h, 800, 600);
    gw2.get_window().set_grid_mode(gw.get_window().get_grid_mode());
    gw2.get_window().set_grid_style(gw.get_window().get_grid_style());
    gw2.get_window().set_grid_dist(gw.get_window().get_grid_dist());
    gw2.display();
    gw2.set_graph(h);
    gw2.redraw();
    gw2.display();


    // Nun zeigen wir fuer alle Knoten den bfsnum-Wert als User-Label an
    // sowie initialisieren den Graphen gelb.
    gw.set_animation_steps(0);
    gw2.set_animation_steps(0);
    gw2.set_flush(false);
	
    node v;
    forall_nodes(v, g) {
        gw.set_label_type(v, user_label);    // User-Label anzeigen (statt Index-Label)
        gw.set_color(v, yellow);

        //node n = gw2.new_node(gw.get_position(v));
        //gw2.set_label_type(h[n], user_label);    
        //gw2.set_width(n, gw.get_width(v));
        //gw2.set_height(n, gw.get_height(v));
        gw2.set_position(v, gw.get_position(v));
        gw2.set_color(v, violet);
        gw2.set_user_label(v, string("%s", gw.get_user_label(v)));

        //gw2_nodes[v] = n;

    }

    gw2.redraw();
    gw2.set_zoom_objects(false);
    gw2.zoom_graph();
    gw2.set_directed(false); // directed auf false setzen



    edge e;
    forall_edges(e, g) {
        gw.set_color(e, yellow);

        //node source = g.source(e);
        //node target = g.target(e);

        //edge new_edge = gw2.new_edge(gw2_nodes[source], gw2_nodes[target]);
        //gw2.set_width(new_edge, 1);
        gw2.set_color(e, violet);
        gw2.set_user_label(e, string("%s", gw.get_user_label(e)));

        //gw2_edges[e] = new_edge;
    }

    gw2.redraw();
    gw2.set_flush(true);


    running = 1;
    weightedmatching(g, gw, h, gw2);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufraeumen und Ende
    gw.close();
    gw2.close();
    destroy_control();
    exit(0);
}
