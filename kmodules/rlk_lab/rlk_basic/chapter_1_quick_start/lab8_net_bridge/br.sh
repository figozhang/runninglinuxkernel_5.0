#!/bin/sh
netname="ens33"
ip=`ifconfig $netname|grep "inet "|awk '{print $2}'`
netmask=`ifconfig $netname|grep "inet "|awk '{print $4}'` 
gateway=`route -n|grep UG|awk '{print $2}'`
brctl addbr br0
ifconfig br0 $ip netmask $netmask
ifconfig $netname 0.0.0.0
brctl addif br0 $netname
route add default gw $gateway
echo "nameserver 114.114.114.114" >> /etc/resolv.conf
