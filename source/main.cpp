//the main
#include "filter.h"
#include "kinematics.h"
#include <time.h>

void map(float *pos, float qx, float qy, float qz, float arm) {
	pos[0] = pos[0] + (qx * arm);
	pos[1] = pos[1] + (qy * arm);
	pos[2] = pos[2] + (qz * arm);
}

int main(int argc, char** argv)
{
	const int dim = 3, timeInt = 24000;
	float scaleA = 1.0, scaleV = 1.0, scaleS = 1.0;
	//for storing the accelerometer data and the position vector data
	float *accelIn = (float*)malloc(dim * sizeof(float));
	float *position = (float*)malloc(dim * sizeof(float));
	float armLength = 2.0; //stores the  armlength of the person using the program
	
	// We catch any exceptions that might occur below -- see the catch statement for more details.
	try {

		// First, we create a Hub with our application identifier.
		myo::Hub hub("com.myohack.myopad");

		std::cout << "Attempting to find a Myo..." << std::endl;

		// Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
		// immediately.
		// waitForAnyMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
		// if that fails, the function will return a null pointer.
		myo::Myo* myo = hub.waitForMyo(10000);

		// If waitForAnyMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
		if (!myo) {
			throw std::runtime_error("Unable to find a Myo!");
		}

		// We've found a Myo.
		std::cout << "Connected to a Myo armband!" << std::endl << std::endl;

		// Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
		Filter collector;
		Kinematic accToPos(dim, scaleA, scaleV, scaleS); //adds the integral class 


		// Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
		// Hub::run() to send events to all registered device listeners.
		hub.addListener(&collector);
		int currentTime = clock();
		// loop keeps running and mapping the coordinates on the window
		while (true) {
			//gets 48 packets of data every second
			hub.run(1000 / 24);
			
			//store the accelerometer data in an array
			accelIn[0] = collector.accelX;
			accelIn[1] = collector.accelY;
			accelIn[2] = collector.accelZ;

			//integrate the data
			position = accToPos.update(accelIn, position);

			//send the value to map
			map(position, collector.quatX, collector.quatY, collector.quatZ, armLength);

			//print the position
			for (int i = 0; i < dim, i++)
				std::cout << position[i] << " ";
		}

		// If a standard exception occurred, we print out its message and exit.
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		std::cerr << "Press enter to continue.";
		std::cin.ignore();
		return 1;
	}
};