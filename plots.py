import matplotlib.pyplot as plt
#Numbers for link failure %
# x = [0,2,4,6,8,10]
# y1 = [0,10.16539087,10.16539087,25.98659422,28.46547416,36.21941062]
# y2 = [0,3.499441341,3.682681564,6.601117318,8.339664804,8.339664804]
# y3 = [0,0,2.502798299,2.909497207,4.674860335,6.91396648]

#Numbers for latency
x = [200,300,400]
y1 = [1.174040,1.785784,14.697442]
y2 = [1.674906,4.617969,18.885585]
y3 = [1.913164,4.673944, 19.744481]


plt.plot(x,y1)
plt.plot(x,y2)
plt.plot(x,y3)
# plt.ylabel('% Packet Loss')
# plt.xlabel('Number of Link Failures')
# plt.title('% Packet Loss on Link Failures in TOR vs TOR-less nw')
# plt.legend(['TOR', 'TOR-less-w-3-links', 'TOR-less-w-4-links'], loc='upper left')
# plt.savefig('tor-link-fails.png')
plt.ylabel('Average Flow Latency(ms)')
plt.xlabel('App Rate(kbps)')
plt.title('Average flow latency with varying app rates')
plt.legend(['TOR', 'TOR-less-w-hw', 'TOR-less-w-sw'], loc='upper left')
plt.savefig('latency-comparisons.png')
