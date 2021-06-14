# CS610 Assignment #1 Implementing queue data type
- Implement a queue data type. Then use it to simulate the evolution of waiting queues, by invoking several instances of the queue data type.

## Process
- We have two classes of service: first class and coach. Each class of service has a dedicated queue.
- There are three service stations for first-class and a single first-class queue.
- There are four coach service stations and a single coach queue.
- Each service station takes passengers from the corresponding queue, but if a first-class service station is free and the queue for the coach is not empty then the service station serves passengers from the coach queue.
- Passenger arrival times are random, but subject to average arrival times; for example, we get a first-class passenger every five minutes and a coach passenger every two minutes on average. Actual arrival times are random.
- Passenger service times are also random, but subject to average service times; for example, first-class passengers usually require six minutes of service and coach passengers get on average 2 minutes of service. These are average times; actual times vary. All times are measured in minutes.
- The simulation starts with empty waiting queues and free service stations. At some point in time (usually 40 minutes prior to departure time) the company closes the queues (no more passengers are admitted); then the simulation ends when all the queues are empty and all the service stations are free.

## Inputs to the simulation (During runtime)
- The duration of the check-in (make it arbitrarily long, do not worry about it being or not being realistic.
- The coach average arrival rate, and average service rate.
- The first-class average arrival rate and average service rate.

## Outputs of the Simulation
- The duration of the simulation (which may be longer than the input parameter, as when check-in closes, there may be passengers in the waiting queues and service stations).
- The maximum length of the queue for each queue.
- The average and maximum waiting time for each queue.
- The rate of occupancy of each service station (percentage of time each station was busy).
- If you want: show the real-time evolution of the queues during the run-time simulation.
