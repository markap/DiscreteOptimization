// Animation von Springembedder
// Beginnend mit einer DFS, um die einzelnen Komponenten des Graphen zu ermitteln
// (unbesuchter Teil wird gelb (Default) angezeigt, erledigter Teil blau,
// noch in Bearbeitung befindlicher Teil rot, Rückwärtskanten grün)
// Falls nicht zusammenhänged, wird ein temporärer Verbindungsknoten eingefügt
// Anschließend wird solang der Springembedder Algorithmus ausgeführt, bis die Gesamtkraft gegen 0 geht
// (abhängig von Anzahl der Knoten und Anzalh der Kanten)


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
#define VISUALIZE 25 //Anzahl der Runden, nach denen visualisiert wird
#define FORCE_STOP 0.5 //zur Berechnung der Abbruchbedingung


#define C0 2700 // gibt an, wie stark sich zwei Knoten abstoßen
#define C1 0.2 // Federkonstante, je größer der Wert, ge größer die Anziehung bei einer Distanz größer der Federlänge und je größer die Abstoßung bei einer Distanz kleiner der Federlänge
// Quotient von C0/C1 = 13.500, damit die Verschiebung der Nodes im Graphen ein akzeptables Ergebnis abwirft
// Wir setzen den Faktor zur Abstoßung C0 gemessen zur Anziehung durch C1 also sehr viel größer
#define DELTA 0.7 // "Stärke" der Kraft, wie stark beeinflussen F0 und F1 die Neupositionierung von einer Node, je größer je stärker wird die Node verschoben
#define D_SQUARED 16000 // Maximale Distanz zum Quadrat, bei der Kraft zwischen Nodes noch berücksichtigt wird
#define L 35 // Länge der Feder, legt den Abstand zwischen Nodes fest, an dem sich der resultierende Graph orientiert

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

// rekursive Funktion zur Realisierung der Tiefensuche und Bestimmung der Komponenten (dfs);
// Parameter:
//    parent: Knoten, von dem aus der aktuelle Knoten besucht wird
//            (parent == v, falls der aktuelle Knoten der Startknoten ist)
//    v: aktueller Knoten
//    g: zu durchsuchender Graph (als Referenz)
//    gw: Darstellungsfenster des Graphen (als Referenz)
//    dfsnum: Feld zur Zuordnung von DFS-Nummern zu Knoten (als Referenz)
//    akt: nächste freie Nummer (als Referenz)
//	  components: Zuordnung der Nodes auf jeweilige Komponente
//	  index: Komponentenindex
void dfs(node parent, node v, graph &g, GraphWin &gw, node_array<int> &dfsnum, int &akt, node_array<int> &components, int &index) {
    
	gw.message(string("DFS to investigate if graph is composed of subcomponents")); // Informiere Benutzer

	
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


double f_zero_x(double squared_euclidean_metric, int xv, int xu, int yv, int yu) { // F0 in x-Richtung berechnen
    return (C0/squared_euclidean_metric) * ((xv - xu)/sqrt(squared_euclidean_metric));
}


double f_zero_y(double squared_euclidean_metric, int xv, int xu, int yv, int yu) { // F0 in y-Richtung berechnen
    return (C0/squared_euclidean_metric) * ((yv - yu)/sqrt(squared_euclidean_metric));
}


double f_one_x(int xv, int xu, int yv, int yu) { // F1 in x-Richtung berechnen
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2);
    return -C1 *(sqrt(squared_euclidean_metric) - L) * ((xv - xu)/sqrt(squared_euclidean_metric));
}


double f_one_y(int xv, int xu, int yv, int yu) { // F1 in y-Richtung berechnen
    double squared_euclidean_metric = pow(xv - xu, 2) + pow(yv -yu, 2);
    return -C1 *(sqrt(squared_euclidean_metric) - L) * ((yv - yu)/sqrt(squared_euclidean_metric));
}


double distance(int xv, int xu, int yv, int yu) { //euklidische Distanz^2 berechnen
    return pow(xv - xu, 2) + pow(yv -yu, 2);
}

//Springembedder zur Verschiebung der Nodes je nach wirkenden Kräften
// 		Parameter:
//   		 g: zu bearbeitender Graph (als Referenz)
//   		 gw: Darstellungsfenster des Graphen (als Referenz)			
void springembedder(graph &g, GraphWin &gw) {
    


    
    node_array<int> components(g, 0); // gibt an, zu welcher Graphkomponente node gehört, 0 bedeutet bisher keine Komponente zugeordnet
    int component_index = 0;		  // zum Durchiterieren der Komponenten
    int akt = 0;					  // node counter
    list<node> one_node_per_component;//zum Speichern einer Node einer jeden Komponente


    do {
        node v;
        forall_nodes(v, g) {
            if (components[v] == 0) {
                node_array<int> dfsnum(g, -1); //initialisiere node_array für Tiefensuche
                component_index++; //component index inkrementieren
                p(component_index);
                dfs(v, v, g, gw, dfsnum, akt, components, component_index); // Tiefensuche
                one_node_per_component.append(v); //Node der aktuellen Komponente in Liste appenden
                break;
            }
        }

    } while(akt < g.number_of_nodes()); // so lange, wie noch alle nodes nicht besucht sind

    node new_node = NULL; // neue Node zur Verbindung mit jeder einzelnen Komponente

    int forceindex = 1;
    if (component_index > 1) {
        forceindex = 1.5;
    }

    double force_stop = FORCE_STOP * g.number_of_nodes() * forceindex; //Abbruchzähler für Anzahl Durchläufe; verwendet für Abbruchbedingung
    p("stop is ");
    p(force_stop);

    p(component_index);
    if (component_index > 1) {
		gw.message(string("Add a node to connect all components")); // Informiere Benutzer
        point p1 = gw.get_position(one_node_per_component.front());
        new_node = gw.new_node(point(p1.xcoord() + 10, p1.ycoord() + 10)); // positioniere zusätzliche Node 10 in X-Richtung und 10 in Y-Richtung von Node einer bestimmten Komponente entfernt
        node v;
        forall(v, one_node_per_component) {
            gw.new_edge(v, new_node); //verbinde alle Komponenten über eine Node mit der zusätzlichen Node

        }

        gw.update_graph(); // update Graph
    } else {
        gw.message(string("Graph is connected"));
    }
    control_wait(0.7);

    edge e;
    forall_edges(e, g) {
        gw.set_color(e, blue);
    }

    int visualize = 1;

    node_array<point> node_position(g); //zum Speichern der Koordinaten der Nodes
    node n;
    forall_nodes(n, g) {
        node_position[n] = gw.get_position(n);//speichere alle Koordinaten aller Nodes
    }

    double total_force; // Gesamtkraft
    do {
        node v;
        node u;

        node_array<point> node_force(g); // zum Speichern der einzelnen Kräfte auf jede Node

        double x_force_global = 0.0; // Gesamtkraft in X-Richtung
        double y_force_global = 0.0; // Gesamtkraft in Y-Richtung

        forall_nodes(v, g) {

            double x_force = 0;		// Setze Kraft in X-Richtung für Node default auf 0
            double y_force = 0;		// Setze Kraft in Y-Richtung für Node default auf 0
            
            // repulsive force
            // calculate force zero
            // sum of forces to all other nodes
            forall_nodes(u, g) {
                if (u != v) {
                    point pu = node_position[u]; // Position von Node u
                    point pv = node_position[v]; // Position von Node v
                    double ux = pu.xcoord(); 	 // X-Koordinate von Node u
                    double uy = pu.ycoord();	 // Y-Koordinate von Node u
                    double vx = pv.xcoord();	 // X-Koordinate von Node v
                    double vy = pv.ycoord();	 // Y-Koordinate von Node v

                    // clipping
                    double squared_euclidean_metric = distance(vx, ux, vy, uy);
                    if (squared_euclidean_metric < D_SQUARED) { // Durch Clipping werden nur Nodes innerhalb der Distanz 400 berücksichtigt

                        x_force += f_zero_x(squared_euclidean_metric, vx, ux, vy, uy);  //Addiere F0 zur Kraft wirkend in X-Richtung auf Node v
                        y_force += f_zero_y(squared_euclidean_metric, vx, ux, vy, uy);  //Addiere F0 zur Kraft wirkend in Y-Richtung auf Node v
                    } else {
                        p("clipping"); // Es wird geclippt.
                    }
                    //p(f_zero_x(vx, ux, vy, uy));
                    //p(f_zero_y(vx, ux, vy, uy));
                    //p("-----------");

                }
            }

            // sum to all neighbour nodes
            edge e;
            forall_inout_edges(e, v) { //Berechne F1 für alle Nodes
                node u = g.opposite(v, e);

                point pu = node_position[u]; // Position von Node u
                point pv = node_position[v]; // Position von Node v
                double ux = pu.xcoord(); // X-Koordinate von Node u
                double uy = pu.ycoord(); // Y-Koordinate von Node u
                double vx = pv.xcoord(); // X-Koordinate von Node v
                double vy = pv.ycoord(); // Y-Koordinate von Node v

                x_force += f_one_x(vx, ux, vy, uy);  //Addiere F0 zur Kraft wirkend in X-Richtung auf Node v
                y_force += f_one_y(vx, ux, vy, uy);  //Addiere F0 zur Kraft wirkend in Y-Richtung auf Node v

            }

            node_force[v] = point(x_force, y_force);  // Kraft wirkend auf eine Node

            x_force_global += fabs(x_force); //Addiere zur Gesamtkraft in X-Richtung
            y_force_global += fabs(y_force); //Addiere zur Gesamtkraft in Y-Richtung

        }

        total_force = x_force_global + y_force_global; // Gesamtkraft

        gw.message(string("iteration %d, total force is %.2f, done if force is smaller than  %.2f", visualize, total_force, force_stop)); // Informiere Benutzer

        node n;
        forall_nodes(n, g) {
            
            point pn = node_position[n]; //Position von Node n
            point pf = node_force[n]; // Kraft wirkend auf Node n

            double new_x = pn.xcoord() + DELTA * pf.xcoord(); // Neue X-Koordinate von Node n
            double new_y = pn.ycoord() + DELTA * pf.ycoord(); // Neue Y-Koordinate von Node n

            node_position[n] = point(new_x, new_y); // Neue Position von Node n
            if (visualize % VISUALIZE == 0 || total_force < force_stop) { // Visualisiere nur jede <VISUALIZE> Runde
                gw.set_position(n, node_position[n]); // Beim Visualisieren werden die neuen Nodepositionen auch in GraphWin gesetzt
            }
        }

        if ((visualize % VISUALIZE == 0 || total_force < force_stop) && g.number_of_nodes() > 15) { // Bei Graphen mit mehr als 15 Nodes wird ein Zoom durchgeführt
            gw.set_animation_steps(1); // Schnellere Animation für Zoomen
            gw.zoom_graph();
            gw.set_animation_steps(20); // Langsamere Animation für Repositionierung
        }

        visualize++; // inkrementiere visualize

        p(total_force);
    } while (total_force > force_stop); // Abbruchbedingung, falls force_stop von der Gesamtkraft unterschritten wird


    if (new_node != NULL) {
        gw.del_node(new_node); //Entferne die zusätzliche Node, die die Komponenten miteinander verbindet
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
