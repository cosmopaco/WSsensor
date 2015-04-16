/**
 * WSsensor Arduino library for Wheather Station sensor receiver.
 * 
 * Copyright (c) 2015 @cosmopaco. All right reserved.
 * 
 * 
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Francisco Cosmo
 * Creation date: 22/03/2015
 */

#ifndef WSsensor_h
#define WSsensor_h

#include <Arduino.h>

#define HEADER 0b1010 //Pending clarification
#define FILLING 0b11
#define TIME_MARK 490
#define TOLERANCE 10

const unsigned int MARK_MIN = TIME_MARK - (TIME_MARK * TOLERANCE/100);
const unsigned int MARK_MAX = TIME_MARK + (TIME_MARK * TOLERANCE/100);
const unsigned int BIT0_MIN = MARK_MIN * 4;
const unsigned int BIT0_MAX = MARK_MAX * 4;
const unsigned int BIT1_MIN = MARK_MIN * 8;
const unsigned int BIT1_MAX = MARK_MAX * 8;

/**
*
*
* This is a pure static class, for simplicity and to limit memory-use.
*/

class WSsensor {

	public:
	WSsensor();
	void enableReceive();
	void enableReceive(int8_t interrupt);
	void enableReceive(int8_t interrupt, uint8_t minRepeats);		
	void disableReceive();
	virtual uint8_t peek();
	bool avalaible();
	void resetAvailable();
        int readId();
        int readTe();
        int readHu();
        int readUn();
        int readCh();
		
	union  sensor{
		struct {
			uint32_t humidity : 8;
			uint32_t temperature : 12;
			uint32_t chanel: 2;
			uint32_t padding: 2;
			uint32_t randomID: 8;
		};
			uint32_t all;
		};
		
	private:
	
		static sensor _receivedData;
		static sensor _previousData;
		static sensor _outBuffer;
		
		static int8_t _interrupt;				// Radio input interrupt		
		static uint8_t _minRepeats;
		static int8_t _bitCount;				// Counter bit input.		
		static bool _inCallback;				// When true, the callback function is being executed; prevents re-entrance.
		static bool _enabled;				    // If true, monitoring and decoding is enabled. If false, interruptHandler will return immediately.
		static bool _haveData;	
		
		// 
		static void reset();
		static void endPacket();
		static void interruptHandler();          

};

#endif