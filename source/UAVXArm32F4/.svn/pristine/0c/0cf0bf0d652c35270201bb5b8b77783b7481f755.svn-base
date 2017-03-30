/* 
 Extended Kalman Filter class by Sam Tabor, 2013.
 http://diydrones.com/forum/topics/autonomous-soaring
 Set up for identifying thermals of Gaussian form, but could be adapted to other
 purposes by adapting the equations for the jacobians.
 */
#ifndef ExtendedKalmanFilter_h
#define ExtendedKalmanFilter_h

real32 EKFmeasurementpredandjacobian(real32* a);

struct {
	real32 X[4];
	real32 P[4][4];
	real32 Q[4][4];
	real32 R[1][1];
} ekf;

void EKFreset(real32 X[4], real32 P[4][4], real32 Q[4][4], real32 R[1][1]);
void EKFupdate(real32 z, real32 Vx, real32 Vy);

#endif
