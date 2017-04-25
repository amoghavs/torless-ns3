#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/mobility-module.h"
#include "ns3/netanim-module.h"
#include "ns3/assert.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/flow-monitor-module.h"

using namespace std;
using namespace ns3;
void print_stats(FlowMonitor::FlowStatsContainer stats, Ptr<Ipv4FlowClassifier> classifier, double AppRunTime)
{
	double avgLatency_allFlows = 0.0f; double avgThroughput_allFlows = 0.0f; double temp = 0.0; int count = 0;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator itr = stats.begin (); itr !=  stats.end();  ++itr)  
	{ 
		if  (itr->first > 0)  
		{ 
			Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(itr->first); 
			std::cout <<  "Flow " <<  itr->first  <<  " ("  <<  t.sourceAddress <<  " ->  " <<  t.destinationAddress  <<  ")"; 
			std::cout << "\t"   << itr->second.rxBytes;
			std::cout << "\t"   << itr->second.rxPackets; 
			temp =  itr->second.rxBytes * 8.0 / AppRunTime / 1000  / 1000;  avgThroughput_allFlows+=temp;
			std::cout <<  "\t"  <<  temp <<  " Mbps";  
			temp = (itr->second.delaySum.GetNanoSeconds()) / (float(itr->second.rxPackets)*1000*1000); avgLatency_allFlows+=temp;
			std::cout   <<  "\t"  << temp <<"\n";      
			count+=1;
		} 
	}   
	std::cout<<"\t avgLatency_allFlows "<< (avgLatency_allFlows/count)<<"\t avgThroughput_allFlows "<<(avgThroughput_allFlows/count)<<"\n";
} 
