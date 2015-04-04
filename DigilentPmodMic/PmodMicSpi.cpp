#include "PmodMicSpi.h"
using namespace std;
/**********************************************************
 * spiOpen() :function is called by the constructor.
 * It is responsible for opening the spidev device
 * "devspi" and then setting up the spidev interface.
 * private member variables are used to configure spidev.
 * They must be set appropriately by constructor before calling
 * this function.
 * *********************************************************/
int pmdodSpi::spiOpen(std::string devspi){
    int statusVal = -1;
    this->spifd = open(devspi.c_str(), O_RDWR);
    if(this->spifd < 0){
        perror("could not open SPI device");
        exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_WR_MODE, &(this->mode));
    if(statusVal < 0){
        perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_RD_MODE, &(this->mode));
    if(statusVal < 0) {
      perror("Could not set SPIMode (RD)...ioctl fail");
      exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_WR_BITS_PER_WORD, &(this->bitsPerWord));
    if(statusVal < 0) {
      perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
      exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_RD_BITS_PER_WORD, &(this->bitsPerWord));
    if(statusVal < 0) {
      perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
      exit(1);
    }  
 
    statusVal = ioctl (this->spifd, SPI_IOC_WR_MAX_SPEED_HZ, &(this->speed));    
    if(statusVal < 0) {
      perror("Could not set SPI speed (WR)...ioctl fail");
      exit(1);
    }
 
    statusVal = ioctl (this->spifd, SPI_IOC_RD_MAX_SPEED_HZ, &(this->speed));    
    if(statusVal < 0) {
      perror("Could not set SPI speed (RD)...ioctl fail");
      exit(1);
    }
    return statusVal;
}
 
/***********************************************************
 * spiClose(): Responsible for closing the spidev interface.
 * Called in destructor
 * *********************************************************/
 
int pmodSpi::spiClose(){
    int statusVal = -1;
    statusVal = close(this->spifd);
        if(statusVal < 0) {
      perror("Could not close SPI device");
      exit(1);
    }
    return statusVal;
}
 
/********************************************************************
 * This function writes data "data" of length "length" to the spidev
 * device. Data shifted in from the spidev device is saved back into
 * "data".
 * ******************************************************************/
int pmodSpi::spiWriteRead( unsigned char *data, int length){
 
  struct spi_ioc_transfer spi;//[length];
  int i = 0;
  int retVal = -1; 
 // bzero(spi, sizeof spi); // ioctl struct must be zeroed 
 memset (&spi, 0, sizeof (spi)) ;
// one spi transfer for each byte
  spi.tx_buf        = (unsigned long)data; // transmit from "data"
  spi.rx_buf        = (unsigned long)data;// receive into "data"
  spi.len           = length ;
  spi.delay_usecs   = 0 ;
  spi.speed_hz      = this->speed ;
  spi.bits_per_word = this->bitsPerWord ;
  spi.cs_change = 0;
 
 retVal = ioctl (this->spifd, SPI_IOC_MESSAGE(1), &spi) ;
 
 if(retVal < 0){
    perror("Problem transmitting spi data..ioctl");
    exit(1);
 }
 
return retVal;
 
}
 
/*************************************************
 * Default constructor. Set member variables to
 * default values and then call spiOpen()
 * ***********************************************/
 
pmodSpi::pmodSpi(){
    this->mode = SPI_MODE_0 ;
    this->bitsPerWord = 8;
    this->speed = 1000000;
    this->spifd = -1;
 
    this->spiOpen(std::string("/dev/spidev0.0"));
 
    }
 
/*************************************************
 * overloaded constructor. let user set member variables to
 * and then call spiOpen()
 * ***********************************************/
pmodSpi::pmodSpi(std::string devspi, unsigned char spiMode, unsigned int spiSpeed, unsigned char spibitsPerWord){
    this->mode = spiMode ;
    this->bitsPerWord = spibitsPerWord;
    this->speed = spiSpeed;
    this->spifd = -1;
 
    this->spiOpen(devspi);
 
}
 
/**********************************************
 * Destructor: calls spiClose()
 * ********************************************/
pmodSpi::~pmodSpi(){
    this->spiClose();
}
