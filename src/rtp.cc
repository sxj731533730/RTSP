#include "rtp.h"

RTP_Header::RTP_Header(
    //byte 0
    const uint8_t __version,
    const uint8_t __padding,
    const uint8_t __extension,
    const uint8_t __csrcCount,
    //byte 1
    const uint8_t __marker,
    const uint8_t __payloadType,
    //byte 2, 3
    const uint16_t __seq,
    //byte 4-7
    const uint32_t __timestamp,
    //byte 8-11
    const uint32_t __ssrc)
{
    bzero(this->header, sizeof(this->header));

    uint16_t *pSeq = reinterpret_cast<uint16_t *>(&this->header[2]);
    uint32_t *pTimeStamp = reinterpret_cast<uint32_t *>(&this->header[4]);
    uint32_t *pSSRC = reinterpret_cast<uint32_t *>(&this->header[8]);

    this->header[0] = (__csrcCount & 0x0F) | (__extension << 4) | (__padding << 5) | (__version << 6);
    this->header[1] = (__marker << 7) | (__payloadType & 0x7F);
    *pSeq = htons(__seq);
    *pTimeStamp = htonl(__timestamp);
    *pSSRC = htonl(__ssrc);
}

RTP_Header::RTP_Header(const uint16_t __seq, const uint32_t __timestamp, const uint32_t __ssrc)
{
    bzero(this->header, sizeof(this->header));

    uint16_t *pSeq = reinterpret_cast<uint16_t *>(&this->header[2]);
    uint32_t *pTimeStamp = reinterpret_cast<uint32_t *>(&this->header[4]);
    uint32_t *pSSRC = reinterpret_cast<uint32_t *>(&this->header[8]);

    this->header[0] = RTP_VERSION << 6;
    this->header[1] = RTP_PAYLOAD_TYPE_H264 & 0x7F;
    *pSeq = htons(__seq);
    *pTimeStamp = htonl(__timestamp);
    *pSSRC = htonl(__ssrc);
}

RTP_Packet::RTP_Packet(const RTP_Header &rtpHeader) : header(rtpHeader), packetLen(RTP_HEADER_SIZE) { memcpy(this->RTP_Payload, rtpHeader.getHeader(), RTP_HEADER_SIZE); }

RTP_Packet::RTP_Packet(const RTP_Header &rtpHeader, const uint8_t *data, const size_t dataSize, const size_t bias) : header(rtpHeader), packetLen(RTP_HEADER_SIZE + dataSize + bias)
{
    memcpy(this->RTP_Payload, rtpHeader.getHeader(), RTP_HEADER_SIZE);
    memcpy(this->RTP_Payload + RTP_HEADER_SIZE + bias, data, std::min(dataSize, RTP_MAX_DATA_SIZE - bias));
}

ssize_t RTP_Packet::rtp_sendto(int sockfd, int flags, const sockaddr *to) { return sendto(sockfd, this->getRealPacket(), this->getPacketLen(), flags, to, sizeof(sockaddr)); }
