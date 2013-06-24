// Heuristic based on simulated annealing to find a short round trip visiting all nodes
// Using a 3-opt step which runs in constant time the algorithm gradually improves the initial solution
// We consider cases a to e to select the best solution among 5 different 3-opt steps


#include <iostream>
#include <fstream>
#include <ctime>
#include <cstring>
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

#define TEMP 50.0 // Temparature
#define L 5  // number of improving steps
#define K 10 // number of 3-opt steps
//#define ALPHA 0.8 // annealing factor
#define BETA 0.004 // other factor to gradually reduce temparature

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

// Function tsp
//    	Parameters:
//		file_name: File where the output is written in
//		matrix: Adjacency matrix
//		dimension: Number of nodes


void tsp(std::string file_name, int** matrix, int dimension) {
    /**
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
    */

    // find any cycle

    int temp_up_count = 0; // 
    int start_node = 0; //assign 0 to start_node
    int node_order[dimension + 1];// oder in which nodes are visited
    int node_visited[dimension];//
    int cost = 0; // Cost of the round trip; default value 0
    for (int i = 0; i < dimension; i++) {
        node_visited[i] = 0; // in the beginning no node has been visited
    }
    node_order[0] = start_node;
    for (int i = 1; i < dimension; i++) {// Greedy algorithm to find one possible round trip solution
        int* line = matrix[start_node];  
        int min = numeric_limits<int>::max();
        int min_node;
        for (int j = 1; j < dimension; j++) { // search node with minimum distance to start_node
            if (min > line[j] && node_visited[j] == 0) {
                min = line[j];
                min_node = j;
            }
        } 

        node_order[i] = min_node;
        node_visited[min_node] = 1;// flag as visited
        cost += matrix[start_node][min_node];//update cost
        start_node = min_node; // new start_node is min_node
    }

    cost += matrix[node_order[dimension-1]][node_order[0]];//add the last connection from last node to first node to the costs
    node_order[dimension] = node_order[0];

    int start_node_order[dimension + 1]; // new node oder
    memcpy(&start_node_order, node_order, sizeof(node_order)); 

    int start_cost = cost;// initial cost is the current minimal cost

    p("cost is");
    p(cost);

    int update = 0;// number of improving steps until next temparature change
    int steps = 0; // number of 3-opt steps until next temparature change
    double temp = TEMP;

    int count = 0;
    int anneal_count = 0; // number of annealings
    int delta_count = 0;
    
    while (temp > 0.1) {// as long as temparature > 0.1 run tsp to find better solution

        int new_cost = cost; // new cost
        int new_node_order[dimension + 1]; // new node oder
        memcpy(&new_node_order, node_order, sizeof(node_order)); 
        

        // 3 - opt - in constant time
        random_source random_number(1, dimension - 1);//determine random node

        int j = random_number(); // random node j
        int k = j;
        while (k == j) {
            k = random_number();// random node k
        }
        int m = k;
        while (m == k || m == j) {
            m = random_number(); // random node m
        } 

		// Nodes  j, k, m
        int j_node = new_node_order[j];
       
        

        int k_node = new_node_order[k];
        int m_node = new_node_order[m];
       
        new_cost = 0;

        // Case a
        int a_cost = cost;
        a_cost -= matrix[new_node_order[k-1]][k_node];
        a_cost -= matrix[new_node_order[k+1]][k_node];
        a_cost -= matrix[new_node_order[m-1]][m_node];
        a_cost -= matrix[new_node_order[m+1]][m_node];

        new_node_order[j] = j_node;
        new_node_order[k] = m_node;
        new_node_order[m] = k_node;

        a_cost += matrix[new_node_order[k-1]][m_node];
        a_cost += matrix[new_node_order[k+1]][m_node];
        a_cost += matrix[new_node_order[m-1]][k_node];
        a_cost += matrix[new_node_order[m+1]][k_node];


        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // Case b
        int b_cost = cost;
        b_cost -= matrix[new_node_order[j-1]][j_node];
        b_cost -= matrix[new_node_order[j+1]][j_node];
        b_cost -= matrix[new_node_order[k-1]][k_node];
        b_cost -= matrix[new_node_order[k+1]][k_node];
        b_cost -= matrix[new_node_order[m-1]][m_node];
        b_cost -= matrix[new_node_order[m+1]][m_node];
        if (k == j+1 || j == k+1) {
            b_cost += matrix[new_node_order[k]][new_node_order[j]];
        }
        if (m == j+1 || j == m+1) {
            b_cost += matrix[new_node_order[m]][new_node_order[j]];
        }
        if (k == m+1 || m == k+1) {
            b_cost += matrix[new_node_order[k]][new_node_order[m]];
        }

        new_node_order[j] = m_node;
        new_node_order[k] = j_node;
        new_node_order[m] = k_node;

		// Update cost
        b_cost += matrix[new_node_order[j-1]][new_node_order[j]];
        b_cost += matrix[new_node_order[j]][new_node_order[j+1]];
        b_cost += matrix[new_node_order[k-1]][new_node_order[k]];
        b_cost += matrix[new_node_order[k]][new_node_order[k+1]];
        b_cost += matrix[new_node_order[m-1]][new_node_order[m]];
        b_cost += matrix[new_node_order[m]][new_node_order[m+1]];

        if (k == j+1 || j == k+1) {
            b_cost -= matrix[new_node_order[k]][new_node_order[j]];
        }
        if (m == j+1 || j == m+1) {
            b_cost -= matrix[new_node_order[m]][new_node_order[j]];
        }
        if (k == m+1 || m == k+1) {
            b_cost -= matrix[new_node_order[k]][new_node_order[m]];
        }

        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // Case c
        int c_cost = cost;
		//Update cost
        c_cost -= matrix[new_node_order[j-1]][j_node];
        c_cost -= matrix[new_node_order[j+1]][j_node];
        c_cost -= matrix[new_node_order[m-1]][m_node];
        c_cost -= matrix[new_node_order[m+1]][m_node];

        new_node_order[j] = m_node;
        new_node_order[k] = k_node;
        new_node_order[m] = j_node;

        c_cost += matrix[new_node_order[j-1]][m_node];
        c_cost += matrix[new_node_order[j+1]][m_node];
        c_cost += matrix[new_node_order[m-1]][j_node];
        c_cost += matrix[new_node_order[m+1]][j_node];


        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;


        // Case d
        int d_cost = cost;
		
		// Update cost
        d_cost -= matrix[new_node_order[j-1]][j_node];
        d_cost -= matrix[new_node_order[j+1]][j_node];
        d_cost -= matrix[new_node_order[k-1]][k_node];
        d_cost -= matrix[new_node_order[k+1]][k_node];

        new_node_order[j] = k_node;
        new_node_order[k] = j_node;
        new_node_order[m] = m_node;


        d_cost += matrix[new_node_order[j-1]][k_node];
        d_cost += matrix[new_node_order[j+1]][k_node];
        d_cost += matrix[new_node_order[k-1]][j_node];
        d_cost += matrix[new_node_order[k+1]][j_node];

        new_node_order[j] = j_node;
        new_node_order[k] = k_node;
        new_node_order[m] = m_node;

        // Case e
        int e_cost = cost;
		// Update cost
        e_cost -= matrix[new_node_order[j-1]][j_node];
        e_cost -= matrix[new_node_order[j+1]][j_node];
        e_cost -= matrix[new_node_order[k-1]][k_node];
        e_cost -= matrix[new_node_order[k+1]][k_node];
        e_cost -= matrix[new_node_order[m-1]][m_node];
        e_cost -= matrix[new_node_order[m+1]][m_node];

        if (k == j+1 || j == k+1) {
            e_cost += matrix[new_node_order[k]][new_node_order[j]];
        }
        if (m == j+1 || j == m+1) {
            e_cost += matrix[new_node_order[m]][new_node_order[j]];
        }
        if (k == m+1 || m == k+1) {
            e_cost += matrix[new_node_order[k]][new_node_order[m]];
        }

        new_node_order[j] = k_node;
        new_node_order[k] = m_node;
        new_node_order[m] = j_node;

        e_cost += matrix[new_node_order[j-1]][new_node_order[j]];
        e_cost += matrix[new_node_order[j]][new_node_order[j+1]];
        e_cost += matrix[new_node_order[k-1]][new_node_order[k]];
        e_cost += matrix[new_node_order[k]][new_node_order[k+1]];
        e_cost += matrix[new_node_order[m-1]][new_node_order[m]];
        e_cost += matrix[new_node_order[m]][new_node_order[m+1]];

        if (k == j+1 || j == k+1) {
            e_cost -= matrix[new_node_order[k]][new_node_order[j]];
        }
        if (m == j+1 || j == m+1) {
            e_cost -= matrix[new_node_order[m]][new_node_order[j]];
        }
        if (k == m+1 || m == k+1) {
            e_cost -= matrix[new_node_order[k]][new_node_order[m]];
        }

		// Compare costs of different cases and select the one with the least cost
        if (e_cost < a_cost && e_cost < b_cost && e_cost < c_cost && e_cost < d_cost) {
            new_cost = e_cost;

        } else if (d_cost < a_cost && d_cost < b_cost && d_cost < c_cost) {
            new_cost = d_cost;

            new_node_order[j] = k_node;
            new_node_order[k] = j_node;
            new_node_order[m] = m_node;
        
        } else if (c_cost < a_cost && c_cost < b_cost) {
            new_cost = c_cost;

            new_node_order[j] = m_node;
            new_node_order[k] = k_node;
            new_node_order[m] = j_node;
        } else if (b_cost < a_cost) {
            new_cost = b_cost;

            new_node_order[j] = m_node;
            new_node_order[k] = j_node;
            new_node_order[m] = k_node;
        } else {
            new_cost = a_cost;

            new_node_order[j] = j_node;
            new_node_order[k] = m_node;
            new_node_order[m] = k_node;
        }



        int delta = new_cost - cost;
        double rand_number = ((double) rand() / RAND_MAX);

        count++;
        if (delta < 0) {
            delta_count++;
            update++;
        }
            
        // If costs are lower than costs of current solution or e^(-delta/temparature) > random number between 0 and 1 
        if (delta <= 0 || exp(-delta/temp) > rand_number) {
            if (delta > 0) {
                anneal_count++;
            }
           
            cost = new_cost;
           
            memcpy(&node_order, new_node_order, sizeof(node_order));

        }
        steps++;
   
		// update temparature
        if (steps > K || update > L) {
            temp = temp/ (1 +  BETA * temp);
            steps = 0;
            update = 0;
           
            // after heating up several times without success use start route
            if (temp <= 0.1 && temp_up_count >= 5 && start_cost <= cost) {
                p(temp_up_count);
                p("break temp_up_count");
                memcpy(&node_order, start_node_order, sizeof(start_node_order));
                cost = start_cost;
                break;
            }
            if (temp <= 0.1 && start_cost <= cost) {
                temp_up_count++;
                temp += TEMP / 5;
                p("temp up again");
                continue;
            }

            if (temp <= 0.1) {
                break;
            }
        }

 
        
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
    p("anneal");
    p(anneal_count);

    for (int i = 0; i < dimension; i++) {
        cout << node_order[i] << " ";
    }

    cout << endl;
    
    // writing to file

    std::ofstream out_file (file_name.c_str());
    out_file << cost << "\n"; // write cost
    for (int i = 0; i<dimension; i++) { // write node order
        out_file << node_order[i] <<" ";
    }
        
    out_file.close();

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
            //p("next line");
            //p(line);

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

                    //p(node_number);
                    //p(to_node_number);
                    //p(token);
                    matrix[node_number][to_node_number] = atoi(token.c_str());
                    matrix[to_node_number][node_number] = atoi(token.c_str());
                
                    to_node_number++;
                }

                token = line;
                //p(node_number);
                //p(to_node_number);
                //p(token);
                matrix[node_number][to_node_number] = atoi(token.c_str());

            }
            node_number++;
        }
		
        myfile.close();
		// Create string for output file name
        std::string file_output_name = file_name.substr (0, file_name.size() -2 );
        file_output_name.append("out");
        p(file_output_name); 
        tsp(file_output_name, matrix, dimension);

        

    } else {
        p("unable to open file");
    }

    exit(0);
}
