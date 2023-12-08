#include "jI2C_Soft.h"

#define  GETACK        1                      // get ack                        
#define  GETNAK        0                      // get nak   

#ifndef  HIGH
	#define  HIGH          1
#endif
#ifndef  LOW
	#define  LOW           0
#endif


//#define F_SCL //100000

#define SPEED_FAKTOR 1.0f

#define T_LOW ((int) (2 * SPEED_FAKTOR))
#define T_HIGH ((int) (2 * SPEED_FAKTOR))

#define T_HD_STA ((int) (2 * SPEED_FAKTOR)) //[us] > 0.6µS
#define T_SU_STO ((int) (2 * SPEED_FAKTOR)) //[us] STOP time between SCL and SDA going HIGH > 0.6µS 
#define T_SU_STA ((int) (2 * SPEED_FAKTOR)) //[us] REPEAT START (SCL SDA are HIGH but no STOP)


#define T_BUF ((int) (10 * SPEED_FAKTOR)) //[us] Time between STOP and new Start

#define T_ACK_TIMEOUT ((int) (100 * SPEED_FAKTOR)) //[us] to wait for ACK before interpret as NACK



#define T_SCL (T_LOW + T_HIGH)



/*************************************************************************************************
	Function Name: begin
	Description:  config IO
	Parameters: Sda: Scl:
	Return: none
*************************************************************************************************/
void SoftwareI2C::begin(int Sda, int Scl) {
	pinSda = Sda;
	pinScl = Scl;

	pinMode(pinScl, OUTPUT);
	pinMode(pinSda, OUTPUT);
	sda_in_out = OUTPUT;
	digitalWrite(pinScl, HIGH); // Default HIGH
	digitalWrite(pinSda, HIGH); // Default HIGH
}

/*************************************************************************************************
	Function Name: sdaSet
	Description:  set sda
	Parameters: ucDta: HIGH or LOW
	Return: none
*************************************************************************************************/
void SoftwareI2C::sdaSet(uint8_t ucDta) {

	if (sda_in_out != OUTPUT) {
		sda_in_out = OUTPUT;
		digitalWrite(pinSda, ucDta);
		pinMode(pinSda, OUTPUT);
	}
	digitalWrite(pinSda, ucDta);
}

/*************************************************************************************************
	Function Name: sclSet
	Description:  set scl
	Parameters: ucDta: HIGH or LOW
	Return: none
*************************************************************************************************/
void SoftwareI2C::sclSet(uint8_t ucDta) {
	digitalWrite(pinScl, ucDta);
	//jdelay(1);
}


/*************************************************************************************************
	Function Name: sendStart
	Description:   send start clock
	Parameters: None
	Return: None
*************************************************************************************************/
void SoftwareI2C::sendStart(void) {
	sdaSet(LOW);
	delayMicroseconds(T_HD_STA); 
	sclSet(LOW);
	delayMicroseconds(T_HD_STA);
}

/*************************************************************************************************
	Function Name: setClockDivider
	Description:  setup clock divider for spi bus
	Parameters: divider – clock divider
	Return: 0 – setup ok; 1 – setup failed
*************************************************************************************************/
void SoftwareI2C::sendStop(void) {
	// Should already be LOW
	sclSet(LOW);
	
	// SDA to LOW
	sdaSet(LOW);
	delayMicroseconds(T_LOW);

	// SCL Rising with SDA LOW
	sclSet(HIGH);
	delayMicroseconds(T_SU_STO);

	// SDA HIGH
	sdaSet(HIGH);
	
	delayMicroseconds(T_BUF);

	// SDA and SCL HIGH
}

/*************************************************************************************************
	Function Name: sendByte
	Description:  send a byte
	Parameters: ucDta: data to send
	Return: None
*************************************************************************************************/
void SoftwareI2C::sendByte(uint8_t ucDta) {
	for (int i = 0; i < 8; i++) {
		//sclSet(LOW);
		
		// [ DATA HOLD TIME = 0]

		// Set SDA to MSB
		sdaSet((ucDta & 0x80) != 0); 
		ucDta <<= 1; // Shift Left to get next Bit to MSB

		// SCL LOW TIME
		delayMicroseconds(T_LOW);

		// SCL Rising
		sclSet(HIGH);

		// SCL HIGH TIME
		delayMicroseconds(T_HIGH);

		// SCL Falling 
		sclSet(LOW);
	}

	
}


/*************************************************************************************************
	Function Name: getAck
	Description:  reads ACK by waiting for SDA to be pulled down by Slave. Leaves SCL LOW at end
	Parameters: None
	Return: 0 – Nak; 1 – Ack
*************************************************************************************************/
uint8_t SoftwareI2C::getAck(void) {

	pinMode(pinSda, INPUT);
	//pinMode(pinSda, INPUT_PULLUP);

	sda_in_out = INPUT;

	sclSet(LOW);

	delayMicroseconds(T_LOW);

	sclSet(HIGH);
	delayMicroseconds(T_HIGH);
	uint32_t timer_t = micros();
	while (1) {

		// Wait for slaveto pull SDA Low
		if (digitalRead(pinSda) == LOW) { // if SDA pulled LOW by Slave-> ACK                            // get ack
			sclSet(LOW);
			//sdaSet(LOW);
			//Log(">>> I2C ACK");
			return GETACK;
		}

		// if nothing happens (SDA stays HIGH) for some time -> NACK
		if (micros() - timer_t > T_ACK_TIMEOUT) {
			sclSet(LOW);
			//Log(">>> I2C NACK");
			return GETNAK;
		}
	}
}


/*************************************************************************************************
	Function Name: sendByteAck
	Description:  send a byte and get ack signal
	Parameters: ucDta: data to send
	Return: 0: get nak  1: get ack
*************************************************************************************************/
uint8_t SoftwareI2C::sendByteAck(uint8_t ucDta) {
	sendByte(ucDta);
	return getAck();
}

/*************************************************************************************************
	Function Name: beginTransmission
	Description:  send begin signal
	Parameters: divider – clock divider
	Return: 0: get nak  1: get ack
*************************************************************************************************/
uint8_t SoftwareI2C::beginTransmission(uint8_t addr) {
	sendStart();                           	// start signal
	return sendByteAck(addr << 1 + 0);    	// send write address and get ack
}

/*************************************************************************************************
	Function Name: endTransmission
	Description:  send stop signal
	Parameters: None
	Return: None
*************************************************************************************************/
uint8_t SoftwareI2C::endTransmission() {
	endTransmission(true);
	return 0;
}

uint8_t SoftwareI2C::endTransmission(bool _sendStop)
{
	if (_sendStop)
		sendStop();
	else
	{
		// Start Repeat wait time
		sdaSet(HIGH);
		// maybe add delay here
		sclSet(HIGH);
		delayMicroseconds(T_SU_STA);
	}
	
	// Default SCL, SDA High
	sclSet(HIGH);
	sdaSet(HIGH);
	return 0;
}

/*************************************************************************************************
	Function Name: write
	Description:  send a byte
	Parameters: dta: data to send
	Return: 0: get nak  1: get ack
*************************************************************************************************/
uint8_t SoftwareI2C::write(uint8_t dta) {
	return sendByteAck(dta);
}

/*************************************************************************************************
	Function Name: write
	Description:  write array
	Parameters: len - length of the array
				dta - array to be sent
	Return: 0: get nak  1: get ack
*************************************************************************************************/
/*uint8_t SoftwareI2C::write(uint8_t len, uint8_t* dta) {
	for (int i = 0; i < len; i++) {

		if (GETACK != write(dta[i])) {
			return GETNAK;
		}
	}

	return GETACK;
}
*/


/*************************************************************************************************
	Function Name: write
	Description:  write array
	Parameters: data - array to be sent
				quantity - length of the array
	old -> Return: 0: get nak  1: get ack
	Return: amount of bytes written
*************************************************************************************************/
uint16_t SoftwareI2C::write(const uint8_t* data, uint16_t quantity)
{
	for (uint16_t i = 0; i < quantity; ++i) {
		if (!write(data[i])) {
			return i;
		}
	}
	return quantity;
}


/*************************************************************************************************
	Function Name: requestFrom
	Description:  request data from slave
	Parameters: addr - address of slave
			   len  - length of request
	old -> Return: 0: get nak  1: get ack
	Return: the number of bytes (returned from the slave device) requested
*************************************************************************************************/
uint8_t SoftwareI2C::requestFrom(uint8_t addr, uint8_t len) {
	sendStart();                       // start signal
	recv_len = len;
	uint8_t ret = sendByteAck((addr << 1) + 1);   // send read address and get ack
	//sclSet(LOW);
	//return ret;
	return ret ? len : 0;
}

/*************************************************************************************************
	Function Name: read
	Description:  read a byte from i2c
	Parameters: None
	Return: data get
*************************************************************************************************/
uint8_t SoftwareI2C::read() {
	if (!recv_len) {
		return 0;
	}

	uint8_t ucRt = 0;

	pinMode(pinSda, INPUT);
	sda_in_out = INPUT;

	sclSet(LOW);
	delayMicroseconds(T_LOW);

	// Read 8 Bytes by Cycling SCL
	for (int i = 0; i < 8; i++) {
		unsigned  char  ucBit;
		
		// SCL RISING
		sclSet(HIGH);
		delayMicroseconds(T_HIGH);

		// READ from SDA
		ucBit = digitalRead(pinSda);
		ucRt = (ucRt << 1) + ucBit;

		// SCL FALING
		sclSet(LOW);
		delayMicroseconds(T_LOW);
	}

	uint8_t dta = ucRt;
	recv_len--;

	if (recv_len > 0) {     // send ACK
		//sclSet(LOW);
		
		// ACK -> SDA LOW
		sdaSet(LOW);
		
		// SCL RISING
		sclSet(HIGH);
		
		delayMicroseconds(T_HIGH);
		
		sclSet(LOW);
	} else {                // send NAK
		//sclSet(LOW); 
		
		// NAK -> SDA HIGH
		sdaSet(HIGH);  
		
		// SCL RISING
		sclSet(HIGH);  
		
		delayMicroseconds(T_HIGH);

		// SCL FALING
		sclSet(LOW);
		
		sendStop();
	}
	return dta;
}