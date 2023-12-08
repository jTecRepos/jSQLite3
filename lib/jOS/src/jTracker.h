#pragma once

#include "jDefines.h"


class jTrackerTraffic {
	protected:

		uint32_t time_start = -1;
		//uint32_t time_start_total = -1;
		uint32_t count = 0;//[1] accumulator of elements of current epoch
		uint32_t count_total = 0; //[1] total amount of elements counted
		uint16_t sample_count = 0; //[1] count of counter increments
		uint32_t last_rate = 0; //[1/s]

	public:

		jTrackerTraffic()
		{
			reset();
		}
		
		virtual void reset()
		{
			time_start = millis();
			//time_start_total = millis();
			
			sample_count = 0;
			
			count = 0;
			count_total = 0;
		}

		virtual void inc_counter(uint32_t v = 1)
		{
			count += v;
			count_total += v;
			sample_count++;
		}

		// end epoch/track period and return stats
		// dt: duration of epoch in [ms]
		// c: counted in this epoch in [1]
		// rate: counter rate during epoch in [1/s]
		virtual void epoch(uint16_t& dt, uint32_t& c, uint32_t& rate)
		{
			if(time_start == -1)
			{
				dt = 0;
				c = 0;
				rate = 0;
				
			}
			else
			{
				dt = millis() - time_start;
				c = count;
				rate = (dt > 0) ? 1000 * count / dt : 0;
			}
			last_rate = rate;
			time_start = millis();
			count = 0;
		}

		// finishes epoch and prints results
		virtual String print(const String& tag);

		//[1/s] rate in counts/seconds 
		virtual uint32_t get_rate()
		{
			return last_rate;
		}

		uint32_t get_total_count() { return count_total; }
};


class jTrackerTrafficCost : public jTrackerTraffic {
	protected:
		float cost_per_kilocount; //[cost/1000counts] like €/kb

	public:

		jTrackerTrafficCost(float cost_per_kilo)
			: cost_per_kilocount(cost_per_kilo)
		{

		}
		

		String print(const String& tag) override;

		float getCost_perHour()
		{
			float kilorate_p_hour = (float) (get_rate() * FACTOR_hour_2_s) / 1000.0f; //[1000count/hour]
			return kilorate_p_hour * cost_per_kilocount;
		}

};


// tracks avarage of samples during epoch
// make polymoph to allow use for float
class jTrackerAvg {

protected:

	
	uint32_t acc = 0;
	uint16_t count = 0;
	float last_avg = 0.0f;
	int16_t last_count = 0.0f;
	uint16_t value_max = 0;
	uint16_t value_min = 0;

public:

	jTrackerAvg()
	{

	}
	
	virtual void reset()
	{
		acc = 0;
		count = 0;
		last_avg = 0;
	}

	virtual void add_sample(uint16_t v = 1)
	{
		if(count == 0)
		{
			value_max = v;
			value_min = v;
		}
		else
		{
			value_max = MAX(value_max, v);
			value_min = MIN(value_min, v);
		}

		acc += v;
		count++;
	}

	// end epoch/track period and return stats
	virtual void epoch(float& avg)
	{
		if(count == 0)
		{
			avg = 0.0f;
		}
		else
		{
			avg = ((float) acc) / count;
		}
		last_count = count;
		last_avg = avg;
		count = 0;
		acc = 0;
	}
	

	String print(const String& tag);

	String print(const String& tag, float display_factor);

	float getAvarage()
	{
		return last_avg;
	}

};



class jTrackerTransfer {
	/*
	Tracks transfers like sd writes or data transfer over network
	
	a Transfer consits of multiple chunk transmission

	mainly tracks unit amounts and durations taken


	use Example1: 

		tracker.start_transfer();

		while(data available())
		{
			tracker.transfer_chunk_start();
			
			file.write(data);
			
			tracker.transfer_chunk_end(data length);

			EXECUTE_EVERY(1000)
				tracker.print("tracker");
			EXECUTE_EVERY_END
		}

		tracker.end_transfer();

		tracker.print_total("tracker");

	__________________________________________

	*/

	protected:

		uint32_t target_count = 0; //[1] total to transfer amount

		uint32_t count = 0; //[1] accumulates unit amount
		uint32_t duration = 0; //[us] accumulates duration taken

		uint32_t count_total = 0; //[1] total transfered amount
		uint32_t duration_total = 0; //[us] total duration of all chunk transfers
		
		uint16_t sample_count = 0; //[1]  counts chunk transfers
		uint32_t last_rate = 0; //[1/s] rate of last epoch
		uint32_t max_duration = 0; //[us] maximum duration/dt (longest transmission)
		uint32_t max_count = 0; //[1] maximum count (max elements transfered in one transmission)

		uint32_t last_time_inc_counter = 0; //[us] timestamp of last inc counter call to track dt 
		uint32_t time_chunk_start = 0; //[us]
		
		uint32_t time_transfer_start = 0; //[ms] timestamp of total transfer start
		uint32_t time_transfer_end = 0; //[ms] timestamp of total transfer end

	public:

		jTrackerTransfer()
		{
			reset();
		}
		
		virtual void reset()
		{
			sample_count = 0;

			count = 0;
			duration = 0;
			count_total = 0;
			duration_total = 0;
			max_duration = 0;
			max_count = 0;

			time_transfer_start = millis();
			last_time_inc_counter = 0;
		}

		
		// helper function to indicate the start of a chunk of data 
		void transfer_chunk_start()
		{
			time_chunk_start = micros();
			last_time_inc_counter = micros();
		}

		// helper function to indicate the end of a chunk of data 
		void transfer_chunk_end(uint32_t v)
		{
			uint32_t time_chunk_end = micros();
			//int32_t dt_us = (time_chunk_start > time_chunk_end) ? (0xFFFFFFFF - time_chunk_start) + time_chunk_end : time_chunk_end - time_chunk_start;
			int32_t dt_us = MICROS_DT(time_chunk_start, time_chunk_end);
			inc_counter(dt_us, v);
		}


		// starts the transfer
		void start_transfer(uint32_t target_count)
		{
			reset();
			this->target_count = target_count;
			time_transfer_start = millis();
			last_time_inc_counter = 0;
		}

		// ends the transfer
		void end_transfer()
		{
			time_transfer_end = millis();
		}


		// register new transmission 
		// dt: [us] time the transmission took
		// len: [1] length of data transmitted
		virtual void inc_counter(uint32_t dt, uint32_t len)
		{
			count += len;
			count_total += len;
			
			duration += dt;
			duration_total += dt;

			max_duration = MAX(max_duration, dt);
			max_count = MAX(max_count, len);
			
			sample_count++;
		}	

		// register new transmission that was going on since last inc_counter call
		// len: [1] length of data transmitted
		virtual void inc_counter(uint32_t len)
		{
			uint32_t t = micros();
			int32_t dt = last_time_inc_counter==0 ? 0 : MICROS_DT(last_time_inc_counter, t);
			last_time_inc_counter = t;

			count += len;
			count_total += len;
			
			duration += dt;
			duration_total += dt;

			max_duration = MAX(max_duration, dt);
			max_count = MAX(max_count, len);
			
			sample_count++;
		}	

		// end epoch/track period and return stats
		// dt: duration accumulated in this epoch in [ms]
		// c: counted in this epoch in [1]
		// rate: counter rate during epoch in [1/s]
		virtual void epoch(uint16_t& dt, uint32_t& c, uint32_t& rate)
		{
			if(sample_count == 0)
			{
				dt = 0;
				c = 0;
				rate = 0;
			}
			else
			{
				dt = duration / FACTOR_ms_2_us;
				c = count;
				//rate = (dt > 0) ? count * FACTOR_s_2_us / duration : 0; //[1] / [s] = [1/s]
				rate = (dt > 0) ? ((uint32_t) ((float) count / (duration * FACTOR_us_2_s))) : 0; //[1] / [s] = [1/s]
			}
			last_rate = rate;
			count = 0;
			duration = 0;
		}


		// finishes epoch and prints results
		virtual String print(const String& tag);


		// print summary of complete transmission after transmission ends
		virtual String print_total(const String& tag);



		// rate in counts/seconds [1/s]
		virtual uint32_t get_rate()
		{
			return last_rate;
		}

		// Total Rate in [1/s]
		uint32_t get_total_rate()
		{
			if(duration_total == 0)
				return 0;
			return (uint32_t) ((float) count_total / (duration_total * FACTOR_us_2_s));
		}

		// Total Elements in [1]
		uint32_t get_total_count() { return count_total; }
};