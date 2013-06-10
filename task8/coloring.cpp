//Goldberg-Tarjan algorithm to find maximal flow in a directed graph

// nodes in active node queue are displayed in bold
// all nodes are by default depicted in orange
// the currently investigated node is coloured in violet
// current edge is coloured in violet
// nodes that are currently in the active node queue are marked with a bold frame
// the opposite node in the push phase which gets pushed some flow is depicted in cyan
// while relabelling the corresponding nodes is depicted in red
// the minimum neighbour node is coloured in cyan and the respective edge in violet 


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

#define WAIT 0  //Wartezeit
#define WAIT_LONGER 0.5  //Wartezeit

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
using leda::brown;
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
using leda::gw_node_shape;
using leda::rectangle_node;


// iterative function to conduct breadth-first-search (bfs);
// parameters:
//    v: current node
//    g: graph to be searched in (as reference)
//    gw: Diplaying window of graph (as reference)
//    height: Node array with all heights
//    distance: distance from starting node
//    flow: Edge array with all preflows
//    capacity: Edge array with all capacities
void bfs(node v, graph &g, GraphWin &gw, node_array<int> &height, int &distance, edge_array<double> &flow, edge_array<double> &capacity) {

    gw.message("start with a bfs to determine the height");

    queue<node> fifo_queue;    // queue for next bfs node

    fifo_queue.push(v); // add the starting node
    
    //gw.set_user_label(v, string("%d (%d)", distance, height[v]));  // Display bfs number
    //gw.set_color(v, red);                           // Color node in red
    //gw.redraw();                                  // Update displayed graph
    //control_wait(WAIT);                              // Wait for 0.5 sec 
    distance++;


    // loop through all queue elements and add all childs to queue
    // also add some graphical hints
    do {

        v = fifo_queue.pop(); // pop first element of search queue
        //gw.set_color(v, green); // current node of queue, mark it green
        //control_wait(WAIT); // Wait for 0.5sec

        edge e;
        forall_in_edges(e, v) {  // all neighbour nodes of v
            if (flow[e] != capacity[e]) {
                node w = g.opposite(v, e); // neighbour node on the other side of edge e
                if (height[w] < 0) { // if node w has not been visited yet
                    height[w] = height[v]+1; // assign to current node last assigned bfs number incremented by one
                    //gw.set_color(e, red);     // color edge in red     
                    //gw.set_color(w, red);	// color node in red
                    //gw.set_width(e, 2);           
                    //gw.set_user_label(w, string("%d (%d)", distance++, height[w])); // display order first, in brackets distance to starting node
                    fifo_queue.append(w); // add the child node to the queue
                    //control_wait(WAIT);             // Wait for 0.5 sec 
                } else {    // if node w has already been visited
                    if (height[w] < height[v]) { // Backward edge
                        //if (gw.get_color(e) == red){ // check, if edge is the connection between parent and child node
			            //    gw.set_color(e, blue); // color edge in blue
			                //gw.set_width(e, 2);
			                //control_wait(WAIT); // wait for 0.5 sec
		                //} else {
                        //     gw.set_color(e, green);
                        //} 
                    } else { // edge to a node in queue
                        //gw.set_color(e, green); // color edge 
                        //gw.set_width(e, 2);
                        //control_wait(WAIT); // wait for 0.5 sec
                    }
                }
            } else {
                //gw.set_color(e, violet);
                //control_wait(WAIT);
            }
        }
        //gw.set_color(v, blue); // Color node in blue
        //control_wait(WAIT); // wait for 0.5 sec
        //gw.redraw();
	    // Update displayed graph (to ensure correct updation)
    } while (!fifo_queue.empty());  // loop until queue is empty
}

//function to run goldberg algorith
//parameters:
//      graph: graph to be searched in (as reference)
//      gw: displayed window (as reference)
//      source_node: source node
//      target_node: target node
void goldberg(graph &g, GraphWin &gw, node &source_node, node &target_node) {
    
    queue<node> fifo_queue;                     // Queue with active nodes

    edge_array<double> capacity(g);             // saves capacity of edges
    edge_array<double> flow(g, 0);              // saves pre-flow of edges
    node_array<int> height(g, -1);              // height of the nodes

    // get capacity from the edges
    edge e;
    forall_edges(e, g) { 
        string s = gw.get_user_label(e);
        leda::string_istream I(s);
        I >> capacity[e];
    }

    // init flow from source to neighbours
    // for all edges directed away from source node set the pre-flow to the capacity flow
    forall_out_edges(e, source_node) {

        node opposite_node = g.opposite(source_node, e);

        flow[e] = capacity[e];

        if (opposite_node != target_node) {
            fifo_queue.append(opposite_node);
            gw.set_border_width(opposite_node, 5);
        }
    }


    // init height
    height[target_node] = 0; // assign height 0 to target node
   
    int distance = 0;
    bfs(target_node, g, gw, height, distance, flow, capacity); // run bfs

    //Nodes that cannot reach target node are deleted
    node n;

    node_array<int> delteted_nodes(g, 0);
   
    forall_nodes(n, g) {

        if (height[n] == -1 && n != source_node) {
            gw.del_node(n);
            delteted_nodes[n] = 1;
        }
       
    }

    gw.update_graph();


    height[source_node] = g.number_of_nodes(); // assign height equal to number of nodes to source node
   

        // init excess
    node_array<double> excess(g); // nodes array to save excess of all nodes

    gw.set_flush(false);
   
    forall_nodes(n, g) { // compute the excess for all nodes in graph
        double current_excess = 0;
        
        edge e;
        forall_out_edges(e, n) {
            current_excess -= flow[e];
        }
        forall_in_edges(e, n) {
            current_excess += flow[e];
        }
        excess[n] = current_excess;
        
        if (n == target_node) {
            gw.set_user_label(n, string("t (%d)", height[n]));
        } else if (n == source_node) {
            gw.set_user_label(n, string("s (%d)", height[n]));
        } else {
            gw.set_user_label(n, string("%.0f (%d)", excess[n], height[n]));
        }         
        gw.set_color(n, orange);
    }


    // display edges user labels
    forall_edges(e, g) {
        gw.set_user_label(e, string("%.0f/%.0f", flow[e], capacity[e]));
        gw.set_color(e, orange);
        gw.set_width(e, 2);
    }

    gw.set_flush(true);
    gw.redraw();

    control_wait(WAIT_LONGER * 3);

    int round = 0;
    do {
        //gw.message(string("round is %d", round));
        p("round ...");
        p(round++);

        node current_node = fifo_queue.pop(); // pop node from active node queue

        if (delteted_nodes[current_node] == 1) {
            continue;
        }

        gw.set_color(current_node, violet);
        control_wait(WAIT_LONGER);
        
       // Conduct push phase
        if (excess[current_node] > 0) {// only consider active nodes
            p("suche nach ausgehenden");
            gw.message("push");
            // check ob es zulässige ausgehende kante gibt
            edge e;
            forall_out_edges(e, current_node) {
                p("next edge");
                node opposite_node = g.opposite(current_node, e);
                if (excess[current_node] > 0) {
                    double opposite_excess = excess[opposite_node];
                    if (flow[e] < capacity[e] && height[current_node] == height[opposite_node] + 1) {//condition for push
                        gw.set_color(e, violet);
                        gw.set_color(opposite_node, cyan);
                        control_wait(WAIT_LONGER);

                        // push 
                        // forward edge
                        double min;
                        double res_capacity = capacity[e] - flow[e]; // compute residual capacity
                        min = std::min(excess[current_node], res_capacity);//calculate delta 
                        flow[e] = flow[e] + min;// calculate new preflow


                        excess[current_node] = excess[current_node] - min;//update excess
                        excess[opposite_node] = excess[opposite_node] + min;// update excess

                        gw.set_user_label(e, string("%.0f/%.0f", flow[e], capacity[e]));
                        gw.set_user_label(current_node, string("%.0f (%d)", excess[current_node], height[current_node]));

                        // update user labels
                        if (opposite_node == target_node) {
                            gw.set_user_label(opposite_node, string("t (%d)", height[opposite_node]));
                        } else if (opposite_node == source_node) {
                            gw.set_user_label(opposite_node, string("s (%d)", height[opposite_node]));
                        } else {
                            gw.set_user_label(opposite_node, string("%.0f (%d)", excess[opposite_node], height[opposite_node]));
                        }

                        p("höhe ist");
                        p(height[current_node]);
                        p(height[opposite_node]);

                        // add only node w to active node queue if active and neither target nor source node
                        if (opposite_excess <= 0 && excess[opposite_node] > 0 && opposite_node != target_node && opposite_node != source_node){
                            fifo_queue.append(opposite_node);
                            gw.set_border_width(opposite_node, 5);
                        }
		                gw.set_color(e, orange); 
                        gw.set_color(opposite_node, orange);
                        control_wait(WAIT_LONGER);
                    }
            //update depicted colors
                } else {
		            gw.set_color(e,orange);
                    break;
                }

            }
        }
// backward edge
        if (excess[current_node] > 0) {
            // check ob es zulässige ausgehende kante gibt
            p("suche nach eingehenden");
            edge e;
            forall_in_edges(e, current_node) {
                node opposite_node = g.opposite(current_node, e);
                p("next edge");
                if (excess[current_node] > 0) {
                    double opposite_excess = excess[opposite_node];
                    if (flow[e] > 0 && height[current_node] == height[opposite_node] + 1) {//condition for push
                        gw.set_color(e, violet);
                        gw.set_color(opposite_node, cyan);
                        control_wait(WAIT_LONGER);

                        // push 
                        // backward edge
                        double res_capacity = flow[e];// compute residual capacity
                        double min = std::min(excess[current_node], res_capacity);// calculate delta
                        
                        flow[e] = flow[e] - min;// update pre flow

                        excess[current_node] = excess[current_node] - min;// update excess
                        excess[opposite_node] = excess[opposite_node] + min;//update excess
                        
                        // update user labels
                        gw.set_user_label(e, string("%.0f/%.0f", flow[e], capacity[e]));
                        gw.set_user_label(current_node, string("%.0f (%d)", excess[current_node], height[current_node]));
                        if (opposite_node == target_node) {
                            gw.set_user_label(opposite_node, string("t (%d)", height[opposite_node]));
                        } else if (opposite_node == source_node) {
                            gw.set_user_label(opposite_node, string("s (%d)", height[opposite_node]));
                        } else {
                            gw.set_user_label(opposite_node, string("%.0f (%d)", excess[opposite_node], height[opposite_node]));
                        }

                        // condition for appending to active node queue
                        if (opposite_excess <= 0 && excess[opposite_node] > 0 && opposite_node != target_node && opposite_node != source_node) {
                            fifo_queue.append(opposite_node);
                            
                            gw.set_border_width(opposite_node, 5);
                        }
		                gw.set_color(e, orange); 
                        gw.set_color(opposite_node, orange);
                        control_wait(WAIT_LONGER);
                    }
            
                } else {
		            gw.set_color(e, orange);
                    break;
                } 
            }

        }

        if (excess[current_node] > 0) {  // relabelling

            int min = numeric_limits<int>::max();// set inital value to integer max value to find minimum height
            p("relabel");
            gw.message("relabel");
            gw.set_color(current_node, red);
            control_wait(WAIT_LONGER);
            node min_node;
            edge min_edge;

            //check for edge in residual graph
            //find edge with minimal height
            forall_out_edges(e, current_node) {
                if (flow[e] < capacity[e]) {
                    p("1 found edge");
                    node opposite_node = g.opposite(e, current_node); 
                    int current_min = height[opposite_node] + 1;
                    p(current_min);
                    p(min);
                    if (current_min < min) {// if there is a lower height
                        min = current_min;
                        min_node = opposite_node;
                        min_edge = e;
                        p("1 min adjustment");
                    }
                }

            }

            forall_in_edges(e, current_node) {
                if (flow[e] > 0) {
                    p("1 found edge");
                    node opposite_node = g.opposite(e, current_node); 
                    int current_min = height[opposite_node] + 1;
                    p(current_min);
                    p(min);
                    if (current_min < min) {
                        min = current_min;
                        p("1 min adjustment");
                        min_node = opposite_node;
                        min_edge = e;
                    }

                }
            }
            height[current_node] = min;// update height of current with minimal height found +1

            gw.set_color(min_node, cyan);
            gw.set_color(min_edge, violet);

            gw.set_user_label(current_node, string("%.0f (%d)", excess[current_node], height[current_node]));

            control_wait(WAIT_LONGER);

            fifo_queue.append(current_node);
            gw.set_color(min_node, orange);
            gw.set_color(min_edge, orange);

        } else {
            gw.set_border_width(current_node, 1);
        }

        gw.set_color(current_node, orange);
        control_wait(WAIT_LONGER);



    } while(!fifo_queue.empty());// run algorithm until active node queue is empty

    //Compute maximum flow
    int maximum_flow = 0;

    forall_in_edges(e, target_node) {
        maximum_flow += flow[e];
    } 
    gw.message(string("Maximum flow is %d", maximum_flow));   


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

    gw.set_flush(false);

    node v;
    forall_nodes(v, g) {
        gw.set_label_type(v, user_label);    // User-Label anzeigen (statt Index-Label)
        gw.set_color(v, yellow);

        gw.set_shape(v, rectangle_node);
        gw.set_width(v, 90);
    }


    edge e;
    forall_edges(e, g) {
        gw.set_color(e, yellow);

    }

    gw.set_flush(true);
    gw.redraw();

    // Nutzer darf start node wählen
    node source_node;
    while ((source_node = gw.read_node()) == NULL);

    // Nutzer darf target node wählen
    node target_node;
    do {
        while ((target_node = gw.read_node()) == NULL);
    } while(target_node == source_node);


    goldberg(g, gw, source_node, target_node);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufraeumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
