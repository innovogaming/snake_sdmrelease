/* rand example: guess the number */
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stdint.h>
#include <string.h>
#include "storage.h"
#include "crc16.h"
#include "fram.h"

namespace Fram = Tinker_Fram;

namespace Tinker_Storage {

	namespace {

		// Each copy physically begins with the Header. All bytes after the header
		// and till the very end are xored with the gamma derived from the header's iv
		// and a PRNG function. The encrypted, second header follows, which stores the
		// sequence number, the id of the machine, and the size of the actual user
		// data stored in the copy. The encrypted CRC16 of the encrypted headers follows.
		// Then follows the data itself. If its size is odd. an extra zero byte is
		// padded at its end (and then gets encrypted as well). The last two bytes are
		// the encrypted CRC16 of all the user data.

		enum
		{
			CopyIsValid = 0x5A,
			CopyIsBlank = 0xA5,
			CopyIsInvalid = 0
		};


		struct Header
		{
			uint8_t copyIsValid;	// The validity byte -- the copy can only be valid
									// when this byte equals to CopyIsValid value
			uint32_t iv; 			// Initialization vector -- a random value generated
									// each new save
		} __attribute__((packed));

		struct EncryptedHeader
		{
			uint64_t sequenceNumber;
			uint32_t machineId;
			uint16_t dataSize;
		};

	}

	Class::Class( uint64_t * sequenceCounter_, unsigned maxLogicalSize_,
				unsigned physOffset_ ):
				sequenceCounter( sequenceCounter_ ), maxLogicalSize( maxLogicalSize_ ),
				physOffset( physOffset_ ),
				updateWasStarted( 0 ),
				readWasStarted( 0 )
	{
		srand (time(NULL));

		if( Fram::fram_is_present() != BP_SUCCESS)
		{
			Fram::error("FRAM no detectada");
		}
	}

	unsigned Class::getMaxPhysicalSize()
	{
		// All headers, two crcs and all the data itself
		return sizeof( Header ) + sizeof( EncryptedHeader ) + 4 + maxLogicalSize;
	}

	void Class::xorBlock( uint16_t * block, unsigned size )
	{
		while( size-- )
		{
			// An MWC PRNG generator
			currentRngValue = 18000 * ( currentRngValue & 65535 )
                      + ( currentRngValue >> 16 );
                      
			*block++ ^= ( currentRngValue & 0xFFFF );
		}
	}

	Error Class::startUpdate( unsigned size, int ensureReadable, void * workBuffer )
	{
		if ( size > maxLogicalSize ) {
			Fram::error("Size loo LArge");
			return SizeTooLarge;
		}

		if ( !Fram::fram_setBlockProtection( Fram::All ) )
		{
			Fram::error("CommunicationError");
			return CommunicationError;
		}

		readWasStarted = 0;

		if ( ensureReadable )
		{
			if ( isCopyValid( 1, workBuffer ) < 0 )
			{
				// The second copy is invalid. Ok, check if the first one is valid,
				// to use it to repair the first one.
				int copySize = isCopyValid( 0, workBuffer );

				if ( copySize < 0 ) {
					Fram::error("No Valid Copies");
					return NoValidCopies;
				}

				// Ok, copy the second over the first one
				Error err = makeCopy( 0, (unsigned) copySize, workBuffer );

				if ( err != NoError ) {
					Fram::error("Err");
					return err;
				}

				// If the second copy is still invalid, flag an error
				if ( isCopyValid( 1, workBuffer ) < 0 ) {
					Fram::error("Corrupted Data Written");
					return CorruptedDataWritten;
				}
			}
		}	

		if ( !Fram::fram_setBlockProtection( Fram::SecondHalf ) )
		{
			Fram::error("Communication Error");
			return CommunicationError;
		}

		updateWasStarted = 1;
		bytesLeft = size;
		currentPhysOffset = physOffset;

		currentRngValue = rand();
		{
			Header h;

			h.copyIsValid = CopyIsInvalid;
			h.iv = currentRngValue;

			Fram::fram_write_enable();
			Fram::fram_write( currentPhysOffset, &h, sizeof( h ) );
			Fram::fram_write_disable();
			
			currentPhysOffset += sizeof( h );
		}

		EncryptedHeader eh;

		eh.machineId = 1234;
		eh.sequenceNumber = *sequenceCounter + 1;
		eh.dataSize = size;

		Tinker_Crc16::init();
		// We know here that sizeof( eh ) is even
		Tinker_Crc16::add( (uint16_t const * ) &eh, sizeof( eh ) / 2 );
		Tinker_Crc16::finalize();

		uint16_t crc = Tinker_Crc16::get();

		xorBlock( (uint16_t *) &eh, sizeof( eh ) / 2 );
		xorBlock( &crc, 1 );

		Fram::fram_write_enable();
		Fram::fram_write( currentPhysOffset, &eh, sizeof( eh ) );
		Fram::fram_write_disable();	
		
		currentPhysOffset += sizeof( eh );
		
		Fram::fram_write_enable();
		Fram::fram_write( currentPhysOffset, &crc, sizeof( crc ) );
		Fram::fram_write_disable();	
		
		currentPhysOffset += sizeof( crc );

		// Ok, now we're ready to write the user data

		currentCrcValue = 0x00;

		if ( !Fram::fram_setBlockProtection( Fram::All ) )
		{
			updateWasStarted = 0;
			Fram::error("CommunicationError");
			return CommunicationError;
		}

		return NoError;
	}

	Error Class::addData( void const * data, unsigned size )
	{
		if ( !updateWasStarted ) {
			Fram::error("Update Not Started");
			return UpdateNotStarted;
		}

		if ( !size )
			return NoError;

		if ( size > bytesLeft ) {
			Fram::error("Size too large");
			return SizeTooLarge;
		}

		if ( size & 1 )
		{
			// Size is odd, this is only allowed for last block

			if ( size != bytesLeft ) {
				Fram::error("Odd Size For Middle Block");
				return OddSizeForMiddleBlock;
			}

			// Pad an extra zero byte
			((unsigned char *)data)[ size ] = 0;
		}
  
		if ( !Fram::fram_setBlockProtection( Fram::SecondHalf ) )
		{
			updateWasStarted = 0;
			Fram::error("Communication Error");
			return CommunicationError;
		}

		uint16_t * ptr = ( uint16_t *) data;

		Tinker_Crc16::init( currentCrcValue );
		Tinker_Crc16::add( ptr, ( size + 1 ) / 2 );

		currentCrcValue = Tinker_Crc16::get();

		xorBlock( ptr, ( size + 1 ) / 2 );
		
		Fram::fram_write_enable();
		Fram::fram_write( currentPhysOffset, ptr, size );
		Fram::fram_write_disable();

		currentPhysOffset += size;

		bytesLeft -= size;

		if ( !Fram::fram_setBlockProtection( Fram::All ) )
		{
			updateWasStarted = 0;
			Fram::error("Communication Error");
			return CommunicationError;
		}
		//printf("Salio del addData Ok...\n");
		return NoError;
	}

	Error Class::finalizeUpdate( void * workBuffer )
	{
		if ( !updateWasStarted ) {
			Fram::error("Update Not Started");
			return UpdateNotStarted;
		}

		if ( bytesLeft ) {
			Fram::error("Not All Data Added");
			return NotAllDataAdded;
		}

		if ( !Fram::fram_setBlockProtection( Fram::SecondHalf ) )
		{
			updateWasStarted = 0;
			Fram::error("Communication Error");
			return CommunicationError;
		}

		// Finalize and write resulting crc

		Tinker_Crc16::init( currentCrcValue );
		Tinker_Crc16::finalize();

		uint16_t crc = Tinker_Crc16::get();

		xorBlock( &crc, 1 );

		Fram::fram_write_enable();
		Fram::fram_write( currentPhysOffset, &crc, sizeof( crc ) );
		Fram::fram_write_disable();

		currentPhysOffset += sizeof( crc );

		// Now mark the copy as valid

		char validByte = CopyIsValid;

		Fram::fram_write_enable();
		Fram::fram_write( physOffset, &validByte, 1 );
		Fram::fram_write_disable();

		// First copy is now complete. Check it for consistency.
		updateWasStarted = 0;

		if ( !Fram::fram_setBlockProtection( Fram::All ) ) {
			Fram::error("Communication Error");
			return CommunicationError;
		}

		uint64_t sequenceCounterBefore = *sequenceCounter;

		if ( isCopyValid( 0, workBuffer ) < 0 )
		{
			Fram::error("Corrupted Data Written");
			return CorruptedDataWritten;
		}

		if ( *sequenceCounter == sequenceCounterBefore )
		{
			// The sequence counter didn't change. Our writes didn't seem to have had
			// any effect.
			Fram::error("Read Only Media");
			return ReadOnlyMedia;
		}

		// Now copy the first copy over the second one.
		return makeCopy( 1, currentPhysOffset - physOffset, workBuffer );
	}

	Error Class::makeCopy( int firstToSecond,
                       unsigned rawTotalCopySize, void * workBuffer )
	{
		unsigned srcOffset;
		unsigned destOffset;

		if ( firstToSecond )
		{
			srcOffset = physOffset;
			destOffset = physOffset + Fram::Size / 2;
		}
		else
		{
			srcOffset = physOffset + Fram::Size / 2;
			destOffset = physOffset;
		}

		while( rawTotalCopySize )
		{
			unsigned toProcess = rawTotalCopySize > 8192 ? 8192 : rawTotalCopySize;

			if ( !Fram::fram_setBlockProtection( Fram::All ) ) {
				Fram::error("Communication Error");
				return CommunicationError;
			}

			Fram::fram_read( srcOffset, workBuffer, toProcess );
			if ( !Fram::fram_setBlockProtection( firstToSecond ? Fram::None : Fram::SecondHalf ) ) {
				Fram::error("Communication Error");
				return CommunicationError;
			}

			Fram::fram_write_enable();
			Fram::fram_write( destOffset, workBuffer, toProcess );
			Fram::fram_write_disable();

			srcOffset += toProcess;
			destOffset += toProcess;

			rawTotalCopySize -= toProcess;
		}

		if ( !Fram::fram_setBlockProtection( Fram::All ) ) {
			Fram::error("Communication Error");
			return CommunicationError;
		}

		return NoError;
	}

	int Class::isCopyValid(int secondCopy, void * workBuffer )
	{

		unsigned initialOffset = physOffset;
	
		if ( secondCopy )
		{
			initialOffset += Fram::Size / 2;
		}
		else
		{

		}

		unsigned currentOffset = initialOffset;

		Header h;

		Fram::fram_read( currentOffset, &h, sizeof( h ) );

		currentOffset += sizeof( h );
		if ( h.copyIsValid == CopyIsValid )
		{
			// The copy is marked as valid. Check the crcs of the encrypted
			// header and of the data.
			currentRngValue = h.iv;

			EncryptedHeader eh;

			Fram::fram_read( currentOffset, &eh, sizeof( eh ) );

			currentOffset += sizeof( eh );

			xorBlock( (uint16_t *) &eh, sizeof( eh ) / 2 );

			uint16_t crc;

			Fram::fram_read( currentOffset, &crc, sizeof( crc ) );
			xorBlock( &crc, 1 );

			currentOffset += sizeof( crc );

			Tinker_Crc16::init();
			Tinker_Crc16::add( (uint16_t const * ) &eh, sizeof( eh ) / 2 );
			Tinker_Crc16::finalize();
			
			if ( Tinker_Crc16::get() == crc )
			{
				// The crc of the encrypted header is correct
				// Check the machine id and sequence
				if ( (eh.machineId == 1234 &&
					eh.sequenceNumber >= *sequenceCounter)  || 1)
				{
					// They are ok. Proceed checking the user data.
					unsigned bytesLeft = eh.dataSize;

					Tinker_Crc16::init();

					// Here we perform a custom loop where crc, spi and xoring are done
					// all in parallel

					while( bytesLeft )
					{
						unsigned toProcess = bytesLeft > 8192 ? 8192 : bytesLeft;

						Fram::fram_read( currentOffset, workBuffer, toProcess );
	
						xorBlock( (uint16_t *) workBuffer, ( toProcess + 1 ) / 2 );

						if ( toProcess & 1 )
							( ( unsigned char * ) workBuffer )[ toProcess ] = 0;
						Tinker_Crc16::add( (uint16_t const * ) workBuffer, ( toProcess + 1 ) / 2 );
						currentOffset += toProcess;
						bytesLeft -= toProcess;
					}

					Tinker_Crc16::finalize();

					Fram::fram_read( currentOffset, &crc, sizeof( crc ) );

					xorBlock( &crc, 1 );

					currentOffset += sizeof( crc );
        
					if ( crc == Tinker_Crc16::get() )
					{
						// The crc of the data is valid. The copy is fully valid.
						// Use the new value of the sequence counter from that copy.
						*sequenceCounter = eh.sequenceNumber;
						return (int) ( currentOffset - initialOffset );
					}
					else
					{

					}
				}
			}
			else
			{

			}
		}
  
		if ( h.copyIsValid == CopyIsBlank )
		{
			*sequenceCounter = 0;
			return 0;
		}
		return -1;
	}

	Error Class::checkAndRepair( void * workBuffer )
	{
		// Try the first copy first
		if ( !Fram::fram_setBlockProtection( Fram::All ) )
		{
			Fram::error("Communication Error");
			return CommunicationError;
		}

		int result = isCopyValid( 0, workBuffer );
  
		if ( result == 0 )
		{
			// The copy was fine. If the second one isn't, copy it over the second one
			return NoError;
		}

		if ( result > 0 )
		{
			// The copy was fine. If the second one isn't, copy it over the second one
			if ( isCopyValid( 1, workBuffer ) < 0 )
				return makeCopy( 1, (unsigned) result, workBuffer );

			return NoError;
		}

		// The first copy was no success, try the second one

		result = isCopyValid( 1, workBuffer );

		if ( result >= 0 )
		{
			// The copy was fine, copy it over the first one
			return makeCopy( 0, (unsigned) result, workBuffer );
		}

		// No copies were valid. This is an error.
		Fram::error("No Valied Copies");
		return NoValidCopies;
	}

	Error Class::startReading( uint16_t & size, void * workBuffer )
	{
		// End any update in progress. It shouldn't have corrupted anything.
		updateWasStarted = 0;
		Error checkError = checkAndRepair( workBuffer );

		if ( checkError != NoError ) {
			Fram::error("Check Error");
			return checkError;
		}

		// Here we have to pretty much repeat the checkAndRepair steps for
		// the first copy, now that we know it is valid.
		currentPhysOffset = physOffset;
		Header h;
		Fram::fram_read( currentPhysOffset, &h, sizeof( h ) );
		currentPhysOffset += sizeof( h );
		EncryptedHeader eh;

		Fram::fram_read( currentPhysOffset, &eh, sizeof( eh ) );

		// Position the current offset for the first byte of user data
		currentPhysOffset += sizeof( eh ) + sizeof( uint16_t );

		// Decrypt the encrypted header
		currentRngValue = h.iv;
		xorBlock( (uint16_t *) &eh, sizeof( eh ) / 2 );
		uint16_t dummy;
		xorBlock( &dummy, 1 ); // Account for the skipped crc

		// Not sure we need to calc the crc at all. For now we don't

		size = eh.dataSize;
		bytesLeft = eh.dataSize;
		readWasStarted = 1;
		return NoError;
	}

	Error Class::readData( void * data, unsigned size )
	{
		if ( !readWasStarted ) {
			Fram::error("Read Not Started");
			return ReadNotStarted;
		}

		if ( size > bytesLeft ) {
			Fram::error("Size too large");
			return SizeTooLarge;
		}

		if ( size & 1 && size != bytesLeft )
		{
			// Size is odd, this is only allowed for last block
			Fram::error("Odd Size For Middle Block");
			return OddSizeForMiddleBlock;
		}

		if ( !Fram::fram_setBlockProtection( Fram::All ) ) {
			Fram::error("Communication Error");
			return CommunicationError;
		}

		uint16_t * ptr = ( uint16_t *) data;

		Fram::fram_read( currentPhysOffset, ptr, size );

		xorBlock( ptr, ( size + 1 ) / 2 );
		currentPhysOffset += size;
		bytesLeft -= size;
		return NoError;
	}

	int Class::format()
	{
	
		uint16_t size;
		size = getMaxPhysicalSize();
		uint8_t *memory = new uint8_t[size];

		if ( Fram::fram_setBlockProtection( Fram::None ) != BP_SUCCESS)
		{
			Fram::error("No se pudo Modificar la proteccion de bloques");
			return BP_FAILURE;
		}
		
		if(Fram::fram_write_enable() != BP_SUCCESS) {
			Fram::error("Falla write enable");
			return BP_FAILURE;
		}
	
		memset(memory, 0x00, size);
		memory[0]=0xA5;
	
		if(Fram::fram_write(physOffset, &memory[0], sizeof(memory)) != BP_SUCCESS) {
			Fram::error("Falla fram write");
			return BP_FAILURE;
		}

		if(Fram::fram_write_disable() != BP_SUCCESS) {
			Fram::error("Falla write disable");
			return BP_FAILURE;
		}
	
		if ( Fram::fram_setBlockProtection( Fram::All ) != BP_SUCCESS)
		{
			printf("No se pudo Modificar la proteccion de bloques");
			return BP_FAILURE;
		}	
		
		return BP_SUCCESS;
	}

}
