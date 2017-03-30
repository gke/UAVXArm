/* 
 Thermalling Controller by Samuel Tabor
 Provides a layer between the thermal centering algorithm and the main code for
 managing navigation targets, data logging, tuning parameters, algorithm inputs
 and eventually other soaring strategies such as speed-to-fly.
 */
#ifndef ThermallingController_h
#define ThermallingController_h

// Thermal

#define EXP_THERMAL_SINK_MPS -0.7f
#define INIT_THERMAL_STRENGTH 2.0f
#define INIT_THERMAL_RADIUS_M 150.0f

#define THERMAL_MIN_MPS (-EXP_THERMAL_SINK_MPS*2.0f)
#define THERMAL_DIST_AHEAD_M 5.0f
#define THERMAL_MIN_MS 5000L // 20000L
#define CRUISE_MIN_MS 30000L
#define ALT_MAX_M 250.0f // 350.0f
#define ALT_MIN_M 50.0f
#define ALT_CUTOFF_M (ALT_MIN_M+(ALT_MAX_M-ALT_MIN_M)*0.2f)

typedef struct {
	real32 x, y, z;
} Vector3f;

// Thermalling log structure
struct {
	uint32 mS;
	real32 vario;
	real32 dx;
	real32 dy;
	real32 x0;
	real32 x1;
	real32 x2;
	real32 x3;
	uint32 lat;
	uint32 lon;
	real32 alt;
	real32 dx_w;
	real32 dy_w;
} SoaringTune;

boolean ThrottleSuppressed;

real32 AltCutoffM, AltMaxM, AltMinM;

WPStruct TH;

void UpdateVario(void);

void InitThermalling(void);
boolean SuppressThrottle(void);
boolean CommenceThermalling(void);
boolean ResumeGlide(void);
void UpdateThermalEstimate(void);
void DoGlider(void);

#endif
