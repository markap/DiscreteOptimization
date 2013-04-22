// Animation einer Tiefensuche in ungerichteten Graphen
// (unbesuchter Teil wird gelb (Default) angezeigt, erledigter Teil blau,
// noch in Bearbeitung befindlicher Teil rot, Rückwärtskanten grün)

#include <iostream>
#include <climits>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/graphics/color.h>
#include <LEDA/system/basic.h>

#include "control.h" // Fernbedienung

#define p(str) ( std::cout << str << std::endl )

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

// rekursive Funktion zur Realisierung der Tiefensuche (dfs);
// Parameter:
//    parent: Knoten, von dem aus der aktuelle Knoten besucht wird
//            (parent == v, falls der aktuelle Knoten der Startknoten ist)
//    v: aktueller Knoten
//    g: zu durchsuchender Graph (als Referenz)
//    gw: Darstellungsfenster des Graphen (als Referenz)
//    dfsnum: Feld zur Zuordnung von DFS-Nummern zu Knoten (als Referenz)
//    akt: nächste freie Nummer (als Referenz)
void dfs(node parent, node v, graph &g, GraphWin &gw, node_array<int> &dfsnum, int &akt) {
    dfsnum[v] = akt++;                              // DFS-Nummer zuweisen
    gw.set_user_label(v, string("%d", dfsnum[v]));  // DFS-Nummer anzeigen
    gw.set_color(v, red);                           // Knoten rot färben
    gw.redraw();                                    // Darstellung aktualisieren
    control_wait(0.5);                              // 0.5 sec warten

    // GraphWin-Graphen sind immer gerichtet, auch wenn auf dem Bildschirm
    // keine Pfeile sichtbar sind

    edge e;
    forall_inout_edges(e, v) {  // alle Nachbarkanten von v
        node w = g.opposite(v, e); // Knoten am anderen Ende von e
        if (w != parent) {     // die Kante zum parent ignorieren wir
            if (dfsnum[w] < 0) { // falls Knoten w noch nicht besucht
                gw.set_color(e, red);          // Kante zu w rot färben
                gw.set_width(e, 2);            // Kante fett anzeigen
                dfs(v, w, g, gw, dfsnum, akt); // rekursiver Aufruf fuer w
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

// Haupt-Programm
int main(int argc, char *argv[]) {
    p("los gehts");
    // Fenster der Größe 800 x 600 zur Graphendarstellung erzeugen
    GraphWin gw(800, 600);

    gw.display(); // Fenster auf den Bildschirm bringen
    create_control(); // "Fernbedienung" anzeigen
    gw.set_directed(false); // ungerichtete Darstellung (keine Pfeile an Kanten)
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
    // (und gleichzeitig die Zahl der schon besuchten Knoten)
    int akt = 0;

    do {
        // jetzt lassen wir den Benutzer mit der Maus einen unbesuchten Knoten
        // auswählen (wenn er danebenklickt, wird NULL zurückgeliefert),
        while ((v = gw.read_node()) == NULL || dfsnum[v] >= 0) ;

        // nun rufen wir die rekursive DFS-Funktion auf
        //dfs(v, v, g, gw, dfsnum, akt);
        p(g.adj_edges(v).length());
    } while (akt < g.number_of_nodes()); // bis alle Knoten besucht wurden

    gw.acknowledge("Ready!"); // Dialogbox anzeigen und bestätigen lassen
    gw.edit(); // nochmal in den Edit-Modus, zum Anschauen :)

    // Aufräumen und Ende
    gw.close();
    destroy_control();
    exit(0);
}
