#ifndef _X_CRC_H
#define _X_CRC_H

//"CRC8/ITU", "CRC8", "07", "00", "55", false, false,
//"CRC8/MAXIM", "CRC8", "31", "00", "00", true, false,
//"CRC8/ROHC", "CRC8", "07", "FF", "00", true, false,
//"CRC16", "CRC16", "8005", "0000", "0000", true, false,
//"CRC16/A", "CRC16", "1021", "C6C6", "0000", true, false,
//"CRC16/CCITT", "CRC16", "1021", "0000", "0000", true, false,
//"CRC16/CCITT-FALSE", "CRC16", "1021", "FFFF", "0000", false, false,
//"CRC16/DECT R", "CRC16", "0589", "0000", "0001", false, false,
//"CRC16/DECT X", "CRC16", "0589", "0000", "0000", false, false,
//"CRC16/DNP", "CRC16", "3D65", "0000", "FFFF", true, false,
//"CRC16/GENIBUS", "CRC16", "1021", "FFFF", "FFFF", false, false,
//"CRC16/MAXIM", "CRC16", "8005", "0000", "FFFF", true, false,
//"CRC16/MODBUS", "CRC16", "8005", "FFFF", "0000", true, false,
//"CRC16/USB", "CRC16", "8005", "FFFF", "FFFF", true, false,
//"CRC16/X25", "CRC16", "1021", "FFFF", "FFFF", true, false,
//"CRC16/XMODEM", "CRC16", "1021", "0000", "0000", false, false,
//"CRC32", "CRC32", "04C11DB7", "FFFFFFFF", "FFFFFFFF", true, false,
//"CRC32/B", "CRC32", "04C11DB7", "FFFFFFFF", "FFFFFFFF", false, false,
//"CRC32/C", "CRC32", "1EDC6F41", "FFFFFFFF", "FFFFFFFF", true, false,
//"CRC32/D", "CRC32", "A833982B", "FFFFFFFF", "FFFFFFFF", true, false,
//"CRC32/MPEG-2", "CRC32", "04C11DB7", "FFFFFFFF", "00000000", false, false,
//"CRC32/POSIX", "CRC32", "04C11DB7", "00000000", "FFFFFFFF", false, false,

#include <cstdint>
#include <cassert>

template<typename T>
class XCRC{
public:
	enum CRC8Type{CRC8,CRC8_DARC,CRC8_ITU,CRC8_MAXIM,CRC8_ROHC};
	enum CRC16Type{CRC16,CRC16_A,CRC16_CCITT,CRC16_CCITT_FALSE,CRC16_DECT_R,CRC16_DECT_X,
		CRC16_DNP,CRC16_GENIBUS,CRC16_MAXIM,CRC16_MODBUS,CRC16_USB,CRC16_X25,CRC16_XMODEM};
	enum CRC32Type{CRC32,CRC32_B,CRC32_C,CRC32_D,CRC32_MPEG_2,CRC32_POSIX};
	XCRC(T poly, T init = 0, T xOrOut = 0, bool refIn = false, bool refOut = false)
		:_poly(poly), _init(init), _refIn(refIn), _refOut(refOut), _xOrOut(xOrOut){
		WIDTH = sizeof(T) * 8;
		TOPBIT = (1 << (WIDTH - 1));
		createTable();
	}
	T calc(const char* data, int len){
		T reg = _init;
		if (!_refIn){
			int shift = (sizeof(T) - 1) * 8+4;
			while (len--){
				uint8_t c = *data++;
				uint8_t da = reg >> shift;
				reg <<= 4;
				reg ^= _table[da ^ (c >>4)];
				da = reg >> shift;
				reg <<= 4;
				reg ^= _table[da ^ (c & 0x0F)];
			}
		}
		else{
			while (len--){
				uint8_t c = *data++;
				uint8_t da = (uint8_t)(reg & 0x0F);
				reg >>= 4;
				reg ^= _table[da ^ (c & 0x0F)];
				da = (uint8_t)(reg&0x0F);
				reg >>= 4;
				reg ^= _table[da^(c >> 4)];
			}
		}

		if (!_refOut)
			return reg^_xOrOut;
		else
			return reflected(reg)^_xOrOut;
	}

	static XCRC<uint8_t> crc8(CRC8Type type){
		if (type == CRC8)		return XCRC<uint8_t>(0x07, 0x00, 0x00, false, false);
		if (type == CRC8_DARC)	return XCRC<uint8_t>(0x39, 0x00, 0x00, true, false);
		assert(false);
		return XCRC<uint8_t>(0);
	}
	static XCRC<uint32_t> crc32(CRC32Type type)
	{
		if (type == CRC32)		return XCRC<uint32_t>(0x04C11DB7, 0xFFFFFFFF, 0xFFFFFFFF, true, false);
		assert(false);
		return XCRC<uint32_t>(0);
	}

private:
private:
	T reflected(T v){
		T retval = 0;
		for (int i = 0; i < WIDTH; i++)
		{
			if (v&(0x01 << i))
				retval |= (0x01 << (WIDTH - i - 1));
		}
		return retval;
	}
	void createTable(){
		if (!_refIn)
		{
			for (int i = 0; i < 16; i++)
			{
				T reg = i;
				for (int bit = 0; bit < WIDTH; bit++)
				{
					if (reg & TOPBIT)
						reg = (reg << 1) ^ _poly;
					else
						reg <<= 1;
				}
				_table[i] = reg;
			}
		}
		else{
			T usePoly = reflected(_poly);
			for (int i = 0; i < 16; i++)
			{
				T reg = i;
				for (int bit = 0; bit < 4; bit++)
				{
					if (reg & 0x01)
						reg = (reg >> 1) ^ usePoly;
					else
						reg >>= 1;
				}
				_table[i] = reg;
			}
		}
	}

private:
	T		_poly;
	T		_init;
	bool	_refIn;
	bool	_refOut;
	T		_xOrOut;
	T		_table[16];

	int		WIDTH;
	T		TOPBIT;
};

#define XCRC32 (XCRC<uint32_t>::crc32(XCRC<uint32_t>::CRC32))


#endif //_X_CRC_H
