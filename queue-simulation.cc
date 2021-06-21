/**
 * @file queue-sim.cc
 * @author Glorian Kosi
 * @brief 
 * 
 * See README
 * 
 */

#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define MAGENTA "\033[35m"
#define L_GREEN "\033[92m"
#define CYAN "\033[36m"

#include <iostream>
#include <iterator>
#include <random>
#include <vector>
#include <cmath>

using std::cin;
using std::cout;
using std::mt19937;
using std::normal_distribution;
using std::random_device;
using std::reference_wrapper;
using std::string;
using std::uniform_int_distribution;
using std::vector;

long getSimDuration();

long getCoachAvgArrivalRate();
long getCoachAvgServiceDuration();

long getFirstClassAvgArrivalRate();
long getFirstClassAvgServiceDuration();

long getFromNormalDist(long, long);
long getFromUniformDist(long, long);

struct Node
{
    long pasId;
    long serviceTimeNeeded;
    Node *next;
    Node() : pasId(-1), serviceTimeNeeded(-1), next(nullptr){};
    Node(long pasId, long serviceTimeNeeded, long arrivalTime) : pasId(pasId),
                                                                 serviceTimeNeeded(serviceTimeNeeded),
                                                                 next(nullptr){};
};

struct Queue
{
    long maxSize;
    long currentSize;

    Node *front;
    Node *back;

    void enqueue(Node *node)
    {
        if (back == nullptr)
        {
            currentSize++;
            currentSize >= maxSize ? maxSize = currentSize : maxSize;
            front = back = node;
            return;
        }
        back->next = node;
        back = node;
    }
    void dequeue()
    {
        if (front == nullptr)
            return;
        currentSize--;
        Node *temp = front;
        front = front->next;

        if (front == nullptr)
            back = nullptr;
    }

    Queue() : maxSize(0), currentSize(0), front(nullptr), back(nullptr){};
};

struct ServiceStation
{
    long totalTimeInService;
    long numOfPassengersServiced;
    Node *currentPas;
    ServiceStation() : totalTimeInService(0), currentPas(nullptr){};
};

vector<ServiceStation *> createServiceStationVector(int);
Node *getPas(long, long, long, long);

void emptyQueue(Queue *, Queue *,
                vector<ServiceStation *> &, vector<ServiceStation *> &,
                vector<ServiceStation *> &, vector<ServiceStation *> &, long);

void printFinalStats(Queue *, Queue *, vector<ServiceStation *> &, vector<ServiceStation *> &, long &);
void printFinishedServiced(long, string);
void printPasArrivalToQueue(long, string);
void printEmptyingQueueMsg();

void getFinishedServicedPassengers(vector<ServiceStation *> &, vector<ServiceStation *> &, string, long &);
void servicePas(Queue *, vector<ServiceStation *> &, long, long, string);
void addToQueue(Queue *, long, Node *, string);
void start(Queue *, Queue *, vector<ServiceStation *> &, vector<ServiceStation *> &);

int main(int argc, char *argv[])
{

    auto *coachQueue = new Queue();
    auto *firstClassQueue = new Queue();

    vector<ServiceStation *> coachStations = createServiceStationVector(3);
    vector<ServiceStation *> firstClassStations = createServiceStationVector(2);

    start(coachQueue, firstClassQueue, coachStations, firstClassStations);

    return 0;
}

void printFinalStats(Queue *coachQueue, Queue *firstClassQueue, vector<ServiceStation *> &coachStations, vector<ServiceStation *> &firstClassStations, long &simDuration)
{
    long totalCoachPassengersServiced = 0;
    long totalFirstclassPassengersServiced = 0;
    long totalPassengersServiced = 0;

    for (auto &i : coachStations)
    {
        totalCoachPassengersServiced += i->numOfPassengersServiced;
    }

    for (auto &i : firstClassStations)
    {
        totalFirstclassPassengersServiced += i->numOfPassengersServiced;
    }

    totalPassengersServiced = totalCoachPassengersServiced + totalFirstclassPassengersServiced;

    cout << "------------------Final Stats------------------"
         << "\n"
         << "\n";

    cout << "Total # of Coach Passengers serviced: " << totalCoachPassengersServiced << "\n";
    cout << "Total # of First Class Passengers serviced: " << totalFirstclassPassengersServiced << "\n";
    cout << "Total # of Passengers serviced: " << totalPassengersServiced << "\n";

    cout << "\n\n";

    int counter = 0;

    for (auto &i : coachStations)
    {
        counter++;
        cout << "\n";
        cout << "COACH Service Station " << counter << "\n"
             << "Serviced: " << i->numOfPassengersServiced << " passengers."
             << "\n";
        cout << " Percentage of time in use: " << i->totalTimeInService / simDuration;
        cout << "\n";
    }
    for (auto &i : firstClassStations)
    {
        counter++;
        cout << "\n";
        cout << "FIRST CLASS Service Station " << counter << "\n"
             << "Serviced: " << i->numOfPassengersServiced << " passengers."
             << "\n";
        cout << " Percentage of time in use: " << i->totalTimeInService / simDuration;
        cout << "\n";
    }
    cout << "\n\n";
    cout << "Total time of simulation: " << simDuration << "\n";
}

void printEmptyingQueueMsg()
{
    cout << MAGENTA << "------------------"
         << RESET
         << L_GREEN << "Emptying Queue"
         << RESET
         << CYAN << "------------------" << RESET
         << "\n";
}

void printCurrentPasServicing(long pasId, long arrivalToServiceTime, string serviceType)
{
    cout << L_GREEN << "Passenger " << pasId << " is currently being serviced at time: "
         << arrivalToServiceTime << "." << RESET << RED << serviceType << RESET << "\n";
}

void printFinishedServiced(long pasId, string serviceType)
{
    cout << CYAN << "Passenger " << pasId << " has finished service."
         << serviceType << RESET << "\n";
}

void servicePas(Queue *queue, vector<ServiceStation *> &vacantServiceStations, long arrivalToServiceTime, string serviceType)
{
    if (queue->front == nullptr || vacantServiceStations.empty() == true)
    {
        return;
    }
    auto station = vacantServiceStations.back();

    station->numOfPassengersServiced++;
    station->currentPas = queue->front;
    station->totalTimeInService += station->currentPas->serviceTimeNeeded;

    vacantServiceStations.pop_back();

    printCurrentPasServicing(queue->front->pasId, arrivalToServiceTime, serviceType);
    queue->dequeue();
}

void getFinishedServicedPassengers(vector<ServiceStation *> &vacantServiceStations, vector<ServiceStation *> &serviceStations, string serviceType, long &finishInterval)
{
    bool resetFinishInterval = true;
    if (vacantServiceStations.empty())
    {
        for (auto &i : serviceStations)
        {
            if (i->currentPas->serviceTimeNeeded <= finishInterval)
            {
                printFinishedServiced(i->currentPas->pasId, serviceType);
                i->currentPas = nullptr;
                vacantServiceStations.emplace_back(i);
            }
            else //
            {
                resetFinishInterval = false;
            }
        }
    }
    if (resetFinishInterval == true) //
    {
        finishInterval = 0;
    }
}

long getSimDuration()
{
    long simuDuration;
    cout << "Please enter an unsigned integer value, this will be "
         << "used as the stopping point for the simulation, all values represents the number of "
         << "minutes elapsed in the simulation, which goes by iterations in a loop," << MAGENTA << " not real time." << RESET;
    cout << '\n';
    cout << "Duration time: ";
    cin >> simuDuration;
    return simuDuration;
};

long getCoachAvgArrivalRate()
{
    long coachPasAvgArrivalRate;
    cout << "Average passenger arrival rate of COACH station (for best measure, enter value significantly less than duration time): ";
    cin >> coachPasAvgArrivalRate;
    return coachPasAvgArrivalRate;
};

long getCoachAvgServiceDuration()
{
    long coachPasAvgServiceDuration;
    cout << "Average passenger service duration of COACH station: ";
    cin >> coachPasAvgServiceDuration;
    return coachPasAvgServiceDuration;
}

long getFirstClassAvgArrivalRate()
{
    long firstClassPasAvgArrivalRate;
    cout << "Average passenger arrival rate of FIRST CLASS station (for best measure, enter value significantly less than duration time): ";
    cin >> firstClassPasAvgArrivalRate;
    return firstClassPasAvgArrivalRate;
}

long getFirstClassAvgServiceDuration()
{
    long firstClassPasAvgServiceDuration;

    cout << "Average passenger service duration of FIRST CLASS station: ";
    cin >> firstClassPasAvgServiceDuration;

    return firstClassPasAvgServiceDuration;
}

long getFromNormalDist(long a, long b)
{
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<long double> nD(a, b);
    return ceil(nD(gen));
}

long int getFromUniformDist(long a, long b)
{
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<long int> uD(a, b);
    return uD(gen);
};

vector<ServiceStation *> createServiceStationVector(int n)
{
    vector<ServiceStation *> stationVec;
    for (int i = 0; i <= n; i++)
    {
        stationVec.emplace_back(new ServiceStation());
    }
    return stationVec;
}

Node *getPas(long pasId, long serviceTimeNeeded, long arrivalTime)
{
    auto node = new Node(
        pasId, serviceTimeNeeded, arrivalTime);
    return node;
}

void printPasArrivalToQueue(long pasId, string serviceType)
{
    cout << YELLOW << "Passenger " << pasId << " arrived." << RESET << MAGENTA << serviceType << RESET << '\n';
}

void addToQueue(Queue *queue, Node *node, string serviceType)
{
    queue->enqueue(node);
    printPasArrivalToQueue(queue->back->pasId, serviceType);
}

void emptyQueue(Queue *coachQueue, Queue *firstClassQueue,
                vector<ServiceStation *> &vacantCoachStations, vector<ServiceStation *> &vacantFirstClassStations,
                vector<ServiceStation *> &coachStations, vector<ServiceStation *> &firstClassStations,
                long simDuration)
{
    printEmptyingQueueMsg();
    long coachServiceFinishInterval = 0;
    long firstClassServiceFinishInterval = 0;
    while (coachQueue->front != nullptr && firstClassQueue->front != nullptr)
    {
        coachServiceFinishInterval++;
        firstClassServiceFinishInterval++;
        simDuration++;
        servicePas(coachQueue, vacantCoachStations, simDuration, "COACH");
        servicePas(firstClassQueue, vacantFirstClassStations, simDuration, "FIRST CLASS");

        getFinishedServicedPassengers(vacantCoachStations, coachStations, "COACH", coachServiceFinishInterval);
        getFinishedServicedPassengers(vacantFirstClassStations, firstClassStations, "FIRST CLASS", firstClassServiceFinishInterval);
    }
}

/**
 * @brief 
 * 
 * @param coachQueue 
 * @param firstClassQueue 
 * @param coachStations 
 * @param firstClassStations 
 * @return ** void 
 * 
 * Driver Code
 */
void start(Queue *coachQueue, Queue *firstClassQueue, vector<ServiceStation *> &coachStations, vector<ServiceStation *> &firstClassStations)
{
    auto simDuration = getSimDuration();

    auto coachAvgrrivalInterval = getCoachAvgArrivalRate();
    auto coachAvgServiceDuration = getCoachAvgServiceDuration();

    auto firstClassAvgArrivalRate = getFirstClassAvgArrivalRate();
    auto firstClassAvgServiceDuration = getFirstClassAvgServiceDuration();

    auto nextCoachArrivalTime = getFromNormalDist(coachAvgrrivalInterval, 1);
    auto nextFirstClassArrivalTime = getFromNormalDist(firstClassAvgArrivalRate, 1);

    vector<ServiceStation *> vacantCoachStations = coachStations;
    vector<ServiceStation *> vacantFirstClassStations = firstClassStations;

    long nextCoachArrivalInterval = 0;
    long nextFirstClassArrivalInterval = 0;

    long coachServiceFinishInterval = 0;
    long firstClassServiceFinishInterval = 0;

    long currentPasId = 0;

    for (long i = 0; i <= simDuration; i++)
    {
        nextCoachArrivalInterval++;
        nextFirstClassArrivalInterval++;
        coachServiceFinishInterval++;
        firstClassServiceFinishInterval++;

        if (nextCoachArrivalTime <= nextCoachArrivalInterval)
        {
            nextCoachArrivalTime = getFromNormalDist(coachAvgrrivalInterval, 1);
            addToQueue(coachQueue, getPas(currentPasId, i, getFromNormalDist(coachAvgServiceDuration, 1)), "COACH");
            nextCoachArrivalInterval = 0;
            currentPasId++;
        };

        if (nextFirstClassArrivalTime <= nextFirstClassArrivalInterval)
        {
            nextFirstClassArrivalTime = getFromNormalDist(firstClassAvgArrivalRate, 1);
            addToQueue(firstClassQueue, getPas(currentPasId, i, getFromNormalDist(firstClassAvgServiceDuration, 1)), "FIRST CLASS");
            nextFirstClassArrivalInterval = 0;
            currentPasId++;
        };

        servicePas(coachQueue, vacantCoachStations, i, "COACH");
        servicePas(firstClassQueue, vacantFirstClassStations, i, "FIRST CLASS");

        getFinishedServicedPassengers(vacantCoachStations, coachStations, "COACH", coachServiceFinishInterval);
        getFinishedServicedPassengers(vacantFirstClassStations, firstClassStations, "FIRST CLASS", firstClassServiceFinishInterval);
    }

    emptyQueue(coachQueue, firstClassQueue, vacantCoachStations, vacantFirstClassStations, coachStations, firstClassStations, simDuration);
    printFinalStats(coachQueue, firstClassQueue, coachStations, firstClassStations, simDuration);
}