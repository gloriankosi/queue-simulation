/**
 * @file queue-simulation.cc
 * @author Glorian Kosi
 * @brief 
 * 
 * See readme
 * 
 */
#include <iostream>
#include <iterator>
#include <random>
#include <vector>

using std::cin;
using std::cout;
using std::vector;

typedef struct
{
    int hour;
    int min;
    int sec;
} Clock;

typedef class ServiceStation
{

} ServiceStation;

typedef class
{
    long int passId;
    int serviceType;
    Clock arrivalTime; // Arrival to queue
    Clock serviceTime; // Time elapsed to enter service station
} PassengerType;

int main(int argc, char *argv[])
{
    double checkInDuration;
    cout << "Duration of check-in (in minutes): ";
    cin >> checkInDuration;

    return 0;
}