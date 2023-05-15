// Network topology
//
//       n0    n1   n2   n3       n21
//       |     |    |    |        |
//       ================= ... ===
//              LAN 10.0.0.x
//
// - Servidor em n0, clientes em n1 ... n21
// - Todos os clientes pingam no servidor e em todos os outros clientes, um de cada vez

#include <fstream>
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/internet-module.h"

#include "ns3/netanim-module.h"
#include "ns3/mobility-module.h"
#include "ns3/v4ping-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("UdpClientsServerIOT-v2");

int 
main (int argc, char *argv[]){
    uint16_t numClients = 20;

    // --- LOGGING --- //
    LogComponentEnable ("UdpClientsServerIOT-v2", LOG_LEVEL_ALL);

    // --- CRIACAO DOS NODES --- //    
    NS_LOG_INFO ("Create nodes.");

    NodeContainer allNodes; //node[numClients] eh o servidor (ultimo node)
    allNodes.Create(numClients + 1); 

    // --- INTERNET --- // 
    NS_LOG_INFO("Install internet protocols stack.");
    InternetStackHelper internet;
    internet.Install (allNodes);

    // --- SET E INSTALL CANAL --- //
    NS_LOG_INFO("Set channel attributes.");
    CsmaHelper csma;
    csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate (5000000)));
    csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
    csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));

    NS_LOG_INFO("Install channel.");
    NetDeviceContainer csmaDevices = csma.Install (allNodes);

    // --- SET E INSTALL IP ADRESSES --- //
    Ipv4AddressHelper address;
    address.SetBase ("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interface = address.Assign (csmaDevices);

    // --- APLICACOES --- //
    // So existem aplicacoes ping nos clientes, no servidor nao ha aplicacao
    // Cada client tem numNodes-1 aplicacoes ping, cada uma com um endereco de destino (so nao tem a do proprio endereco)
    // Cada aplicacao eh uma posicao do vetor pingApps
    NS_LOG_INFO("Set applications.");
    std::vector<ApplicationContainer> pingApps(numClients);
    for(uint16_t i = 0; i < numClients; ++i){
        for(uint16_t j = 0; j < (numClients + 1); ++j){
            if(j != i){
                V4PingHelper pingHelperAux(interface.GetAddress(j)); //pinga inclusive no servidor
                pingHelperAux.SetAttribute ("Verbose", BooleanValue (false));
                pingHelperAux.SetAttribute ("Interval", TimeValue (Seconds(numClients)));
                pingHelperAux.SetAttribute ("Size", UintegerValue (16));
                pingApps[i].Add(pingHelperAux.Install(allNodes.Get(i))); // Instala ping(dest: j) no cliente i
            }
        }
    }

    // Os nodes comecam a pingar com 1 segundo de diferenca
    // pingApps[i] == aplicacao do cliente [i]
    for(uint16_t i = 0; i < numClients; ++i){
        uint16_t appStart = 1.0 + 1.0*i;
        pingApps[i].Start(Seconds(appStart));
        pingApps[i].Stop(Seconds(appStart + (numClients) * 2 + 1.0)); //Termina apos enviar 3 pacotes
    }
    
    // TRACING
    AsciiTraceHelper ascii;
    csma.EnableAsciiAll (ascii.CreateFileStream ("clients-server-IOT-v2.tr"));
    csma.EnablePcapAll ("clients-server-IOT-v2.tr", false);

    // --- NETANIM --- //
    NS_LOG_INFO("Set animation.");
    AnimationInterface anim ("clients-server-IOT-v2-anim.xml");

    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
    mobility.Install(allNodes);

    uint32_t X0 = 20, Y0 = 20, x = 0, y = 0;
    for(uint32_t i= 0; i<numClients; ++i){
        x = X0 + (10 * (i%5));
        y = Y0 + (10 * int(i/5));
        anim.SetConstantPosition(allNodes.Get(i), x, y);
    }
    anim.SetConstantPosition(allNodes.Get(numClients), (X0 + 20), (X0 - 20)); //server = node[numClients]

    // --- EXECUCAO --- //
    NS_LOG_INFO("Run simulation.");
    Simulator::Run ();
    Simulator::Destroy ();
    NS_LOG_INFO ("Done.");

    return 0;
}