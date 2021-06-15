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
using std::mt19937;
using std::normal_distribution;
using std::random_device;
using std::uniform_int_distribution;
using std::vector;

struct Clock;
class Passenger;
class ServiceStation;
struct Queue;

struct Clock
{
    int hour;
    int min;
};

class Passenger
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
    Passenger pas;
    Queue *next;
};

class ServiceStation
{
    Passenger currentPas;
    Clock remainingServiceTime;
    Clock arrivalTime;
};

int main(int argc, char *argv[])
{
    random_device rd{};
    mt19937 gen{rd()};

    uniform_int_distribution<int> hourUd(0, 23), minUd(0, 59);

    Clock worldClock{hourUd(rd), minUd(rd)};

    Queue coachQueue, firstClassQueue;

    ServiceStation coachServiceStation1, coachServiceStation2, coachServiceStation3;
    ServiceStation firstClassServiceStation1, firstClassServiceStation2, firstClassServiceStation3;

    long checkInDuration; // Total duration of simulation

    long coachPasAvgArrivalRate;
    long coachPasAvgServiceDuration; // Average time it takes to service the next customer in queue

    long firstPasClassAvgArrivalRate;
    long firstPasClassAvgServiceDuration;

    cout << "Please enter an unsigned integer value, this will be "
         << "used as the stopping point for the simulation, this value represents the number of\n"
         << "minutes elapsed in the simulation " << MAGENTA << "not real time." << RESET << '\n';
    cout << "Duration time: ";
    cin >> checkInDuration;

    cout << "Average passenger arrival rate of COACH station (in minutes): ";
    cin >> coachPasAvgArrivalRate;
    normal_distribution<double> coachPasAvgArrivalRateNd(coachPasAvgArrivalRate, 1);

    cout << "Average passenger service duration of COACH station (in minutes): ";
    cin >> coachPasAvgServiceDuration;
    // Draw a new value that denotes how long the Passenger actually took, use ceil to bring it to the nearest min
    normal_distribution<double> coachPasAvgServiceDurationNd(coachPasAvgServiceDuration, 1);

    cout << "Average passenger arrival rate of FIRST CLASS station (in minutes): ";
    cin >> firstPasClassAvgArrivalRate;
    normal_distribution<double> firstPasClassAvgArrivalRateNd(firstPasClassAvgArrivalRate, 1);

    cout << "Average passenger service duration of FIRST CLASS station (in minutes): ";
    cin >> firstPasClassAvgServiceDuration;
    normal_distribution<double> firstPasClassAvgServiceDurationNd(firstPasClassAvgServiceDuration, 1);

    return 0;
}
