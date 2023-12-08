#include "jPWM.h"

#include "jSystem.h"

String jPWM::print(const String& name)
{
	return format("%s: freq=%.1f, duty=%.4f, duty_ms=%.2f", name.c_str(),
		freq,
		duty,
		duty * (1.0f / freq) * FACTOR_s_2_ms	
	);
}


#ifdef PLATFORM_AVR

// https://docs.arduino.cc/tutorials/generic/secrets-of-arduino-pwm

bool jPWM_Hard::begin()
{
	
	pinMode(pin, OUTPUT);
	
	setFrequency(freq);
	setDuty(0);

	return true;
}


void jPWM_Hard::end()
{	
	digitalWrite(pin, 0);
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	// TODO implement
}


void jPWM_Hard::setDuty(float duty) 
{
	duty = LIMIT(0.0f, duty, 1.0f);

	this->duty = duty;

	analogWrite(pin, (uint8_t) (duty * 255));

}


#endif // PLATFORM_AVR


#ifdef PLATFORM_ESP8266

// https://docs.arduino.cc/tutorials/generic/secrets-of-arduino-pwm

bool jPWM_Hard::begin()
{
	
	pinMode(pin, OUTPUT);
	
	setFrequency(freq);
	setDuty(0);

	return true;
}


void jPWM_Hard::end()
{	
	digitalWrite(pin, 0);
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	// TODO implement
}


void jPWM_Hard::setDuty(float duty) 
{
	duty = LIMIT(0.0f, duty, 1.0f);

	this->duty = duty;

	analogWrite(pin, (uint8_t) (duty * 255));

}


#endif // PLATFORM_ESP8266


#ifdef PLATFORM_STM32

// https://docs.arduino.cc/tutorials/generic/secrets-of-arduino-pwm

bool jPWM_Hard::begin()
{
	
	pinMode(pin, OUTPUT);
	
	setFrequency(freq);
	setDuty(0);

	return true;
}


void jPWM_Hard::end()
{	
	digitalWrite(pin, 0);
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	// TODO implement
}


void jPWM_Hard::setDuty(float duty) 
{
	duty = LIMIT(0.0f, duty, 1.0f);

	this->duty = duty;

	analogWrite(pin, (uint8_t) (duty * 255));

}


#endif // PLATFORM_STM32


#ifdef PLATFORM_SAMD

// https://docs.arduino.cc/tutorials/generic/secrets-of-arduino-pwm

bool jPWM_Hard::begin()
{
	
	pinMode(pin, OUTPUT);
	
	setFrequency(freq);
	setDuty(0);

	return true;
}


void jPWM_Hard::end()
{	
	digitalWrite(pin, 0);
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	// TODO implement
}


void jPWM_Hard::setDuty(float duty) 
{
	duty = LIMIT(0.0f, duty, 1.0f);

	this->duty = duty;

	analogWrite(pin, (uint8_t) (duty * 255));

}


#endif // PLATFORM_SAMD


#ifdef PLATFORM_RP2040

#include "hardware/pwm.h"

//freq in [Hz]
// duty in [0.0 - 1.0]
static uint32_t pwm_set_freq_duty(uint slice_num, uint channel, uint32_t freq, float duty)
{
	// https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=1
	
	uint32_t clock = 125000000; //125 Mhz (default clock)
	uint32_t divider16 = clock / freq / 4096 + (clock % (freq * 4096) != 0);
	if (divider16 / 16 == 0)
		divider16 = 16;
	
	uint32_t wrap = clock * 16 / divider16 / freq - 1;
	pwm_set_clkdiv_int_frac(slice_num, divider16 / 16, divider16 & 0xF);
	pwm_set_wrap(slice_num, wrap);
	pwm_set_chan_level(slice_num, channel, wrap * duty);
	return wrap;
}
/*

void setup_pwm() {
	// https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__pwm.html#pwm_example

	

	// Tell GPIO 0 and 1 they are allocated to the PWM
	gpio_set_function(0, GPIO_FUNC_PWM);
	gpio_set_function(1, GPIO_FUNC_PWM);

	// Find out which PWM slice is connected to GPIO 0 (it's slice 0)
	uint slice_num = pwm_gpio_to_slice_num(0);
	
	// Set period of 4 cycles (0 to 3 inclusive)
	//pwm_set_wrap(slice_num, 3);
	// Set channel A output high for one cycle before dropping
	//pwm_set_chan_level(slice_num, PWM_CHAN_A, 1);
	// Set initial B output high for three cycles before dropping
	//pwm_set_chan_level(slice_num, PWM_CHAN_B, 3);
	// Set the PWM running
	//pwm_set_enabled(slice_num, true);

	// https://www.i-programmer.info/programming/hardware/14849-the-pico-in-c-basic-pwm.html?start=2
	//pwm_set_clkdiv (slice_num, 38.1875);

	pwm_set_clkdiv_int_frac (slice_num, 38,3);
	pwm_set_wrap(slice_num, 65465);
	pwm_set_chan_level(slice_num, PWM_CHAN_A, 65465/2);
	

	pwm_set_clkdiv (slice_num, 38.1875f);

	pwm_set_freq_duty(slice_num, PWM_CHAN_A, 50, 0.75f);
	pwm_set_enabled(slice_num, true);
	

	pwm_configure(pwm_pin, 50, 0.10);
}

*/



bool jPWM_Hard::begin()
{
	
	// Tell pin is allocated to the PWM
	gpio_set_function(pin, GPIO_FUNC_PWM);

	setFrequency(freq);
	
	pwm_set_enabled(pwm_gpio_to_slice_num(pin), true);

	return true;
}


void jPWM_Hard::end()
{	
	pwm_set_enabled(pwm_gpio_to_slice_num(pin), false);
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	// Find out which PWM slice is connected to pin (GPIO0 -> slice 0)
	uint slice_num = pwm_gpio_to_slice_num(pin);
	uint channel = pwm_gpio_to_channel(pin);
	
	this->wrap = pwm_set_freq_duty(slice_num, channel, freq, duty);
}


void jPWM_Hard::setDuty(float duty) 
{
	duty = LIMIT(0.0f, duty, 1.0f);

	this->duty = duty;

	uint slice_num = pwm_gpio_to_slice_num(pin);
	uint channel = pwm_gpio_to_channel(pin);
	pwm_set_chan_level(slice_num, channel, wrap * duty);

}


#endif // PLATFORM_RP2040


#ifdef PLATFORM_TEENSY

bool jPWM_Hard::begin()
{
	analogWriteResolution(12);  // analogWrite value 0 to 4095, or 4096 for high
	
	// TODO implement
	this->setFrequency(this->freq);
	this->setDuty(this->duty);

	return true;
}


void jPWM_Hard::end()
{	
	// TODO implement
	analogWrite(this->pin, 0);
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	// PWM Frequency
	// The PWM signals are created by hardware timers. 
	// PWM pins common to each timer always have the same frequency, so if you change one pin's PWM frequency, all other pins for the same timer change.
	analogWriteFrequency(this->pin, this->freq);
}


void jPWM_Hard::setDuty(float duty) 
{
	// TODO implement
	analogWrite(this->pin, 0 * 4096);
}


#endif // PLATFORM_TEENSY


#ifdef PLATFORM_ESP32


bool jPWM_Hard::begin()
{
	channel = pin % 15; //improve channel selection
	
	ledcAttachPin(pin, channel); 

	setFrequency(freq);

	setDuty(duty);

	return true;
}


void jPWM_Hard::end()
{	
	
}

void jPWM_Hard::setFrequency(float freq) 
{
	this->freq = freq;

	ledcSetup(channel, freq, resolution);
}


void jPWM_Hard::setDuty(float duty) 
{
	duty = LIMIT(0.0f, duty, 1.0f);

	this->duty = duty;


	uint32_t value = duty * (1 << resolution); //8bit res: 0 - 256

	ledcWrite(channel, value);

}


#endif // PLATFORM_ESP32
