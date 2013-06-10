
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

#define WAIT 0.3  //Wartezeit

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

graph global_graph;
int compare(const node &a, const node &b) {
    return global_graph.degree(b) - global_graph.degree(a);
}


void coloring(graph &g, GraphWin &gw) {

    int max_color = -1;
    
    global_graph = g;
    g.sort_nodes(compare);
    

    node_array<int> node_color(g, -1);

    
    // todo better - perhaps for all nodes???
    leda::color my_colors[7] = {red, violet, green, blue, orange, cyan, black};

    node n;
    forall_nodes(n, g) {

        gw.set_border_width(n, 10);
        control_wait(WAIT);

        // todo better - perhaps for all nodes???
        int free_colors[7] = {1,1,1,1,1,1,1};
        
        edge e;
        forall_inout_edges(e, n) {
            node opposite_node = g.opposite(n, e);
            if (node_color[opposite_node] != -1) {
                free_colors[node_color[opposite_node]] = 0;
            }
        }

        int next_color = 0;
        while (1) {
            p("check for ");
            p(next_color);
            if (free_colors[next_color] == 1) {
                break;
            }
            next_color++;
        }

        if (next_color > max_color) {
            max_color = next_color;
            gw.message(string("used colors %d", max_color + 1));
        }

        node_color[n] = next_color;
        gw.set_color(n, my_colors[next_color]);
        p(next_color);
        gw.set_user_label(n, string("%d", next_color));
        gw.set_border_width(n, 1);
        control_wait(WAIT);
        
    }
    

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

    }


    edge e;
    forall_edges(e, g) {
        gw.set_color(e, yellow);

    }

    gw.set_flush(true);
    gw.redraw();

    coloring(g, gw);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufraeumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
