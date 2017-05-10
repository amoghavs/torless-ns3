#!/usr/bin/python
import re
import matplotlib.pyplot as plt
f = open("all-log-3_11_1.log")
f = f.read().split("\n")
f = f[0:len(f) -1]
current_data_rate = "10"
current_type = "TORLESS"
table = {}
table["TORLESS"] = {}
table["TORLESS-ECMP"] = {}
table["TOR"] = {}
for i in table:
    table[i]["AppRate"] = []
    table[i]["LinkRate"] = []
    table[i]["AvgLatency"] = []
    table[i]["AvgThroughput"] = []
    table[i]["ReceivedPkts"] = []
    table[i]["LostPkts"] = []
    table[i]["TimesFwded"] = []

#imp_data_points = "10", "20", "40", "80", "100", "110", "120", "130", "140", "150", "160"
for i in f:
    if re.search("TORLESS-ECMP", i) != None:
        current_data_rate, current_type = i.split()
    elif re.search("TORLESS", i) != None:
        current_data_rate, current_type = i.split()
    elif re.search("TOR", i) != None:
        current_data_rate, current_type = i.split()
    if re.search(r"^Stats", i) != None:
        #print current_data_rate, current_type, i.split()
        '''
        flag = False
        for j in imp_data_points:
            if j == current_data_rate:
                flag = True
        if flag == False:
            continue
        '''
        z, apprate, linkrate, latency, throughput, receivedpkts, lostpkts, timesfwded = i.split()
        table[current_type]["AppRate"].append(apprate[0:len(apprate)-4])
        table[current_type]["LinkRate"].append(linkrate)
        table[current_type]["AvgLatency"].append(latency)
        table[current_type]["AvgThroughput"].append(throughput)
        table[current_type]["ReceivedPkts"].append(receivedpkts)
        table[current_type]["LostPkts"].append(lostpkts)
        table[current_type]["TimesFwded"].append(timesfwded)


# TEST1
ax = plt.subplot(111)
x = range(len(table[current_type]["AppRate"]))
x1 = []
x2 = []
x3 = []
w = 0.2
w1 = 0.25
for i in x:
    x1.append(i)
    x2.append(i+w1)
    x3.append(i+2*w1)
xl = table["TORLESS"]["AppRate"]
'''
plt.xticks(x1, xl, rotation=45, fontsize=11)
p1 = plt.bar(x2, table["TOR"]["LostPkts"], width=w, color="#ffbb78", edgecolor ="none")
p2 = plt.bar(x1, table["TORLESS"]["LostPkts"], width=w, color="#1f77b4", edgecolor ="none")
plt.legend((p1[0], p2[0]), ('TOR', 'TORLESS'), loc=2)
plt.show()
'''
plt.clf()
plt.xticks(x2, xl, fontsize=11)
p1 = plt.bar(x2, table["TORLESS-ECMP"]["ReceivedPkts"], width=w, color="#c7e9c0", edgecolor ="none")
p2 = plt.bar(x2, table["TORLESS-ECMP"]["LostPkts"],bottom=table["TORLESS-ECMP"]["ReceivedPkts"],width=w, color="#ff7f0e", edgecolor ="none")
p3 = plt.bar(x1, table["TORLESS"]["ReceivedPkts"], width=w, color="#aec7e8", edgecolor ="none")
p4 = plt.bar(x1, table["TORLESS"]["LostPkts"], bottom=table["TORLESS"]["ReceivedPkts"], width=w, color="#ff7f0e", edgecolor ="none")
p5 = plt.bar(x3, table["TOR"]["ReceivedPkts"], width=w, color="#FFC300", edgecolor ="none")
p6 = plt.bar(x3, table["TOR"]["LostPkts"], bottom=table["TOR"]["ReceivedPkts"], width=w, color="#ff7f0e", edgecolor ="none")

print table["TOR"]["ReceivedPkts"], table["TOR"]["ReceivedPkts"]
print table["TORLESS"]["ReceivedPkts"], table["TORLESS"]["ReceivedPkts"]
print table["TORLESS-ECMP"]["ReceivedPkts"], table["TORLESS-ECMP"]["ReceivedPkts"]
plt.legend((p1[0], p3[0], p5[0], p2[0]), ('TORLESS-ECMP', 'TORLESS', 'TOR', 'LOSTPKTS'), loc=2)

plt.title("Packet transferred for different application data rates")
plt.xlabel("Application data rate in Kbps")
plt.ylabel("Number of Packets")

plt.savefig("lostpaktsall.png")

plt.clf()
plt.xticks(x2, xl, fontsize=11)
p1 = plt.bar(x2, table["TORLESS-ECMP"]["TimesFwded"], width=w, color="#c7e9c0", edgecolor ="none")
#p2 = plt.bar(x2, table["TOR"]["TimesFwded"],bottom=table["TOR"]["ReceivedPkts"],width=w, color="#ff7f0e", edgecolor ="none")
p2 = plt.bar(x1, table["TORLESS"]["TimesFwded"], width=w, color="#aec7e8", edgecolor ="none")
#p4 = plt.bar(x1, table["TORLESS"]["TimesFwded"], bottom=table["TORLESS"]["ReceivedPkts"], width=w, color="#ff7f0e", edgecolor ="none")
p3 = plt.bar(x3, table["TOR"]["TimesFwded"], width=w, color="#FFC300", edgecolor ="none")

plt.legend((p1[0], p2[0], p3[0]), ('TORLESS-ECMP', 'TORLESS', 'TOR'), loc=2)

plt.title("Number of packet fowards")
plt.xlabel("Application data rate in Kbps")
plt.ylabel("Number of Packet forwards")

plt.savefig("timesforwardedall.png")


