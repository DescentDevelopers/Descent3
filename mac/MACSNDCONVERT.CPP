#include <Sound.h>
OSErr
ConvertFromWavToRawSound(void* inBufferP, UInt32 inBuffSize, void* outBufferP, UInt32* outBuffSize)
{
	OSErr theErr;
	SoundConverter theSC;
	UInt32 numFrames, numBytes, outBytes;
	UInt32 aditionalOutputFrames, additionalOutputSize;
	SInt32 currentBytes, bytesLeft;
	SoundComponentData input, output; //these hold info for the formats
	input.flags =                   0;
	input.format =                  MicrosoftADPCMFormat; /*one of the wav formats*/
	input.numChannels =     		2;
	input.sampleSize =              16; /*8 for eight bit sound*/
	input.sampleRate =              rate44khz; /* a constant defined in Sound.h*/
	input.sampleCount =				0;
	input.buffer =                  0;
	input.reserved =                0;
	output.flags =                  0;
	output.format =                 kSoundNotCompressed; /*'raw ' */
	output.numChannels =			2;
	output.sampleSize =				16; /*8 for eight bit sound*/
	output.sampleRate =				rate44khz; /* a constant defined in Sound.h*/
	output.sampleCount =			0;
	output.buffer =                 0;
	output.reserved =               0;
	/*This opens a SoundConverter component. the  params are ... 
	output format, output format, and the sound converter*/
	 theErr = SoundConverterOpen(&input, &output, &theSC);
	if(theErr) return theErr;
	/* so how to allocate the new buffer? */
	theErr = SoundConverterGetBufferSize(theSC, inBuffSize, &numFrames, &numBytes, outBuffSize);
	if(theErr) return theErr;
	        
	outBuffer = mem_malloc(outBytes); 
	if(!outBuffer) {
	        mprintf((0, "ConvertFromWavToRawSound: unable to allocate %d bytes", outBytes));
	        Int3();
	}
	theErr = SoundConverterBeginConversion(theSC);
	if(theErr) return theErr;
	        
	if(inBuffSize <= outBytes) { /*we can do it in one big chunk...*/
	        theErr = SoundConverterConvertBuffer(theSC, inBufferP, numFrames, outBufferP, 0, 0);
	        if(theErr) return theErr;
	                
	        theErr = SoundConverterEndConversion(theSC, outBufferP, &aditionalOutputFrames, &additionalOutputSize);
	        if(theErr) return theErr;
	}
	else { /*we need to do it in chunks :(  */
	        bytesLeft = *outBuffSize;
	        while(bytesLeft > 0) {
	                theErr = SoundConvertBuffer(theSC, inBufferP, numFrames, numBytes, outBufferP, 0, 0);
	                if(theErr) return theErr;
	                
	                bytesLeft -= numBytes;
	        }
	        theErr = SoundConverterEndConversion(theSC, outBufferP, &aditionalOutputFrames, &additionalOutputSize);
	        if(theErr) return theErr;
	}
	return theErr;
}
