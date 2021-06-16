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
struct Passenger;
struct ServiceStation;
struct Queue;
struct Node;
struct Parameters;
struct ServiceDuration;
struct Arrival;

void start(Parameters &);

struct ArrivalRate
{
    long coachPasAvgArrivalRate;
    long firstClassPasAvgArrivalRate;
};
struct ServiceDuration
{
    long coachPasAvgServiceDuration;
    long firstClassPasAvgServiceDuration;
};
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
    void enqueue();
    void dequeue();
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
struct Parameters
{
    long simuDuration; // Total duration of simulation
    long int currentPasNumber;
    mt19937 gen{random_device{}()};

    /**
     * @brief 
     * getFromUniformDist generates a random integer from a uniform distribution, in this
     * simulation, even numbers denote coach service, and odd numbers denote first class service
     */
    long int getFromUniformDist(int, int);
    long int getFromNormalDist(long, long);

    ServiceDuration serviceDuration;
    ArrivalRate arrivalRate;
    Clock worldClock, elapsedTime;
};

void start(Parameters &P)
{
    /**
     * @brief 
     * Naming aliases
     */
    auto &coachA = P.arrivalRate.coachPasAvgArrivalRate;      // alias for coachPasAvgArrivalRate from Arrival struct
    auto &firstA = P.arrivalRate.firstClassPasAvgArrivalRate; //alias for firstClassPasAvgArrivalRate from Arrival struct

    // params.getFromNormalDist(params.arrivalRate.coachPasAvgArrivalRate, 1);
};

// TODO: Split main into a function Parameters initParams() for code cleanliness, not needed right now, but needed for later
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

    Parameters params;

    params.currentPasNumber = 0;
    params.worldClock = {0}, params.elapsedTime = {0};

    Queue coachQueue, firstClassQueue;
    Node *coachRoot, *firstClassRoot = new Node(); // Root nodes of Queue (first passenger in line occupate this node)

    ServiceStation coachServiceStation1, coachServiceStation2, coachServiceStation3;
    ServiceStation firstClassServiceStation1, firstClassServiceStation2, firstClassServiceStation3;

    ServiceDuration serviceDuration;
    ArrivalRate arrivalRate;

    cout << "Please enter an unsigned integer value, this will be "
         << "used as the stopping point for the simulation, all values represents the number of "
         << "minutes elapsed in the simulation, which goes by iterations in a loop," << MAGENTA << " not real time." << RESET;
    cout << '\n';

    cout << "Duration time: ";
    cin >> params.simuDuration;

    cout << "Average passenger arrival rate of COACH station: ";
    cin >> arrivalRate.coachPasAvgArrivalRate;

    cout << "Average passenger service duration of COACH station: ";
    cin >> serviceDuration.coachPasAvgServiceDuration;

    cout << "Average passenger arrival rate of FIRST CLASS station: ";
    cin >> arrivalRate.firstClassPasAvgArrivalRate;

    cout << "Average passenger service duration of FIRST CLASS station: ";
    cin >> serviceDuration.firstClassPasAvgServiceDuration;

    params.arrivalRate = arrivalRate;
    params.serviceDuration = serviceDuration;

    Passenger pas{params.currentPasNumber};
    Clock initialPasClock{params.worldClock.min};

    int serviceType = params.getFromUniformDist(0, 9);

    if (serviceType % 2 == 0) // Even numbered passengers go to coach
    {
        pas.serviceType = 0;
        pas.arrivalTime = initialPasClock;
        coachRoot->pas;
        coachQueue.front = coachRoot;
        coachQueue.back = coachRoot;
    }
    else // Odd numbered passengers go to first class
    {
        pas.serviceType = 1;
        pas.arrivalTime = initialPasClock;
        firstClassRoot->pas = pas;
        firstClassQueue.front = firstClassRoot;
        firstClassQueue.back = firstClassRoot;
    }

    cout << "Starting..." << endl;
    start(params);

    return 0;
}
void Queue::enqueue()
{
}
void Queue::dequeue()
{
    this->front = this->front->next;
}
long int Parameters::getFromNormalDist(long a, long b)
{
    normal_distribution<long double> nD(a, b);
    return (long int)nD(Parameters::gen);
}

long int Parameters::getFromUniformDist(int a, int b)
{
    uniform_int_distribution<unsigned long int> uD(a, b);
    return uD(Parameters::gen);
}

void start(Parameters &params)
{
    cout << params.getFromUniformDist(0, 9) << "\n";
    cout << params.getFromNormalDist(50, 100) << "\n";
};