#include "UAVX.h"

#include "soaring/MatrixMath.h"
#include "soaring/ExtendedKalmanFilter.h"

real32 EKFmeasurementpredandjacobian(real32* A) {
	//This function computes the jacobian using equations from
	//analytical derivation of Gaussian updraft distribution
	//This expression gets used lots
	real32 expon = exp(-(pow(ekf.X[2], 2) + pow(ekf.X[3], 2))
			/ pow(ekf.X[1], 2));
	//Expected measurement
	real32 w = ekf.X[0] * expon;

	//Elements of the Jacobian
	A[0] = expon;
	A[1] = 2.0f * ekf.X[0] * ((pow(ekf.X[2], 2) + pow(ekf.X[3], 2)) / pow(
			ekf.X[1], 3)) * expon;
	A[2] = -2.0f * (ekf.X[0] * ekf.X[2] / pow(ekf.X[1], 2)) * expon;
	//A[3]=-2*(ekf.X[0]*ekf.X[3]/pow(ekf.X[1],2))*expon;
	A[3] = A[2] * ekf.X[3] / ekf.X[2];

	return w;

} // EKFmeasurementpredandjacobian

void EKFreset(real32 x[4], real32 p[4][4], real32 q[4][4], real32 r[1][1]) {
	MatrixCopy((real32*) p, 4, 4, (real32*) ekf.P);
	MatrixCopy((real32*) x, 4, 1, (real32*) ekf.X);
	MatrixCopy((real32*) q, 4, 4, (real32*) ekf.Q); //
	MatrixCopy((real32*) r, 1, 1, (real32*) ekf.R); //
} // EKFreset

void EKFupdate(real32 z, real32 Vx, real32 Vy) {
	real32 temp1[4][4];
	real32 H[1][4];
	real32 P12[4][1];
	real32 K[4][1];

	ekf.X[2] -= Vx;
	ekf.X[3] -= Vy;

	MatrixAdd((real32*) ekf.P, (real32*) ekf.Q, 4, 4, (real32*) ekf.P);

	real32 z1 = EKFmeasurementpredandjacobian((real32*) H);

	MatrixMultTranspose((real32*) ekf.P, (real32*) H, 4, 4, 1, (real32*) P12); //cross covariance

	MatrixMult((real32*) H, (real32*) P12, 1, 4, 1, (real32*) temp1);
	real32 temp = 1.0 / (temp1[0][0] + ekf.R[0][0]);
	MatrixMultScalar((real32*) P12, temp, 4, 1, (real32*) K);

	real32 residual = z - z1;
	MatrixMultScalar((real32*) K, residual, 4, 1, (real32*) temp1);
	MatrixAdd((real32*) temp1, (real32*) ekf.X, 4, 1, (real32*) ekf.X);

	MatrixMultTranspose((real32*) K, (real32*) P12, 4, 1, 4, (real32*) temp1);
	MatrixSubtract((real32*) ekf.P, (real32*) temp1, 4, 4, (real32*) ekf.P);

	MatrixForceSymmetry((real32*) ekf.P, 4);
} // EKFupdate

