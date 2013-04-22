// Animation of topological search in a directed graph
// by assigning a distinct number to each node such that
// If (u,v) in E, u's number has to be smaller than v's. 
// In case a cycle is found, search is aborted and cycle is depicted in green

#include <iostream>
#include <climits>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/color.h>
#include <LEDA/system/basic.h>

#include "control.h" // Control window

#define p(str) ( std::cout << str << std::endl ) // print helper


#define WAIT 0.5

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
using leda::list;

// recursive function for applying depth first search (dfs) on current and thus conducting topological sort algorithm
// parameters:
//    parent: Node which is used to visit current node
//            (parent == v, if current node is starting node)
//    v: current node
//    g: graph to be searched in (as reference)
//    gw: Diplayed window of graph (as reference)
//    dfsnum: Field for assigning distinct search numbers to nodes (as reference)
//    akt: current search number (as reference)
//    all_nodes: all nodes the graph contains (as reference)
//    cycle_nodes: Nodes potentially located on a cycle (as reference)
//    cycle_edges_nodes: Edges potentially located on a cycle, assignment node - edge, to loop through illustrate the cycle (as reference)
//
void dfs(node parent, node v, graph &g, GraphWin &gw, node_array<int> &globally_visited, node_array<int> &temporary_visited, int &akt, list<node> &all_nodes, list<node> &cycle_nodes, node_array<edge> &cycle_edges_nodes) {
    // check first if there is a cycle
    if (temporary_visited[v] > -1) {

        // now we have a list of nodes containing the cycle
        // we should mark the cycle nodes and the edges


        // first step: find all the nodes which are really on the cycle
        node tmp_node;

        int found = 0; // cycle flag
        forall (tmp_node, cycle_nodes) { // drawing the cycle

            if (found == 1) {
                gw.set_color(cycle_edges_nodes[tmp_node], green); // mark edge in green
                gw.set_color(tmp_node, green);  // mark node in green
            }
            if (tmp_node == v) {
                found = 1; // cycle found
            }
        }
        // mark also current node
        gw.set_color(cycle_edges_nodes[v], green);
        gw.set_color(v, green);

        gw.acknowledge("There is a cycle (no DAG)!");
        gw.edit();
    } // end of cycle coloring

    // no cycle, so let's start with the real stuff
    temporary_visited[v] = 1;  // mark node temporarily
    gw.set_color(v, red);                           // Color nodes in red
    gw.redraw();                                    // Update displayed graph
    control_wait(WAIT);                              

    edge e;
    forall_out_edges(e, v) {  // all neighbor nodes of v
        node w = g.opposite(v, e); // neighbor nodes on the other side of e
        if (w != parent) {     // the node is not the parent node

            gw.set_color(e, red);
            cycle_nodes.append(v); // there is the possibility of detecting a cycle
            cycle_edges_nodes[v] = e;

            gw.set_width(e, 2);
            if (globally_visited[w] < 0) { // not yet globally visited
                dfs(v, w, g, gw, globally_visited, temporary_visited, akt, all_nodes, cycle_nodes, cycle_edges_nodes); // recursive call with w
                gw.set_color(e, blue);         // Color edge in blue
            } 
            control_wait(WAIT);         
        } else { // mark edge to parent node as blue
            gw.set_color(e, blue);
        }
    }

    gw.set_user_label(v, string("%d", akt--)); // assign a number
    globally_visited[v] = temporary_visited[v]; // mark as globally visited
    all_nodes.remove(v); // remove node from all nodes still to be searched
    cycle_nodes.remove(v); // remove node from potential cycle nodes
    gw.set_color(v, blue); // Color node in blue
    gw.redraw(); // Update displayed graph (to ensure correct updation)
}


// function to conduct topological sort
// selects a starting node and then applies dfs until there are no more nodes to be searched in or a cycle is found
// to be iteratively repeated until all nodes are visited
// parameters:
//     v: current node
//     g: graph to be searched in (as reference)
//    gw: Diplayed window of graph (as reference)
//    akt:  current search number (as reference)
void topsort(node v, graph &g, GraphWin &gw, int &akt) {

    list<node> all_nodes; // all nodes, sort until this is empty
    list<node> cycle_nodes; // all nodes in a possible cycle
    node_array<edge> cycle_edges_nodes(g, NULL);
    
    // load all nodes of graph into a list
    node n;
    forall (n, g.all_nodes()) {
       all_nodes.append(n); 
    }

    node_array<int> globally_visited(g, -1); // no node is visited 
    node_array<int> temporary_visited(g, -1); // no node is temporary visited
    // call the recursive dfs
    dfs(v, v, g, gw, globally_visited, temporary_visited, akt, all_nodes, cycle_nodes, cycle_edges_nodes);

    // for all remaining nodes, use a random node (pop_front)  and search again 
    // until no node is left
    while (akt > 0) {

        v = all_nodes.pop_front();

        // search again recursively 
        dfs(v, v, g, gw, globally_visited, temporary_visited, akt, all_nodes, cycle_nodes, cycle_edges_nodes);
    }
}

// Haupt-Programm
int main(int argc, char *argv[]) {
    p("los gehts");
    // Fenster der Größe 800 x 600 zur Graphendarstellung erzeugen
    GraphWin gw(800, 600);

    gw.display(); // Fenster auf den Bildschirm bringen
    create_control(); // "Fernbedienung" anzeigen
    gw.set_directed(true); // gerichtete Darstellung (Pfeile an Kanten)
    if (argc > 1)    // falls Name als Parameter, Graph laden
        gw.read(argv[1]);

    gw.edit();   // in Editier-Modus gehen, bis der Benutzer "done" klickt

    // jetzt holen wir uns den Graphen, den der Benutzer eingegeben oder geladen hat
    graph &g = gw.get_graph();

    if (g.number_of_nodes() == 0) {  // Ende, wenn der Graph leer ist.
        gw.close(); destroy_control();
        exit(1);
    }

    // Jetzt deklarieren wir ein Feld, das jedem Knoten eine Nummer zuordnet,
    // und initialisieren es mit "-1"
    node_array<int> dfsnum(g, -1);

    // Nun zeigen wir fuer alle Knoten den dfsnum-Wert als User-Label an
    // sowie initialisieren den Graphen gelb.
    node v;
    forall_nodes(v, g) {
        gw.set_label_type(v, user_label);    // User-Label anzeigen (statt Index-Label)
        gw.set_user_label(v, string("%d", dfsnum[v])); // User-Label auf dfsnum[v] setzen
        gw.set_color(v, yellow);
    }
    edge e;
    forall_edges(e, g)
        gw.set_color(e, yellow);

    // in dieser Variable merken wir uns die nächste zu vergebende Nummer
    int akt = g.number_of_nodes();

    do {
        // jetzt lassen wir den Benutzer mit der Maus einen unbesuchten Knoten
        // auswählen (wenn er danebenklickt, wird NULL zurückgeliefert),
        while ((v = gw.read_node()) == NULL || dfsnum[v] >= 0) ;

        // nun rufen wir topsort auf
        topsort(v, g, gw, akt);
    } while (akt > 0); // bis alle Knoten besucht wurden

    gw.acknowledge("Ready!"); // Dialogbox anzeigen und bestätigen lassen
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufräumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
