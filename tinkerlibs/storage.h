#ifndef STORAGE_H
#define STORAGE_H
namespace Tinker_Storage {

	/// Possible errors which may occur while working with the Class
	enum Error
	{
		NoError,
		NoValidCopies,
		UpdateNotStarted,
		SizeTooLarge,
		OddSizeForMiddleBlock,
		NotAllDataAdded,
		ReadNotStarted,
		CorruptedDataWritten,
		ReadOnlyMedia,
		CommunicationError
	};

	class Class
	{
		public:

			int format();

			/// sequenceCounter is an external sequence counter supplied by the user.
			/// It should probably be _PERSISTENT, with the default value of 0. The
			/// counter is increased each time the data is updated and stored alongside
			/// with the data. If, when reading the data from a FRAM, the counter stored
			/// there is smaller than the current value, this is an error. Larger values
			/// are ok, the counter variable would be updated to match it then.
			/// maxSize specifies the maximum logical data size that may be stored in
			/// this storage. It must always be even.
			/// physOffset specifies a byte offset from the beginning of FRAM where the
			/// bytes are to be stored starting from.
			Class( uint64_t * sequenceCounter, unsigned maxSize, unsigned physOffset );

			/// Returns the maximum physical size that this storage would ever use.
			/// This might be handy when calculating the offset for another instance.
			unsigned getMaxPhysicalSize();

			/// Checks the integrity of the storage and repairs it. If the first copy is
			/// valid, it gets copied over the second one. If, instead, only the second
			/// copy is valid, it gets copied over the first one. If no copies are valid.
			/// an error is returned. If a copy seems to be ok, but its sequence counter
			/// is smaller than the current one, the copy is judged to be invalid
			/// nevertheless. This is also true if the current machine's id is different
			/// from the value stored in the FRAM.
			/// The function requires a work buffer of 512 bytes. The Comm's response
			/// buffer is well suited for that purpose.
			/// The result is either NoError or NoValidCopies.
			Error checkAndRepair( void * workBuffer );

			/// Starts the process of updating the data. All data must be written in
			/// full. This function initiates the progress by writing new headers
			/// for the first copy. Then, all the bytes should be fed inside until the
			/// copy is full, which would gradually update the first copy. After all the
			/// bytes were fed inside, the finalizeUpdate() should be called, which
			/// would create the second copy by copying the first copy over it.
			/// The function returns SizeTooLarge if the storage can't hold the number of
			/// bytes requested, or starts the process and returns NoError otherwise.
			/// If there was some reading in progress, it gets cancelled.
			/// If ensureReadable is passed as 1, the integrity of the previous data
			/// is checked before starting to overwrite it, thus ensuring that if
			/// something goes wrong, the previous data would still be recoverable.
			/// The workBuffer is only used if ensureReadable is true -- it must be
			/// aligned and contain at least 512 bytes.
			Error startUpdate( unsigned size, int ensureReadable, void * workBuffer );

			/// Adds more data after the update was started. Each new chunk's size
			/// must be even, unless it's the last block. If the last block's size is odd,
			/// the buffer passed must be able to hold one extra byte. The buffer must
			/// always be aligned to a word boundary. The buffer's contents get trashed
			/// after the data is added.
			Error addData( void const * data, unsigned size );

			/// This function finalizes the update process by marking the first copy
			/// valid and copying it over the second copy. It requires an aligned
			/// work buffer of 512 bytes.
			Error finalizeUpdate( void * workBuffer );

			/// This function initiates the process of reading the data from the
			/// storage. On success, the returned value is NoError, and the size of
			/// the data gets stored to 'size'.
			/// If there was any update in progress, is gets cancelled.
			/// The function requires an aligned work buffer of 512 bytes, because
			/// it calls checkAndRepair() first.
			Error startReading( uint16_t & size, void * workBuffer );

			/// Reads more data. The size must be even for all data chunks except
			/// the last one. The buffer must be aligned and, in case of the last chunk
			/// if the size is odd, must be able to hold one extra byte.
			/// There is no need to finalize the reading -- startReading() would
			/// start another reading, and startUpdate() would start another update,
			/// cancelling any previous readings.
			Error readData( void * data, unsigned size );

		private:

			/// Xors the given block by the output of the PRNF, using and advancing the
			/// currentRngValue. The block is processed as a sequence of words. size is
			/// the number of words to be processed.
			void xorBlock( uint16_t * block, unsigned size );

			/// Copies one copy over the over one. If firstToSecond is true, it copies
			/// the first copy over the second one, and vice versa otherwise.
			Error makeCopy( int firstToSecond, unsigned rawTotalCopySize, void * workBuffer );

			/// Checks if the specified copy is valid -- first copy if secondCopy is 0,
			/// second copy otherwise. If the copy appears to be valid, its sequence
			/// number is copied to our sequenceCounter.
			/// Returns the raw physical size of the copy, or -1 if the copy was invalid.
			int isCopyValid( int secondCopy, void * workBuffer );

			uint64_t * sequenceCounter;
			unsigned maxLogicalSize;
			unsigned physOffset;

			int updateWasStarted; // 1 -- update was started
			int readWasStarted; // 1 -- read was started

			unsigned bytesLeft; // bytes left to be filled or read
			unsigned currentPhysOffset; // the offset for the next data byte to be
                              // written or read
			int32_t currentRngValue;
			uint16_t currentCrcValue;

	};

}
#endif // STORAGE_H
