// https://learn.sparkfun.com/tutorials/raspberry-pi-spi-and-i2c-tutorial/all

#include <iostream>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <errno.h>

// Connecting to CE0
static const int CHANNEL = 0;

void WritePot(short input)
{
	unsigned char transBytes[2] = 
	{
		input >> 8,
		input & 0xFF
	};
	wiringPiSPIDataRW(CHANNEL, transBytes, 2);
}

int main()
{
   std::cout << "Initializing" << std::endl ;

   // Configure the interface.
   // CHANNEL insicates chip select,
   // 500000 indicates bus speed.
   int fd = wiringPiSPISetup(CHANNEL, 500000);

   std::cout << "about to enter loop" << std::endl;

   while(true)
   {
	   std::cout << "Going up" << std::endl;
	   for(unsigned short i = 0x00; i < 0x1FF; ++i)
	   {
		   WritePot(i);
		   usleep(50000);
	   }
	   
	   std::cout << "Going down" << std::endl;
	   for(unsigned short i =  0x1FF; i > 0x00; --i)
	   {
		   WritePot(i);
		   usleep(50000);
	   }
   }
}