//// UAVXSet
// Copyright (C) 2007  Thorsten Raab
// Email: thorsten.raab@gmx.at
// Michael Sachs
// Email: michael.sachs@online.de
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
using System.Text;
using System.Resources;

namespace UAVXGUI
{
    static class Hilfe
    {
        static public void info(ParameterForm formParameter)
        {
           

            // Roll     
            if (formParameter.RollRatePropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Proportional");
            if (formParameter.RollRateDiffNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Differential");
            if (formParameter.RollAnglePropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ProportionalAngle");
            if (formParameter.RollAngleIntNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ProportionalAngle");
            if (formParameter.RollAngleDiffNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Differential");
            if (formParameter.RollIntNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Integral");
            if (formParameter.RollIntLimitNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("IntegralLimiter");
            if (formParameter.RollPitchMixNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("RollYawMix");

            // Pitch
            if (formParameter.PitchRatePropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Proportional");
            if (formParameter.PitchRateDiffNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Differential");
            if (formParameter.PitchAnglePropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ProportionalAngle");
            if (formParameter.PitchAngleDiffNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Differential");
            if (formParameter.PitchAngleIntNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ProportionalAngle");
            if (formParameter.PitchRateIntNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Integral");
            if (formParameter.PitchIntLimitNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("IntegralLimiter");

            if (formParameter.BalanceNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Balance");

            // Yaw
            if (formParameter.YawRatePropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Proportional");
            if (formParameter.CrossTrackNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("CrossTrack");

            if (formParameter.YawScaleNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Damping");
            if (formParameter.YawRateIntLimitNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Limiter");
            //   if (formParameter.RxTypeComboBox.Focused || YawIntLimit2NumericUpDown.Focused)
            //       ParameterForm.helpstring = formParameter.help.GetString("IntegralLimiter");


            // General
            if (formParameter.bit01CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("AuxMode");
            if (formParameter.bit61CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ControlScheme");
            if (formParameter.bit21CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("StickProgMode");
            if (formParameter.bit31CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Emulation");
            if (formParameter.bit41CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("WPNavigation");
            if (formParameter.bit51CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Failsafes");
            if (formParameter.bit11CheckBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("RTHDescend");

            if (formParameter.HoverThrottleNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("HoverThrottle");
            if (formParameter.DriveFiltNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("DriveFilter");
            if (formParameter.InertialSchemeComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("InertialScheme");

            if (formParameter.AFTypeComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("AFType");
            if (formParameter.LowMotorRunNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("LowMotorRun");
            if (formParameter.CameraRollNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("CameraGain");
            if (formParameter.CameraRollTrimNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("CameraRollTrim");
            if (formParameter.YawAnglePropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Compass");
            if (formParameter.AccCompNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("AccGyroComp");
            if (formParameter.BatteryNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Unterspannung");
            if (formParameter.AltPropNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("AltProp");


            if (formParameter.WindIntNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Wind");

            if (formParameter.BatteryCapacityNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("BatteryCapacity");


            if (formParameter.YawRateIntNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ThrottleGainSchedule");

            if (formParameter.MadgwickKpMagNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("Acro");

            if (formParameter.DescDelayNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("DescentDelay");
            if (formParameter.DescentRateNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("DescentRate");
            if (formParameter.BaroAccCompNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("BaroFilter");

            if (formParameter.GyroLPFComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("GyroLPF");

            if (formParameter.MadgwickKpAccNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("MadgwickKp");

            if (formParameter.GyroComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("GyroType");

            if (formParameter.ESCComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("ESCType");

            if (formParameter.TelemetryComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("TelemetryType");

            if (formParameter.RxChannelsNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("RxChannels");

            if (formParameter.RxTypeComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("RxType");

            if (formParameter.TxModeComboBox.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("TxMode");

            //GPS

            if (formParameter.NavMaxVelNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("NavMaxVel");

            if (formParameter.NavRTHAltNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("NavRTHAltitude");
            if (formParameter.NavMagVarNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("NavMagVar");

            if (formParameter.NavMaxAngleNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("NavGPSSlew");

            if (formParameter.NavSlewRateNumericUpDown.Focused)
                ParameterForm.helpstring = formParameter.help.GetString("NavTurnLimit");

           // return helpstring;

        }
    }
}
