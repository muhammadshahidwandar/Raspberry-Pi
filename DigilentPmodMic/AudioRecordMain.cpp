/***********************************************************************
 * PmodRecordMain.cpp. Sample program that tests the pmodmicSpi class.
 * an mcp3008Spi class object (a2d) is created. the a2d object is instantiated
 * using the overloaded constructor. which opens the spidev0.0 device with
 * SPI_MODE_0 (MODE 0) (defined in linux/spi/spidev.h), speed = 1MHz &
 * bitsPerWord=8.
 *
 * call the spiWriteRead function on the a2d object 20 times. Each time make sure
 * that conversion is configured for single ended conversion on CH0
 * i.e. transmit ->  byte1 = 0b00000001 (start bit)
 *                   byte2 = 0b1000000  (SGL/DIF = 1, D2=D1=D0=0)
 *                   byte3 = 0b00000000  (Don't care)
 *      receive  ->  byte1 = junk
 *                   byte2 = junk + b8 + b9
 *                   byte3 = b7 - b0
 *    
 * after conversion must merge data[1] and data[2] to get final result
 *
 *
 *
 * *********************************************************************/
#include <sys/time.h>
#include "PmodMicSpi.h"
#include <fstream>
#include <iostream>
/////////////////////////////////////
/*	struct wavfile_header {
	char	riff_tag[4];
	int	riff_length;
	char	wave_tag[4];
	char	fmt_tag[4];
	int	fmt_length;
	short	audio_format;
	short	num_channels;
	int	sample_rate;
	int	byte_rate;
	short	block_align;
	short	bits_per_sample;
	char	data_tag[4];
	int	data_length;
};*/
	//////////////////////////////////////////
using namespace std;
 #define	B_SIZE	40000
int main(void)
{
	///////////////////////////////////
/*	struct wavfile_header header;

	int samples_per_second = WAVFILE_SAMPLES_PER_SECOND;
	int bits_per_sample = 16;

	strncpy(header.riff_tag,"RIFF",4);
	strncpy(header.wave_tag,"WAVE",4);
	strncpy(header.fmt_tag,"fmt ",4);
	strncpy(header.data_tag,"data",4);

	header.riff_length = 0;
	header.fmt_length = 16;
	header.audio_format = 1;
	header.num_channels = 1;
	header.sample_rate = samples_per_second;
	header.byte_rate = samples_per_second*(bits_per_sample/8);
	header.block_align = bits_per_sample/8;
	header.bits_per_sample = bits_per_sample;
	header.data_length = 0;*/
	////////////////////////////////////////////////////////

    mcp3008Spi a2d("/dev/spidev0.0", SPI_MODE_0, 16000000, 8);
    int i ;
  struct timeval tStart, tEnd, tTaken ;
  unsigned char buffer [B_SIZE] ;
	int a2dVal = 0;
    int a2dChannel = 0;
	unsigned char header[] = {0x52, 0x49, 0x46, 0x46, 0x24, 0x35, 0x0c, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6d, 0x74, 
		0x20, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x80, 0x3e, 0x00, 0x00, 0x00, 0x7d, 0x00, 0x00,
			    0x02, 0x00, 0x08, 0x00, 0x64, 0x61, 0x74, 0x61, 0x00, 0x35, 0x0c, 0x00};


    unsigned char data[2];
		data[0] = 0;
        data[1] = 0; // third byte transmitted....don't care
        gettimeofday (&tStart, NULL) ;

  for (i = 0 ; i < B_SIZE ; ++i)
  {  
        a2d.spiWriteRead(data, sizeof(data) );
        a2dVal = (data[0]<< 8) & 0xf00; //merge data[1] & data[2] to get result
        a2dVal |=  (data[1] & 0xff)-0x350;
        buffer [i] = data[1]-0x90;//a2dVal;
  }
  gettimeofday (&tEnd, NULL) ;
  cout<<"Time taken for %d  reads: "<<B_SIZE<<tTaken.tv_sec<<tTaken.tv_usec<<endl;
        cout << "The Result is: " << a2dVal << endl;
		for (i=0; i<100; ++i)
		{
			a2dVal = buffer[i];
	  cout<<"data----------------      "<<a2dVal<<endl<<endl;
		}
   //std::fstream f_in;	 
   ofstream outfile;
   //fprintf(outfile, "%d\n", val);
   outfile.open("adifile.wav",std::fstream::in | std::fstream::out);//| std::fstream::app);
   // write inputted data into the file.
    //outfile << header << endl;
   outfile.write((char*)header, sizeof(header));//B_SIZE);
   outfile.write((char*)buffer, B_SIZE);
   // close the opened file.
   outfile.close();
    return 0;
}
