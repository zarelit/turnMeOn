/* 
 * File:   magicPacket.cpp
 * Author: David Costa <david@zarel.net>
 * 
 * Created on October 20, 2011, 12:49 PM
 */

#include "magicPacket.h"
#include <QDebug>
#include <QRegExp>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>

/**
 * Sets an IP address for the magic packet.
 *   - If a name will be passed then the name will be resolved.
 *   - If an invalid name is provided, the IP will be reset.
 *
 * @return true if an IP was associated succesfully,
 *         false otherwise.
 * @param[in] addr A string representing an IP address
 * or a hostname that will be resolved.
 */
bool magicPacket::setIP(QString addr){
    qDebug()<<"Resolving "<<addr;
    QHostInfo host=QHostInfo::fromName(addr);
    if(!host.addresses().isEmpty()){
        ip=host.addresses().first();
        qDebug()<<"IP set to:"<<ip.toString();
        return true;
    }else{
        qDebug()<<"No IP found for "<<addr\
			<< ". Resetting the IP.";
		ip=MAGIC_INVALID_IP;
        return false;
    }
}

/**
 * Sets the UDP port for the magic packet.
 * If an invalid port number is passed, it will be reset
 * to something invalid.
 * 
 * @param port the UDP port number
 * @return true if the port is valid, false otherwise.
 */
bool magicPacket::setPort(int port){
    if(port>=0){
        this->port=port;
		qDebug()<<"Port set correctly to "<<port;
        return true;
    }else{
		qDebug()<<"Cannot set port. Resetting";
		port=MAGIC_INVALID_PORT;
        return false;
    }
}

/**
 * Sets the MAC address to be inserted in the
 * magic packet payload.
 * If an invalid MAC is passed, it is reset to empty.
 *
 * @return true if a valid MAC is passed, else otherwise.
 * @param addr the MAC address of the NIC to be activated.
 */
bool magicPacket::setMAC(QByteArray addr){
    QString temp=addr;
	/**
	 * @note
     *   - Bug: temp.length count is wrong as a nibble will
	 * be interpreted as a whole byte.
	 *   - Workaround: a homemade byte count is done,
	 * converting the byte array in a string and counting
	 * how many hex digits are present with a regex.
	 * \n 
	 * Only the count matters as every 6 bytes combination
	 * is a formally valid MAC address.
	 */
    //12 nibbles=6bytes=a valid MAC address
    if(temp.count(QRegExp("[\\da-fA-F]"))==12){
            MACAddress = QByteArray::fromHex(addr);
            qDebug()<<"MAC Address set:"<<MACAddress.toHex();
            return true;
    }else{
		//the empty QByteArray is considered invalid.
		MACAddress=QByteArray();
		qDebug()<<"The provided MAC is not valid.";
        return false;
    }
}

/**
 * Actually sends the magic packet.
 * Three cases can happen:
 *   -# IP is a WAN address.\n
 *   	An UDP datagram is sent to the specified IP.
 *   -# IP is a LAN address.\n
 *   	The UDP datagram is addressed to 255.255.255.255
 *      which is the local limited broadcast address.
 *   	@see https://en.wikipedia.org/wiki/Broadcast_address
 *   -# IP is invalid.
 *      No datagram is sent.
 *
 * @return true if a datagram is sent, false otherwise.
 */
bool magicPacket::send(){
    //check for invalid parameters.
    if(MACAddress.isEmpty()
			|| port==MAGIC_INVALID_PORT
			|| ip==MAGIC_INVALID_IP
	  ) return false;
    
	//LAN check
    if( ip.isInSubnet(QHostAddress("10.0.0.0"),8) ||
		ip.isInSubnet(QHostAddress("172.16.0.0"),12) ||
		ip.isInSubnet(QHostAddress("192.168.0.0"),16) )
    {
        qDebug()<<ip.toString()<<"is a LAN address. "<<
                "Magic packet will be sent as UDP broadcast.";
        
        setIP("255.255.255.255");
    }
    
    //build the payload 
    payload=prefix+MACAddress.repeated(16);
    qDebug() << "Payload built:"<<payload.toHex();
            
    if(sock.writeDatagram(payload,ip,port)==-1){
		//error occured
        qDebug()<<"UDP Socket returned an error:"<<
			sock.errorString();
        return false;
    }else{
        qDebug() << "UDP Socket created and payload sent to"
			<<ip.toString()
            << ":" << port;
        return true;
    }
}

/**
 * The default and only constructor of
 * magicPacket.
 * It accepts no arguments as default
 * properties are invalid and must be set with
 * setMAC(), setIP() and setPort().
 */
magicPacket::magicPacket() {
    prefix.append("\xFF\xFF\xFF\xFF\xFF\xFF");
    port=MAGIC_INVALID_PORT;
	ip=MAGIC_INVALID_IP;
}

//! the empty destructor
magicPacket::~magicPacket() {
}
