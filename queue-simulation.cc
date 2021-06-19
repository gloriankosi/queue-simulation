/**
 * 
 * @file queue-simulation.cc
 * @author Glorian Kosi
 * @brief 
 * 
 * See readme
 * To run:
 * g++ -std=c++2a queue-simulation.cc -o executable_name
 * ./executable_name
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
using std::reference_wrapper;
using std::uniform_int_distribution;
using std::vector;

/********************* Class declarations ***************************/
struct Clock;
struct Passenger;
struct ServiceStation;
struct Queue;
struct Node;
struct Parameters;
struct ServiceDuration;
struct Arrival;

/********************* Function declarations *************************/
void start(Parameters &, vector<ServiceStation> &);
void initialQueueFill(Parameters &);

/********************* Class definitions ***************************/
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
    Clock() : min(0){};
    Clock(long a) : min(a){};
};
struct Passenger
{
    long int pasId;
    int serviceType;
    Clock arrivalTime; // Arrival to queue
    Clock serviceTime; // Time needed for service
    Passenger() : pasId(-1), serviceType(-1){};
    Passenger(long a) : pasId(a){};
};
struct Queue
{
    long maxSize;
    long currentSize;
    Node *front;
    Node *back;
    void enqueue(Clock &, long int &);
    void dequeue(Clock &);

    Queue() : maxSize(0), currentSize(0){};
};
struct Node
{
    Passenger pas;
    Node *next;
};
struct ServiceStation
{
    long totalTimeInUse; // Divide by total simulation time to get percentage
    int type;
    Passenger *currentPas;
    Clock arrivalTime;
    Clock remainingServiceTime;
    ServiceStation(int a) : type(a), currentPas(nullptr){};
};
struct Dist
{
    /**
     * @brief 
     * getFromUniformDist generates a random integer from a uniform distribution, in this
     * simulation, even numbers denote coach service, and odd numbers denote first class service.
     */
    static long int getFromUniformDist(long, long);
    static long int getFromNormalDist(long, long);
};
/**
 * @brief 
 * Contains closely related variables needed in most cases that can't be combined into more classes
 * or can't be removed without the use of global variables.
 */
struct Parameters
{
    long simuDuration; // Total duration of simulation
    long currentPasNumber;

    Queue *firstClassQueue;
    Queue *coachQueue;
    ServiceDuration serviceDuration;
    ArrivalRate arrivalRate;
    Clock worldClock, elapsedTime;
    Parameters() : simuDuration(0), currentPasNumber(0){};
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

    Queue coachQueue, firstClassQueue;
    Node *coachRoot, *firstClassRoot = new Node(); // Root nodes of Queue (first passenger in line occupate this node)

    ServiceStation coachServiceStation1(0), coachServiceStation2(0), coachServiceStation3(0), coachServiceStation4(0);
    ServiceStation firstClassServiceStation1(1), firstClassServiceStation2(1), firstClassServiceStation3(1);

    vector<ServiceStation> serviceStations{coachServiceStation1, coachServiceStation2, coachServiceStation3, coachServiceStation4,
                                           firstClassServiceStation1, firstClassServiceStation2, firstClassServiceStation3};

    ServiceDuration serviceDuration;
    ArrivalRate arrivalRate;

    cout << "Please enter an unsigned integer value, this will be "
         << "used as the stopping point for the simulation, all values represents the number of "
         << "minutes elapsed in the simulation, which goes by iterations in a loop," << MAGENTA << " not real time." << RESET;
    cout << '\n';

    cout << "Duration time: ";
    cin >> params.simuDuration;

    cout << "Average passenger arrival rate of COACH station (for best measure, enter value significantly less than duration time): ";
    cin >> arrivalRate.coachPasAvgArrivalRate;

    cout << "Average passenger service duration of COACH station: ";
    cin >> serviceDuration.coachPasAvgServiceDuration;

    cout << "Average passenger arrival rate of FIRST CLASS station (for best measure, enter value significantly less than duration time): ";
    cin >> arrivalRate.firstClassPasAvgArrivalRate;

    cout << "Average passenger service duration of FIRST CLASS station: ";
    cin >> serviceDuration.firstClassPasAvgServiceDuration;

    params.arrivalRate = arrivalRate;
    params.serviceDuration = serviceDuration;

    Passenger pas{params.currentPasNumber};
    Clock initialPasClock{params.worldClock.min};

    int serviceType = Dist::getFromUniformDist(0, 9);

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
    params.firstClassQueue = &firstClassQueue;
    params.coachQueue = &coachQueue;

    cout << "Starting..." << endl;

    start(params, serviceStations);

    return 0;
}

/********** Function Definitions *************/
void start(Parameters &P, vector<ServiceStation> &serviceStations)
{
    /**
     * @brief 
     * Naming aliases and variable unpacking
     */
    auto &coachA = P.arrivalRate.coachPasAvgArrivalRate;      // alias for coachPasAvgArrivalRate from Arrival struct
    auto &firstA = P.arrivalRate.firstClassPasAvgArrivalRate; //alias for firstClassPasAvgArrivalRate from Arrival struct

    auto &simuDuration = P.simuDuration;
    auto &currentPasNumber = P.currentPasNumber;

    auto &firstClassQueue = P.firstClassQueue;
    auto &coachQueue = P.coachQueue;

    auto &currentTime = P.worldClock.min;

    auto nextCoachPasArrivalTime = Dist::getFromNormalDist(coachA, 1);
    auto nextFirstClassPasArrivalTime = Dist::getFromNormalDist(firstA, 1);

    vector<ServiceStation *> availableCoachStations;
    vector<ServiceStation *> availableFirstClassStations;

    auto checkAvailable = [&availableCoachStations, &availableFirstClassStations](vector<ServiceStation> &serviceStations)
    {
        for (auto &station : serviceStations)
        {
            if (station.currentPas == nullptr)
            {
                if (station.type == 0)
                {
                    availableCoachStations.emplace_back(&station);
                }
                else
                {
                    availableFirstClassStations.emplace_back(&station);
                }
            }
        }
    };

    checkAvailable(serviceStations);

    for (auto i = currentTime; i <= simuDuration; i++)
    {
        if (currentTime > nextCoachPasArrivalTime)
        {
        }
    }
};

void Queue::enqueue(Clock &worldClock, long int &currentPasNumber)
{
    auto *newPas = new Node();
    newPas->pas.pasId = currentPasNumber + 1;
    newPas->pas.serviceType = newPas->pas.arrivalTime.min = worldClock.min;
    currentPasNumber++;
}
void Queue::dequeue(Clock &worldClock)
{
    this->front = this->front->next;
}
long int Dist::getFromNormalDist(long a, long b)
{
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<long double> nD(a, b);
    return ceil(nD(gen));
}
long int Dist::getFromUniformDist(long a, long b)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long int> uD(a, b);
    return uD(gen);
}