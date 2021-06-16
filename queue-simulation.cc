/**
 * @file queue-simulation.cc
 * @author Glorian Kosi
 * @brief 
 * 
 * See readme
 * 
 */

#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define MAGENTA "\033[35m"

#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include <cmath>

using std::ceil;
using std::cin;
using std::cout;
using std::endl;
using std::mt19937;
using std::normal_distribution;
using std::random_device;
using std::uniform_int_distribution;
using std::vector;

struct Clock;
class Passenger;
class ServiceStation;
struct Queue;
struct Node;

struct Clock
{
    int min;
};
struct Passenger
{
    long int pasId;
    int serviceType;
    Clock arrivalTime; // Arrival to queue
    Clock serviceTime; // Time elapsed to enter service station
};
struct Queue
{
    long maxSize;
    long currentSize;
    Node *front;
    Node *back;
};
struct Node
{
    Passenger pas;
    Node *next;
};
struct ServiceStation
{
    Passenger currentPas;
    Clock remainingServiceTime;
    Clock arrivalTime;
};

int main(int argc, char *argv[])
{
    /**
     * @brief 
     * Passenger IDs are incremented by 1 iteratively, currentPasNumber keeps track of the latest reserved
     * long int, the next passenger yet to arrive in line is currentPasNumber + 1, the second is currentPasNumber + 2, and so on.
     * 
     * Queues are not guaranteed to have Passenger IDs in numerical order (nor should it happen, if it does, there
     * is something wrong with the prng)
     * 
     */
    long int currentPasNumber = 0;

    random_device rd{};
    mt19937 gen{rd()};

    uniform_int_distribution<int> serviceClassUd(0, 9);

    /**
     * @brief 
     * serviceClassUd generates a random integer between 0 and 9 from a uniform distribution, in this
     * simulation, even numbers denote coach service, and odd numbers denote first class service
     */
    int serviceType = serviceClassUd(rd);

    Clock worldClock{0}, elapsedTime;

    Queue coachQueue, firstClassQueue;
    Node *coachRoot, *firstClassRoot = new Node(); // Root nodes of Queue (first passenger in line occupate this node)

    ServiceStation coachServiceStation1, coachServiceStation2, coachServiceStation3;
    ServiceStation firstClassServiceStation1, firstClassServiceStation2, firstClassServiceStation3;

    long checkInDuration; // Total duration of simulation

    long coachPasAvgArrivalRate;
    long coachPasAvgServiceDuration; // Average time it takes to service the next customer in queue

    long firstPasClassAvgArrivalRate;
    long firstPasClassAvgServiceDuration;

    cout << "Please enter an unsigned integer value, this will be "
         << "used as the stopping point for the simulation, all values represents the number of "
         << "minutes elapsed in the simulation, which goes by iterations in a loop," << MAGENTA << " not real time." << RESET;
    cout << '\n';

    cout << "Duration time: ";
    cin >> checkInDuration;

    cout << "Average passenger arrival rate of COACH station: ";
    cin >> coachPasAvgArrivalRate;
    normal_distribution<double> coachPasAvgArrivalRateNd(coachPasAvgArrivalRate, 1);

    cout << "Average passenger service duration of COACH station: ";
    cin >> coachPasAvgServiceDuration;
    // Draw a new value that denotes how long the Passenger actually took, use ceil to bring it to the nearest min
    normal_distribution<double> coachPasAvgServiceDurationNd(coachPasAvgServiceDuration, 1);

    cout << "Average passenger arrival rate of FIRST CLASS station: ";
    cin >> firstPasClassAvgArrivalRate;
    normal_distribution<double> firstPasClassAvgArrivalRateNd(firstPasClassAvgArrivalRate, 1);

    cout << "Average passenger service duration of FIRST CLASS station: ";
    cin >> firstPasClassAvgServiceDuration;
    normal_distribution<double> firstPasClassAvgServiceDurationNd(firstPasClassAvgServiceDuration, 1);

    Passenger pas{currentPasNumber};
    Clock initialPasClock{worldClock.min};

    if (serviceType % 2 == 0) // Even numbered passengers go to coach
    {
        pas.serviceType = 0;
        pas.arrivalTime = initialPasClock;
        coachRoot->pas;
        coachRoot->next = nullptr;
        coachQueue.front = coachRoot;
        coachQueue.back = coachRoot;
    }
    else // Odd numbered passengers go to first class
    {
        pas.serviceType = 1;
        pas.arrivalTime = initialPasClock;
        firstClassRoot->pas = pas;
        firstClassRoot->next = nullptr;
        firstClassQueue.front = firstClassRoot;
        firstClassQueue.back = firstClassRoot;
    }

    cout << "Starting..." << endl;

    return 0;
}
