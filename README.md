# Train Track Crossing Simulation

The project involves creating a system that simulates the operation of a railway crossing. In this scenario, multiple trains can approach the crossing from different directions, but only one train can occupy the crossing at a time to avoid collisions. Synchronizing access to the intersection is crucial and will be managed using traffic lights. You will have to apply the concepts of synchronization and mutual exclusion, in addition to thinking about prioritization if trains from different directions have different priorities.

Trains can approach the crossing from tracks A1 or B1 and proceed to tracks A2 or B2. For example, they can come from A1 and go to A2 or come from A1 and go to B2. Or they arrived from B1 and go to B2 (or A2). They always arrive via A1 or B1 and always go to A2 or B2.

## Requirements

- **Train Modeling**: Each train must be represented by a thread (or process). Trains can have different priorities, which influence the order of access to the crossing. You must generate trains at random time intervals coming from one of the A1 or B1 directions. Consider that new trains can always arrive at the crossing.
- **Priorities**: Each train can have 3 different priorities: High, Medium, and Low.
- **Crossing**: Should be treated as a shared resource, where access is controlled by traffic lights. Only one train can pass through the crossing at a time.
- **Traffic Lights**: Use traffic lights to manage access to the intersection. It must be ensured that traffic lights are used to avoid race conditions and deadlocks, allowing safe and efficient passage.
- **User Interface**: A simple interface that shows the current status of trains (approaching, waiting, crossing) and the status of the crossing. It can be a simple text output or a basic graphical interface.

## Functionalities

- **Program Start**: When starting, the program should create multiple threads representing trains approaching the intersection from different directions.
- **Access Request**: When a train approaches the crossing, it must request access. If another train is already crossing, the train must wait until the crossing is clear.
- **Prioritization**: In case of multiple trains waiting, priority (which can be determined by factors such as order of arrival, type of train, etc.) must decide which train gains access to the crossing.
- **Crossing**: Once access is granted, the train "crosses" the crossing. This event must be visible in the user interface. Note that if two trains approach the intersection from the same route (and direction), the one that arrived first must pass first regardless of priority (after all, the train behind cannot pass over the other).
- **Crossing Release**: After crossing, the train releases the crossing to the next train in the queue.

## Output Example
![Texto alternativo](https://github.com/claricesoares/RTS-Project1/blob/main/exemploSaida.jpeg)

## Code Explanation
[Youtube - Train Track Crossing Simulation](https://youtu.be/UT2bSEC5Oa8?si=dylzIIFF3kfX_GBP)

## Team

- [Clarice Soares](https://github.com/claricesoares)

- [Julia Ramalho](https://github.com/ramalhocsjulia)

- [Samara Cardoso](https://github.com/SamaraLimaCardoso)
