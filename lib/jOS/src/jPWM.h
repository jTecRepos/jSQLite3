#pragma once

#include <jDefines.h>



class jPWM
{
protected: 

	uint8_t pin; // pin of pwm
	float freq; // pwm frequency (1 / Period) in [Hz]
	float duty; // duty cycle in percent [0 - 1]

public:
	jPWM(uint8_t pin, float freq, float duty) 
	 : pin(pin), freq(freq), duty(duty)
	{ }

	virtual bool begin() = 0;
	virtual void end() = 0;

	// get PWM Frequency in [Hz]
	float getFrequency() { return freq; }

	// returns PWM Period in [s]
	float getPeriod()
	{
		return 1.0f / freq;
	}

	
	// changes frquency of PWM and updates output
	virtual void setFrequency(float freq) = 0;
	
	// sets duty of PWM and updates output
	virtual void setDuty(float duty) = 0;
	
	virtual void setDuty_ms(float duty_ms) {
		float duty = (duty_ms * FACTOR_ms_2_s) / getPeriod(); // [s] / [s] = [1]
		duty = LIMIT(0.0f, duty, 1.0f);

		setDuty(duty);
	}

	virtual String print(const String& name);
	
};





#ifdef PLATFORM_AVR


class jPWM_Hard : public jPWM
{
protected:

	uint32_t wrap = 0;

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};



#endif // PLATFORM_AVR



#ifdef PLATFORM_ESP8266


class jPWM_Hard : public jPWM
{
protected:

	uint32_t wrap = 0;

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};


#endif // PLATFORM_ESP8266


#ifdef PLATFORM_STM32


class jPWM_Hard : public jPWM
{
protected:

	uint32_t wrap = 0;

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};



#endif // PLATFORM_STM32


#ifdef PLATFORM_SAMD


class jPWM_Hard : public jPWM
{
protected:

	uint32_t wrap = 0;

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};



#endif // PLATFORM_SAMD


#ifdef PLATFORM_RP2040


/*
// rp2040-datasheet.pdf - S. 544 ff
GPIO 		0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15
PWM	Channel	0A	0B	1A	1B	2A	2B	3A	3B	4A	4B	5A	5B	6A	6B	7A	7B
GPIO		16	17	18	19	20	21	22	23	24	25	26	27	28	29
PWM	Channel	0A	0B	1A	1B	2A	2B	3A	3B	4A	4B	5A	5B	6A	6B

Channel A and B must have same Frequency but can have different Duty Cycles
*/

class jPWM_Hard : public jPWM
{
protected:

	uint32_t wrap = 0;

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};



#endif // PLATFORM_RP2040


#ifdef PLATFORM_TEENSY

/*

// https://www.pjrc.com/teensy/td_pulse.html

Board	PWM Capable Pins
Teensy 4.1	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 22, 23,
24, 25, 28, 29, 33, 36, 37, 42, 43, 44, 45, 46, 47, 51, 54
Teensy 4.0	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 18, 19, 22, 23,
24, 25, 28, 29, 33, 34, 35, 36, 37, 38, 39
Teensy 3.6	2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 16, 17, 20, 21, 22, 23, 29, 30, 35, 36, 37, 38
Teensy 3.5	2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 20, 21, 22, 23, 29, 30, 35, 36, 37, 38
Teensy 3.2 & 3.1	3, 4, 5, 6, 9, 10, 20, 21, 22, 23, 25, 32
Teensy LC	3, 4, 6, 9, 10, 16, 17, 20, 22, 23
Teensy 3.0	3, 4, 5, 6, 9, 10, 20, 21, 22, 23
Teensy++ 2.0 	0, 1, 14, 15, 16, 24, 25, 26, 27
Teensy 2.0	4, 5, 9, 10, 12, 14, 15

*/

class jPWM_Hard : public jPWM
{
protected:

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};



#endif // PLATFORM_TEENSY



#if defined(PLATFORM_ESP32)

// https://deepbluembedded.com/esp32-pwm-tutorial-examples-analogwrite-arduino

class jPWM_Hard : public jPWM
{
protected:

	uint8_t channel = 0; //[0 - 15]
	uint8_t resolution = 16; // 1bit - 18bit

public:

	jPWM_Hard(uint8_t pin, float freq, float duty) 
	 : jPWM(pin, freq, duty)
	{
		
	}

	bool begin();

	void end();
	
	
	void setFrequency(float freq);
	
	void setDuty(float duty);
	
	
};



#endif // PLATFORM_ESP32

// #if defined(PLATFORM_AVR) || defined(PLATFORM_SAMD) || defined(PLATFORM_STM32) || defined(PLATFORM_RP2040)
