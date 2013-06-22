#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <string>
#include <math.h>
#include <climits>
#include <LEDA/graphics/graphwin.h>
#include <LEDA/core/random_source.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/color.h>
#include <LEDA/system/basic.h>
#include <LEDA/core/queue.h>
#include <LEDA/graph/node_pq.h>
#include <LEDA/graph/node_map.h>
#include <LEDA/graph/node_partition.h>
#include <LEDA/core/list.h>
#include <LEDA/geo/point.h>

#include "control.h"

#define TEMP 50.0
#define L 2 
#define K 10 
#define ALPHA 0.8
#define BETA 0.004

#define p(str) ( std::cout << str << std::endl ) // print helper
#define len(array) ( sizeof(array) / sizeof(array[0]) )

using namespace std;
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
using leda::random_source;
using leda::green;
using leda::yellow;
using leda::black;
using leda::violet;
using leda::cyan;
using leda::brown;
using leda::pink;
using leda::string;
using leda::white;
using leda::green2;
using leda::blue2;
using leda::grey1;
using leda::grey2;
using leda::grey3;
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

void tsp(int** matrix, int dimension) {
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }

    // find any cycle
    int start_node = 0;
    int node_order[dimension + 1];
    int node_visited[dimension];
    int cost = 0;
    for (int i = 0; i < dimension; i++) {
        node_visited[i] = 0;
    }
    node_order[0] = start_node;
    for (int i = 1; i < dimension; i++) {
        int* line = matrix[start_node];  
        int min = 6000000;//@todo infini
        int min_node;
        for (int j = 1; j < dimension; j++) {
            if (min > line[j] && node_visited[j] == 0) {
                min = line[j];
                min_node = j;
            }
        } 

        node_order[i] = min_node;
        node_visited[min_node] = 1;
        cost += matrix[start_node][min_node];
        start_node = min_node;
    }

    cost += matrix[node_order[dimension-1]][node_order[0]];
    node_order[dimension] = node_order[0];

    for (int i = 0; i < dimension +1; i++) {
        p(node_order[i]);
    }

    p("--");


    int start_cost = cost;

    p("cost is");
    p(cost);

    int update = 0;
    int steps = 0;
    double temp = TEMP;

    int count = 0;
    int anneal_count = 0;
    int delta_count = 0;

    while (temp > 0.5) {

        int new_cost = cost;
        int new_node_order[dimension + 1];
        for (int i = 0; i < dimension + 1; i++) {
            new_node_order[i] = node_order[i];   
        } 


        // 2 - opt - what if start node is changed???
        random_source random_number(1, dimension - 1);

        int j = random_number();
        int k = j;
        while (k == j) {
            k = random_number();
        }
        int m = k;
        while (m == k || m == j) {
            m = random_number();
        } 


        int j_node = new_node_order[j];
        //new_cost -= matrix[new_node_order[j-1]][j_node];
        //new_cost -= matrix[new_node_order[j+1]][j_node];
        

        int k_node = new_node_order[k];
        int m_node = new_node_order[m];
        //new_cost -= matrix[new_node_order[k-1]][k_node];
        //new_cost -= matrix[new_node_order[k+1]][k_node];

        //new_node_order[j] = k_node;
        //new_node_order[k] = j_node;

        new_cost = 0;

/**
        for (int i = 0; i < dimension; i++) {
            new_cost += matrix[new_node_order[i]][new_node_order[i+1]];
        }

        int f = new_cost;

*/
        // aaaaaaaaaaaaaa
        new_node_order[j] = j_node;
        new_node_order[k] = m_node;
        new_node_order[m] = k_node;

        new_cost = 0;//cost;
        
        for (int i = 0; i < dimension; i++) {
            new_cost += matrix[new_node_order[i]][new_node_order[i+1]];    
        }
        if (temp > TEMP/2) {
        int a = new_cost;

        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // bbbbbbbbbbbbbbb
        new_node_order[j] = m_node;
        new_node_order[k] = j_node;
        new_node_order[m] = k_node;

         new_cost = 0;//cost;
        
        for (int i = 0; i < dimension; i++) {
            new_cost += matrix[new_node_order[i]][new_node_order[i+1]];    
        }

        int b = new_cost;

        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // ccccccccccccccc
        new_node_order[j] = m_node;
        new_node_order[k] = k_node;
        new_node_order[m] = j_node;

         new_cost = 0;//cost;
        
        for (int i = 0; i < dimension; i++) {
            new_cost += matrix[new_node_order[i]][new_node_order[i+1]];    
        }

        int c = new_cost;

        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // ddddddddddddddd
        new_node_order[j] = k_node;
        new_node_order[k] = j_node;
        new_node_order[m] = m_node;

         new_cost = 0;//cost;
        
        for (int i = 0; i < dimension; i++) {
            new_cost += matrix[new_node_order[i]][new_node_order[i+1]];    
        }

        int d = new_cost;

        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // eeeeeeeeeeeee
        new_node_order[j] = k_node;
        new_node_order[k] = m_node;
        new_node_order[m] = j_node;
        }
        
        else {
            new_cost -= matrix[node_order[j-1]][j_node];
            new_cost -= matrix[node_order[j+1]][j_node];

            int k_node = node_order[k];
            new_cost -= matrix[node_order[k-1]][k_node];
            new_cost -= matrix[node_order[k+1]][k_node];

            node_order[j] = k_node;
            node_order[k] = j_node;

            new_cost += matrix[node_order[j-1]][k_node];
            new_cost += matrix[node_order[j+1]][k_node];

            new_cost += matrix[node_order[k-1]][j_node];
            new_cost += matrix[node_order[k+1]][j_node];
        }
        
         new_cost = 0;//cost;
        
        for (int i = 0; i < dimension; i++) {
            new_cost += matrix[new_node_order[i]][new_node_order[i+1]];    
        }

        int e = new_cost;


        if (e < a && e < b && e < c && e < d) {
            new_cost = e;

        } else if (d < a && d < b && d < c) {
            new_cost = d;

            new_node_order[j] = k_node;
            new_node_order[k] = j_node;
            new_node_order[m] = m_node;
        
        } else if (c < a && c < b) {
            new_cost = c;

            new_node_order[j] = m_node;
            new_node_order[k] = k_node;
            new_node_order[m] = j_node;
        } else if (b < a) {
            new_cost = b;

            new_node_order[j] = m_node;
            new_node_order[k] = j_node;
            new_node_order[m] = k_node;
        } else {
            new_cost = a;

            new_node_order[j] = j_node;
            new_node_order[k] = m_node;
            new_node_order[m] = k_node;
        }




        //new_cost += matrix[new_node_order[j-1]][k_node];
        //new_cost += matrix[new_node_order[j+1]][k_node];

        //new_cost += matrix[new_node_order[k-1]][j_node];
        //new_cost += matrix[new_node_order[k+1]][j_node];

        for (int i = 0; i < dimension +1; i++) {
            p(new_node_order[i]);
        }

        p("--");

        p("new cost is");
        p(new_cost);

        int delta = new_cost - cost;
        double rand_number = ((double) rand() / RAND_MAX);
        p("delta");
        p(delta);
        p("exp");
        p(exp(-delta/temp));
        p("rand");
        p(rand_number);
        count++;
        if (delta < 0) {
            delta_count++;
        }
            
        
        if (delta <= 0 || exp(-delta/temp) > rand_number) {
            anneal_count++;
            p("update cost ...");
            
            cost = new_cost;
            p(cost);
            fflush(stdout);
            //sleep(5);
            // runtime!!!
            for (int i = 0; i < dimension + 1; i++) {
                node_order[i] = new_node_order[i];   
            } 

            update++;
        }
        steps++;
        p("steps ");
        p(steps);
        p("update ");
        p(update);

        if (steps > K || update > L) {
            temp = temp/ (1 +  BETA * temp);
            steps = 0;
            update = 0;
            p("new temp is");
            p(temp);
        }

        //fflush(stdout);
        //sleep(0);

    }

    p("break - temp is ...");

    p("start kosten");
    p(start_cost);
    p("kosten");
    p(cost);

    p("count");
    p(count);
    p("delta");
    p(delta_count);
    p(anneal_count - delta_count);

    for (int i = 0; i < dimension; i++) {
        cout << node_order[i] << " ";
    }

    cout << endl;

}

// Main program
int main(int argc, char *argv[]) {

    std::string file_name;
    for (int i = 1; i < argc; i++) {
        file_name.append(argv[i]);
    }

    srand(time(NULL));

    int **matrix;
    int dimension;

    std::string line;
    ifstream myfile(file_name.c_str());
    if (myfile.is_open()) {

        int node_number = -1; // first line is not to build the graph

        while (myfile.good()) {

            int to_node_number = 0;

            getline(myfile, line);
            if (line == "") {
                break;
            }
            p("next line");
            p(line);

            if (node_number == -1) {
                dimension = atoi(line.c_str());
                p("dim is ");
                p(dimension);
                matrix = new int*[dimension];
                for (int i = 0; i < dimension; i++) {
                    matrix[i] = new int[dimension];
                }
            } else {

                size_t pos = 0;
                std::string delimiter = " ";
                std::string token;


                while ((pos = line.find(delimiter)) != std::string::npos) {
                    token = line.substr(0, pos);
                    line.erase(0, pos + delimiter.length());

                    p(node_number);
                    p(to_node_number);
                    p(token);
                    matrix[node_number][to_node_number] = atoi(token.c_str());
                    matrix[to_node_number][node_number] = atoi(token.c_str());
                
                    to_node_number++;
                }

                token = line;
                p(node_number);
                p(to_node_number);
                p(token);
                matrix[node_number][to_node_number] = atoi(token.c_str());

            }
            node_number++;
        }

        myfile.close();

        tsp(matrix, dimension);

    } else {
        p("unable to open file");
    }

    exit(0);
}
