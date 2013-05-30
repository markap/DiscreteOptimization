
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

#define WAIT 0.7 

#define C0 = 1
#define C1 = 1
#define L = 1

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


void dfs(node parent, node v, graph &g, GraphWin &gw, node_array<int> &dfsnum, int &akt, node_array<int> &components, int &index) {
    dfsnum[v] = akt++;                              // DFS-Nummer zuweisen
    gw.set_user_label(v, string("%d", dfsnum[v]));  // DFS-Nummer anzeigen
    gw.set_color(v, red);                           // Knoten rot färben
    gw.redraw();                                    // Darstellung aktualisieren
    control_wait(0.5);                              // 0.5 sec warten
    components[v] = index;

    // GraphWin-Graphen sind immer gerichtet, auch wenn auf dem Bildschirm
    // keine Pfeile sichtbar sind

    edge e;
    forall_inout_edges(e, v) {  // alle Nachbarkanten von v
        
        node w = g.opposite(v, e); // Knoten am anderen Ende von e
        if (w != parent) {     // die Kante zum parent ignorieren wir
            if (dfsnum[w] < 0) { // falls Knoten w noch nicht besucht
                gw.set_color(e, red);          // Kante zu w rot färben
                gw.set_width(e, 2);            // Kante fett anzeigen
                dfs(v, w, g, gw, dfsnum, akt, components, index); // rekursiver Aufruf fuer w
                gw.set_color(e, blue);         // Kante blau färben
                control_wait(0.5);             // 0.5 sec warten
            } else {    // Knoten w war schon besucht
                if (dfsnum[w] < dfsnum[v]) { // Rückwärtskante
                    gw.set_color(e, green);    // grün färben
                    control_wait(0.5);         // 0.5 sec warten
                }
            }
        }
    }
    gw.set_color(v, blue); // Knoten blau färben
    gw.redraw(); // Darstellung aktualisieren (zur Sicherheit)
}


double f_zero_x(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2)
    return (C0/squared_euclidean_metric) * ((xv - xu)/sqrt(squared_euclidean_metric));
}


double f_zero_y(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2)
    return (C0/squared_euclidean_metric) * ((yv - yu)/sqrt(squared_euclidean_metric));
}


double f_one_x(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2)
    return -C1 *(sqrt(squared_euclidean_metric) - L) * ((xv - xu)/sqrt(squared_euclidean_metric));
}


double f_one_y(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2)
    return -C1 *(sqrt(squared_euclidean_metric) - L) * ((yv - yu)/sqrt(squared_euclidean_metric));
}



void springembedder(graph &g, GraphWin &gw) {
    
    node_array<int> components(g, 0);
    int component_index = 0;
    int akt = 0;
    list<node> one_node_per_component;


    do {
        node v;
        forall_nodes(v, g) {
            if (components[v] == 0) {
                node_array<int> dfsnum(g, -1);
                component_index++;
                p(component_index);
                dfs(v, v, g, gw, dfsnum, akt, components, component_index);
                one_node_per_component.append(v);
                break;
            }
        }

    } while(akt < g.number_of_nodes());

    p(component_index);
    if (component_index > 1) {
        point p1 = gw.get_position(one_node_per_component.front());
        node new_node = gw.new_node(point(p1.xcoord() + 10, p1.ycoord() + 10));
        node v;
        forall(v, one_node_per_component) {
            gw.new_edge(v, new_node);

        }

        gw.update_graph();
    }


    for (int i = 0; i < 5; i++) {
        node v;
        forall_nodes(v, g) {
            // sum of forces to all other nodes
            // sum to all neighbour nodes

        }
    } 



}


// Main program
int main(int argc, char *argv[]) {

    // Create window for illustrating the graph with size 800 x 600 
    GraphWin gw(800, 600);
    gw.display(); // Display window on the screen
    create_control(); // Display control window
    gw.set_directed(false); // use undirected graph presentation

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


    springembedder(g, gw);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufraeumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
