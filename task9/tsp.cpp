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

#define TEMP 50.0
#define L 5 
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

    int temp_up_count = 0;
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
        int min = numeric_limits<int>::max();
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


    int start_cost = cost;

    p("cost is");
    p(cost);

    int update = 0;
    int steps = 0;
    double temp = TEMP;

    int count = 0;
    int anneal_count = 0;
    int delta_count = 0;
    
    while (temp > 0.1) {

        int new_cost = cost;
        int new_node_order[dimension + 1];
        memcpy(&new_node_order, node_order, sizeof(node_order));
        

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

        // bbbbbbbbbbbbbbb
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

        // ccccccccccccccc
        int c_cost = cost;
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


        // ddddddddddddddd
        int d_cost = cost;
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

        // eeeeeeeeeeeee
        int e_cost = cost;
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




        //new_cost += matrix[new_node_order[j-1]][k_node];
        //new_cost += matrix[new_node_order[j+1]][k_node];

        //new_cost += matrix[new_node_order[k-1]][j_node];
        //new_cost += matrix[new_node_order[k+1]][j_node];


        //p("new cost is");
        //p(new_cost);

        int delta = new_cost - cost;
        double rand_number = ((double) rand() / RAND_MAX);
        /*
        p("delta");
        p(delta);
        p("exp");
        p(exp(-delta/temp));
        p("rand");
        p(rand_number);
        */
        count++;
        if (delta < 0) {
            delta_count++;
            update++;
        }
            
        
        if (delta <= 0 || exp(-delta/temp) > rand_number) {
            if (delta > 0) {
                anneal_count++;
            }
            //p("update cost ...");
            
            cost = new_cost;
            //p(cost);
            //fflush(stdout);
            //sleep(5);
            memcpy(&node_order, new_node_order, sizeof(node_order));

        }
        steps++;
        //p("steps ");
        //p(steps);
        //p("update ");
        //p(update);

        if (steps > K || update > L) {
            temp = temp/ (1 +  BETA * temp);
            steps = 0;
            update = 0;
            //p("new temp is");
            //p(temp);
            if (temp <= 0.1 && temp_up_count >= 5 && start_cost >= cost) {
                p(temp_up_count);
                p("break temp_up_count");
                break;
            }
            if (temp <= 0.1 && start_cost <= cost) {
                temp_up_count++;
                temp += TEMP / 5;
                p("temp up again");
            }
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
    p("anneal");
    p(anneal_count);

    for (int i = 0; i < dimension; i++) {
        cout << node_order[i] << " ";
    }

    cout << endl;
    
    // writing to file
   // remove(file_name.c_str());
    std::ofstream out_file (file_name.c_str());
    out_file << cost << "\n";
    for (int i = 0; i<dimension; i++) {
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
        std::string file_output_name = file_name.substr (0, file_name.size() -2 );
        file_output_name.append("out");
        p(file_output_name); 
        tsp(file_output_name, matrix, dimension);

        

    } else {
        p("unable to open file");
    }

    exit(0);
}
