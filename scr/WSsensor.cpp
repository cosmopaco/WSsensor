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
 
 
#include "WSsensor.h" 



/************
* WSsensor

		_   
'0':   | |____ (M,4M)  487,1950
		_       
'1':   | |________	(M,8M) 487,3900
        _    
'S':   | |____________________	(M,18.9M) 487,~9200

M = Mark period of ~487µs. 

A full frame looks like this:

- 4  bit: header 0b0101 Pending clarification !!!
- 8  bit: Random ID.
- 2  bit: Fixed 0b11 FILLING?
- 2  bit: Chanel. 0 to 2 chanel select 1 to 3.
- 12 bit: Temperature signed.
- 8  bit: Humidity 0-100%
- stop pulse: 1T high, 18.9T low

************/

	
	
	WSsensor::sensor WSsensor::_receivedData;			// Contains received data.
	WSsensor::sensor WSsensor::_previousData;			// Contains previous received data.
	WSsensor::sensor WSsensor::_outBuffer;				// Contains readable data. 
	
	int8_t WSsensor::_interrupt;			// Radio input interrupt		
	uint8_t WSsensor::_minRepeats;
    int8_t WSsensor::_bitCount = -1;
	
	bool WSsensor::_inCallback;				// When true, the callback function is being executed; prevents re-entrance.
	bool WSsensor::_enabled;				// If true, monitoring and decoding is enabled. If false, interruptHandler will return immediately.
	bool WSsensor::_haveData;



WSsensor::WSsensor(){	
	_interrupt = -1;
	_minRepeats = 2;
	_haveData  = false;
}

void WSsensor::enableReceive() {
	if (_interrupt != -1) {
		attachInterrupt(_interrupt, interruptHandler, CHANGE);
		_enabled = true;
	}
}

void WSsensor::enableReceive(int8_t interrupt) {
	_interrupt = interrupt;
	enableReceive();	
}


void WSsensor::enableReceive(int8_t interrupt, uint8_t minRepeats) {
	_interrupt = interrupt;
	_minRepeats = minRepeats;
	enableReceive();
	
}




void WSsensor::disableReceive() {
	if (_interrupt != -1) {
		detachInterrupt(_interrupt);
	}
	_enabled = false;
}


int WSsensor::readId(){
	return _outBuffer.randomID;
}

int WSsensor::readCh(){
	return _outBuffer.chanel+1;
}

int WSsensor::readUn(){
	return _outBuffer.padding;
}

int WSsensor::readTe(){
	int16_t temp;
	if ( _outBuffer.temperature > 511){
		temp= _outBuffer.temperature | 0xF000 ;
	}
	return temp;
}

int WSsensor::readHu(){
	return _outBuffer.humidity;
}
void WSsensor::resetAvailable(){
	_haveData=false;
}

bool WSsensor::avalaible(){
	if(_haveData){
		resetAvailable();
		return true;
	}else 
		return false;

}

/* Valid Packet repeats test .
/
*/
void WSsensor::endPacket(){
    static byte repeats = 0;			
	// memcmp isn't deemed safe
	
	if (_receivedData.all != _previousData.all){ 
		repeats=0;
		_previousData = _receivedData;
	}else
		repeats++;
	
	if (repeats >=_minRepeats) {
		_outBuffer = _receivedData;
		_haveData=true;
	}
	
	_bitCount=0;	
	return;
}

void WSsensor::interruptHandler() {

	static bool mark;
	static unsigned long lastChange;
	
	unsigned long currentTime;
	unsigned int duration;
	
	
	if (!_enabled) {
		return;
	}
	
	currentTime=micros();
	duration=currentTime-lastChange; 
	lastChange=currentTime;
	
//	Serial.println(duration);

//  FILTER NOT USED
//	
//	static unsigned long edgeTimeStamp[3] = {0, };	// Timestamp of edges
//	
//	static bool skip;
//
//	
//	// Filter out too short pulses. This method works as a low pass filter.
//	edgeTimeStamp[1] = edgeTimeStamp[2];
//	edgeTimeStamp[2] = micros();

//	if (skip) {
//		skip = false;
//		return;
//	}
//
//	if (_state >= 0 && edgeTimeStamp[2]-edgeTimeStamp[1] < MARK_MIN) {
//		// Last edge was too short.
//		// Skip this edge, and the next too.
//		skip = true;
//		return;
//	}
//
//	unsigned int duration = edgeTimeStamp[1] - edgeTimeStamp[0];
//	edgeTimeStamp[0] = edgeTimeStamp[1];
	
	if (mark){
		mark = false;
		
		//Waiting sync, to decode.
		if (_bitCount == -1){
			if(duration < 8500){
				return;
			}				
		}		
		else if (_bitCount < 36){
			if (duration > BIT1_MIN && duration < BIT1_MAX){
				_receivedData.all = _receivedData.all << 1;	
				bitSet(_receivedData.humidity, 0);
			}
			else if (duration > BIT0_MIN && duration < BIT0_MAX){
				_receivedData.all = _receivedData.all << 1;
			}else{			
				_bitCount=-2;					
			}			
		}else{
			//End packet, wait sync.			
			if (duration > 8500){
				endPacket();
			}
			else{
				_bitCount=-2;
			}			
		}		
		_bitCount++;
		
		/*	
		//if it is not valid header.
		if (_bitCount == 3 && (_receivedData.humidity & 0x0F) != HEADER){
			Serial.print(_receivedData.humidity);
			Serial.println("=error header");
			_bitCount=-2;
		}
		*/
		

	}
	else{
		if (duration > MARK_MIN && duration < MARK_MAX){
			mark = true;
		}
		else{
			mark=false;
			_bitCount=-1;
		}
	}		

	return;
}

uint8_t WSsensor::peek() {
	return _bitCount;
}
