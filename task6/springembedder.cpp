
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

#define WAIT 0 
#define VISUALIZE 30 
#define FORCE_STOP 0.5


#define C0 2700//4//1 
#define C1 0.2//2//6 
#define DELTA 0.7
#define D 400
double len;

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
    control_wait(WAIT);                              // 0.5 sec warten
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
                control_wait(WAIT);             // 0.5 sec warten
            } else {    // Knoten w war schon besucht
                if (dfsnum[w] < dfsnum[v]) { // Rückwärtskante
                    gw.set_color(e, green);    // grün färben
                    control_wait(WAIT);         // 0.5 sec warten
                }
            }
        }
    }
    gw.set_color(v, blue); // Knoten blau färben
    gw.redraw(); // Darstellung aktualisieren (zur Sicherheit)
}


double f_zero_x(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2);
    return (C0/squared_euclidean_metric) * ((xv - xu)/sqrt(squared_euclidean_metric));
}


double f_zero_y(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2);
    return (C0/squared_euclidean_metric) * ((yv - yu)/sqrt(squared_euclidean_metric));
}


double f_one_x(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2);
    return -C1 *(sqrt(squared_euclidean_metric) - len) * ((xv - xu)/sqrt(squared_euclidean_metric));
}


double f_one_y(int xv, int xu, int yv, int yu) {
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2);
    return -C1 *(sqrt(squared_euclidean_metric) - len) * ((yv - yu)/sqrt(squared_euclidean_metric));
}


double distance(int xv, int xu, int yv, int yu) {
    return pow(xv - xu, 2) + pow(yv -yu, 2);
}


void springembedder(graph &g, GraphWin &gw) {
    

    len = 35;//L / g.number_of_nodes(); 
    p("l is ");
    p(len);
    
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

    node new_node = NULL;

    int forceindex = 1;
    if (component_index > 1) {
        forceindex = 1.5;
    }

    double force_stop = FORCE_STOP * g.number_of_nodes() * forceindex;
    p("stop is ");
    p(force_stop);

    p(component_index);
    if (component_index > 1) {
        point p1 = gw.get_position(one_node_per_component.front());
        new_node = gw.new_node(point(p1.xcoord() + 10, p1.ycoord() + 10));
        node v;
        forall(v, one_node_per_component) {
            gw.new_edge(v, new_node);

        }

        gw.update_graph();
    }

    edge e;
    forall_edges(e, g) {
        gw.set_color(e, blue);
    }

    int visualize = 1;

    node_array<point> node_position(g);
    node n;
    forall_nodes(n, g) {
        node_position[n] = gw.get_position(n);
    }

    double total_force;
    do {
        node v;
        node u;

        node_array<point> node_force(g);

        double x_force_global = 0.0;
        double y_force_global = 0.0;

        forall_nodes(v, g) {

            double x_force = 0;
            double y_force = 0;
            
            // repulsive force
            // calculate force zero
            // sum of forces to all other nodes
            forall_nodes(u, g) {
                if (u != v) {
                    point pu = node_position[u];
                    point pv = node_position[v];
                    double ux = pu.xcoord();
                    double uy = pu.ycoord();
                    double vx = pv.xcoord();
                    double vy = pv.ycoord();

                    // clipping
                    if (distance(vx, ux, vy, uy) < pow(D, 2)) {

                        x_force += f_zero_x(vx, ux, vy, uy); 
                        y_force += f_zero_y(vx, ux, vy, uy); 
                    } else {
                        p("clipping");
                    }
                    //p(f_zero_x(vx, ux, vy, uy));
                    //p(f_zero_y(vx, ux, vy, uy));
                    //p("-----------");

                }
            }

            // sum to all neighbour nodes
            edge e;
            forall_inout_edges(e, v) {
                node u = g.opposite(v, e);

                point pu = node_position[u];
                point pv = node_position[v];
                double ux = pu.xcoord();
                double uy = pu.ycoord();
                double vx = pv.xcoord();
                double vy = pv.ycoord();

                x_force += f_one_x(vx, ux, vy, uy); 
                y_force += f_one_y(vx, ux, vy, uy); 

            }

            node_force[v] = point(x_force, y_force);

            x_force_global += fabs(x_force);
            y_force_global += fabs(y_force);

        }

        total_force = x_force_global + y_force_global;

        gw.message(string("iteration %d, total force is %.1f, done if force is smaller than  %.1f", visualize, total_force, force_stop));

        node n;
        forall_nodes(n, g) {
            
            point pn = node_position[n];
            point pf = node_force[n];

            double new_x = pn.xcoord() + DELTA * pf.xcoord();
            double new_y = pn.ycoord() + DELTA * pf.ycoord();

            node_position[n] = point(new_x, new_y);
            if (visualize % VISUALIZE == 0 || total_force < force_stop) {
                gw.set_position(n, node_position[n]);
            }
        }

        if ((visualize % VISUALIZE == 0 || total_force < force_stop) && g.number_of_nodes() > 15) {
            gw.set_animation_steps(0);
            gw.zoom_graph();
            gw.set_animation_steps(40);
        }

        visualize++;

        p(total_force);
    } while (total_force > force_stop);


    if (new_node != NULL) {
        gw.del_node(new_node);
    }



}


// Main program
int main(int argc, char *argv[]) {

    p(C0);
    p(C1);
    p(DELTA);

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


    springembedder(g, gw);

    gw.acknowledge("Done");
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufraeumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
