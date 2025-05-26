POWER SYSTEM TERM PROJECT
README FILE
WSL Setup on Windows (Windows Subsystem for Linux)
Follow these steps to install and set up WSL on your Windows machine:
Step-by-Step Instructions
1. Enable WSL and Virtual Machine Platform

Open PowerShell as Administrator and run:
        wsl --install
   2. Restart Your Computer
A system reboot is required after enabling features.
   3. Install a Linux Distribution
Example for Ubuntu:
                wsl --install -d Ubuntu
   4. Launch and Set Up Ubuntu
   * Open Ubuntu from Start Menu.
   * Set your UNIX username and password when prompted.
Update and Upgrade Packages
        sudo apt update && sudo apt upgrade -y


NS-3 Setup on Windows (Using WSL)
NS-3 is a network simulator best supported on Linux. To run NS-3 on Windows, use WSL (Windows Subsystem for Linux) with a supported distribution like Ubuntu.
Step-by-Step Installation
   1. Open Ubuntu (in WSL)

Install Required Dependencies

        sudo apt update
sudo apt install -y build-essential g++ python3 python3-dev \
    pkg-config sqlite3 libsqlite3-dev \
    qt5-default mercurial git cmake \
    libc6-dev libc6-dev-i386 gdb \
    valgrind gsl-bin libgsl-dev \
    libgtk-3-dev vtun lxc
      2. Create a Working Directory
mkdir ~/ns3
cd ~/ns3
         3. Download the NS-3 Source Code
Clone the official repository:
        git clone https://gitlab.com/nsnam/ns-3-dev.git
cd ns-3-dev
         4. Build NS-3
First, configure the build system:
        ./ns3 configure
Then, compile the code:
        ./ns3 build
Run a Sample Simulation
                ./ns3 run hello-simulator
         5. (Optional) Use the NS-3 IDE/GUI
NS-3 does not have a full GUI, but you can view animations using:
        ./ns3 run scratch/first --vis


How to run?
1.Create fifo pipelines named /tmp/wind_data_debug.txt     
  //used for sending wind power from simulink to ns3
 /tmp/control_data_debug.txt   //used for receiving commands to simulink to ns3.
Using the commands in WSL Terminal
         “mkfifo /tmp/wind_data_debug.txt”
         “mkfifo /tmp/control_data_debug.txt” 


2. You can check if something is being written into the pipe or not by running the below command
“cat /tmp/control_data_debug.txt” or “cat /tmp/wind_data_debug.txt”  


3. Save the simulink file and Matlab file in the same folder.


4. In Matlab command window run the command addpath('C:\Users\YourName\Documents\MATLAB\MyFunctions');    
( Inside addpath command, write the location of the folder in which both files are saved.) 
(This adds the path for matlab to communicate with Simulink).


5. Set the P_Load value to be 1000W(for example). By running below command in the matlab command window. 
       “ P_Load = 1000”
(NOTE : if you don’t find the variable P_Load in the load block active power parameter then please enter it in the active power tab.)
 
6. Now, Run the simulink file.


// for ns3 running


7. In the WSL terminal, run the command “explorer.exe .” to see all the files of ns3 in file explorer. Here open the scratch folder and save the final_destination.cc file in the scratch folder. Then we need to build the file using the following command.
    Run these commands in the wsl terminal.
   “cd ns-3-dev”
   “./ns3 build scratch/final_destination.cc”


6. Run the simulation.


7. Run the ns3 file using this command.
    ./ns3 run scratch/final_destination.cc


8. Note: First run the Simulink file then run the Ns3 file for better viewing of output.


9. If everything is functioning correctly, the terminal will display the communication process, including our print statements showing wind power values, commands, and data being transmitted between nodes.


10. In the simulink console window , you can see the command sent from the Node 2 of ns3. According to the command , the load value will change . You can see this in the workspace or by clicking the Load.

11. Do the same for “MITM_morph.cc” ns3 file for MITM attack simulation.




THANK YOU.