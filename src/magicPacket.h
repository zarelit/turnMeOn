/* 
 * File:   magicPacket.h
 * Author: zarel
 *
 * Created on October 20, 2011, 12:49 PM
 */

#ifndef MAGICPACKET_H
#define	MAGICPACKET_H

#include <QObject>
#include <QByteArray>
#include <QHostAddress>
#include <QUdpSocket>

#define MAGIC_INVALID_PORT -1
//!< definition of an invalid port. Used by send() checks.
#define MAGIC_INVALID_IP QHostAddress::Null
//!< definition of an invalid ip. Used by send() checks.

/**
 * @brief Represents a magic packet.
 * @see https://en.wikipedia.org/wiki/Wake-on-LAN#Magic_packet
 *
 * This class represents a magic packet
 * and provides a method to send it.
 *
 * Typical usage is (avoiding necessary
 * Qt casts):
 * @code
 * magicPacket mp;
 * mp.setIP("192.168.0.12");
 * mp.setMAC("12:34:56:78:90:AB");
 * mp.setPort(80);
 * mp.send();
 * @endcode
 */
class magicPacket : public QObject {
    Q_OBJECT
public:
    magicPacket();
    virtual ~magicPacket();
    bool setMAC(QByteArray addr);
    bool setIP(QString addr);
    bool setPort(int port);
public slots:
    bool send();
private:
	//! The port number the UDP datagrams will be sent to.
    int port;
	//! The preamble of every magic packet
    QByteArray prefix;

	/**
	 * The network address to wake.
     *	
	 * The MAC address of the remote system's
	 * network interface card.
	 */
    QByteArray MACAddress; 

	/**
	 * The magic frame content.
	 *
	 * The entire magic frame, then incapsulated
	 * in an UDP datagram.
	 */
    QByteArray payload;

	//! The system to wake.
	//!
	//! The remote host to wake up. The IP address is
	//! used to determine if a LAN broadcast UDP datagram
	//! is going to be used or a normal one.
	//! @sa setIP()
    QHostAddress ip;
	//! The socket used to send the datagrams.
    QUdpSocket sock;
};

#endif	/* MAGICPACKET_H */

