#ifndef __DNSServer_H
#define __DNSServer_H
#include <WiFiUdp.h>

#define DNS_QR_QUERY 0
#define DNS_QR_RESPONSE 1
#define DNS_OPCODE_QUERY 0
#define DNS_DEFAULT_TTL 60        // Default Time To Live : time interval in seconds that the resource record should be cached before being discarded
#define DNS_OFFSET_DOMAIN_NAME 12 // Offset in bytes to reach the domain name in the DNS message 
#define DNS_HEADER_SIZE 12 

enum class DNSReplyCode
{
  NoError   = 0,
  FormError = 1,
  ServerFailure     = 2,
  NonExistentDomain = 3,
  NotImplemented    = 4,
  Refused   = 5,
  YXDomain  = 6,
  YXRRSet   = 7,
  NXRRSet   = 8
};

enum DNSType
{
  DNS_TYPE_HOST_ADDRESS      = 1,  // Host Address
  DNS_TYPE_IPV6_ADDRESS   = 28, // IPv6 Address
  DNS_TYPE_SOA    = 6,  // Start Of a zone of Authority
  DNS_TYPE_DOMAIN_PTR    = 12, // Domain name PoinTeR
  DNS_TYPE_DELENAME  = 39  // Delegation Name
} ; 

enum DNSClass
{
  DNS_CLASS_IN = 1, // INternet
  DNS_CLASS_CH = 3  // CHaos
} ; 

enum DNSRDLength
{
  DNS_RDLENGTH_IPV4 = 4 // 4 bytes for an IPv4 address 
} ; 

struct DNSHeader
{
  uint16_t ID;               // identification number
  union {
    struct {
      uint16_t RD : 1;      // recursion desired
      uint16_t Trunc : 1;      // truncated message
      uint16_t AuthAns : 1;      // authoritive answer
      uint16_t OPCode : 4;  // message_type
      uint16_t QRFlag : 1;      // query/response flag
      uint16_t RspCode : 4;   // response code
      uint16_t Z : 3;       // its z! reserved
      uint16_t RecAvail : 1;      // recursion available
    };
    uint16_t Flags;
  };
  uint16_t QuesCnt;          // number of question entries
  uint16_t AnsCnt;          // number of answer entries
  uint16_t AuthCnt;          // number of authority entries
  uint16_t ResCnt;          // number of resource entries
};

struct DNSQuestion
{
  uint8_t   QName[256] ; //need 1 Byte for zero termination!
  uint16_t  QNameLength ; 
  uint16_t  QType ; 
  uint16_t  QClass ; 
} ; 

class DNSServer
{
  public:
    DNSServer();
    ~DNSServer();
    void processNextRequest();
    void setErrorReplyCode(const DNSReplyCode &replyCode);
    void setTTL(const uint32_t &ttl);

    // Returns true if successful, false if there are no sockets available
    bool start(const uint16_t &port,
              const String &domainName,
              const IPAddress &resolvedIP);
    // stops the DNS server
    void stop();

  private:
    WiFiUDP     _Udp;
    uint16_t    _port;
    String      _domain;
    unsigned char   _ResolveIP[4];
    int             _CurPacketSize;
    unsigned char*  _buf;
    DNSHeader*      _DnsHeader;
    uint32_t        _ttl;
    DNSReplyCode    _ErrReplyCode;
    DNSQuestion*    _DnsQuestion ; 


    void downcaseAndRemoveWwwPrefix(String &domainName);
    String getDomainNameWithoutWwwPrefix();
    bool requestIncludesOnlyOneQuestion();
    void replyWithIP();
    void replyWithCustomCode();
};
#endif
