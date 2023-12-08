#pragma once

#include "jSerial.h"

// writes/dumps all traffic through stream data to stream dump
class StreamDebugger
	: public jStream
{
	public:
		StreamDebugger(jStream& data, jStream& dump)
			: _data(data), _dump(dump)
		{}

		virtual size_t write(uint8_t ch) {
			_dump.write(ch);
			return _data.write(ch);
		}
		virtual int read() {
			int ch = _data.read();
			if (ch != -1) { _dump.write(ch); }
			return ch;
		}
		virtual int available() { return _data.available(); }
		virtual int peek()      { return _data.peek();      }
		virtual void flush()    { _data.flush();            }

		void directAccess() {
			while(true) {
				if (_data.available()) {
					_dump.write(_data.read());
				}
				if (_dump.available()) {
					_data.write(_dump.read());
				}
				delay(0);
			}
		}
	private:
		jStream& _data;
		jStream& _dump;
};



// Wrapps given Stream ser_target and forwards all rx and/or tx to ser_dst
class jStreamDuplicator
	: public jStream
{
protected:
	bool _enable_rx;
	bool _enable_tx;
	jStream& _ser_target;
	jStream& _ser_dst;

public:
		jStreamDuplicator(jStream& ser_target, jStream& ser_dst, 
			bool enable_rx=true, //if true rx from ser_target get send to ser_dst
			bool enable_tx=true //if true tx fram ser_target get send to ser_dst
	)
			: _ser_target(ser_target), _ser_dst(ser_dst)
	{
		_enable_rx = enable_rx;
		_enable_tx = enable_tx;
	}

	virtual size_t write(uint8_t ch) override
	{
		if(_enable_tx)
			_ser_dst.write(ch);
		return _ser_target.write(ch);
	}

	virtual int read() override {
		int ch = _ser_target.read();
		
		if (_enable_rx && ch != -1) 
			_ser_dst.write(ch);
		
		return ch;
	}

	virtual int available() { return _ser_target.available(); }
	virtual int peek()	  { return _ser_target.peek(); }
	virtual void flush() { 
		_ser_target.flush(); 
		_ser_dst.flush();
	}
	
};


// Wrapps given ser_target Stream and sends all tx to ser_target to ser_dst_tx and all rx from ser_target to sed_dst_rx
// can be used to log traffic over a given stream
// Traffic through jStreamTrafficRouter is not intercepted and can therefore be used as if ser_target is used directly

class jStreamTrafficRouter
	: public jStream
{

private:
	bool _enable_rx; // it true ser_targer.read() is written to ser_dst_rx
	bool _enable_tx; // it true ser_targer.write() is written to ser_dst_tx
	jStream* _ser_target;
	jStream* _ser_dst_rx = nullptr;
	jStream* _ser_dst_tx = nullptr;

	// these keep track of if to rx or tx dst stream was written to since last flush. this is used to flush in case to other one is written
	bool written_to_rx = false; 
	bool written_to_tx = false; 

public:
	jStreamTrafficRouter(jStream* ser_target, jStream* ser_dst_tx, jStream* ser_dst_rx)
		: _ser_target(ser_target), _ser_dst_rx(ser_dst_rx), _ser_dst_tx(ser_dst_tx)
	{
		_enable_rx = _ser_dst_rx != nullptr;
		_enable_tx = _ser_dst_tx != nullptr;
	}

	// if enabled ser_dst_rx is flushed before ser_dst_tx is written to (and the the other way around)
	// this makes sure that the order/sequence of rx and tx data through ser_target is maintained
	bool enable_flush_other_dst = true; 

	virtual size_t write(uint8_t ch) override
	{
		if(_enable_tx)
		{
			if(_enable_rx && written_to_rx && enable_flush_other_dst)
				_ser_dst_rx->flush();
			
			_ser_dst_tx->write(ch);
			written_to_tx = true;
		}
		return _ser_target->write(ch);
	}

	
	virtual size_t write(const uint8_t *buffer, size_t size) override
	{
		if(_enable_tx)
		{
			if(_enable_rx && written_to_rx &&  enable_flush_other_dst)
				_ser_dst_rx->flush();
			
			_ser_dst_tx->write(buffer, size);
			written_to_tx = true;
		}
		return _ser_target->write(buffer, size);
	}


	virtual int read() override {
		int ch = _ser_target->read();
		
		if (_enable_rx && ch != -1) 
		{
			if(_enable_rx && written_to_tx && enable_flush_other_dst)
				_ser_dst_tx->flush();
			
			_ser_dst_rx->write(ch);
			written_to_rx = true;
		}
		
		return ch;
	}

	virtual size_t readBytes(char *buffer, size_t length) {
		size_t len = _ser_target->readBytes(buffer, length);
	
		if (_enable_rx && len > 0) 
		{
			if(_enable_rx && written_to_tx && enable_flush_other_dst)
				_ser_dst_tx->flush();
			
			_ser_dst_rx->write(buffer, length);
			written_to_rx = true;
		}

		return len;
	}

	virtual int available() { return _ser_target->available(); }
	virtual int peek()	  { return _ser_target->peek(); }
	
	virtual void flush() 
	{ 
		_ser_target->flush(); 
		
		if(_enable_tx)
		{
			written_to_tx = false;
			_ser_dst_tx->flush();
		}
		
		if(_enable_rx)
		{
			written_to_rx = false;
			_ser_dst_rx->flush();
		}		
	}
	
};

