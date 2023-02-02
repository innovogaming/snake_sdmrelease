#ifndef CRC16_H
#define CRC16_H
namespace Tinker_Crc16 {

	// Initializes the calculation to be performed (initial value = 0xFFFF).
	void init();

	// Same as init, but with the possibly different initial value.
	void init( uint16_t initialValue );

	// Adds more words.
	void add( uint16_t const * data, int count );

	// An inline function to add one value
	void addOne( uint16_t v );

	// Adds a final zero word.
	void finalize();

	// Returns the current result.
	uint16_t get();
}
#endif // CRC16_H
