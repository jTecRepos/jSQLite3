#pragma once

//#include "Arduino.h"
#include "jCommon.h"


class SoftwareI2C {
	private:

		int pinSda;
		int pinScl;

		int recv_len;

		int sda_in_out;

	private:

		inline void sdaSet(uint8_t ucDta);
		inline void sclSet(uint8_t ucDta);

		inline void sendStart(void);
		inline void sendStop(void);
		inline uint8_t getAck(void);
		inline void sendByte(uint8_t ucDta);
		inline uint8_t sendByteAck(uint8_t ucDta);                                 // send byte and get ack

	public:

		//SoftwareI2C();
		void begin(int Sda, int Scl);
		uint8_t beginTransmission(uint8_t addr);
		uint8_t endTransmission();
		uint8_t endTransmission(bool sendStop);

		uint8_t write(uint8_t dta);
		//uint8_t write(uint8_t len, uint8_t* dta);
		uint16_t write(const uint8_t*, uint16_t);
		uint8_t requestFrom(uint8_t addr, uint8_t len);
		uint8_t read();
		uint8_t available() {
				return recv_len;
		}
};
