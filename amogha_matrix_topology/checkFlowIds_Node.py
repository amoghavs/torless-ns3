#! /usr/bin/python

import sys,commands

ipAddresses = ["10.0.0.42","10.0.0.74","10.0.0.105","10.0.0.109"]
toCheckFlows = {}

print "\t Now checking for source addresses.. "
for curIpAddress in ipAddresses:
    megaCmd = "grep "+str(curIpAddress)+" n-node-ppp.xml | awk '{if($3==\"sourceAddress=\\\""+str(curIpAddress)+"\\\"\") print}' | awk '{print $2}' | perl -pe 's/=\"/ /g' | perl -pe 's/\"//g' | awk '{print $2}' > blah.log"
    flowIdOutputs = commands.getoutput(megaCmd)

    flowFile = open("blah.log","r")
    for curLine in flowFile.readlines():
        flowId = int(curLine.strip())
        #print "\t newFlow: %d"%(flowId)
        toCheckFlows[flowId] = curIpAddress

for curFlowId in toCheckFlows:
    flowCheckCmd = " grep  \" flowId=\\\""+str(curFlowId)+"\\\" \" n-node-ppp.xml | grep lostPackets |awk '{print $2\"\t\"$12\"\t\"$13\"\t\"$14}' | perl -pe 's/=\"/ /g' | perl -pe 's/\"//g' | awk '{print }' "
    print "ip: %s\t%s "%(toCheckFlows[curFlowId],commands.getoutput(flowCheckCmd))

print "\t Now checking for destination addresses.. "
toCheckFlows = {}
for curIpAddress in ipAddresses:
    megaCmd = "grep "+str(curIpAddress)+" n-node-ppp.xml | awk '{if($4==\"destinationAddress=\\\""+str(curIpAddress)+"\\\"\") print}' | awk '{print $2}' | perl -pe 's/=\"/ /g' | perl -pe 's/\"//g' | awk '{print $2}' > blah.log"
    flowIdOutputs = commands.getoutput(megaCmd)

    flowFile = open("blah.log","r")
    for curLine in flowFile.readlines():
        try:
            flowId = int(curLine.strip())
        except ValueError:
            flowId = curLine.strip()
        #print "\t newFlow: %d"%(flowId)
        #toCheckFlows[flowId] = curIpAddress
    flowFile.close()

    megaCmd = "grep "+str(curIpAddress)+" n-node-ppp.xml | awk '{if($4==\"destinationAddress=\\\""+str(curIpAddress)+"\\\"\") print}' | awk '{print $2\"\\t\"$3}' | perl -pe 's/=\"/ /g' | perl -pe 's/\"//g' | awk '{print $2\"\\t\"$4}' > blah1.log"
    commands.getoutput(megaCmd)
    flowFile = open("blah1.log","r")
    for curLine in flowFile.readlines():
        temp = curLine.strip().split('\t')
        flowId = temp[0]; sourceAddress = temp[1];
        toCheckFlows[flowId] = (sourceAddress,curIpAddress)
    flowFile.close()
    break;

for curFlowId in toCheckFlows:
    flowCheckCmd = " grep  \" flowId=\\\""+str(curFlowId)+"\\\" \" n-node-ppp.xml | grep lostPackets |awk '{print $2\"\t\"$12\"\t\"$13\"\t\"$14}' | perl -pe 's/=\"/ /g' | perl -pe 's/\"//g' | awk '{print }' "
    print "ip: %s\t%s "%(str(toCheckFlows[curFlowId]),commands.getoutput(flowCheckCmd))
