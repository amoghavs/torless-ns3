/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2010 Egemen K. Cetinkaya, Justin P. Rohrer, and Amit Dandekar
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Egemen K. Cetinkaya <ekc@ittc.ku.edu>
 * Author: Justin P. Rohrer    <rohrej@ittc.ku.edu>
 * Author: Amit Dandekar       <dandekar@ittc.ku.edu>
 *
 * James P.G. Sterbenz <jpgs@ittc.ku.edu>, director
 * ResiliNets Research Group  http://wiki.ittc.ku.edu/resilinets
 * Information and Telecommunication Technology Center 
 * and
 * Department of Electrical Engineering and Computer Science
 * The University of Kansas
 * Lawrence, KS  USA
 *
 * Work supported in part by NSF FIND (Future Internet Design) Program
 * under grant CNS-0626918 (Postmodern Internet Architecture) and 
 * by NSF grant CNS-1050226 (Multilayer Network Resilience Analysis and Experimentation on GENI)
 *
 * This program reads an upper triangular adjacency matrix (e.g. adjacency_matrix.txt) and
 * node coordinates file (e.g. node_coordinates.txt). The program also set-ups a
 * wired network topology with P2P links according to the adjacency matrix with
 * nx(n-1) CBR traffic flows, in which n is the number of nodes in the adjacency matrix.
 */

// ---------- Header Includes -------------------------------------------------
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
#include "common_functions.h"

using namespace std;
using namespace ns3;

// ---------- Prototypes ------------------------------------------------------

vector<vector<bool> > readNxNMatrix (std::string adj_mat_file_name);
vector<vector<double> > readCordinatesFile (std::string node_coordinates_file_name);
void printCoordinateArray (const char* description, vector<vector<double> > coord_array);
void printMatrix (const char* description, vector<vector<bool> > array);

NS_LOG_COMPONENT_DEFINE ("GenericTopologyCreation");

int main (int argc, char *argv[])
{

	// ---------- Simulation Variables ------------------------------------------

	// Change the variables and file names only in this block!

	Config::SetDefault  ("ns3::OnOffApplication::PacketSize",StringValue ("2000"));
	Config::SetDefault ("ns3::OnOffApplication::DataRate",  StringValue (AppPacketRate));
	Config::SetDefault ("ns3::Ipv4GlobalRouting::RespondToInterfaceEvents", BooleanValue (true));
	Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting", BooleanValue (true));
	Config::SetDefault("ns3::Ipv4::IpForward", BooleanValue (true));
	Config::SetDefault("ns3::Ipv4::WeakEsModel", BooleanValue (true));
	//  DropTailQueue::MaxPackets affects the # of dropped packets, default value:100
	//  Config::SetDefault ("ns3::DropTailQueue::MaxPackets", UintegerValue (1000));

	srand ( (unsigned)time ( NULL ) );   // generate different seed each time

	std::string tr_name ("n-node-ppp-torless.tr");
	std::string pcap_name ("n-node-ppp-torless");
	std::string flow_name ("n-node-ppp-torless.xml");
	std::string anim_name ("n-node-ppp-torless.anim.xml");

	std::string adj_mat_file_name ("scratch/torless_matrix.txt");
	std::string node_coordinates_file_name ("scratch/torless_node_coordinates.txt");

	CommandLine cmd;
	cmd.Parse (argc, argv);

	// ---------- End of Simulation Variables ----------------------------------

	// ---------- Read Adjacency Matrix ----------------------------------------

	vector<vector<bool> > Adj_Matrix;
	Adj_Matrix = readNxNMatrix (adj_mat_file_name);


	// Optionally display 2-dimensional adjacency matrix (Adj_Matrix) array
	//printMatrix (adj_mat_file_name.c_str (),Adj_Matrix);

	// ---------- End of Read Adjacency Matrix ---------------------------------

	// ---------- Read Node Coordinates File -----------------------------------

	vector<vector<double> > coord_array;
	coord_array = readCordinatesFile (node_coordinates_file_name);

	// Optionally display node co-ordinates file
	// printCoordinateArray (node_coordinates_file_name.c_str (),coord_array);

	int n_nodes = coord_array.size ();
	int matrixDimension = Adj_Matrix.size ();

	if (matrixDimension != n_nodes)
	{
		NS_FATAL_ERROR ("The number of lines in coordinate file is: " << n_nodes << " not equal to the number of nodes in adjacency matrix size " << matrixDimension);
	}

	// ---------- End of Read Node Coordinates File ----------------------------

	// ---------- Network Setup ------------------------------------------------

	NS_LOG_INFO ("Create Nodes.");

	NodeContainer nodes;   // Declare nodes objects
	nodes.Create (n_nodes);

	NS_LOG_INFO ("Create P2P Link Attributes.");

	PointToPointHelper p2p;
	p2p.SetDeviceAttribute ("DataRate", StringValue (LinkRate));
	p2p.SetDeviceAttribute ("InterframeGap", StringValue (InterFrameGap));
	p2p.SetChannelAttribute ("Delay", StringValue (LinkDelay));

	NS_LOG_INFO ("Install Internet Stack to Nodes.");

	InternetStackHelper internet;
	internet.Install (NodeContainer::GetGlobal ());

	NS_LOG_INFO ("Assign Addresses to Nodes.");

	Ipv4AddressHelper ipv4_n;
	ipv4_n.SetBase ("10.0.0.0", "255.255.255.252");

	NS_LOG_INFO ("Create Links Between Nodes.");

	uint32_t linkCount = 0;

	for (size_t i = 0; i < Adj_Matrix.size (); i++)
	{
		for (size_t j = 0; j < Adj_Matrix[i].size (); j++)
		{

			if (Adj_Matrix[i][j] == 1)
			{
				NodeContainer n_links = NodeContainer (nodes.Get (i), nodes.Get (j));
				NetDeviceContainer n_devs = p2p.Install (n_links);
				ipv4_n.Assign (n_devs);
				ipv4_n.NewNetwork ();
				linkCount++;
				NS_LOG_INFO ("matrix element [" << i << "][" << j << "] is 1");
			}
			else
			{
				NS_LOG_INFO ("matrix element [" << i << "][" << j << "] is 0");
			}
		}
	}
	NS_LOG_INFO ("Number of links in the adjacency matrix is: " << linkCount);
	NS_LOG_INFO ("Number of all nodes is: " << nodes.GetN ());

	NS_LOG_INFO ("Initialize Global Routing.");
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	// ---------- End of Network Set-up ----------------------------------------

	// ---------- Allocate Node Positions --------------------------------------

	NS_LOG_INFO ("Allocate Positions to Nodes.");

	MobilityHelper mobility_n;
	Ptr<ListPositionAllocator> positionAlloc_n = CreateObject<ListPositionAllocator> ();

	for (size_t m = 0; m < coord_array.size (); m++)
	{
		positionAlloc_n->Add (Vector (coord_array[m][0], coord_array[m][1], 0));
		Ptr<Node> n0 = nodes.Get (m);
		Ptr<ConstantPositionMobilityModel> nLoc =  n0->GetObject<ConstantPositionMobilityModel> ();
		if (nLoc == 0)
		{
			nLoc = CreateObject<ConstantPositionMobilityModel> ();
			n0->AggregateObject (nLoc);
		}
		// y-coordinates are negated for correct display in NetAnim
		// NetAnim's (0,0) reference coordinates are located on upper left corner
		// by negating the y coordinates, we declare the reference (0,0) coordinate
		// to the bottom left corner
		Vector nVec (coord_array[m][0], -coord_array[m][1], 0);
		nLoc->SetPosition (nVec);

	}
	mobility_n.SetPositionAllocator (positionAlloc_n);
	mobility_n.Install (nodes);

	// ---------- End of Allocate Node Positions -------------------------------

	// ---------- Create n*(n-1) CBR Flows -------------------------------------

	NS_LOG_INFO ("Setup Packet Sinks.");

	uint16_t port = 9;

	for (int i = 0; i < n_nodes; i++)
	{
		PacketSinkHelper sink ("ns3::UdpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), port));
		ApplicationContainer apps_sink = sink.Install (nodes.Get (i));   // sink is installed on all nodes
		apps_sink.Start (Seconds (SinkStartTime));
		apps_sink.Stop (Seconds (SinkStopTime));
	}

	NS_LOG_INFO ("Setup CBR Traffic Sources.");

	for (int i = 0; i < n_nodes; i++)
	{
		for (int j = 0; j < n_nodes; j++)
		{
			if (i == 3 && j == 11)
			{

				// We needed to generate a random number (rn) to be used to eliminate
				// the artificial congestion caused by sending the packets at the
				// same time. This rn is added to AppStartTime to have the sources
				// start at different time, however they will still send at the same rate.

				Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
				x->SetAttribute ("Min", DoubleValue (0));
				x->SetAttribute ("Max", DoubleValue (1));
				double rn = x->GetValue ();
				Ptr<Node> n = nodes.Get (j);
				Ptr<Ipv4> ipv4 = n->GetObject<Ipv4> ();
				Ipv4InterfaceAddress ipv4_int_addr = ipv4->GetAddress (1, 0);
				Ipv4Address ip_addr = ipv4_int_addr.GetLocal ();
				OnOffHelper onoff ("ns3::UdpSocketFactory", InetSocketAddress (ip_addr, port)); // traffic flows from node[i] to node[j]
				onoff.SetConstantRate (DataRate (AppPacketRate), AppPacketSize);
				ApplicationContainer apps = onoff.Install (nodes.Get (i));  // traffic sources are installed on all nodes
				apps.Start (Seconds (AppStartTime + rn));
				apps.Stop (Seconds (AppStopTime));
			}
		}
	}

	// ---------- End of Create n*(n-1) CBR Flows ------------------------------

	// ---------- Simulation Monitoring ----------------------------------------

	NS_LOG_INFO ("Configure Tracing.");

	AsciiTraceHelper ascii;
	p2p.EnableAsciiAll (ascii.CreateFileStream (tr_name.c_str ()));
	// p2p.EnablePcapAll (pcap_name.c_str());

	/*Monte Carlo Link Failure Simulation with randomness*/
	/*for (int i = 0; i < n_nodes; i++) {
		Ptr<Node> n = nodes.Get (i);
		Ptr<Ipv4> ipv4 = n->GetObject<Ipv4> ();
		int limit = ipv4->GetNInterfaces();
		for(int j=1;j<limit-1;j++){
			int seconds = rand() % int(SimTime-2) + 1;
			int prob = rand() % 100;
			int fail_prob = 20;
			if(prob<fail_prob) {
				std::cout<<"Scheduling link down for node " <<i <<
						" port " <<j<<" at " <<seconds<<"s.\n";
				Simulator::Schedule (Seconds(seconds),&Ipv4::SetDown,ipv4, j);
			}
		}
	}*/

	/*Monte Carlo Link Failure Simulation with fixed num failures*/
	//int num_of_failures = 10;
	/*Set the seed here*/
	srand(10);

	for(int k=0; k < NumLinkFailures; k++) {
		int i = rand() % n_nodes;
		Ptr<Node> n = nodes.Get (i);
		Ptr<Ipv4> ipv4 = n->GetObject<Ipv4> ();
		int limit = ipv4->GetNInterfaces();
		int j = rand() % (limit-1) + 1;
		//int seconds = rand() % int(SimTime-2) + 1;
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/AppRunTime));
		double seconds = AppStartTime + r;
		std::cout<<"Scheduling link down for node " <<i <<
					" port " <<j<<" at " <<seconds<<"s.\n";
		Simulator::Schedule (Seconds(seconds),&Ipv4::SetDown,ipv4, j);
	}

	Ptr<FlowMonitor> flowmon;
	FlowMonitorHelper flowmonHelper;
	flowmon = flowmonHelper.InstallAll ();

	// Configure animator with default settings

	AnimationInterface anim (anim_name.c_str ());
	char buffer [100];
	for (int i = 0; i < n_nodes; i++) {
		if (i== 0) {
			snprintf ( buffer, 100, "Core-Switch");
		} else if (i < 16)
			snprintf ( buffer, 100, "R1-%d", i-1);
		else {
			snprintf ( buffer, 100, "R2-%d", i-17);
		}
		//snprintf ( buffer, 100, "%d",i);
		anim.UpdateNodeDescription(nodes.Get(i), buffer);
	}
	NS_LOG_INFO ("Run Simulation.");

	Simulator::Stop (Seconds (SimTime));
	Simulator::Run ();
	// flowmon->SerializeToXmlFile (flow_name.c_str(), true, true);
	flowmon->SerializeToXmlFile (flow_name.c_str(), true, true);

	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmonHelper.GetClassifier());
	FlowMonitor::FlowStatsContainer stats = flowmon->GetFlowStats ();
	print_stats(stats, classifier, AppRunTime);
	Simulator::Destroy ();

	// ---------- End of Simulation Monitoring ---------------------------------

	return 0;

}

// ---------- Function Definitions -------------------------------------------

vector<vector<bool> > readNxNMatrix (std::string adj_mat_file_name)
{
	ifstream adj_mat_file;
	cout <<adj_mat_file_name.c_str ()<<endl;
	adj_mat_file.open (adj_mat_file_name.c_str (), ios::in);
	if (adj_mat_file.fail ())
	{
		NS_FATAL_ERROR ("File " << adj_mat_file_name.c_str () << " not found");
	}
	vector<vector<bool> > array;
	int i = 0;
	int n_nodes = 0;

	while (!adj_mat_file.eof ())
	{
		string line;
		getline (adj_mat_file, line);
		if (line == "")
		{
			NS_LOG_WARN ("WARNING: Ignoring blank row in the array: " << i);
			break;
		}

		istringstream iss (line);
		bool element;
		vector<bool> row;
		int j = 0;

		while (iss >> element) {
			row.push_back (element);
			j++;
		}

		if (i == 0) {
			n_nodes = j;
		}

		if (j != n_nodes ) {
			NS_LOG_ERROR ("ERROR: Number of elements in line " << i << ": " << j << " not equal to number of elements in line 0: " << n_nodes);
			NS_FATAL_ERROR ("ERROR: The number of rows is not equal to the number of columns! in the adjacency matrix");
		}
		else {
			array.push_back (row);
		}
		i++;
	}

	if (i != n_nodes)
	{
		NS_LOG_ERROR ("There are " << i << " rows and " << n_nodes << " columns.");
		NS_FATAL_ERROR ("ERROR: The number of rows is not equal to the number of columns! in the adjacency matrix");
	}

	adj_mat_file.close ();
	return array;

}

vector<vector<double> > readCordinatesFile (std::string node_coordinates_file_name)
{
	ifstream node_coordinates_file;
	node_coordinates_file.open (node_coordinates_file_name.c_str (), ios::in);
	if (node_coordinates_file.fail ())
	{
		NS_FATAL_ERROR ("File " << node_coordinates_file_name.c_str () << " not found");
	}
	vector<vector<double> > coord_array;
	int m = 0;

	while (!node_coordinates_file.eof ())
	{
		string line;
		getline (node_coordinates_file, line);

		if (line == "")
		{
			NS_LOG_WARN ("WARNING: Ignoring blank row: " << m);
			break;
		}

		istringstream iss (line);
		double coordinate;
		vector<double> row;
		int n = 0;
		while (iss >> coordinate)
		{
			row.push_back (coordinate);
			n++;
		}

		if (n != 2)
		{
			NS_LOG_ERROR ("ERROR: Number of elements at line#" << m << " is "  << n << " which is not equal to 2 for node coordinates file");
			exit (1);
		}

		else
		{
			coord_array.push_back (row);
		}
		m++;
	}
	node_coordinates_file.close ();
	return coord_array;

}

void printMatrix (const char* description, vector<vector<bool> > array)
{
	cout << "**** Start " << description << "********" << endl;
	cout <<array.size()<< " " <<array[0].size()<<endl;
	for (size_t m = 0; m < array.size (); m++)
	{
		for (size_t n = 0; n < array[m].size (); n++)
		{
			cout << array[m][n] << ' ';
		}
		cout << endl;
	}
	cout << "**** End " << description << "********" << endl;

}

void printCoordinateArray (const char* description, vector<vector<double> > coord_array)
{
	cout << "**** Start " << description << "********" << endl;
	for (size_t m = 0; m < coord_array.size (); m++)
	{
		for (size_t n = 0; n < coord_array[m].size (); n++)
		{
			cout << coord_array[m][n] << ' ';
		}
		cout << endl;
	}
	cout << "**** End " << description << "********" << endl;

}

// ---------- End of Function Definitions ------------------------------------
