#include "DNSServer.h"
#include <lwip/def.h>
#include <Arduino.h>

DNSServer::DNSServer()
{
  _ttl = htonl(DNS_DEFAULT_TTL);
  _ErrReplyCode = DNSReplyCode::NonExistentDomain;
  _DnsHeader    = (DNSHeader*) malloc( sizeof(DNSHeader) ) ;
  _DnsQuestion  = (DNSQuestion*) malloc( sizeof(DNSQuestion) ) ;     
  _buf     = NULL;
  _CurPacketSize = 0;
  _port = 0;
}

DNSServer::~DNSServer()
{
  if (_DnsHeader) {
    free(_DnsHeader);
    _DnsHeader = NULL;
  }
  if (_DnsQuestion) {
    free(_DnsQuestion);
    _DnsQuestion = NULL;
  }
  if (_buf) {
    free(_buf);
    _buf = NULL;
  }
}

bool DNSServer::start(const uint16_t &port, const String &domainName,
                     const IPAddress &resolvedIP)
{
  _port = port;
  _buf = NULL;
  _domain = domainName;
  _ResolveIP[0] = resolvedIP[0];
  _ResolveIP[1] = resolvedIP[1];
  _ResolveIP[2] = resolvedIP[2];
  _ResolveIP[3] = resolvedIP[3];
  downcaseAndRemoveWwwPrefix(_domain);
  return _Udp.begin(_port) == 1;
}

void DNSServer::setErrorReplyCode(const DNSReplyCode &replyCode)
{
  _ErrReplyCode = replyCode;
}

void DNSServer::setTTL(const uint32_t &ttl)
{
  _ttl = htonl(ttl);
}

void DNSServer::stop()
{
  _Udp.stop();
  free(_buf);
  _buf = NULL;
}

void DNSServer::downcaseAndRemoveWwwPrefix(String &domainName)
{
  domainName.toLowerCase();
  domainName.replace("www.", "");
}

void DNSServer::processNextRequest()
{
  _CurPacketSize = _Udp.parsePacket();
  if (_CurPacketSize)
  {
    // Allocate buffer for the DNS query
    if (_buf != NULL) 
      free(_buf);
    _buf = (unsigned char*)malloc(_CurPacketSize * sizeof(char));
    if (_buf == NULL) 
      return;

    // Put the packet received in the buffer and get DNS header (beginning of message)
    // and the question
    _Udp.read(_buf, _CurPacketSize);
    memcpy( _DnsHeader, _buf, DNS_HEADER_SIZE ) ; 
    if ( requestIncludesOnlyOneQuestion() )
    {
      // The QName has a variable length, maximum 255 bytes and is comprised of multiple labels.
      // Each label contains a byte to describe its length and the label itself. The list of 
      // labels terminates with a zero-valued byte. In "github.com", we have two labels "github" & "com"
      // Iterate through the labels and copy them as they come into a single buffer (for simplicity's sake)
      _DnsQuestion->QNameLength = 0 ;
      while ( _buf[ DNS_HEADER_SIZE + _DnsQuestion->QNameLength ] != 0 )
      {
        memcpy( (void*) &_DnsQuestion->QName[_DnsQuestion->QNameLength], (void*) &_buf[DNS_HEADER_SIZE + _DnsQuestion->QNameLength], _buf[DNS_HEADER_SIZE + _DnsQuestion->QNameLength] + 1 ) ;
        _DnsQuestion->QNameLength += _buf[DNS_HEADER_SIZE + _DnsQuestion->QNameLength] + 1 ; 
      }
      _DnsQuestion->QName[_DnsQuestion->QNameLength] = 0 ; 
      _DnsQuestion->QNameLength++ ;   

      // Copy the QType and QClass 
      memcpy( &_DnsQuestion->QType, (void*) &_buf[DNS_HEADER_SIZE + _DnsQuestion->QNameLength], sizeof(_DnsQuestion->QType) ) ;
      memcpy( &_DnsQuestion->QClass, (void*) &_buf[DNS_HEADER_SIZE + _DnsQuestion->QNameLength + sizeof(_DnsQuestion->QType)], sizeof(_DnsQuestion->QClass) ) ;
    }
    

    if (_DnsHeader->QRFlag == DNS_QR_QUERY &&
        _DnsHeader->OPCode == DNS_OPCODE_QUERY &&
        requestIncludesOnlyOneQuestion() &&
        (_domain == "*" || getDomainNameWithoutWwwPrefix() == _domain)
       )
    {
      replyWithIP();
    }
    else if (_DnsHeader->QRFlag == DNS_QR_QUERY)
    {
      replyWithCustomCode();
    }

    free(_buf);
    _buf = NULL;
  }
}

bool DNSServer::requestIncludesOnlyOneQuestion()
{
  return ntohs(_DnsHeader->QuesCnt) == 1 &&
         _DnsHeader->AnsCnt == 0 &&
         _DnsHeader->AuthCnt == 0 &&
         _DnsHeader->ResCnt == 0;
}


String DNSServer::getDomainNameWithoutWwwPrefix()
{
  // Error checking : if the buffer containing the DNS request is a null pointer, return an empty domain
  String parsedDomainName = "";
  if (_buf == NULL) 
    return parsedDomainName;
  
  // Set the start of the domain just after the header (12 bytes). If equal to null character, return an empty domain
  unsigned char *start = _buf + DNS_OFFSET_DOMAIN_NAME;
  if (*start == 0)
  {
    return parsedDomainName;
  }

  int pos = 0;
  while(true)
  {
    unsigned char labelLength = *(start + pos);
    for(int i = 0; i < labelLength; i++)
    {
      pos++;
      parsedDomainName += (char)*(start + pos);
    }
    pos++;
    if (*(start + pos) == 0)
    {
      downcaseAndRemoveWwwPrefix(parsedDomainName);
      return parsedDomainName;
    }
    else
    {
      parsedDomainName += ".";
    }
  }
}

void DNSServer::replyWithIP()
{
  _Udp.beginPacket(_Udp.remoteIP(), _Udp.remotePort());
  
  // Change the type of message to a response and set the number of answers equal to 
  // the number of questions in the header
  _DnsHeader->QRFlag      = DNS_QR_RESPONSE;
  _DnsHeader->AnsCnt = _DnsHeader->QuesCnt;
  _Udp.write( (unsigned char*) _DnsHeader, DNS_HEADER_SIZE ) ;

  // Write the question
  _Udp.write(_DnsQuestion->QName, _DnsQuestion->QNameLength) ;
  _Udp.write( (unsigned char*) &_DnsQuestion->QType, 2 ) ;
  _Udp.write( (unsigned char*) &_DnsQuestion->QClass, 2 ) ;

  // Write the answer 
  // Use DNS name compression : instead of repeating the name in this RNAME occurrence,
  // set the two MSB of the byte corresponding normally to the length to 1. The following
  // 14 bits must be used to specify the offset of the domain name in the message 
  // (<255 here so the first byte has the 6 LSB at 0) 
  _Udp.write((uint8_t) 0xC0); 
  _Udp.write((uint8_t) DNS_OFFSET_DOMAIN_NAME);  

  // DNS type A : host address, DNS class IN for INternet, returning an IPv4 address 
  uint16_t answerType = htons(DNS_TYPE_HOST_ADDRESS), answerClass = htons(DNS_CLASS_IN), answerIPv4 = htons(DNS_RDLENGTH_IPV4)  ; 
  _Udp.write((unsigned char*) &answerType, 2 );
  _Udp.write((unsigned char*) &answerClass, 2 );
  _Udp.write((unsigned char*) &_ttl, 4);        // DNS Time To Live
  _Udp.write((unsigned char*) &answerIPv4, 2 );
  _Udp.write(_ResolveIP, sizeof(_ResolveIP)); // The IP address to return
  _Udp.endPacket();

}

void DNSServer::replyWithCustomCode()
{
  _DnsHeader->QRFlag = DNS_QR_RESPONSE;
  _DnsHeader->RspCode = (unsigned char)_ErrReplyCode;
  _DnsHeader->QuesCnt = 0;

  _Udp.beginPacket(_Udp.remoteIP(), _Udp.remotePort());
  _Udp.write((unsigned char*)_DnsHeader, sizeof(DNSHeader));
  _Udp.endPacket();
}
