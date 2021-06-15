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

    Queue coachQueue;
    Queue firstClassQueue;

    ServiceStation coachServiceStation1;
    ServiceStation coachServiceStation2;
    ServiceStation coachServiceStation3;

    ServiceStation firstClassServiceStation1;
    ServiceStation firstClassServiceStation2;
    ServiceStation firstClassServiceStation3;

    long checkInDuration; // Total duration of simulation

    long coachPasAvgArrivalRate;
    long coachPasAvgServiceRate; // Average time it takes to service the next customer in queue

    long firstPasClassAvgArrivalRate;
    long firstPasClassAvgServiceRate;

    cout << "Please enter an unsigned integer value, this will be "
         << "used as the stopping point for the simulation, this value represents the number of\n"
         << "minutes elapsed in the simulation " << MAGENTA << "not real time." << RESET << '\n';
    cout << "Duration time: ";
    cin >> checkInDuration;

    cout << "Average passenger arrival rate of COACH station (in minutes): ";
    cin >> coachPasAvgArrivalRate;

    cout << "Average passenger service duration of COACH station (in minutes): ";
    cin >> coachPasAvgServiceRate;
    // Draw a new value that denotes how long the Passenger actually took, use ceil to bring it to the nearest min
    normal_distribution<double> coachPasAvgServiceRateNd(coachPasAvgServiceRate, 1);

    return 0;
}
