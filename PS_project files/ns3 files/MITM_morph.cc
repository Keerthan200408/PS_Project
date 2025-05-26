#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/udp-socket.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/point-to-point-module.h"

#include <fstream>
#include <string>

using namespace ns3;

double t1 = 0.0;
double t2 = 0.0;

NS_LOG_COMPONENT_DEFINE("SimulinkPipeReceiver");

Ptr<Socket> udpSocket;
Ptr<Socket> controlSocket;
std::ifstream inputFile;
std::string lastLine = "";

// Global flag for attack
bool attackActive = false;

void node0Receiver()
{
    std::ifstream inputFile("/tmp/wind_data_debug.txt");
    std::string line, lastFileLine;

    if (inputFile.is_open())
    {
        // Get only the last line
        while (std::getline(inputFile, line))
        {
            if (!line.empty())
                lastFileLine = line;
        }
        inputFile.close();

        if (lastFileLine != lastLine)
        {
            lastLine = lastFileLine;

            t1 = Simulator::Now().GetSeconds();
            NS_LOG_INFO("Node0 received and sending: " << lastLine << " at t = " << t1 << "s");

            // Send to Node1
            Ptr<Packet> packet = Create<Packet>((uint8_t*)lastLine.c_str(), lastLine.length());
            udpSocket->SendTo(packet, 0, InetSocketAddress(Ipv4Address("10.1.2.2"), 9999));

            // Optional: clear pipe if needed
            std::ofstream clearFile("/tmp/wind_data_pipe", std::ofstream::out | std::ofstream::trunc);
        }
    }
    else
    {
        NS_LOG_ERROR("Could not open /tmp/wind_data_debug.txt");
    }

    // Schedule next check
    Simulator::Schedule(MilliSeconds(500), &node0Receiver);
}

void Node1Receive(Ptr<Socket> socket)
{
    Ptr<Packet> packet;
    Address from;
    while ((packet = socket->RecvFrom(from)))
    {
        // Copy for use in scheduler
        Ptr<Packet> myCopy = packet->Copy();

        Simulator::Schedule(MilliSeconds(100), [myCopy]() {
            uint8_t *buffer = new uint8_t[myCopy->GetSize() + 1];
            myCopy->CopyData(buffer, myCopy->GetSize());
            buffer[myCopy->GetSize()] = '\0';

            std::string receivedData = std::string((char*)buffer);
            NS_LOG_INFO("Node 1 received: " << receivedData << " at t = " << Simulator::Now().GetSeconds() << "s");

            std::string controlInstruction;
            double windPower = std::stod(receivedData);

            if (!attackActive)
            {
                // Normal operation
                if (windPower < 200.0)
                    controlInstruction = "x";
                else if (200.0 < windPower && windPower < 500.0)
                    controlInstruction = "y";
                else
                    controlInstruction = "z";
                Ptr<Packet> controlPacket = Create<Packet>((uint8_t*)controlInstruction.c_str(), controlInstruction.length());
                controlSocket->Send(controlPacket);
                NS_LOG_INFO("Node 1 sent control to Node 2: " << controlInstruction << " at t = " << Simulator::Now().GetSeconds() << "s");
            }
            else
            {
                // MITM: Modify and delay
                if (windPower < 200.0)
                    controlInstruction = "z"; // wrong command
                else if (200.0 < windPower && windPower < 500.0)
                    controlInstruction = "x"; // wrong command
                else
                    controlInstruction = "y";
                Ptr<Packet> controlPacket = Create<Packet>((uint8_t*)controlInstruction.c_str(), controlInstruction.length());

                Simulator::Schedule(Seconds(2.0), [controlPacket]() {
                    controlSocket->Send(controlPacket);
                    NS_LOG_INFO("***** MITM Attack Modified & Delayed Control Sent *****");
                });

                NS_LOG_INFO("***** MITM Attack Modified Control: " << controlInstruction << " *****");
            }

            delete[] buffer;
        });
    }
}

void SetupNode1Receiver(Ptr<Node> node)
{
    Ptr<Socket> recvSocket = Socket::CreateSocket(node, UdpSocketFactory::GetTypeId());
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 9999);
    recvSocket->Bind(local);
    recvSocket->SetRecvCallback(MakeCallback(&Node1Receive));
}

void Node2SendToSimulink(std::string controlData)
{
    std::ofstream outputFile("/tmp/control_data_debug.txt", std::ios::out | std::ios::trunc);
    if (outputFile.is_open())
    {
        outputFile << controlData << std::endl;
        outputFile.close();
        t2 = Simulator::Now().GetSeconds();
        NS_LOG_INFO("Node 2 wrote to Simulink: " << controlData << " at t = " << t2 << "s");
        NS_LOG_INFO("We are having a total delay time of " << t2 - t1 << "s per data packet");
    }
    else
    {
        NS_LOG_ERROR("Failed to open control_debug.txt to write");
    }
}

void Node2Receive(Ptr<Socket> socket)
{
    Address from;
    Ptr<Packet> packet;

    while ((packet = socket->RecvFrom(from)))
    {
        Simulator::Schedule(MilliSeconds(200), [packet]() {
            uint8_t *buffer = new uint8_t[packet->GetSize() + 1];
            packet->CopyData(buffer, packet->GetSize());
            buffer[packet->GetSize()] = '\0';

            std::string receivedInstruction((char *)buffer);
            NS_LOG_INFO("Node 2 received from Node 1: " << receivedInstruction << " at t = " << Simulator::Now().GetSeconds() << "s");
            Node2SendToSimulink(receivedInstruction);

            delete[] buffer;
        });
    }
}

void SetupNode2Receiver(Ptr<Node> node)
{
    Ptr<Socket> recvSocket = Socket::CreateSocket(node, UdpSocketFactory::GetTypeId());
    InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 9999);
    recvSocket->Bind(local);
    recvSocket->SetRecvCallback(MakeCallback(&Node2Receive));
}

int main(int argc, char *argv[])
{
    LogComponentEnable("SimulinkPipeReceiver", LOG_LEVEL_INFO);
    GlobalValue::Bind("SimulatorImplementationType", StringValue("ns3::RealtimeSimulatorImpl"));

    NodeContainer nodes;
    nodes.Create(3); // Node 0, Node1 and Node2

    PointToPointHelper p2p1;
    p2p1.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p1.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devices1 = p2p1.Install(nodes.Get(0), nodes.Get(1));

    PointToPointHelper p2p2;
    p2p2.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p2.SetChannelAttribute("Delay", StringValue("2ms"));
    NetDeviceContainer devices2 = p2p2.Install(nodes.Get(1), nodes.Get(2));

    InternetStackHelper stack;
    stack.Install(nodes);

    Ipv4AddressHelper address1;
    address1.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces1 = address1.Assign(devices1);

    Ipv4AddressHelper address2;
    address2.SetBase("10.1.3.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces2 = address2.Assign(devices2);

    // UDP socket on Node 0 to send to Node1
    udpSocket = Socket::CreateSocket(nodes.Get(0), UdpSocketFactory::GetTypeId());
    udpSocket->Connect(InetSocketAddress(Ipv4Address("10.1.2.2"), 9999));

    // UDP socket from Node1 to Node2
    controlSocket = Socket::CreateSocket(nodes.Get(1), UdpSocketFactory::GetTypeId());
    controlSocket->Connect(InetSocketAddress(Ipv4Address("10.1.3.2"), 9999));

    // Set up receivers
    SetupNode1Receiver(nodes.Get(1));
    SetupNode2Receiver(nodes.Get(2));

    // Schedule the MITM attack activation and deactivation
    Simulator::Schedule(Seconds(60.0), []() { // 1 min
        attackActive = true;
        NS_LOG_INFO("***** MITM ATTACK STARTED *****");
    });
    Simulator::Schedule(Seconds(200.0), []() { // 1:45
        attackActive = false;
        NS_LOG_INFO("***** MITM ATTACK STOPPED *****");
    });

    // Start receiving and printing the data from the pipe
    Simulator::Schedule(Seconds(1.0), &node0Receiver);

    Simulator::Stop(Seconds(300.0));
    Simulator::Run();
    Simulator::Destroy();

    std::ofstream clearFile("/tmp/wind_data_debug.txt", std::ofstream::out | std::ofstream::trunc);

    return 0;
}