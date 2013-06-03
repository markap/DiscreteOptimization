
#include <iostream>
#include <math.h>
#include <climits>
#include <LEDA/graphics/graphwin.h>
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

#define WAIT 2  //Wartezeit

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
using leda::gw_font_type;
using leda::roman_font;
using leda::bold_font;


// iterative function to conduct breadth-first-search (bfs);
// parameters:
//    v: current node
//    g: graph to be searched in (as reference)
//    gw: Diplaying window of graph (as reference)
//    distance: distance from starting node
void bfs(node v, graph &g, GraphWin &gw, node_array<int> &height, int &distance, edge_array<double> &flow, edge_array<double> &capacity) {

    queue<node> fifo_queue;    // queue for next bfs node

    fifo_queue.push(v); // add the starting node
    
    gw.set_user_label(v, string("%d (%d)", distance, height[v]));  // Display bfs number
    gw.set_color(v, red);                           // Color node in red
    gw.redraw();                                  // Update displayed graph
    control_wait(WAIT);                              // Wait for 0.5 sec 
    distance++;


    // loop through all queue elements and add all childs to queue
    // also add some graphical hints
    do {

        v = fifo_queue.pop(); // pop first element of search queue
        gw.set_color(v, green); // current node of queue, mark it green
        control_wait(WAIT); // Wait for 0.5sec

        edge e;
        forall_in_edges(e, v) {  // all neighbour nodes of v
            if (flow[e] != capacity[e]) {
                node w = g.opposite(v, e); // neighbour node on the other side of edge e
                if (height[w] < 0) { // if node w has not been visited yet
                    height[w] = height[v]+1; // assign to current node last assigned bfs number incremented by one
                    gw.set_color(e, red);     // color edge in red     
                    gw.set_color(w, red);	// color node in red
                    gw.set_width(e, 2);           
                    gw.set_user_label(w, string("%d (%d)", distance++, height[w])); // display order first, in brackets distance to starting node
                    height[w] = distance;
                    fifo_queue.append(w); // add the child node to the queue
                    control_wait(WAIT);             // Wait for 0.5 sec 
                } else {    // if node w has already been visited
                    if (height[w] < height[v]) { // Backward edge
                        if (gw.get_color(e) == red){ // check, if edge is the connection between parent and child node
			                gw.set_color(e, blue); // color edge in blue
			                gw.set_width(e, 2);
			                control_wait(WAIT); // wait for 0.5 sec
		                }
                    } else { // edge to a node in queue
                        gw.set_color(e, green); // color edge 
                        gw.set_width(e, 2);
                        control_wait(WAIT); // wait for 0.5 sec
                    }
                }
            } else {
                gw.set_color(e, violet);
                control_wait(WAIT);
            }
        }
        gw.set_color(v, blue); // Color node in blue
        control_wait(WAIT); // wait for 0.5 sec
        gw.redraw();
	    // Update displayed graph (to ensure correct updation)
    } while (!fifo_queue.empty());  // loop until queue is empty
}


void goldberg(graph &g, GraphWin &gw, node &source_node, node &target_node) {
    
    queue<node> fifo_queue;

    edge_array<double> capacity(g);
    edge_array<double> flow(g, 0);
    node_array<int> height(g, -1);

    // get capacity from the edges
    edge e;
    forall_edges(e, g) { 
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> capacity[e];
    }

    // init flow from source to neighbours
    forall_out_edges(e, source_node) {

        node opposite_node = g.opposite(source_node, e);

        flow[e] = capacity[e];

        if (opposite_node != target_node) {
            fifo_queue.append(opposite_node);
        }
    }

    // init height
    height[target_node] = 0;
    height[source_node] = g.number_of_nodes();
    int distance = 0;
    bfs(target_node, g, gw, height, distance, flow, capacity);


    // init excess
    node_array<double> excess(g);
    node n;
    forall_nodes(n, g) {
        double current_excess = 0;
        // wahrscheinlich vorher schon speichern!!!
        edge e;
        forall_out_edges(e, current_node) {
            current_excess -= flow[e];
        }
        forall_in_edges(e, current_node) {
            current_excess += flow[e];
        }
        excess[n] = current_excess;
    }


    do {
        node current_node = fifo_queue.pop(); 


        if (current_excess > 0) {
            // check ob es zulässige ausgehende kante gibt
            int push = 0;
            edge e;
            forall_out_edges(e, current_node) {
                node opposite_node = g.opposite(current_node, e);
                if (flow[e] < capacity[e] && height[current_node] == height[opposite_node] + 1) {
                    push = 1;
                    // push 
                    // vorwärtskante
                    double min;
                    if (capacity[e] > flow[e]) {
                        double res_capacity = capacity[e] - flow[e];
                        min = std::min(current_excess, res_capacity);
                        flow[e] = flow[e] + min;


                    } else { // rückwärtskante
                        double res_capacity = flow[e];
                        min = std::min(current_excess, res_capacity);
                        
                        flow[e] = flow[e] - min;
                    }

                    excess[v] = excess[v] - min;
                    excess[opposite_node] = excess[opposite_node] + min;

                    if (excess[opposite_node] > 0) {
                        fifo_queue.append(opposite_node);
                    }
                }
            
            }

            if (push == 0) {  // relabel

            }
        }



    } while(!fifo_queue.empty());

}


// Main program
int main(int argc, char *argv[]) {

    // Create window for illustrating the graph with size 800 x 600 
    GraphWin gw(800, 600);
    gw.display(); // Display window on the screen
    create_control(); // Display control window
    gw.set_directed(false); // use undirected graph presentation
    gw.set_animation_steps(40);

    if (argc > 1) {    // falls Name als Parameter, Graph laden
        gw.read(argv[1]);
    }

    gw.edit();   // switch to edit mode until user presses 'done'

    // jetzt holen wir uns den Graphen, den der Benutzer eingegeben oder geladen hat
    graph &g = gw.get_graph();

    if (g.number_of_nodes() == 0) {  // Ende, wenn der Graph leer ist.
        gw.close(); destroy_control();
        exit(1);
    }

    node v;
    forall_nodes(v, g) {
        gw.set_label_type(v, user_label);    // User-Label anzeigen (statt Index-Label)
        gw.set_color(v, yellow);

    }


    edge e;
    forall_edges(e, g) {
        gw.set_color(e, yellow);

    }

    // Nutzer darf start node wählen
    node source_node;
    while ((source_node = gw.read_node()) == NULL);

    // Nutzer darf target node wählen
    node target_node;
    while ((target_node = gw.read_node()) == NULL);


    goldberg(g, gw, source_node, target_node);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufraeumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
