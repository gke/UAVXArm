// ===============================================================================================
// =                                UAVX Quadrocopter Controller                                 =
// =                           Copyright (c) 2008 by Prof. Greg Egan                             =
// =                 Original V3.15 Copyright (c) 2007 Ing. Wolfgang Mahringer                   =
// =                     http://code.google.com/p/uavp-mods/ http://uavp.ch                      =
// ===============================================================================================

//    This is part of UAVX.

//    UAVX is free software: you can redistribute it and/or modify it under the terms of the GNU 
//    General Public License as published by the Free Software Foundation, either version 3 of the 
//    License, or (at your option) any later version.

//    UAVX is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without
//    even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
//    See the GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along with this program.  
//    If not, see http://www.gnu.org/licenses/

#include "UAVX.h"

Flags F;
uint8 State;
uint32 CurrPIDCycleuS = PID_CYCLE_US;
volatile uint32 uS[uSLastArrayEntry];
volatile uint32 mS[mSLastArrayEntry];

uint8 ch;
int8 i, m;

void InitMisc(void) {
	uint8 i;

	State = Preflight;

	for (i = 0; i < FLAG_BYTES; i++)
		F.AllFlags[i] = false;

	for (i = 0; i < mSLastArrayEntry; i++)
		mS[i] = 0;

	for (i = 0; i < uSLastArrayEntry; i++)
		uS[i] = 0;

	ThrNeutral = ThrLow = ThrHigh = RC_MAXIMUM;
	IdleThrottle = 0.1f;
	InitialThrottle = RC_MAXIMUM;

} // InitMisc

void CalculatedT(uint32 NowuS) {

	dT = dTUpdate(NowuS, &LastInertialUpdateuS);
	dTOn2 = 0.5f * dT;
	dTR = 1.0f / dT;
	dTROn2 = dTR * 0.5f;

} // CalculatedT

int main() {
	uint32 NowuS;
	uint32 NowmS;
	static uint32 LastUpdateuS = 0;

	CheckParametersInitialised();

	InitClocks();

	InitHarness();

	InitMisc();
	InitAnalog();
	InitLEDs();

	if (sizeof(NV) >= NV_FLASH_SIZE)
		Catastrophe();

	InitExtMem();

#if defined(COMMISSIONING_TEST)

	ReadBlockNV(0, sizeof(NV), (int8 *) (&NV));

	NV.CurrPS = 0;
	for (i = 0; i < MAX_PARAMETERS; i++)
	SetP(DefaultParams[i].tag, DefaultParams[i].p[0]);

	CommissioningTest(0);

#else

#if defined(V4_BOARD)
	for (i = 0; i < 4; i++)
	spiSelect(i, false); // TODO do it again but why is this being changed?
#endif

	InitParameters();

	if ((P(Config2Bits) & UseBLHeliMask) != 0)
		DoBLHeliSuite(TelemetrySerial);
	else
		Delay1mS(1000); // let things settle!

	InitIMU(); // connects pass through for mag
	InitMagnetometer();
	InitMadgwick();

	InitBarometer();

	InitControl();

	InitNavigation();
	InitTemperature();

	InitPollRxPacket();

	if (CurrComboPort1Config != ParallelPPM)
		RxEnabled[RCSerial] = true;

	FirstPass = true;
	mSTimer(mSClock(), LastBattery, 0);
	uSTimer(uSClock(), NextCycleUpdate, CurrPIDCycleuS);

	State = Preflight;

	while (true) {

		if ((UAVXAirframe == Instrumentation) || (UAVXAirframe == IREmulation)) {
			// Force everything!
#if defined(V4_BOARD)
			CurrTelType = FrSkyTelemetry;
			ArmingMethod = SwitchArming;
#endif
			F.Signal = true;
			StickThrottle = 0.0f;
			RCStart = 0;
			//F.ThrottleOpen = F.Navigate = F.ReturnHome = false;
		} else {
			CheckRxLoopback();
			UpdateControls(); // avoid loop sync delay
		}

		NowuS = uSClock();
		if (NowuS >= uS[NextCycleUpdate]) {

			//Probe(1);

			//---------------
			CalculatedT(NowuS);
			UpdateInertial();
			//---------------

			uSTimer(NowuS, NextCycleUpdate, CurrPIDCycleuS);

			uSTimer(NowuS, LastCycleTime, -LastUpdateuS);
			LastUpdateuS = NowuS;

			// Housekeeping here


			switch (State) {
			case Preflight:

				F.HoldingAlt = F.IsArmed = false;
				DesiredThrottle = 0.0;
				ZeroCompensation();

				StopDrives();

				if (FailPreflight()) {
					LEDOn(LEDRedSel);
					LEDOff(LEDGreenSel);

				} else {

					LEDOff(LEDRedSel);
					LEDOn(LEDGreenSel);

					DoBeep(8, 2);

					FirstPass = F.OriginValid = F.NavigationActive
							= F.LostModel = false;

					mSTimer(mSClock(), FailsafeTimeout, FAILSAFE_TIMEOUT_MS);
					FailState = Monitoring;

					InitialThrottle = StickThrottle;

					State = Ready;
				}

				break;
			case Ready:
				if (Armed()) {
					LEDOn(LEDYellowSel);
					RxLoopbackEnabled = false;
					State = Starting;
				} else
					DoStickProgramming();

				break;
			case Starting:

				DoBeep(8, 2);

				if (!((CurrComboPort1Config == CPPM_GPS_M7to10)
						|| (CurrComboPort2Config == GPS_RF_V4))) // using Telemetry port
					InitGPS();

				DoBeep(8, 2);
				InitBlackBox();

				InitControl();
				CheckInitEmulation();
				InitNavigation();

				if (F.UsingAnalogGyros || !UsingFastStart) // most do as we don't have temperature drift compensation
					ErectGyros(5);

				ZeroStats();

				F.IsArmed = true;

				mSTimer(mSClock(), WarmupTimeout, WARMUP_TIMEOUT_MS);

				State = Warmup;

				break;
			case Warmup:

				BatteryCurrentADCZero
						= SoftFilter(BatteryCurrentADCZero, analogRead(BattCurrentAnalogSel));

				if (mSClock() > mS[WarmupTimeout]) {
					UbxSaveConfig(GPSTxSerial); //does this save ephemeris stuff?

					DoBeeps(3);
					DoBeep(8, 2);

					NowmS = mSClock();
					mSTimer(NowmS, ArmedTimeout, ARMED_TIMEOUT_MS);
					mSTimer(NowmS, RxFailsafeTimeout, RC_NO_CHANGE_TIMEOUT_MS);
					F.SticksUnchangedFailsafe = F.LostModel = false;
					LEDOff(LEDYellowSel);

					State = (UAVXAirframe == IREmulation) ? IREmulate : Landed;
				}
				break;
			case Landed:
				ZeroCompensation();

				if (Armed()) {
					F.DrivesArmed = CurrESCType == DCMotorsWithIdle;
					DesiredThrottle
							= CurrESCType == F.DrivesArmed ? IdleThrottle
									: 0.0f;

					CheckInitEmulation();

					ZeroPIDIntegrals();
					DesiredHeading = Nav.TakeoffBearing = Nav.DesiredHeading
							= Heading;

					DoStickProgramming();
					F.HoldingAlt = false;

					if (UAVXAirframe == Instrumentation) {
						SetGPSOrigin();
						if (F.OriginValid) { // for now only works with GPS
							LEDsOff();
							UbxSaveConfig(GPSTxSerial);
							State = InFlight;
						}
					} else {
						NowmS = mSClock();
						if (NowmS > mS[ArmedTimeout])
							DoShutdown();
						else {
							if (StickThrottle < IdleThrottle) {

								// disabled for perching uncomment for reset every landing
								// F.OriginValid = false;
								SetGPSOrigin(); // only when throttle definitely closed

								if (F.NewCommands)
									F.LostModel = F.SticksUnchangedFailsafe;
							} else {

								mSTimer(NowmS, NavActiveTime,
										NAV_ACTIVE_DELAY_MS);
								mSTimer(NowmS, RxFailsafeTimeout,
										RC_NO_CHANGE_TIMEOUT_MS);
								NV.Stats[RCGlitchesS] = 0;
								F.SticksUnchangedFailsafe = F.LostModel = false;
								F.DrivesArmed = true;
								LEDsOff();
								State = InFlight;
							}
						}
					}
				} else
					// became disarmed mid state change
					State = Preflight;

				break;
			case Landing:
				if (StickThrottle > IdleThrottle) {
					mS[StartTime] = mSClock();
					DesiredThrottle = 0.0f;
					F.DrivesArmed = true;
					State = InFlight;
				} else {
					if (mSClock() < mS[ThrottleIdleTimeout])
						DesiredThrottle = IdleThrottle;
					else {
						ResetNavHold = true;
						AltComp = 0.0f; // to catch cycles between Rx updates
						F.DrivesArmed = CurrESCType == DCMotorsWithIdle;
						DesiredThrottle = F.DrivesArmed ? IdleThrottle : 0.0f;
						if (Tuning)
							SetP(TuningParamIndex, OldUntunedParam);
						UpdateNV(); // also captures stick programming
						mSTimer(mSClock(), ArmedTimeout, ARMED_TIMEOUT_MS);
						LEDOn(LEDGreenSel);
						State = Landed;
					}
				}
				break;
			case Shutdown:
				if ((StickThrottle < IdleThrottle) && !(F.ReturnHome
						|| F.Navigate)) {
					uint32 NowmS = mSClock();
					mSTimer(NowmS, ArmedTimeout, ARMED_TIMEOUT_MS);
					mSTimer(NowmS, RxFailsafeTimeout, RC_NO_CHANGE_TIMEOUT_MS);
					F.SticksUnchangedFailsafe = F.LostModel = false;
					DoBeeps(3);
					F.NavigationActive = false;
					NavState = PIC;
					LEDsOff();
					LEDOn(LEDGreenSel);
					FailState = Monitoring;
					State = Landed;
				} else {
					LEDsOn();
					DesiredThrottle = 0.0f;
					ZeroCompensation();
					StopDrives();
				}
				break;
			case InFlight:

				LEDChaser();

				if (UAVXAirframe == Instrumentation) {

					F.NavigationEnabled = true;
					DoNavigation();

					// stay in inflight mode until power cycle or disarm
					if (!Armed())
						State = Preflight;
				} else {

					DoNavigation();

					CheckCrashed(); // roll/pitch > 90deg for more than a second! => kill motors

					if ((State != Shutdown) && (StickThrottle < IdleThrottle)
							&& (IsMulticopter
									|| ((ArmingMethod == SwitchArming)
											&& !Armed()))) {
						ZeroCompensation();
						mSTimer(mSClock(), ThrottleIdleTimeout,
								THR_LOW_DELAY_MS);
						State = Landing;
					} else { // fixed wing so no exit when using roll/yaw stick arming

						DFT8(RawAcc[X], DFT); // 145uS
						UpdateMagHist(); // ~1uS?
						DoAltitudeControl();

					}
				}
				break;
			case IREmulate:
				if (!Armed())
					State = Preflight;
				break;
			} // switch state

			F.GPSPosUpdated = false;

			NV.Stats[UtilisationS] = State == InFlight ? ((uSClock() - NowuS)
					* 100.0f) / CurrPIDCycleuS : 0;

			//Probe(0);
		} // if next cycle

		CheckBatteries();
		CheckAlarms();
		DoCalibrationAlarm();

		CheckTelemetry(TelemetrySerial);

	} // while true

#endif // COMMISSIONING_TEST
	return (0);

} // loop


