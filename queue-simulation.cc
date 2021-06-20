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

const int COACH_SIZE = 4;
const int FIRST_CLASS_SIZE = 3;

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
void servicePas(vector<ServiceStation *> &, Node *, vector<ServiceStation *> &, long &);

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
    long min;
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
    void enqueue(long &, long &, int, long);
    void dequeue(long &);

    Queue() : maxSize(0), currentSize(0), front(nullptr){};
};
struct Node
{
    Passenger pas;
    Node *next;
    ServiceStation *stationAssigned;
    Node() : stationAssigned(nullptr), next(nullptr){};
};
struct ServiceStation
{
    long totalTimeInUse; // Divide by total simulation time to get percentage
    long totalPassengersServiced;
    int type;
    Node *node;
    Clock arrivalTime;
    Clock remainingServiceTime;
    ServiceStation(int a) : type(a), node(nullptr), totalTimeInUse(0), totalPassengersServiced(0), remainingServiceTime(-1){};
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
        pas.serviceTime = Dist::getFromNormalDist(params.serviceDuration.coachPasAvgServiceDuration, 1);
        coachRoot->pas;
        coachQueue.front = coachRoot;
        coachQueue.back = coachRoot;
    }
    else // Odd numbered passengers go to first class
    {
        pas.serviceType = 1;
        pas.arrivalTime = initialPasClock;
        pas.serviceTime = Dist::getFromNormalDist(params.serviceDuration.firstClassPasAvgServiceDuration, 1);
        firstClassRoot->pas = pas;
        firstClassQueue.front = firstClassRoot;
        firstClassQueue.back = firstClassRoot;
    }
    params.firstClassQueue = &firstClassQueue;
    params.coachQueue = &coachQueue;

    cout << "Running..." << endl;

    start(params, serviceStations);

    return 0;
}

/********** Function Definitions *************/
void start(Parameters &P, vector<ServiceStation> &serviceStations)
{
    // vectors operating as stacks for vacant service stations, need to be split because first class can take coach when all of coach are occupied
    vector<ServiceStation *> vacantCoachStations;
    vector<ServiceStation *> vacantFirstClassStations;

    vector<ServiceStation *> occupiedCoachStations;
    vector<ServiceStation *> occupiedFirstClassStations;
    /**
    * @brief 
    * Lambda functions
    */
    auto getNextPasArrivalTime = [](long a, long b)
    {
        return Dist::getFromNormalDist(a, b);
    };
    auto getPasServiceTime = [](long a, long b)
    {
        return Dist::getFromNormalDist(a, b);
    };
    auto getPasServiceType = [](int a, int b)
    {
        auto i = Dist::getFromUniformDist(a, b);
        return i % 2;
    };
    // When all stations are occupied, run this again
    auto setVacantStations = [&vacantCoachStations, &vacantFirstClassStations](vector<ServiceStation> &serviceStations)
    {
        for (auto &station : serviceStations)
        {
            if (station.node == nullptr)
            {
                if (station.type == 0)
                {
                    vacantCoachStations.emplace_back(&station);
                }
                else
                {
                    vacantFirstClassStations.emplace_back(&station);
                }
            }
        };
    };

    /**
     * @brief 
     * Naming aliases and variable unpacking
     */
    auto &coachPasAvgArrivalRate = P.arrivalRate.coachPasAvgArrivalRate;           // alias for coachPasAvgArrivalRate from Arrival struct
    auto &firstClassPasAvgArrivalRate = P.arrivalRate.firstClassPasAvgArrivalRate; //alias for firstClassPasAvgArrivalRate from Arrival struct

    auto &firstClassAvgServiceTime = P.serviceDuration.firstClassPasAvgServiceDuration;
    auto &coachAvgServiceTime = P.serviceDuration.coachPasAvgServiceDuration;

    auto &simuDuration = P.simuDuration;
    auto &currentPasNumber = P.currentPasNumber;

    auto &firstClassQueue = P.firstClassQueue;
    auto &coachQueue = P.coachQueue;

    auto &currentTime = P.worldClock.min;

    auto nextCoachPasArrivalTime = getNextPasArrivalTime(coachPasAvgArrivalRate, 1);
    auto nextFirstClassPasArrivalTime = getNextPasArrivalTime(firstClassPasAvgArrivalRate, 1);

    long coachArrivalInterval = 0;
    long firstClassArrivalInterval = 0;

    long finishInterval = -1;

    // set initial vacant service stations (should be all)
    setVacantStations(serviceStations);

    for (long i = currentTime; i <= simuDuration; i++)
    {
        coachArrivalInterval++;
        firstClassArrivalInterval++;
        finishInterval++;
        if (vacantCoachStations.empty() || vacantFirstClassStations.empty())
        {
            setVacantStations(serviceStations);
        }

        if (coachArrivalInterval >= nextCoachPasArrivalTime)
        {
            coachQueue->enqueue(currentTime, currentPasNumber, 0, getPasServiceTime(coachAvgServiceTime, 1));
            cout << YELLOW << "Passenger " << coachQueue->back->pas.pasId << " arrived." << RESET << MAGENTA << "(COACH)" << RESET << '\n';
            nextCoachPasArrivalTime = getNextPasArrivalTime(coachPasAvgArrivalRate, 1);
            currentPasNumber++;
            coachArrivalInterval = 0;
        }
        if (firstClassArrivalInterval >= nextFirstClassPasArrivalTime) // if interval is greater or equal to the next arrival time, add the next pas in queue
        {
            firstClassQueue->enqueue(currentTime, currentPasNumber, 0, getPasServiceTime(firstClassAvgServiceTime, 1));
            cout << YELLOW << "Passenger " << firstClassQueue->back->pas.pasId << " arrived." << RESET << MAGENTA << "(FIRST CLASS)" << RESET << '\n';
            nextFirstClassPasArrivalTime = getNextPasArrivalTime(firstClassPasAvgArrivalRate, 1);
            currentPasNumber++;
            firstClassArrivalInterval = 0;
        }
        if ((firstClassQueue->front != nullptr) && (!vacantFirstClassStations.empty()))
        {
            servicePas(vacantFirstClassStations, firstClassQueue->front, occupiedFirstClassStations, finishInterval);
        }
        if ((coachQueue->front != nullptr) && (!vacantCoachStations.empty()))
        {
            servicePas(vacantCoachStations, coachQueue->front, occupiedCoachStations, finishInterval);
        }
        if (!occupiedCoachStations.empty())
        {
            for (auto &station : occupiedCoachStations)
            {
                if (station->remainingServiceTime.min <= finishInterval)
                {
                    cout << MAGENTA << "Passenger " << station->node->pas.pasId << " is finished being serviced."
                         << RESET << "\n";
                    station->remainingServiceTime = 0;
                    occupiedCoachStations.erase(std::remove(occupiedCoachStations.begin(), occupiedCoachStations.end(), station), occupiedCoachStations.end());
                    coachQueue->dequeue(i);
                }
            }
        }
        if (occupiedFirstClassStations.empty() == false)
        {
            for (auto &station : occupiedFirstClassStations)
            {
                if (station->remainingServiceTime.min <= finishInterval)
                {
                    cout << MAGENTA << "Passenger " << station->node->pas.pasId << " is finished being serviced."
                         << RESET << "\n";
                    station->remainingServiceTime = 0;
                    occupiedFirstClassStations.erase(std::remove(occupiedFirstClassStations.begin(), occupiedFirstClassStations.end(), station), occupiedFirstClassStations.end());
                    firstClassQueue->dequeue(i);
                }
            }
        }
    }
};

void servicePas(vector<ServiceStation *> &availableStations, Node *node, vector<ServiceStation *> &occupiedServiceStation, long &finishInterval)
{
    auto station = availableStations.back();

    station->node = node;
    station->remainingServiceTime.min = node->pas.serviceTime.min;
    station->totalTimeInUse += node->pas.serviceTime.min;

    node->stationAssigned = station;

    station->totalPassengersServiced += 1;

    if (finishInterval > 0)
    {
        if (station->remainingServiceTime.min < finishInterval)
        {
            finishInterval = station->remainingServiceTime.min;
        }
    }
    else
    {
        finishInterval = station->remainingServiceTime.min;
    }

    occupiedServiceStation.emplace_back(station);
    availableStations.pop_back();
};

void Queue::enqueue(long &currentTime, long &currentPasNumber, int serviceType, long serviceTime)
{
    auto newPas = new Node();
    newPas->pas.pasId = currentPasNumber;
    newPas->pas.serviceType = serviceType;
    newPas->pas.arrivalTime.min = currentTime;
    newPas->pas.serviceTime.min = serviceTime;

    if (this->front == nullptr) // means the entire queue is empty
    {
        this->front = newPas;
        this->front->next = newPas;
        this->back = newPas;
    }

    this->back->next = newPas; // should be nullptr initially, set it to newPas
    this->back = this->back->next;

    this->currentSize = this->currentSize + 1;

    if (this->currentSize >= this->maxSize)
    {
        this->maxSize = this->currentSize;
    }
};

void Queue::dequeue(long &currentTime)
{
    if (this->front == nullptr)
    {
        this->currentSize = 0;
        this->back = nullptr;
        return;
    }
    else
    {
        this->currentSize = this->currentSize - 1;
        this->front = this->front->next;
    }
};

long int Dist::getFromNormalDist(long a, long b)
{
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<long double> nD(a, b);
    return ceil(nD(gen));
};

long int Dist::getFromUniformDist(long a, long b)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long int> uD(a, b);
    return uD(gen);
};