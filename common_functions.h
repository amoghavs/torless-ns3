#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cfloat>
#include <cstddef>
#include <math.h>

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
double SimTime        = 10.00;
double SinkStartTime  = 2.0001;
double SinkStopTime   = 8.90001;
double AppStartTime   = 3.0001;
double AppStopTime    = 4.00001;
double AppRunTime = AppStopTime - AppStartTime;

int AppPacketSize = 1000;
//std::string AppPacketRate ("40Kbps");
//std::string AppPacketRate ("80Kbps");
std::string AppPacketRate ("160Kbps");
//std::string AppPacketRate ("320Kbps");
std::string LinkRate ("10Mbps");

//std::string AppPacketRate ("400Kbps");
//std::string AppPacketRate ("800Kbps");
//std::string AppPacketRate ("1600Kbps");
//std::string AppPacketRate ("3200Kbps");
//std::string LinkRate ("100Mbps");

//std::string AppPacketRate ("400Kbps");
//std::string AppPacketRate ("800Kbps");
//std::string AppPacketRate ("1600Kbps");
//std::string AppPacketRate ("3200Kbps");
//std::string LinkRate ("1000Mbps");

std::string LinkDelay ("2ms");
 


void print_stats(FlowMonitor::FlowStatsContainer stats, Ptr<Ipv4FlowClassifier> classifier, double AppRunTime)
{
	double avgLatency_allFlows = 0.0f; double avgThroughput_allFlows = 0.0f; double temp = 0.0; int count = 0;
	double minLatency = DBL_MAX;
	double maxLatency = 0;
	double minThroughput = DBL_MAX;
	double maxThroughput = 0;
	long int totalPackets = 0;
	long int lostPackets = 0;
	long int timesForwarded = 0;
	for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator itr = stats.begin (); itr !=  stats.end();  ++itr)  
	{ 
		if  (itr->first > 0)  
		{ 
			//Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(itr->first); 
			//std::cout <<  "Flow " <<  itr->first  <<  " ("  <<  t.sourceAddress <<  " ->  " <<  t.destinationAddress  <<  ")"; 
			//std::cout << "\t"   << itr->second.rxBytes;
			//std::cout << "\t"   << itr->second.rxPackets; 
			totalPackets += itr->second.rxPackets;
			lostPackets += itr->second.lostPackets;
			timesForwarded += itr->second.timesForwarded;
			temp =  itr->second.rxBytes * 8.0 / AppRunTime / 1000  / 1000;  avgThroughput_allFlows+=temp;
			if (temp > maxThroughput)
				maxThroughput = temp;
			if (temp < minThroughput)
				minThroughput = temp;
			//std::cout <<  "\t"  <<  temp <<  " Mbps";  
			temp = (itr->second.delaySum.GetNanoSeconds()) / (float(itr->second.rxPackets)*1000*1000); 
			if(~isnan(temp))
				avgLatency_allFlows+=temp;
			if (temp > maxLatency)
				maxLatency = temp;
			if (temp < minLatency)
				minLatency = temp;
			//std::cout   <<  "\t"  << temp <<"\n";      
			count+=1;
		} 
	}   
	double avgLatency = (avgLatency_allFlows/count);
	double avgThroughput = (avgThroughput_allFlows/count);
	std::cout<<"Stats\t"<<AppPacketRate<<"\t"<<LinkRate<<"\t"<<avgLatency<<"\t"<<avgThroughput<<"\t"<<totalPackets<<"\t"<<lostPackets<<"\t"<<timesForwarded<<"\n";
	printf("Avg latency = %lf, min latency = %lf, max latency = %lf\n", avgLatency, minLatency, maxLatency); 
	printf("Avg thoughput = %lf, min thoughput = %lf, max thoughput = %lf\n", avgThroughput, minThroughput, maxThroughput); 
	printf("Total Packets = %ld\n", totalPackets);
	printf("Lost Packets = %ld\n", lostPackets);
	printf("Times forwarded = %ld\n", timesForwarded);
}
