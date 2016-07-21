#define CRC16TEST(crc)      ((crc) == 0xf0b8 ? 1:0)
#define CRC32TEST(crc)      ((crc) == 0xdebb20e3L ? 1:0)

#define updcrc16ccitt(crc,cp)   (TableCRC16CCITT[((int)(crc) ^ cp) & 0xff] ^ (((crc) >> 8) & 0x00FF))
#define updcrc32ccitt(crc,cp)   (TableCRC32CCITT[((int)(crc) ^ cp) & 0xff] ^ (((crc) >> 8) & 0x00FFFFFF))

#define updcrc32(cp,crc)	(TableCRC32CCITT[((int)(crc) ^ cp) & 0xff] ^ (((crc) >> 8) & 0x00FFFFFF))
#define updcrc16(cp, crc)       (TableCRC16[(((int)(crc) >> 8) & 0xff)] ^ ((crc) << 8) ^ (cp))

extern unsigned long  TableCRC32CCITT[];
extern unsigned short TableCRC16[];
extern unsigned short TableCRC16CCITT[];

unsigned long  getcrc32ccitt(const unsigned char *buffer, size_t buflen);
unsigned short getcrc16(const unsigned char *buffer, size_t buflen);
unsigned short getcrc16ccitt(const unsigned char *buffer, size_t buflen);
void   getcrc16uue(unsigned char *buffer,size_t len,unsigned short *crc);
unsigned short   getcrcfile(char *pathname,size_t seek,size_t len);

