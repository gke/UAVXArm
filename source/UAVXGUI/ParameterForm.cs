// Fragments of the original UAVPSet Copyright (C) 2007  Thorsten Raab
// 
// Adapted for UAVX Copyright (C) 2010 Prof Greg Egan
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Resources;
using System.Configuration;
using System.Reflection;
using System.Globalization;
using System.Threading;
using System.IO;

namespace UAVXGUI
{
    public partial class ParameterForm : Form
    {

        public enum Farbe { black, green, orange, red };

        public struct ParameterSetsStruc
        {
            public int Value;
            public bool Changed;
        }

        private System.Windows.Forms.OpenFileDialog parameterOpenFileDialog = new System.Windows.Forms.OpenFileDialog();
        private System.Windows.Forms.SaveFileDialog parameterSaveFileDialog = new System.Windows.Forms.SaveFileDialog();
        private System.Windows.Forms.SaveFileDialog hexOpenFileDialog = new System.Windows.Forms.SaveFileDialog();

        public static ParameterSetsStruc[,] P = new ParameterSetsStruc[FormMain.MAX_PARAM_SETS, FormMain.MAX_PARAMS];
        public static ParameterSetsStruc[] UAVXP = new ParameterSetsStruc[FormMain.MAX_PARAMS];

        public Cursor cursor = Cursor.Current;
 
        public volatile static bool UpdateParamForm = false;
        public static bool ParamsStale = true;
        public bool writeUpdate = false;
        public string pfad;
        public bool picBootModus = false;

        public static bool[] SenseButton = new bool[8];
        public static byte CurrPS = 0;

        public ResourceManager help;
        public ResourceManager labels;

        public static string helpstring;

        byte[] def = {

        	20,			// RollKpRate, 			01 UAVP 21
			0,	 		// AltPosKi,			02 UAVP 10
			25,			// RollKpAngle,			03 // 25
			2,	        // ArmingMode,				04
			40,	 		// RollIntLimit,		05 UAVP 80

			20,	 		// PitchKpRate,			06 UAVP 21
			20,	 		// AltPosKp,			07 UAVP 10
			25,	 		// PitchKpAngle			08 // 25
			0, 	        // RFUsed,				09 UAVP 1
			40,	 		// PitchIntLimit,		10 UAVP 80

			20,	 		// YawKpRate, 			11 // 40
			45,	 		// RollKdRate,			12
			0,          // IMU,					13
			5,	 		// AltVelKd,		    14 // 12
			0, 	        // RCType,				15 was CompoundPPM

			0,          // ConfigBits,			16c
			1,			// RxThrottleCh,		17
			16, 		// LowVoltThres,		18c
			10, 		// CamRollKp,			19c
			27, 		// PercentCruiseThr,	20c

			2, 		// StickHysteresis,			21c
			0, 			// RollPitchMix,		22c
			10, 		// PercentIdleThr,		23c
			3, 			// RollKiAngle,			24
			3, 			// PitchKiAngle,		25
			10, 		// CamPitchKp,			26c
			8, 			// YawKpAngle(Compass),	27
			45,			// PitchKdRate,			28 UAVP 10
			20, 		// NavVelKp,			29
			20, 		// AltVelKp,			30

			30, 			// Horizon,	    	31
			50,			// MadgwickKpMag,	    32
			15, 		    // NavRTHAlt,			33
			0,			// NavMagVar,			34c
			4,  	    // SensorHint,     		35c
			7, 		    // ESCType,				36c
			7, 			// UnusedRxChannels,			37c
			2,			// RxRollCh,			38
			20,			// MadgwickKpAcc,		39c
			1,			// CamRollTrim,			40c

			3,			// NavMaxVelMPS,		41
			3,			// RxPitchCh,			42
			4,			// RxYawCh,				43
			4,	        // AFType,				44c
			1,          // TelemetryType,	45c
			10,		    // MaxDescentRateDmpS, 	46
			15,			// DescentDelayS,		47
			2, 	        // GyroLPF,		        48 UAVP MPU_RA_DLPF_BW_98
			4,			// NavCrossTrackKp,		49
			5,			// RxGearCh,			50c

			6,			// RxAux1Ch,			51
			0,			// ServoSense			52c
			4,			// AccConfSD,			53c
			3,			// BatteryCapacity,		54c
			7,			// RxAux2Ch,			55c
			8,			// RxAux3Ch,			56
			20, 	    // NavPosKp,			57

			20,			// AltLPF,				58
			50,			// Balance,				59
			9,			// RxAux4Ch,			60
			5,			// NavVelIntLimit,		61

			1,	        // GPSProtocol,			62
			25,			// AltThrottleFF,	    63
			10,			// StickScaleYaw,		64
            //TODO:
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        };

        public ParameterForm()
        {
            int s, p;

            InitializeComponent();

            help = new ResourceManager("UAVXGUI.Resources.hilfe", this.GetType().Assembly);
            labels = new ResourceManager("UAVXGUI.Resources.hilfe", this.GetType().Assembly);

            for (s = 0; s < FormMain.MAX_PARAM_SETS; s++)
                for (p = 0; p < FormMain.MAX_PARAMS; p++)
                {
                    P[s, p].Value = def[p];
                    P[s, p].Changed = true;
                }
            for (p = 0; p < FormMain.MAX_PARAMS; p++)
            {
                UAVXP[p].Value = P[0, p].Value;
                UAVXP[p].Changed = P[0, p].Changed;
            }

            CurrPS = 0;
            ReadParamsButton.BackColor = System.Drawing.Color.Orange;
            WriteParamsButton.BackColor = System.Drawing.Color.Green;
            WriteParamsButton.Visible = false;
            ParamsStale = true;
            UpdateParamForm = false;
            FormMain.SendRequestPacket(FormMain.UAVXParamPacketTag, 254, 0);

            updateForm();
        }

        int Limit(int v, int Min, int Max)
        {
            if (v < Min) return (Min); else if (v > Max) return Max; else return v;
        } // Limit

         private void CheckDownLinkTimer_Tick(object sender, EventArgs e)
        {

            if (UpdateParamForm)
            {
                UpdateParamForm = false;
                ParamSetNumericUpDown.Value = CurrPS;
                ReadParamsButton.BackColor = System.Drawing.Color.Green;
                WriteParamsButton.Visible = true;
                ParamsStale= false;
                updateForm();
            }

      

            // CPPM, Deltang1024, Spek1024, Spek2048, FutabaSBus
            // ParallelPPM, DM9GKE, Deltang1024GKE, Spek1024GKE, Spek2048GKE

            RxChannelsNumericUpDown.Visible = ComboPort1ComboBox.SelectedIndex == 5;
            RefreshRxChannels();
            UpdateRCChannels();

            RxLoopbackButton.BackColor = FormMain.RxLoopbackEnabled ?
   Color.Orange : RCGroupBox.BackColor;

  
        }

         private void RxLoopbackButton_Click(object sender, EventArgs e)
         {
             if (((FormMain.StateT == FormMain.FlightStates.Preflight) || (FormMain.StateT == FormMain.FlightStates.Ready)))

                 FormMain.SendRequestPacket(FormMain.UAVXMiscPacketTag, (byte)FormMain.MiscComms.miscLB, 0);

         }

        private void bitCheckBox_CheckedChanged(object sender, EventArgs e)
        {
           //bitTextChange(sender);
        }

        private void bitTextChange(Object changeBoxObject)
        {
            CheckBox changeBox = (CheckBox)changeBoxObject;
            if (changeBox.Checked)
                changeBox.Text = labels.GetString(changeBox.Name.Substring(0, 4) + "1"); 
            else
                changeBox.Text = labels.GetString(changeBox.Name.Substring(0, 4)); 
        }


        public void info(ParameterForm parameterForm)
        {

            // Roll     
            if (parameterForm.RollRatePropNumericUpDown.Focused)
                helpstring = help.GetString("Proportional");
            if (parameterForm.RollRateDiffNumericUpDown.Focused)
                helpstring = help.GetString("Differential");
            if (parameterForm.RollAnglePropNumericUpDown.Focused)
                helpstring = help.GetString("ProportionalAngle");
            if (parameterForm.RollAngleIntNumericUpDown.Focused)
                helpstring = help.GetString("ProportionalAngle");
            if (parameterForm.AltVelKdNumericUpDown.Focused)
                helpstring = help.GetString("AltDifferential");
            if (parameterForm.RollIntLimitNumericUpDown.Focused)
                helpstring = help.GetString("IntegralLimiter");

            // Pitch
            if (parameterForm.PitchRatePropNumericUpDown.Focused)
                helpstring = help.GetString("Proportional");
            if (parameterForm.PitchRateDiffNumericUpDown.Focused)
                helpstring = help.GetString("Differential");
            if (parameterForm.PitchAnglePropNumericUpDown.Focused)
                helpstring = help.GetString("ProportionalAngle");
            if (parameterForm.PitchAngleIntNumericUpDown.Focused)
                helpstring = help.GetString("ProportionalAngle");
            if (parameterForm.AltLPFNumericUpDown.Focused)
                helpstring = help.GetString("DifferentialFC");
            if (parameterForm.PitchIntLimitNumericUpDown.Focused)
                helpstring = help.GetString("IntegralLimiter");

            if (parameterForm.BalanceNumericUpDown.Focused)
                helpstring = help.GetString("Balance");

            // Yaw
            if (parameterForm.YawRatePropNumericUpDown.Focused)
                helpstring = help.GetString("Proportional");
            if (parameterForm.CrossTrackNumericUpDown.Focused)
                helpstring = help.GetString("CrossTrack");

            if (parameterForm.YawScaleNumericUpDown.Focused)
                helpstring = help.GetString("YawScale");
            if (parameterForm.RollPitchScaleNumericUpDown.Focused)
                helpstring = help.GetString("RollPitchScale");
            if (parameterForm.AltPosKpNumericUpDown.Focused)
                helpstring = help.GetString("Limiter");
            //   if (parameterForm.RxTypeComboBox.Focused || YawIntLimit2NumericUpDown.Focused)
            //       helpstring = help.GetString("IntegralLimiter");


            // General

            if (parameterForm.AttThrFFNumericUpDown.Focused)
                helpstring = help.GetString("AttThrFF");
            if (parameterForm.FWRollPitchFFNumericUpDown.Focused)
                helpstring = help.GetString("FWRollPitchFF");
            if (parameterForm.FWPitchThrottleFFNumericUpDown.Focused)
                helpstring = help.GetString("FWPitchThrottleFF");
            if (parameterForm.MaxAttitudeAngleNumericUpDown.Focused)
                helpstring = help.GetString("MaxPitchAngle");
            if (parameterForm.FWMaxClimbAngleNumericUpDown.Focused)
                helpstring = help.GetString("MaxRollAngle");
            if (parameterForm.BestROCNumericUpDown.Focused)
                helpstring = help.GetString("BestROC");
            if (parameterForm.MaxAltCompNumericUpDown.Focused)
                helpstring = help.GetString("MaxAltComp");
            if (parameterForm.FWAileronDifferentialNumericUpDown.Focused)
                helpstring = help.GetString("FWDifferential");
            if (parameterForm.AirspeedComboBox.Focused)
                helpstring = help.GetString("AirspeedSensor");

           if (parameterForm.FWGlideAngleOffsetLabel.Focused)
                helpstring = help.GetString("FWGlideAngleOffset");
            if (parameterForm.FWFlapDecayTimeNumericUpDown.Focused)
                helpstring = help.GetString("FWFlapDecayTime");
            if (parameterForm.FWAltHoldDecayTimeNumericUpDown.Focused)
                helpstring = help.GetString("FWAltHoldDecayTime");



            if (parameterForm.bit01CheckBox.Focused)
                helpstring = help.GetString("AuxMode");
            if (parameterForm.bit61CheckBox.Focused)
                helpstring = help.GetString("ControlScheme");
            if (parameterForm.bit21CheckBox.Focused)
                helpstring = help.GetString("LEDs");
            if (parameterForm.bit31CheckBox.Focused)
                helpstring = help.GetString("Emulation");
            if (parameterForm.bit41CheckBox.Focused)
                helpstring = help.GetString("WPNavigation");
            if (parameterForm.bit51CheckBox.Focused)
                helpstring = help.GetString("Failsafes");
            if (parameterForm.bit11CheckBox.Focused)
                helpstring = help.GetString("RTHDescend");

            if (parameterForm.bit02CheckBox.Focused)
                helpstring = help.GetString("ManualAltHold");
            if (parameterForm.bit12CheckBox.Focused)
                helpstring = help.GetString("FastStart");
            if (parameterForm.bit22CheckBox.Focused)
                helpstring = help.GetString("BLHeli");
            if (parameterForm.bit32CheckBox.Focused)
                helpstring = help.GetString("Glider");
            if (parameterForm.bit42CheckBox.Focused)
                helpstring = help.GetString("RebootConfig");
            if (parameterForm.bit52CheckBox.Focused)
                helpstring = help.GetString("unassigned");
            if (parameterForm.bit62CheckBox.Focused)
                helpstring = help.GetString("unassigned");

            if (parameterForm.RxLoopbackButton.Focused)
                helpstring = help.GetString("RxLoopBack");

            if (parameterForm.CruiseThrNumericUpDown.Focused)
                helpstring = help.GetString("AircraftMass");
            if (parameterForm.NavPosKpNumericUpDown.Focused)
                helpstring = help.GetString("NavPosition");
            if (parameterForm.NavVelKpNumericUpDown.Focused)
                helpstring = help.GetString("NavVelocity");
            if (parameterForm.InertialSchemeComboBox.Focused)
                helpstring = help.GetString("InertialScheme");

            if (parameterForm.AFTypeComboBox.Focused)
                helpstring = help.GetString("AFType");
            if (parameterForm.LowMotorRunNumericUpDown.Focused)
                helpstring = help.GetString("LowMotorRun");
            if (parameterForm.CameraRollNumericUpDown.Focused)
                helpstring = help.GetString("CameraGain");
            if (parameterForm.CameraRollTrimNumericUpDown.Focused)
                helpstring = help.GetString("CameraRollTrim");
            if (parameterForm.YawAnglePropNumericUpDown.Focused)
                helpstring = help.GetString("Compass");
            if (parameterForm.AccConfNumericUpDown.Focused)
                helpstring = help.GetString("AccGyroComp");
            if (parameterForm.BatteryNumericUpDown.Focused)
                helpstring = help.GetString("Unterspannung");
            if (parameterForm.AltVelKpNumericUpDown.Focused)
                helpstring = help.GetString("AltProp");


            if (parameterForm.NavPosKpNumericUpDown.Focused)
                helpstring = help.GetString("Fence");

            if (parameterForm.BatteryCapacityNumericUpDown.Focused)
                helpstring = help.GetString("BatteryCapacity");


            if (parameterForm.HorizonNumericUpDown.Focused)
                helpstring = help.GetString("Horizon");

            if (parameterForm.MadgwickKpMagNumericUpDown.Focused)
                helpstring = help.GetString("Acro");

            if (parameterForm.DescDelayNumericUpDown.Focused)
                helpstring = help.GetString("DescentDelay");
            if (parameterForm.DescentRateNumericUpDown.Focused)
                helpstring = help.GetString("DescentRate");
            if (parameterForm.HysteresisNumericUpDown.Focused)
                helpstring = help.GetString("Hysteresis");

            if (parameterForm.GyroLPFComboBox.Focused)
                helpstring = help.GetString("GyroLPF");

            if (parameterForm.MadgwickKpAccNumericUpDown.Focused)
                helpstring = help.GetString("MadgwickKp");

            if (parameterForm.GyroComboBox.Focused)
                helpstring = help.GetString("GyroType");

            if (parameterForm.ESCComboBox.Focused)
                helpstring = help.GetString("ESCType");

            if (parameterForm.TelemetryComboBox.Focused)
                helpstring = help.GetString("TelemetryType");

            if (parameterForm.RxChannelsNumericUpDown.Focused)
                helpstring = help.GetString("RxChannels");

            if (parameterForm.ComboPort1ComboBox.Focused)
                helpstring = help.GetString("RxType");

            if (parameterForm.ArmingModeComboBox.Focused)
                helpstring = help.GetString("ArmingMode");

            //GPS

            if (parameterForm.NavMaxGroundSpeedNumericUpDown.Focused)
                helpstring = help.GetString("NavMaxGroundSpeed");

            if (parameterForm.NavRTHAltNumericUpDown.Focused)
                helpstring = help.GetString("NavRTHAltitude");
            if (parameterForm.NavMagVarNumericUpDown.Focused)
                helpstring = help.GetString("NavMagVar");

            if (parameterForm.NavVelKiNumericUpDown.Focused)
                helpstring = help.GetString("NavWind");

            if (parameterForm.AltPosKiNumericUpDown.Focused)
                helpstring = help.GetString("AltIntegral");

            // return helpstring;

        }


        private void RCuSCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            RCGroupBox.Text = RCuSCheckBox.Checked ? "RC (uS)" : "RC (%)";
        }

        //_____________________________________________________


        private void LoadParamsButton_Click(object sender, EventArgs e)
        {
            int p, s, nps;

            parameterOpenFileDialog.Filter = "Parameters (*.txt)|*.txt";
            parameterOpenFileDialog.InitialDirectory = Properties.Settings.Default.ParamDirectory;

            if (parameterOpenFileDialog.ShowDialog() == DialogResult.OK)
            {
                Properties.Settings.Default.ParamDirectory = parameterOpenFileDialog.InitialDirectory;
                StreamReader sw = new StreamReader(parameterOpenFileDialog.FileName);//, false, Encoding.GetEncoding("windows-1252"));

                nps = Convert.ToInt32(sw.ReadLine());
                for (s = 0; s < nps; s++)
                    for (p = 0; p < nps; p++)
                    {
                        P[s, p].Value = Convert.ToInt32(sw.ReadLine());
                        P[s, p].Changed = true;
                    }

                CurrPS = Convert.ToByte(ParamSetNumericUpDown.Text);

                for (p = 0; p < FormMain.MAX_PARAMS; p++)
                {
                    UAVXP[p].Value = P[CurrPS, p].Value;
                    UAVXP[p].Changed = true;
                }

                UpdateParamForm = true;

                //sw.Flush();
                sw.Close();

                updateForm();
            }
        }

        public void ParamSetNumericUpDown_KeyDown(object sender, EventArgs e)
        {
            CurrPS = Convert.ToByte(ParamSetNumericUpDown.Text);
            ReadParamsButton.BackColor = System.Drawing.Color.Orange;
            WriteParamsButton.Visible = false;
            ParamsStale = true;
        }

        public void ParamUpdate_Click_KeyDown(object sender, EventArgs e)
        {
            ParamUpdate(sender);

            // special cases
            BatteryValueLabel.Text = (Decimal.Round(BatteryNumericUpDown.Value * 0.2m, 1)).ToString(); // 0.2V units
        }

        public void ParamUpdate_KeyDown(object sender, KeyEventArgs e)
        {
            ParamUpdate(sender);

            // special cases
            BatteryValueLabel.Text = (Decimal.Round(BatteryNumericUpDown.Value * 0.2m, 1)).ToString(); // 0.2V units
        }

        private void SaveParamsButton_Click(object sender, EventArgs e)
        {
            int p, s;

             parameterSaveFileDialog.Filter = "Parameters (*.txt)|*.txt";
             parameterSaveFileDialog.InitialDirectory = Properties.Settings.Default.ParamDirectory;

             parameterSaveFileDialog.FileName = FormMain.AFNames[AFTypeComboBox.SelectedIndex] + "_" +
               DateTime.Now.Year + "_" +
               DateTime.Now.Month + "_" +
               DateTime.Now.Day + "_" +
               DateTime.Now.Hour + "_" +
               DateTime.Now.Minute +
               ".txt";

               if (parameterSaveFileDialog.ShowDialog() == DialogResult.OK)
               {
                   Properties.Settings.Default.ParamDirectory = parameterSaveFileDialog.InitialDirectory;
                    StreamWriter sw = new StreamWriter(parameterSaveFileDialog.FileName, false, Encoding.GetEncoding("windows-1252"));

                   sw.WriteLine(FormMain.MAX_PARAM_SETS);
                   for (s = 0; s < FormMain.MAX_PARAM_SETS; s++)
                       for (p = 0; p < FormMain.MAX_PARAMS; p++)
                           sw.WriteLine(P[s, p].Value);

                   sw.Flush();
                   sw.Close();

                   // zzz updateForm();
               }
          
        }

        public void ReadParamsButton_Click(object sender, EventArgs e)
        {
            CurrPS = Convert.ToByte(ParamSetNumericUpDown.Text);
            FormMain.SendRequestPacket(FormMain.UAVXParamPacketTag, CurrPS, 0);
        }

        public void WriteParamsButton_Click(object sender, EventArgs e)
        {
            if (!ParamsStale) {
                CurrPS = Convert.ToByte(ParamSetNumericUpDown.Text);
                FormMain.SendParamsPacket();
            }

            Enabled = true;
            writeUpdate = true;
        }


        public void SetDefaultParamButton_Click(object sender, EventArgs e)
        {
            FormMain.SendRequestPacket(FormMain.UAVXParamPacketTag, 255, 0);
            ParamSetNumericUpDown_KeyDown(sender, e);
        }

        private void infoGetFocus(object sender, EventArgs e)
        {
            info(this);
            infoTextBox.Text = helpstring;
            if (sender.GetType().Name == "NumericUpDown")
            {
                NumericUpDown temp = (NumericUpDown)sender;
                temp.Select(0, 3);
            }
        }

        private void ComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            ParamUpdate(sender);
        }


        private void SenseButton0_Click(object sender, EventArgs e)
        {
            SenseButton[0] = !SenseButton[0];
            infoTextBox.Text = "Right Aileron/Elevon servo sense - not for multicopter use - grey is reversed.";
            ParamUpdate(sender);
        }

        private void SenseButton1_Click(object sender, EventArgs e)
        {
            SenseButton[1] = !SenseButton[1];
            infoTextBox.Text = "Left Aileron/Elevon servo sense - not for multicopter use - grey is reversed.";
            ParamUpdate(sender);
        }

        private void SenseButton2_Click(object sender, EventArgs e)
        {
            SenseButton[2] = !SenseButton[2];
            infoTextBox.Text = "Elevator servo sense - not for multicopter use - grey is reversed.";
            ParamUpdate(sender);
        }

        private void SenseButton3_Click(object sender, EventArgs e)
        {
            SenseButton[3] = !SenseButton[3];
            infoTextBox.Text = "Rudder or Camera roll servo sense - grey is reversed.";
            ParamUpdate(sender);
        }

        private void SenseButton4_Click(object sender, EventArgs e)
        {
            SenseButton[4] = !SenseButton[4];
            infoTextBox.Text = "Right Flap/Spoiler or Camera pitch servo sense - grey is reversed.";
            ParamUpdate(sender);
        }


        private void SenseButton5_Click(object sender, EventArgs e)
        {
            SenseButton[5] = !SenseButton[5];
            infoTextBox.Text = "Left Flap/Spoiler servo sense -  grey is reversed.";
            ParamUpdate(sender);
        }

        private void PropSense1Button_Click(object sender, EventArgs e)
        {
            SenseButton[6] = !SenseButton[6];
            infoTextBox.Text = "Blue for conventional prop orientation - i.e M1 clockwise";
            ParamUpdate(sender);
        }

        private Color ChannelColour(bool s)
        {
            return( s ? System.Drawing.Color.White: System.Drawing.Color.Orange);
        }

        private void RefreshRxChannels()
        {

            ChNumericUpDown.BackColor = System.Drawing.Color.White;
            Ch2NumericUpDown.BackColor = System.Drawing.Color.White;
            Ch3NumericUpDown.BackColor = System.Drawing.Color.White;
            Ch4NumericUpDown.BackColor = System.Drawing.Color.White;

            Ch5NumericUpDown.BackColor = ChannelColour(Ch5NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch6NumericUpDown.BackColor = ChannelColour(Ch6NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch7NumericUpDown.BackColor = ChannelColour(Ch7NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch8NumericUpDown.BackColor = ChannelColour(Ch8NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch9NumericUpDown.BackColor = ChannelColour(Ch9NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch10NumericUpDown.BackColor = ChannelColour(Ch10NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch11NumericUpDown.BackColor = ChannelColour(Ch11NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);
            Ch12NumericUpDown.BackColor = ChannelColour(Ch12NumericUpDown.Value <= FormMain.DiscoveredRCChannelsT);

        }

        private void RxChannels_Changed(object sender, System.EventArgs e)
        {
            ParamUpdate(sender);
            RefreshRxChannels();
        }

        Color RCRange(short min, short max, short c)
        {
            return ((FormMain.RCChannel[c] < min || FormMain.RCChannel[c] > max) && (c < FormMain.DiscoveredRCChannelsT) ? Color.Orange : Color.White);
        }

        int RescalePercent(int v)
        {
            return (Limit(Convert.ToInt16(Convert.ToDouble(v - 800) * 2200.0 / 1400.0), 0, 2200));
        }

        void UpdateRCChannels()
        {

            DiscoveredRCChannelsLabel.Text = "#Ch " + string.Format("{0:n0}",  FormMain.DiscoveredRCChannelsT);
            RCPacketIntervalLabel.Text = string.Format("{0:n1}", FormMain.RCPacketIntervalT/1000.0f) +"mS";

            if (!RCuSCheckBox.Checked)
            {
                RC0ProgressBar.Value = RescalePercent(FormMain.RCChannel[0]);
                RC1ProgressBar.Value = RescalePercent(FormMain.RCChannel[1]);
                RC2ProgressBar.Value = RescalePercent(FormMain.RCChannel[2]);
                RC3ProgressBar.Value = RescalePercent(FormMain.RCChannel[3]);
                RC4ProgressBar.Value = RescalePercent(FormMain.RCChannel[4]);
                RC5ProgressBar.Value = RescalePercent(FormMain.RCChannel[5]);
                RC6ProgressBar.Value = RescalePercent(FormMain.RCChannel[6]);
                RC7ProgressBar.Value = RescalePercent(FormMain.RCChannel[7]);
                RC8ProgressBar.Value = RescalePercent(FormMain.RCChannel[8]);
                RC9ProgressBar.Value = RescalePercent(FormMain.RCChannel[9]);
                RC10ProgressBar.Value = RescalePercent(FormMain.RCChannel[10]);
                RC11ProgressBar.Value = RescalePercent(FormMain.RCChannel[11]);

                RC0ProgressBar.BackColor = RCRange(950, 2050, 0);
                if (FormMain.RCChannel[0] > 1000) RC0ProgressBar.BackColor = Color.Red;
                RC1ProgressBar.BackColor = RCRange(950, 2050, 1);
                RC2ProgressBar.BackColor = RCRange(950, 2050, 2);
                RC3ProgressBar.BackColor = RCRange(950, 2050, 3);
                RC4ProgressBar.BackColor = RCRange(950, 2050, 4);
                if (FormMain.RCChannel[4] > 1250) RC4ProgressBar.BackColor = Color.Red;
                RC5ProgressBar.BackColor = RCRange(950, 2050, 5);
                RC6ProgressBar.BackColor = RCRange(950, 2050, 6);
                RC7ProgressBar.BackColor = RCRange(950, 2050, 7);
                RC8ProgressBar.BackColor = RCRange(950, 2050, 8);

                RC9ProgressBar.BackColor = RCRange(950, 2050, 9);
                RC10ProgressBar.BackColor = RCRange(950, 2050, 10);
                RC11ProgressBar.BackColor = RCRange(950, 2050, 11);

                RC0TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[0] - 1000) * 0.1);
                RC1TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[1] - 1000) * 0.1);
                RC2TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[2] - 1000) * 0.1);
                RC3TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[3] - 1000) * 0.1);
                RC4TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[4] - 1000) * 0.1);
                RC5TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[5] - 1000) * 0.1);
                RC6TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[6] - 1000) * 0.1);
                RC7TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[7] - 1000) * 0.1);
                RC8TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[8] - 1000) * 0.1);

                RC9TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[9] - 1000) * 0.1);
                RC10TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[10] - 1000) * 0.1);
                RC11TextBox.Text = string.Format("{0:n1}", (FormMain.RCChannel[11] - 1000) * 0.1);
            }
            else
            {
                RC0ProgressBar.Value = Limit(FormMain.RCChannel[0], 0, 2200);
                RC1ProgressBar.Value = Limit(FormMain.RCChannel[1], 0, 2200);
                RC2ProgressBar.Value = Limit(FormMain.RCChannel[2], 0, 2200);
                RC3ProgressBar.Value = Limit(FormMain.RCChannel[3], 0, 2200);
                RC4ProgressBar.Value = Limit(FormMain.RCChannel[4], 0, 2200);
                RC5ProgressBar.Value = Limit(FormMain.RCChannel[5], 0, 2200);
                RC6ProgressBar.Value = Limit(FormMain.RCChannel[6], 0, 2200);
                RC7ProgressBar.Value = Limit(FormMain.RCChannel[7], 0, 2200);
                RC8ProgressBar.Value = Limit(FormMain.RCChannel[8], 0, 2200);
                RC9ProgressBar.Value = Limit(FormMain.RCChannel[9], 0, 2200);
                RC10ProgressBar.Value = Limit(FormMain.RCChannel[10], 0, 2200);
                RC11ProgressBar.Value = Limit(FormMain.RCChannel[11], 0, 2200);
 
                RC0ProgressBar.BackColor = RCRange(950, 2050, 0);
                if (FormMain.RCChannel[0] > 1000) RC0ProgressBar.BackColor = Color.Red;
                RC1ProgressBar.BackColor = RCRange(950, 2050, 1);
                RC2ProgressBar.BackColor = RCRange(950, 2050, 2);
                RC3ProgressBar.BackColor = RCRange(950, 2050, 3);
                RC4ProgressBar.BackColor = RCRange(950, 2050, 4);
                if (FormMain.RCChannel[4] > 1250) RC4ProgressBar.BackColor = Color.Red;
                RC5ProgressBar.BackColor = RCRange(950, 2050, 5);
                RC6ProgressBar.BackColor = RCRange(950, 2050, 6);
                RC7ProgressBar.BackColor = RCRange(950, 2050, 7);

                RC8ProgressBar.BackColor = RCRange(950, 2050, 8);
                RC9ProgressBar.BackColor = RCRange(950, 2050, 9);
                RC10ProgressBar.BackColor = RCRange(950, 2050, 10);
                RC11ProgressBar.BackColor = RCRange(950, 2050, 11);

                RC0TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[0]);
                RC1TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[1]);
                RC2TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[2]);
                RC3TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[3]);
                RC4TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[4]);
                RC5TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[5]);
                RC6TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[6]);
                RC7TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[7]);
                RC8TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[8]);
                RC9TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[9]);
                RC10TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[10]);
                RC11TextBox.Text = string.Format("{0:n0}", FormMain.RCChannel[11]);
            }

        }

        private void CheckBoxColours(Object Object, int c, int p) 
        {
            CheckBox Field = (CheckBox)Object;

            int cbm = 1 << c;

            P[CurrPS, p-1].Value = Field.Checked ? P[CurrPS, p-1].Value | cbm : P[CurrPS, p-1].Value & (255-cbm);

            if (UAVXP[p-1].Changed)
                if ((P[CurrPS, p-1].Value & cbm) == (UAVXP[p-1].Value & cbm))
                    Field.ForeColor = Color.Black;
                else
                    Field.ForeColor = writeUpdate ? Color.Red : Color.Orange;
            else
                Field.ForeColor = Color.Black;

        }

        void SenseButtonColours(Object Object, int b)
        {
            Button Field = (Button)Object;

            int m = 1 << b;

            if (ParameterForm.SenseButton[b])
            {
                P[CurrPS, 51].Value = (P[CurrPS, 51].Value) | m;
                Field.BackColor = Color.LightGray;
            }
            else
            {
                P[CurrPS, 51].Value = (P[CurrPS, 51].Value) & (255 - m);
                Field.BackColor = Color.White;
            }

            if (UAVXP[51].Changed)
                if  ((P[CurrPS, 51].Value & m) == (UAVXP[51].Value & m))
                    Field.ForeColor = Color.Green;
                else
                    Field.ForeColor = writeUpdate ? Color.Red : Color.Orange;
            else
                Field.ForeColor = Color.Black;
        }

        public void ParamUpdate(Object Object)
        {
            if (Object.GetType().Name == "ComboBox")
            {
                ComboBox Field = (ComboBox)Object;

                P[CurrPS, Convert.ToInt16(Field.Tag) - 1].Value = Field.SelectedIndex;

                if (UAVXP[Convert.ToInt16(Field.Tag) - 1].Changed == true)
                    if (P[CurrPS, Convert.ToInt16(Field.Tag) - 1].Value ==
                        UAVXP[Convert.ToInt16(Field.Tag) - 1].Value)
                        Field.ForeColor = Color.Green;
                    else
                        if (writeUpdate == true)
                            Field.ForeColor = Color.Red;
                        else
                            Field.ForeColor = Color.Orange;
                else
                    Field.ForeColor = Color.Black;
            }
            else

            if (Object.GetType().Name == "NumericUpDown")
            {
                NumericUpDown Field = (NumericUpDown)Object;

                int p = Convert.ToInt16(Field.Tag);

                if ((p == 22) || (p == 32) || (p == 39) || (p == 46) || (p == 53) || (p == 64) || (p == 58) || (p == 70) || (p == 75))
                    P[CurrPS, p - 1].Value = Convert.ToByte(Convert.ToDouble(Field.Value) * 10.0);
                else
                    P[CurrPS, p - 1].Value = Convert.ToByte(Field.Value);

                if (UAVXP[p - 1].Changed == true) {
                    if (P[CurrPS, p - 1].Value == UAVXP[p - 1].Value)
                        Field.ForeColor = Color.Green;
                    else
                        Field.ForeColor = writeUpdate ? Color.Red : Color.Orange;
               } else
                   Field.ForeColor = Color.Black;
            }
            else
                if (Object.GetType().Name == "Button")
                {
                    Button Field = (Button)Object;


                    switch (Field.Name)
                    {
                        case "Sense01Button": // Right Aileron
                            SenseButtonColours(Object, 0);
                            break;
                        case "Sense11Button":
                            SenseButtonColours(Object, 1);
                            break;
                        case "Sense21Button":
                            SenseButtonColours(Object, 2);
                            break;
                        case "Sense31Button":
                            SenseButtonColours(Object, 3);
                            break;
                        case "Sense41Button":
                            SenseButtonColours(Object, 4);
                            break;
                        case "Sense51Button":
                        SenseButtonColours(Object, 5);
                            break;
                        case "PropSense1Button":
                            //SenseButtonColours(Object, 6);
                            int m = 1 << 6;

                            if (ParameterForm.SenseButton[6])
                            {
                                P[CurrPS, 51].Value = (P[CurrPS, 51].Value) | m;
                                Field.BackColor = Color.Blue;
                            }
                            else
                            {
                                P[CurrPS, 51].Value = (P[CurrPS, 51].Value) & (255 - m);
                                Field.BackColor = Color.White;
                            }

                            break;
                    }
                }
              else
                if (Object.GetType().Name == "CheckBox")
                {
                    CheckBox Field = (CheckBox)Object;

                    switch (Field.Name)
                    {
                            // Config 1
                        case "bit01CheckBox":
                            CheckBoxColours(Object, 0, 16);
                            break;
                        case "bit11CheckBox":
                            CheckBoxColours(Object, 1, 16);
                            break;
                        case "bit21CheckBox":
                            CheckBoxColours(Object, 2, 16);
                            break;
                        case "bit31CheckBox":
                            CheckBoxColours(Object, 3, 16);
                            break;
                        case "bit41CheckBox":
                            CheckBoxColours(Object, 4, 16);
                            break;
                        case "bit51CheckBox":
                            CheckBoxColours(Object, 5, 16);
                            break;
                        case "bit61CheckBox":
                            CheckBoxColours(Object, 6, 16);
                            break;
                        // Config 2
                        case "bit02CheckBox":
                            CheckBoxColours(Object, 0, 74);
                            break;
                        case "bit12CheckBox":
                            CheckBoxColours(Object, 1, 74);
                            break;
                        case "bit22CheckBox":
                            CheckBoxColours(Object, 2, 74);
                            break;
                        case "bit32CheckBox":
                            CheckBoxColours(Object, 3, 74);
                            break;
                        case "bit42CheckBox":
                            CheckBoxColours(Object, 4, 74);
                            break;
                        case "bit52CheckBox":
                            CheckBoxColours(Object, 5, 74);
                            break;
                        case "bit62CheckBox":
                            CheckBoxColours(Object, 6, 74);
                            break;
                    }
               }
        }


        public void updateForm()
        {
            int p;

            RxLoopbackButton.BackColor = FormMain.RxLoopbackEnabled ?
                Color.Orange : RCGroupBox.BackColor;

            for (p = 1; p <= FormMain.MAX_PARAMS; p++ )
            {
                switch (p)
                {
                    case 1:
                        RollRatePropNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(RollRatePropNumericUpDown);
                        break;
                    case 2:
                        AltPosKiNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(AltPosKiNumericUpDown);
                        break;
                    case 3:
                        RollAnglePropNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(RollAnglePropNumericUpDown);
                        break;
                    case 4:
                        ArmingModeComboBox.SelectedIndex = UAVXP[p-1].Value;
                        //ArmingModeComboBox.BackColor = ArmingModeComboBox.SelectedIndex == 2 ?
                        //    Color.Orange : Color.White;

                        ParamUpdate(ArmingModeComboBox);
                        break;
                    case 5:
                        RollIntLimitNumericUpDown.Value = UAVXP[p-1].Value <= 0 ?
                            1: UAVXP[p-1].Value;
                        ParamUpdate(RollIntLimitNumericUpDown);
                        break;
                    case 6:
                        PitchRatePropNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(PitchRatePropNumericUpDown);
                        break;
                    case 7:
                        AltPosKpNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(AltPosKpNumericUpDown);
                        break;
                    case 8:
                        PitchAnglePropNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(PitchAnglePropNumericUpDown);
                        break;
                    case 9:
                        RangefinderComboBox.SelectedIndex = UAVXP[p-1].Value;
                        ParamUpdate(RangefinderComboBox);
                        break;
                    case 10:
                        PitchIntLimitNumericUpDown.Value = UAVXP[p-1].Value <= 0 ?
                            1 : UAVXP[p-1].Value;

                        ParamUpdate(PitchIntLimitNumericUpDown);
                        break;
                    case 11:
                        YawRatePropNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(YawRatePropNumericUpDown);
                        break;
                    case 12:
                        RollRateDiffNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(RollRateDiffNumericUpDown);
                        break;
                    case 13:
                        InertialSchemeComboBox.SelectedIndex = UAVXP[p-1].Value;
                        ParamUpdate(InertialSchemeComboBox);
                        break;
                    case 14:
                        AltVelKdNumericUpDown.Value =  UAVXP[p-1].Value;
                        ParamUpdate(AltVelKdNumericUpDown);
                        break;
                    case 15:
                        ComboPort1ComboBox.SelectedIndex = UAVXP[p-1].Value;
                        ParamUpdate(ComboPort1ComboBox);
                        break;
                    case 16:
                        int config = UAVXP[p-1].Value;

                        if (config == -1)
                            config = 0;

                        bit01CheckBox.Checked = (config & 1) != 0;
                        ParamUpdate(bit01CheckBox);
                        bit11CheckBox.Checked = (config & 2) != 0;
                        ParamUpdate(bit11CheckBox);
                        bit21CheckBox.Checked = (config & 4) != 0;
                        ParamUpdate(bit21CheckBox);
                        bit31CheckBox.Checked = (config & 8) != 0;
                        ParamUpdate(bit31CheckBox);
                        bit41CheckBox.Checked = (config & 16) != 0;
                        ParamUpdate(bit41CheckBox);
                        bit51CheckBox.Checked = (config & 32) != 0;
                        ParamUpdate(bit51CheckBox);
                        bit61CheckBox.Checked = (config & 64) != 0;
                        ParamUpdate(bit61CheckBox);

                        P[CurrPS, p-1].Value = config;
                        break;
                    case 17:
                        ChNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(ChNumericUpDown);
                        break;
                    case 18:
                        BatteryNumericUpDown.Value = UAVXP[p-1].Value < 0 ?
                            0 : Convert.ToDecimal(Math.Round(Convert.ToDouble(UAVXP[p-1].Value), 1, MidpointRounding.AwayFromZero));
                        ParamUpdate(BatteryNumericUpDown);
                        break;
                    case 19:
                        CameraRollNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(CameraRollNumericUpDown);
                        break;
                    case 20:
                        CruiseThrNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(CruiseThrNumericUpDown);
                        break;
                    case 21:
                        HysteresisNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(HysteresisNumericUpDown);
                        break;
                    case 22:
                        FWAltHoldDecayTimeNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(FWAltHoldDecayTimeNumericUpDown);
                        break;
                    case 23:
                        LowMotorRunNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(LowMotorRunNumericUpDown);
                        break;
                    case 24:
                        RollAngleIntNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(RollAngleIntNumericUpDown);
                        break;
                    case 25:
                        PitchAngleIntNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(PitchAngleIntNumericUpDown);
                        break;
                    case 26:
                        CameraPitchNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(CameraPitchNumericUpDown);
                        break;
                    case 27:
                        YawAnglePropNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(YawAnglePropNumericUpDown);
                        break;
                    case 28:
                        PitchRateDiffNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(PitchRateDiffNumericUpDown);
                        break;
                    case 29:
                        NavVelKpNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(NavVelKpNumericUpDown);
                        break;
                    case 30:
                        AltVelKpNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(AltVelKpNumericUpDown);
                        break;
                    case 31:
                        HorizonNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(HorizonNumericUpDown);
                        break;
                    case 32:
                        MadgwickKpMagNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(MadgwickKpMagNumericUpDown);
                        break;
                    case 33:
                        NavRTHAltNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(NavRTHAltNumericUpDown);
                        break;
                    case 34:
                        NavMagVarNumericUpDown.Value = UAVXP[p-1].Value;
                        NavMagVarNumericUpDown.BackColor = UAVXP[p - 1].Value != 0 ? System.Drawing.Color.White : System.Drawing.Color.Orange;
                        ParamUpdate(NavMagVarNumericUpDown);
                        break;
                    case 35:
                        GyroComboBox.SelectedIndex = UAVXP[p - 1].Value;
                        GyroComboBox.BackColor = GyroComboBox.SelectedIndex >= 6 ?
                            Color.Red : Color.White;
                        ParamUpdate(GyroComboBox);
                        break;
                    case 36:
                        ESCComboBox.SelectedIndex = UAVXP[p-1].Value;
                        ParamUpdate(ESCComboBox);
                        break;
                    case 37:
                        RxChannelsNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(RxChannelsNumericUpDown);
                        break;
                    case 38:
                        Ch2NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch2NumericUpDown);
                        break;
                    case 39:
                        MadgwickKpAccNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(MadgwickKpAccNumericUpDown);
                        break;
                    case 40:
                        CameraRollTrimNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(CameraRollTrimNumericUpDown);
                        break;
                    case 41:
                        NavMaxGroundSpeedNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(NavMaxGroundSpeedNumericUpDown);
                        break;
                    case 42:
                        Ch3NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch3NumericUpDown);
                        break;
                    case 43:
                        Ch4NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch4NumericUpDown);
                        break;
                    case 44:
                        AFTypeComboBox.SelectedIndex = UAVXP[p-1].Value;
                        if (AFTypeComboBox.SelectedIndex >= 10)
                            AFTypeComboBox.BackColor = Color.Orange;
                        else
                            AFTypeComboBox.BackColor =(AFTypeComboBox.SelectedIndex == 0) ||
                                (AFTypeComboBox.SelectedIndex >= 11) ?
                                Color.Orange : Color.White;
                        ParamUpdate(AFTypeComboBox);
                        break;
                    case 45:
                        TelemetryComboBox.SelectedIndex = UAVXP[p-1].Value;
                        ParamUpdate(TelemetryComboBox);
                        break;
                    case 46:
                        DescentRateNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(DescentRateNumericUpDown);
                        break;
                    case 47:
                        DescDelayNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(DescDelayNumericUpDown);
                        break;
                    case 48:
                        GyroLPFComboBox.SelectedIndex = UAVXP[p-1].Value;
                        if (GyroLPFComboBox.SelectedIndex < 1)
                            GyroLPFComboBox.BackColor = Color.Red;
                        else
                            GyroLPFComboBox.BackColor = GyroLPFComboBox.SelectedIndex > 4 ?
                                Color.Orange : Color.White;
                        ParamUpdate(GyroLPFComboBox);
                        break;
                    case 49:
                        CrossTrackNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(CrossTrackNumericUpDown);
                        break;
                    case 50:
                        Ch5NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch5NumericUpDown);
                        break;
                    case 51:
                        Ch6NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch6NumericUpDown);
                        break;
                    case 52:
                        int sense = UAVXP[p-1].Value;

                        ParameterForm.SenseButton[0] = (sense & 1) !=0 ;
                        ParamUpdate(Sense01Button);
                        ParameterForm.SenseButton[1] = (sense & 2) != 0;
                        ParamUpdate(Sense11Button);
                        ParameterForm.SenseButton[2] = (sense & 4) != 0;
                        ParamUpdate(Sense21Button);
                        ParameterForm.SenseButton[3] = (sense & 8) != 0;
                        ParamUpdate(Sense31Button);
                        ParameterForm.SenseButton[4] = (sense & 16) != 0;
                        ParamUpdate(Sense41Button);
                        ParameterForm.SenseButton[5] = (sense & 32) != 0;
                        ParamUpdate(Sense51Button);
                        ParameterForm.SenseButton[6] = (sense & 64) != 0;
                        ParamUpdate(PropSense1Button);
           
                        break;
                    case 53:
                        AccConfNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(AccConfNumericUpDown);
                        break;
                    case 54:
                        BatteryCapacityNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(BatteryCapacityNumericUpDown);
                        break;
                    case 55:
                        Ch7NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch7NumericUpDown);
                        break;
                    case 56:
                        Ch8NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch8NumericUpDown);
                        break;
                    case 57:
                        NavPosKpNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(NavPosKpNumericUpDown);
                        break;
                    case 58:
                       AltLPFNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                       ParamUpdate(AltLPFNumericUpDown);
                        break;
                    case 59:
                        BalanceNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(BalanceNumericUpDown);
                        break;
                    case 60:
                        Ch9NumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(Ch9NumericUpDown);
                        break;
                    case 61:
                        NavVelKiNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(NavVelKiNumericUpDown);
                        break;
                    case 62:
                        GPSTypeComboBox.SelectedIndex = UAVXP[p-1].Value;
                        ParamUpdate(GPSTypeComboBox);
                        break;
                    case 63:
                        AttThrFFNumericUpDown.Value = UAVXP[p-1].Value;
                        ParamUpdate(AttThrFFNumericUpDown);
                        break;
                    case 64:
                        YawScaleNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(YawScaleNumericUpDown);
                        break;

                    // Extension largely for Fixed Wing

                    case 65:
                        FWRollPitchFFNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(FWRollPitchFFNumericUpDown);
                        break;
                    case 66:
                        FWPitchThrottleFFNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(FWPitchThrottleFFNumericUpDown);
                        break;
                    case 67:
                        MaxAltCompNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(MaxAltCompNumericUpDown);
                        break;
                    case 68:
                        MaxAttitudeAngleNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(MaxAttitudeAngleNumericUpDown);
                        break;
                    case 69:
                        GlideOffsetErrorLabel.Text = string.Format("{0:n0}", UAVXP[p - 1].Value);
                        P[CurrPS, p-1].Value = UAVXP[p - 1].Value;
                        break;
                    case 70:
                        FWFlapDecayTimeNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(FWFlapDecayTimeNumericUpDown);
                        break;
                    case 71:
                        FWAileronDifferentialNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(FWAileronDifferentialNumericUpDown);
                        break;
                    case 72:
                        AirspeedComboBox.SelectedIndex = UAVXP[p - 1].Value;
                        ParamUpdate(AirspeedComboBox);
                        break;
                    case 73:
                        BestROCNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(BestROCNumericUpDown);
                        break;

// etc to param 96
                    case 74:
                        int config2 = UAVXP[p - 1].Value;

                        if (config2 == -1)
                            config2 = 0;

                        bit02CheckBox.Checked = (config2 & 1) != 0;
                        ParamUpdate(bit02CheckBox);             
                        bit12CheckBox.Checked = (config2 & 2) != 0;
                        ParamUpdate(bit12CheckBox);
                        bit22CheckBox.Checked = (config2 & 4) != 0;
                        ParamUpdate(bit22CheckBox);
                        bit32CheckBox.Checked = (config2 & 8) != 0;
                        ParamUpdate(bit32CheckBox);
                        bit42CheckBox.Checked = (config2 & 16) != 0;
                        ParamUpdate(bit42CheckBox);
                        bit52CheckBox.Checked = (config2 & 32) != 0;
                        ParamUpdate(bit52CheckBox);
                        bit62CheckBox.Checked = (config2 & 64) != 0;
                        ParamUpdate(bit62CheckBox);
                    
                        P[CurrPS, p-1].Value = config2;
                        break;

                    case 75:
                        RollPitchScaleNumericUpDown.Value = Convert.ToDecimal(UAVXP[p - 1].Value * 0.1);
                        ParamUpdate(RollPitchScaleNumericUpDown);
                        break;
                    case 76:
                        ComboPort2ComboBox.SelectedIndex = UAVXP[p - 1].Value;
                        ParamUpdate(ComboPort2ComboBox);
                        break;

                    case 77:
                        FWMaxClimbAngleNumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(FWMaxClimbAngleNumericUpDown);
                        break;

                    case 78:
                        TuneParamComboBox.SelectedIndex = UAVXP[p - 1].Value;
                        ParamUpdate(TuneParamComboBox);
                        break;
                    case 79:
                        TuneParameterLabel.Text = string.Format("{0:n0}", UAVXP[p - 1].Value);
                        P[CurrPS, p-1].Value = UAVXP[p - 1].Value;
                        break;
                    case 94: // Aux5
                        Ch10NumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(Ch10NumericUpDown);
                        break;
                    case 95: // Aux6
                        Ch11NumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(Ch11NumericUpDown);
                        break;
                    case 96: // Aux7
                        Ch12NumericUpDown.Value = UAVXP[p - 1].Value;
                        ParamUpdate(Ch12NumericUpDown);
                        break;

                    default: break; // up to case 64 available
                }
            }
        }

      

        private void ParameterForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            // turn off parameter button colour
        }


 
   
   
    }
}
