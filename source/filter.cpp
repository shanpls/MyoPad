#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <algorithm>
#include "filter.h"
// The only file that needs to be included to use the Myo C++ SDK is myo.hpp.
#include <myo/myo.hpp>

	Filter::Filter()
		: onArm(false), currentPose(), quatX(0), quatY(0), quatZ(0), quatW(0)
	{
		accelX = 0.0;
		accelY = 0.0;
		accelZ = 0.0;

		float roll_i = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch_i = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw_i = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
	}

	// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
	void Filter::onUnpair(myo::Myo* myo, uint64_t timestamp)
	{
		onArm = false;
	}

	// onOrientationData() is called whenever the Myo device provides its current orientation, which is represented
	// as a unit quaternion.
	void Filter::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
	{
		quatX = quat.x(); //Return the x-component of this quaternion's vector
		quatY = quat.y(); //Return the y -component of this quaternion's vector
		quatZ = quat.z(); //Return the z-component of this quaternion's vector
		quatW = quat.w(); //Return the w-component (scalar) of this quaternion's vector
	}

	void Filter::onAccelerometerData(myo::Myo *myo, uint64_t timestamp, const myo::Vector3< float > &accel, const myo::Quaternion<float>& quat){
		accelX_old = accelX;
		accelY_old = accelY;
		accelZ_old = accelZ;

		using std::atan2;
		using std::asin;
		using std::sqrt;
		using std::cos;
		using std::sin;
		using std::max;
		using std::min;

		//shift the coordinate system in order to cancel out gravity in the right axis
		float roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
			1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
		float pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
		float yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
			1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));

		roll = roll - roll_i;
		pitch = pitch - pitch_i;
		yaw = yaw - yaw_i;

		accelX = accel.y();
		accelY = accel.z();
		accelZ = accel.x();

		// pitch = alpha
		// yaw = beta
		// roll = gamma

		accelX *= cos(yaw) * cos(roll) + cos(yaw) * sin(roll) - sin(yaw);
		accelY *= cos(roll) * sin(pitch) * sin(yaw) - cos(pitch) * sin(roll) + cos(pitch) * cos(roll) + sin(pitch) * sin(yaw) * sin(roll) + cos(yaw) * sin(pitch);
		accelZ *= cos(pitch) * cos(roll) * sin(yaw) + sin(pitch) * sin(roll) - cos(roll) * sin(pitch) + cos(pitch) * sin(yaw) * sin(roll) + cos(pitch) * cos(yaw);



	}
	
	// We define this function to print the current values that were updated by the on...() functions above.