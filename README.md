# Cyber-Physical Testbed for Smart Grid Analysis

This project demonstrates a **cyber-physical co-simulation** framework where a 3-bus power system with wind integration is modeled in **Simulink**, and its communication/control network is simulated using **NS-3**. The testbed evaluates how network performance impacts real-time control and system stability in smart grids.

## 🧠 Objective

To implement a demand response scenario in a smart grid environment using a 3-bus power system:

Bus 1: Wind power generator (renewable and fluctuating supply).

Bus 2: Slack bus with oil generator (dispatchable conventional source).

Bus 3: Load bus with varying demand.

This setup was used to:

Simulate real-time control and response to changes in demand and wind generation, Basically implement a demand response scenario in which based on varying wind power we change the slack bus power generated and also change the load power drawn upto some extent to use the wind power(renewable source) upto its maximum extent and when lower power is generated at the wind bus than required at load bus then we slighlty change the load power drawn upto 10-30% based on the wind power generated.

Summary:

-Wind power is utilized to the maximum possible extent.

-Slack bus generation dynamically adjusts to compensate for wind fluctuations.

-Load demand is partially flexible—reduced by 10–30% when wind generation is insufficient—ensuring system stability while maintaining efficiency.

Create a cyber-physical co-simulation where:

Simulink models the physical dynamics and generation/load profiles.

NS-3 simulates communication between nodes (sensor, controller, actuator).

Study the impact of network performance (delay, loss, attack) on control loop stability and overall power system resilience.

---

## 🛠️ Tools & Technologies

- **Simulink**: Models the physical 3-bus power system, including wind generation and load.
- **NS-3**: Simulates the IP-based communication network with three nodes.
- **MATLAB (writeToPipe.m)**: Writes sensor data from Simulink to a file for NS-3 to read.
- **WSL (Ubuntu)**: Runs NS-3 and manages file-based communication.
- **File-based Pipelines**: Used for real-time data transfer between Simulink and NS-3.
- **UDP (via TAP interface)**: Optional real-time data transport method for testing.

---

## ⚙️ System Architecture

Simulink (Wind Bus)
↓
writeToPipe.m → debug file 1
↓
NS-3 Node0 (Sensor Node)
↓
NS-3 Node1 (Command Center) → control logic
↓
debug file 2 → Simulink (Load Bus)
↓



## Applies control instruction
- **Node0** receives sensor data from Simulink and forwards it to **Node1**.
- **Node1** processes the data (e.g., checks thresholds, applies control logic).
- **Node2** sends the resulting control command back to Simulink through a file.

---

## 🔄 Data Flow

1. **Simulink → NS-3**:  
   Real-time wind power values are generated and saved to a file using `writeToPipe.m`.

2. **NS-3**:  
   - Node0 reads the value, sends it to Node1.
   - Node1 executes control logic, saves the response in a second debug file and the executed control logic is sent to Node2
   - Node2 sends the control logic to a second debug file and sends to the simulink again which changes the load and slack power generated accordingly with the logic received.

3. **NS-3 → Simulink**:  
   Simulink reads the control command from the second file and updates the load bus.

---

## 📁 File Structure

PS_project files/

├── PS_Readme_file.txt

├── base paper

├── ns3 files/

│   ├── final_destination.cc(final ns3 file)

│   ├── MITM_morph.cc(fully not implemented the MITM attack)

├── Simulink files/

│   ├── sendToPipe.m

│   ├── TestBed_simulation.slx


## ✅ Features Implemented

- Full **3-bus power system simulation** with wind power dynamics.
- **Bidirectional data exchange** using file and UDP pipelines.
- Custom **control logic** at Node1 (e.g., simple thresholding).
- **Routing setup** using `Ipv4GlobalRoutingHelper::PopulateRoutingTables()` for packet flow.

---

## 🧪 Future Improvements

- Replace file-based pipeline with robust **real-time UDP communication**.
- Implement **cyber-attack simulation modules** in NS-3.
- Expand power system to include **more buses and dynamic loads**.
- Perform **quantitative stability analysis** under varying network conditions.

---

## 📚 References

- PS_Base paper attached in the repo.
- NS-3 documentation: https://www.nsnam.org/
- Simulink and MATLAB Real-Time Communication Blocks

---

## 👨‍💻 Author

Keerthan – B.Tech Electrical Engineering, IIT Ropar  
Project focused on Smart Grid Cyber-Physical Security and Simulation

