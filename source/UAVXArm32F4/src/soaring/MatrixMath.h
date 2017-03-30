/*
 *  MatrixMath.h Library for Matrix Math
 *
 *  Created by Charlie Matlack on 12/18/10.
 *  Modified from code by RobH45345 on Arduino Forums, taken from unknown source.
 */

#ifndef MatrixMath_h
#define MatrixMath_h

//void MatrixPrint(real32* A, int m, int n, char[] label);
void MatrixCopy(real32* A, int16 n, int16 m, real32* B);
void MatrixMult(real32* A, real32* B, int16 m, int16 p, int16 n, real32* C);
void MatrixMultTranspose(real32* A, real32* B, int16 m, int16 p, int16 n,
		real32* C);
void MatrixAdd(real32* A, real32* B, int16 m, int16 n, real32* C);
void MatrixSubtract(real32* A, real32* B, int16 m, int16 n, real32* C);
void MatrixTranspose(real32* A, int16 m, int16 n, real32* C);
void MatrixMultScalar(real32* A, real32 s, int16 m, int16 n, real32* C);
int16 MatrixInvert(real32* A, int16 n);
void MatrixForceSymmetry(real32* A, int16 n);

#endif
