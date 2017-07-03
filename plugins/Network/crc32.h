#ifndef CRC32_H
#define CRC32_H

#include <QByteArray>

class CRC32
{
public:
	static quint32 crc32( const QByteArray &A );

private:
	static inline quint32 UPDC32( quint8 octet, quint32 crc )
	{
		return( crc_32_tab[ ( crc ^ octet ) & 0xff ] ^ ( crc  >> 8 ) );
	}

	static const quint32 crc_32_tab[];
};

#endif // CRC32_H
