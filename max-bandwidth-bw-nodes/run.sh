#!/bin/bash
for i in {"10","12","14","16","20","22","24","26","28","30"}
do
	j=`echo $i*2.625 | bc`
	cat scratch/common_functions_prot.h | sed "s/XXXXX/$i/g"  > scratch/common_functions.h
	echo $i "TORLESS"
	./waf --run torless-topology-2racks 
	echo $i "TORLESS-ECMP"
	./waf --run torless-ecmp-topology-2racks  
	cat scratch/common_functions_prot.h | sed "s/XXXXX/$j/g"  > scratch/common_functions.h
	echo $i "TOR"
	./waf --run tor-topology-2racks  
	cp n-node-ppp-torless.xml results/n-node-ppp-torless-$i.xml
	cp n-node-ppp-tor.xml results/n-node-ppp-tor-$i.xml
	cp n-node-ppp-tor.anim.xml results/n-node-ppp-tor-$i.anim.xml
	cp n-node-ppp-torless.anim.xml results/n-node-ppp-torless-$i.anim.xml
done
#10 20 30 40 60 80 100 120 160
