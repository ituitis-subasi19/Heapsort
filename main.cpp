//////////////////////////////////
/* Ayşe Zeynep Subaşı */
/* 150190046          */
//////////////////////////////////

// Compilation: g++ -std=c++11 main.cpp
// ./a.out number_of_extract_decrease_insert_operations
// ex: ./a.out 1000

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <queue>
#include <iomanip>
#include <string>
#include <time.h>

using namespace std;

const int LENGTH_VEHICLES = 1642; // since the number of vehicles is fixed

class Vehicle{ // Created a class named Vehicle with the values and functions all public to achieve them easily
    public:
    int vehicle_id;
    string location;
    double distance;
    int speed;
    double time;
    Vehicle(const int id, const string loc, const double dist, const int sp) : vehicle_id(id), location(loc), distance(dist), speed(sp){}
    Vehicle() : vehicle_id(0), location(""), distance(0), speed(0){}
    bool operator<(const Vehicle&);
    int calculate_time(){
        time = distance/double(speed);
        return time;
    }
};

bool Vehicle::operator<(const Vehicle& v){ // Written comparison function based on time as wanted
    return time < v.time ? true : false;
}

class Request{ // Created a class named Request with the values and functions all public to achieve them easily
    public:
    string location;
    double distance;
    int lucky_number;
    Request(const string loc, const double dist, const int l_num) : location(loc), distance(dist), lucky_number(l_num){}
    Request() : location(NULL), distance(0), lucky_number(0){}
};

void read_from_files(Vehicle* vehicles, queue<Request>& requests, const char* requests_f, const char* vehicles_f){
    ifstream fin;
    fin.open(vehicles_f); //opened the input file to read
    int vehicle_id, speed;
    string location, line;
    double distance;

    getline(fin, line); // get the first line of the file, since there are headers, didn't write into vehicles array

    for(int i=0; i < 1642 && getline(fin, line); i++){ // got all the lines till the end (1642 vehicles)
        istringstream l(line);
        l >> vehicle_id >> location >> distance >> speed; // assigned the values from the line to the variables
            Vehicle vehicle(vehicle_id, location, distance, speed); // created the vehicle
            vehicle.calculate_time(); // calculated the time
            vehicles[i] = vehicle; // and added it into the array
    }

    fin.close();

    fin.open(requests_f); //opened the input file to read
    int lucky_number;

    getline(fin, line); // get the first line of the file, since there are headers, didn't pushed into requests queue

    for(int i=0; getline(fin, line); i++){ // got all requests
        istringstream l(line);
        l >> location >> distance >> lucky_number; // assigned the values from the line to the variables
            Request request(location, distance, lucky_number); // created the request
            requests.push(request); // and pushed it into the queue
    }

    fin.close();
}

void min_heapify(Vehicle* vehicles, const int& x, const int& heap_size){
    Vehicle temp;
    int l = 2*x + 1; //since the array starts from 0, the left and right nodes' indexes are calculated like that
    int r = 2*x + 2;
    int smallest = x;
    if(l < heap_size && vehicles[l] < vehicles[x]) //take the index of nodes as smaller than heap_size since 0 is the index of the first element and found the node with the smallest value
        smallest = l;
    if(r < heap_size && vehicles[r] < vehicles[smallest])
        smallest = r;
    if(smallest != x){ // swapped the values if x is not the smallest one to take the smallest node as the parent of the bigger ones
        temp = vehicles[x];
        vehicles[x] = vehicles[smallest];
        vehicles[smallest] = temp;
        min_heapify(vehicles, smallest, heap_size); // and called the function again for the value which is swapped with its smallest leaf (the value we had in the beginning) to compare it with the lower nodes (the subtrees' roots)
    }
}

void build_min_heap(Vehicle* vehicles, int& heap_size){ // I took the heap_size with reference since I needed to update the variable itself to use the changed value in the other functions
    heap_size = LENGTH_VEHICLES; // equalised heap_size to the size of the vehicles array
    for(int i = LENGTH_VEHICLES/2 - 1; i >= 0; i--){ // I took i as LENGTH_VEHICLES/2 - 1 since first index is 0
        min_heapify(vehicles, i, heap_size);
    }
}

void heap_extract_min(Vehicle* vehicles, int& heap_size){ // I didn't check the condition for heap_size's being smaller than 1 since after calling this function and decreasing the heap_size by 1, I called the insertion function to insert the extracted vehicle with the new values and increased the heap_size by 1. The smallest value heap_size can be is 1641 since this function cannot be called again untill the insertion function is called.
    Vehicle min = vehicles[0];
    vehicles[0] = vehicles[heap_size-1];
    heap_size--;
    vehicles[heap_size] = min; // I equalized the discarded heap node to the extracted value to be able to keep the value in the array and insert it to the heap with new values after customer's using it.
    min_heapify(vehicles, 0, heap_size); // sorted the new heap
}

void heap_decrease_key(Vehicle* vehicles, int i){ // I didn't take a key value for the vehicle because i changed the values of it outside the function and just after that called the function to change its position. I did it like that because I didn't want to create a new vehicle and after that, copy its values not to slow down the code.
    Vehicle temp;
    while(i > 0 && vehicles[i] < vehicles[(i-1)/2]){ // the parent node's index is calculated by using (i-1)/2 eqaution since the first index of the array is 0.
        temp = vehicles[i];
        vehicles[i] = vehicles[(i-1)/2];
        vehicles[(i-1)/2] = temp;
        i = (i-1)/2;
    }
}

void min_heap_insert(Vehicle* vehicles, int& heap_size){ // since there is no need to give a key into heap_decrease_key function, I didn't take a key value in here too.
    heap_size++;
    heap_decrease_key(vehicles, heap_size-1);
}

void take_request(Vehicle* vehicles, queue<Request>& requests, int& heap_size, const int& n, int& i, ofstream& fout, clock_t& end){
    clock_t start, stop;
    Request request = requests.front(); // wrote the first request into a variable
    requests.pop(); // and popped it

    if(request.lucky_number != 0 && i<n){ // checked if the customer had a lucky_number
        vehicles[request.lucky_number].time = -1; // I equalized time to -1 since it would be smaller than all time values and I can move it to the position 0(first element of the heap) by using heap_decrease_key function to be able to take it out of heap by using heap_extract_min. I didn't take the key equalization(time in here) into time calculation since it is O(1) and doesn't effect the complexity.
        start = clock();
        heap_decrease_key(vehicles, request.lucky_number); // By using this function, I carried the wanted vehicle to the top of the heap by decreasing its time to -1. I didn't write its original time back since after that function, the vehicle will be extracted from the heap and after being used by the customer, it will be inserted into the heap with new values.
        stop = clock();
        end += (stop-start); // increased end which will be the total amount of time by the time heap_decrease_key needed
        i++; // increased the count for checking the amount of operations
    }

    if(i<n){
        start = clock();
        heap_extract_min(vehicles, heap_size); // extracted the root vehicle (which has the lucky_number as index if there is a lucky_number)
        stop = clock();
        end += (stop-start); // increased end which will be the total amount of time by the time heap_extract_min needed
        i++; // increased the count for checking the amount of operations
        fout << vehicles[heap_size].vehicle_id << endl; // wrote the id of the used vehicle into the output file

        vehicles[heap_size].location = request.location; //Equalized the new values of the vehicle after customer's using it. I didn't take the key equalizations(new values of the used vehicle) into time calculation of min_heap_insert function since each of them are O(1) (total 3*O(1) since there are 3 of them) and doesn't effect the complexity.
        vehicles[heap_size].distance = request.distance;
        vehicles[heap_size].calculate_time(); // calculated the new time of the vehicle
    }

    if(i<n){
        start = clock();
        min_heap_insert(vehicles, heap_size); // inserted the used vehicle with the new values into the heap
        stop = clock();
        end += (stop-start); // increased end which will be the total amount of time by the time min_heap_insert needed
        i++; // increased the count for checking the amount of operations
    }
}

int main(int argc, char** argv){
    if(argc != 2) // checked if the number of the input arguments of the main is equal to 2
        return 1;

    int n = stoi(argv[1]); // change the string number which is the number of operations to an integer

    Vehicle vehicles[LENGTH_VEHICLES]; // created a vehicle array with the wanted size
    queue<Request> requests; // created a queue to keep the requests
    int heap_size; // didn't equalised any value to heap_size since build_min_heap would do it

    read_from_files(vehicles, requests, "requests.txt", "vehicles.txt"); // read from the files and keep the values into the proper structures
    build_min_heap(vehicles, heap_size); // build the heap which is sorted as taking the root as the vehicle with the smallest time

    ofstream fout;
    fout.open("call_history.txt"); // opened/created the output file

    int i = 0; // defined a counter as 0 to keep the number of the operations done
    clock_t end = 0;
    while(i < n){ // took the request by obeying the operation number rule
        take_request(vehicles, requests, heap_size, n, i, fout, end);
    }

    fout.close();
    
    cout << "Finished in " << end/(CLOCKS_PER_SEC/1000) << " milliseconds" << endl; // calculated the time as milliseconds

    return 0;
}