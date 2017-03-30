// UAVXSet
// Copyright (C) 2007  Thorsten Raab
// Email: thorsten.raab@gmx.at
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
using System.IO;
using System.IO.Ports;
using System.Windows.Forms;
using System.Drawing;
using System.Resources;
using System.Collections;
using System.Threading;
using UAVP.UAVXSet.Debug;


namespace UAVP.UAVXSet
{

    public class PICConnect
    {
        public PICConnect()
        {
            labels = new ResourceManager("UAVP.UAVXSet.Resources.language", this.GetType().Assembly);
            errorLabels = new ResourceManager("UAVP.UAVXSet.Resources.error", this.GetType().Assembly);
        }
        
        ResourceManager labels;
        ResourceManager errorLabels;
        public SerialPort sp = new SerialPort();

        public bool connect(FormMain mainForm, bool testSoftware, bool connectionTesten)
        {

            if (!sp.IsOpen) 
            {
                sp.PortName = Properties.Settings.Default.comPort;
                sp.BaudRate = Properties.Settings.Default.baudRate;
                switch (Properties.Settings.Default.stopBits)
                { 
                    case "1":
                        sp.StopBits = StopBits.One;
                        break;
                    case "1.5":
                        sp.StopBits = StopBits.OnePointFive;
                        break;
                    case "2":
                        sp.StopBits = StopBits.Two;
                        break;
                    case "None":
                        sp.StopBits = StopBits.None;
                        break;
                }
                sp.DataBits = 8;
                switch (Properties.Settings.Default.parity)
                { 
                    case "Even":
                        sp.Parity = Parity.Even;
                        break;
                    case "Mark":
                        sp.Parity = Parity.Mark;
                        break;
                    case "None":
                        sp.Parity = Parity.None;
                        break;
                    case "Odd":
                        sp.Parity = Parity.Odd;
                        break;
                    case "Space":
                        sp.Parity = Parity.Space;
                        break;
                }
                switch (Properties.Settings.Default.handshake)
                {
                    case "None":
                        sp.Handshake = Handshake.None;
                        break;
                    case "RequestToSend":
                        sp.Handshake = Handshake.RequestToSend;
                        break;
                    case "RequestToSendXOnXOff":
                        sp.Handshake = Handshake.RequestToSendXOnXOff;
                        break;
                    case "XOnXOff":
                        sp.Handshake = Handshake.XOnXOff;
                        break;
                }
                sp.ReadTimeout = Convert.ToInt16(Properties.Settings.Default.time);

                Log.write(mainForm, Properties.Settings.Default.comPort,1);
                
                
                try 
                {
                    sp.Open();
                    if (connectionTesten == true)
                    {
                        ArrayList test = testComm(mainForm);
                        // schreiben in Status und log

                        if (test.Count > 0)
                        {
                            mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("picConnected");
                            Log.write(mainForm, labels.GetString("picConnected"), 0);
                            mainForm.listViewJobs.Items[0].Text = labels.GetString("listviewConnected");
                            mainForm.listViewJobs.Items[0].ForeColor = Color.Green;

                            if (test[0].ToString() != "ERR\r")
                            {
 //zzz                               if (test[1].ToString().Substring(0, 8) == "UAVX TEST" && testSoftware == false)
 //                               {
 //                                   MessageBox.Show(errorLabels.GetString("testSoft"), "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
 //                                   Log.write(mainForm, errorLabels.GetString("testSoft"), 1);
 //                               }
                           }
                        }
                        else
                        {
                            MessageBox.Show(errorLabels.GetString("askPic"), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                            closeConnection(mainForm);
                            return false;
                        }
                    }
                } 
                catch(Exception e) 
                {
                    MessageBox.Show(errorLabels.GetString("connect") + "\r\n" + e.ToString(), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    Log.write(mainForm, e.ToString(),1);
                    closeConnection(mainForm);
                    return false;
                }
            } 
            else 
                closeConnection(mainForm);

            return true;
        }

        public void closeConnection(FormMain mainForm)
        {
            mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("disconnected");
            Log.write(mainForm, labels.GetString("disconnected"), 0);
            mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("disconnected");
            mainForm.listViewJobs.Items[0].Text = labels.GetString("disconnected");
            mainForm.listViewJobs.Items[0].ForeColor = Color.Black;

            sp.Close();
        }

        public void readBlackBox(string FileName, FormMain mainForm)
        {
           FileStream fs = new FileStream(FileName, FileMode.Create);
           BinaryWriter sw = new BinaryWriter(fs);

            mainForm.Cursor = Cursors.WaitCursor;

            bool con = true;
 
            if (!sp.IsOpen)
                con = connect(mainForm, false, true);
            else
                sp.DiscardInBuffer();

           byte b;

           sp.ReadTimeout = 1000;

            try
           {
                sp.WriteLine("G");

                Thread.Sleep(1);

                while (true)
                {
                   b = (byte) sp.ReadByte();
                   sw.Write(b);
                }

          }
          catch (TimeoutException)
          {
             sw.Flush();
             sw.Close();
          }
 
        }

        public void readParameters(FormMain mainForm, ParameterSets parameter)
        {
            // Cursor auf wait setzen
            mainForm.Cursor = Cursors.WaitCursor;

            ArrayList para = askPic(mainForm, "L");
       
            if (Properties.Settings.Default.logLevel > 0) 
                foreach (string output in para) 
                    Log.write(mainForm, output, 1);
            else 
                if (para.Count > 0) 
                {
                    try
                    {
                        Log.write(mainForm, para[1].ToString(), 0);
                    }
                    catch {
                        Log.write(mainForm, "Fehler beim schreiben Para.Count - PICConnect.cs line 197", 1);
                    }
                }

            if (para.Count == 0) 
            {
                mainForm.Cursor = Cursors.Default;
                Log.write(mainForm, "Fehler in Verbindung", 1);
                return;
            }

            Log.write(mainForm, "Springe auf TAB", 1);

               if (para[1].ToString().Substring(0, 24) == "Parameter list for set #")
               {
                   mainForm.tabControlParameter.SelectedIndex = Convert.ToInt16(para[1].ToString().Substring(24, 1)) - 1;

                    if (mainForm.tabControlParameter.SelectedIndex == 0)
                    {
                        mainForm.tabPageParameterSet1.Text = labels.GetString("paraSetAktiv1");
                        mainForm.tabPageParameterSet2.Text = labels.GetString("paraSet2");
                        Log.write(mainForm, "Markieren Tab 1", 1);
                    }
                    else
                    {
                        mainForm.tabPageParameterSet1.Text = labels.GetString("paraSet1");
                        mainForm.tabPageParameterSet2.Text = labels.GetString("paraSetAktiv2");
                        Log.write(mainForm, "Markieren Tab 2", 1);
                    }

                    if (mainForm.tabControlParameter.SelectedIndex == 0)
                    {
                        Log.write(mainForm, "beginne löschen Chapter ...", 1);
                        for (int i = 0; i < parameter.parameterPic1.Length; i++)
                        {
                            Log.write(mainForm, "alle Chapter löschen [" + i.ToString() + "]", 1);
                            parameter.parameterPic1[i].Chapter = "";
                        }

                        Log.write(mainForm, "beginne Para struc speichern ...", 1);
                        for (int i = 0; i < para.Count - 2; i++)
                        {
                            Log.write(mainForm, "alle Para struc speichern [" + i.ToString() + "]", 1);
                            //Log.write(mainForm, para[i].ToString(), 0);
                            // + in den Parametern ausschneiden
                            if (para[i + 2].ToString().Substring(15, 1) == "+")
                                para[i + 2] = Convert.ToInt16(para[i + 2].ToString().Substring(16, 3));
                            else
                                para[i + 2] = Convert.ToInt16(para[i + 2].ToString().Substring(15, 4));

                            parameter.parameterPic1[i].Command = "Register " + (i + 1).ToString();
                            parameter.parameterPic1[i].Value = para[i + 2].ToString();
                            parameter.parameterPic1[i].Chapter = "ChannelSet";

                        }
                        parameter.updateForm(parameter.parameterPic1, mainForm);
                    }
                    else
                    {
                        for (int i = 0; i < parameter.parameterPic2.Length; i++)
                            parameter.parameterPic2[i].Chapter = "";

                        for (int i = 0; i < para.Count - 2; i++)
                        {
                            if (para[i + 2].ToString().Substring(15, 1) == "+")
                                para[i + 2] = Convert.ToInt16(para[i + 2].ToString().Substring(16, 3));
                            else
                                para[i + 2] = Convert.ToInt16(para[i + 2].ToString().Substring(15, 4));

                            parameter.parameterPic2[i].Command = "Register " + (i + 1).ToString();
                            parameter.parameterPic2[i].Value = para[i + 2].ToString();
                            parameter.parameterPic2[i].Chapter = "ChannelSet";

                        }
                        // ganzes Form mit den Daten updaten
                        parameter.updateForm(parameter.parameterPic2, mainForm);
                    }
           } // else dialog from UAVX has a Z command in it!

            // Cursor auf default setzen
            mainForm.Cursor = Cursors.Default;       
        }

        public void writeParameters(FormMain mainForm, ParameterSets parameter)
        {
            int noofparams = 64;
            bool err = true;
            mainForm.writeUpdate = true;
            // Verbindung öffnen wenn noch nicht verbunden
            if (!sp.IsOpen) 
                err = connect(mainForm, false, true);

            // wenn fehler
            if (err == false) 
                return;
 
            // Progressbar einblenden und Cursor auf wait setzen
            mainForm.toolStripProgressBar.Visible = true;
            mainForm.Cursor = Cursors.WaitCursor;
            mainForm.Enabled = false;

            // wenn Prameter Set 1
            if (mainForm.tabControlParameter.SelectedIndex == 0)
            {
                // alle Parameter schreiben
                foreach (ParameterSets.ParameterSetsStruc value in parameter.parameterForm1)
                    // wenn fehler beim schreiben dann aussteigen
                    if (Convert.ToInt16(value.Command.Substring(9)) <= noofparams )
                        if (err = parameterWrite(mainForm, value) == false)
                            break;
            }
            else
            {
                // wenn SET 2
                // alle Parameter schreiben
                foreach (ParameterSets.ParameterSetsStruc value in parameter.parameterForm2)
                    // wenn fehler beim schreiben dann aussteigen
                    if (Convert.ToInt16(value.Command.Substring(9)) <= noofparams )
                        if (err = parameterWrite(mainForm, value) == false)
                            break;
            }

            mainForm.toolStripProgressBar.Value = 0;
            mainForm.toolStripProgressBar.Visible = false;
            mainForm.Cursor = Cursors.Default;
            mainForm.Enabled = true;

            mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("writeOk");
            Log.write(mainForm, labels.GetString("writeOk"), 0);

            mainForm.writeUpdate = true;
            readParameters(mainForm, parameter);
            mainForm.writeUpdate = false;

        }

        private bool parameterWrite(FormMain mainForm, ParameterSets.ParameterSetsStruc value)
        {
            mainForm.Cursor = Cursors.WaitCursor;
            try 
            {
                string temp;

                sp.WriteLine("M");
                if (Properties.Settings.Default.logLevel > 0) 
                    Log.write(mainForm, "send: \"M\" for Parameter: " + 
                        Convert.ToInt16(value.Command.Substring(9)).ToString("00"), 1);

                Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                temp = sp.ReadLine();
                
                while (temp != "Register ") 
                {
                    Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                    temp = sp.ReadLine();
                    if (Properties.Settings.Default.logLevel > 0) 
                        Log.write(mainForm, temp + "first write M", 1);
                }

                Thread.Sleep(10);

                if (Properties.Settings.Default.logLevel > 0) 
                        Log.write(mainForm, value.Command.Substring(9).ToString() + " = " +
                                    value.Value, 1);
                
                sp.Write(Convert.ToInt16(value.Command.Substring(9)).ToString("00"));

                Thread.Sleep(10);

                if (Properties.Settings.Default.logLevel > 0)
                    Log.write(mainForm, "Write: " + Convert.ToInt16(value.Command.Substring(9)).ToString("00").ToString(), 1);

                sp.Write(Convert.ToInt16(value.Value).ToString("000"));//zzz


                if (Properties.Settings.Default.logLevel > 0)
                    Log.write(mainForm, "Write: " + Convert.ToInt16(value.Value).ToString("00").ToString(), 1);

                Thread.Sleep(Properties.Settings.Default.iReadSleep);
                Thread.Sleep(10);

                temp = sp.ReadLine();
                
                if (temp != Convert.ToInt16(value.Command.Substring(9)).ToString("00") +
                    " = " + Convert.ToInt16(value.Value).ToString("000") + "\r") //zzz
                    throw (new Exception());

                Log.write(mainForm, (labels.GetString("writeParameter") +
                Convert.ToInt16(value.Command.Substring(9)).ToString("00") + " = " +
                Convert.ToInt16(value.Value).ToString("000")), 0); // zzz now 3 digits but still int8
             
                mainForm.toolStripProgressBar.Increment(2);
                mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("writeParameter") + Convert.ToInt16(value.Command.Substring(9)).ToString("00") + " = " + Convert.ToInt16(value.Value).ToString("000"); // zzz
                Application.DoEvents();
                mainForm.Cursor = Cursors.Default;
                return true;
            } 
            catch (Exception e) 
            {
                MessageBox.Show(errorLabels.GetString("askPic"), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);
                Log.write(mainForm, e.ToString() + "\r\nTest Return from PIC", 1);
                mainForm.Cursor = Cursors.Default;
                return false;
            }
        }

        public void parameterSet(FormMain mainForm)
        {

            // pic fragen
            ArrayList setup = askPic(mainForm, "S");

            // logausgabe
            if (Properties.Settings.Default.logLevel > 0) 
                foreach (string output in setup)
                    Log.write(mainForm, output, 1);
            else 
                // wenn keine Verbindung besteht gibt es kein setup[5]
                if (setup.Count > 4) 
                    Log.write(mainForm, setup[5].ToString(), 0);
                else 
                    Log.write(mainForm, "Error: Setup <= 4", 1);

            if (setup.Count <= 4) 
                return;

            //Parameterset auslesen
            int chanelSet = 0;
            foreach (string chanel in setup)
                if (chanel.Length > 11)
                    if (chanel.Substring(0, 11) == "Param set: ")
                        chanelSet = Convert.ToInt16(chanel.ToString().Substring(11, 1)) - 1;
            
            // aktives TAB selectieren
            mainForm.tabControlParameter.SelectedIndex = chanelSet;
            if (mainForm.tabControlParameter.SelectedIndex == 0) 
            {
                mainForm.tabPageParameterSet1.Text = labels.GetString("paraSetAktiv1");
                mainForm.tabPageParameterSet2.Text = labels.GetString("paraSet2");
            } 
            else 
            {
                mainForm.tabPageParameterSet1.Text = labels.GetString("paraSet1");
                mainForm.tabPageParameterSet2.Text = labels.GetString("paraSetAktiv2");
            }
        }

        public ArrayList testComm(FormMain mainForm)
        {
            ArrayList ret = askPic(mainForm, "S");
            Log.write(mainForm, "Function testCom:", 1);
            
            // logausgabe
            if (Properties.Settings.Default.logLevel > 0) 
                foreach (string output in ret) 
                    Log.write(mainForm, output, 1);

            return ret;
        }

        public ArrayList askPic(FormMain mainForm, string key)
        {
            bool con = true;
            // wenn noch keine Verbindung besteht automatisch öffnen
            if (!sp.IsOpen) 
                con = connect(mainForm, false, true);
            else 
                sp.DiscardInBuffer();
            // Puffer leeren wenn Boad neu gestartet wird kommt info sonst in den buffer
            
            ArrayList ret = new ArrayList();
            
            // wenn verbindung nicht ok
            if (con == false) 
                return ret;

            string temp;

            try 
            {
                // keine Zeiteinsparung mit write statt writeline - ende mit > leichter erfassbar.
                // deshalb wieder auf writeline umgestellt
                sp.WriteLine(key);
                // log über sendekey
                if (Properties.Settings.Default.logLevel > 0) 
                    Log.write(mainForm, labels.GetString("writeParameter") + key, 1);

                Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                temp = sp.ReadLine();

                if (temp == "ERR\r") 
                {
                    Log.write(mainForm, "Board is in Bootloader Status!", 1);
                    ret.Add(temp);
                    mainForm.picBootModus = true;
                    MessageBox.Show(errorLabels.GetString("picBurnMode"), "Info!", MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                    return ret;
                }
     
                while (temp != ">") // T wird nur für Receiver benötigt wenn nur write und nicht writeline 
                                    // || temp.Substring(0,1) == "T")
                                    
                {
                    if (temp == "Press the CONTINUE button when done\r" || temp == "# ")
                        break;

                   if (temp != "" ) // zzz
                        ret.Add(temp);

                    Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                    temp = sp.ReadLine();
                    
                    // log schreiben wenn debuglevel
                    if (Properties.Settings.Default.logLevel > 0) 
                        Log.write(mainForm, temp, 1);
                }
            } 
            catch (Exception e) 
            {
                if (con == false) 
                    MessageBox.Show(errorLabels.GetString("askPic"), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);

                Log.write(mainForm, e.ToString(), 1);
            }
            return ret;
        }


        public void setparamset(FormMain mainForm)
        {
            ArrayList ret = askPic(mainForm, "Z");
            if (mainForm.tabControlParameter.SelectedIndex == 0)
                sp.Write(" 01");
            else
                sp.Write(" 02");
         }

        public void receiver(FormMain mainForm)
        {
            //ArrayList ret = askPic(mainForm, "R");
            //Log.write(mainForm, "R: " + ret[1].ToString(), 0);
            closeConnection(mainForm);
            Receiver receiverWindow = new Receiver(mainForm);
            // dialog anzeigen
            receiverWindow.ShowDialog();
        }

        public void testSoftware(FormMain mainForm)
        {
            //sp.Close();   // übergabe der Connection klasse
            TestSoftware testSoftware = new TestSoftware(mainForm, this);
            if (testSoftware.errorFlag == false)
                testSoftware.ShowDialog();

        }

        public void debugSoftware(FormMain mainForm)
        {
            //sp.Close();   // übergabe der Connection klasse
            DebugSoftware debugSoftware = new DebugSoftware(mainForm, this);
                debugSoftware.ShowDialog();
        }


        public void BurnPIC(FormMain mainForm, string fileName)
        {

            //Hex laden
            // StreamReader-Instanz für die Datei erzeugen
            StreamReader sr = null;
            try 
            {
                sr = new StreamReader(fileName, Encoding.GetEncoding("windows-1252"));
            } 
            catch (Exception ex) 
            {
                MessageBox.Show(ex.Message, Application.ProductName, MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                Log.write(mainForm, ex.ToString(), 1);
                return;
            }

            // Datei zeilenweise einlesen
            
            ArrayList hexFile = new ArrayList();
            string line = null;
            while ((line = sr.ReadLine()) != null) 
                hexFile.Add(line);

            Log.write(mainForm, ("Read File: " + fileName), 0);
            Log.write(mainForm, ("Lines: " + hexFile.Count.ToString()), 1);


            // Progressbar einblenden und Cursor auf wait setzen
            mainForm.toolStripProgressBar.Visible = true;
            mainForm.Cursor = Cursors.WaitCursor;
            mainForm.Enabled = false;

            // Brennen in PIC
            // wenn noch keine Verbindung besteht automatisch öffnen
            if (!sp.IsOpen) 
                connect(mainForm, false, true);

            Log.write(mainForm, Properties.Settings.Default.comPort, 1);
             
            int i = 1;
            string temp = "no answer from PIC after B";

            if (mainForm.picBootModus == false)
            {
                sp.DiscardInBuffer();
                sp.Write("B");
                try
                {
                    Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                    temp = sp.ReadLine();
                    if (temp == "B\r")
                    {
                        Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                        temp = sp.ReadLine();
                    }

                }
                catch (Exception e)
                {
                    Log.write(mainForm, e.ToString(), 1);
                }
            }
            Log.write(mainForm, temp, 1);
            mainForm.toolStripProgressBar.Maximum = Convert.ToInt16(hexFile.Count);

            //alle zeilen des Files schreiben
            try 
            {
                foreach (string zeile in hexFile) 
                {
                    mainForm.Cursor = Cursors.WaitCursor;
                    if (Properties.Settings.Default.logLevel > 0) 
                        Log.write(mainForm, zeile, 1);

                    // Progressbar hochzählen
                    mainForm.toolStripProgressBar.Increment(1);
                    // Infotext in status schreiben
                    mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("writeLine") +
                        " " + i.ToString() + " " + labels.GetString("writeMax") + " " + 
                        Convert.ToInt16(hexFile.Count) + "  ";
                    
                    // Infotext in Log schreiben
                    Log.write(mainForm, ("Line: " + i.ToString() + ": " + zeile), 0);
                    Application.DoEvents();
                    i++;

                    // zeile senden
                    sp.Write(zeile + "\r\n");
                    // für die höhere Übertragungsrate nach jeder Zeile etwas warten.
                    Thread.Sleep(1);
                    // hier wurde von 4 schon auf 10 erhöht - er kommt trozdem immer wieder zu einem Timeout
                    for (int j = 0; j < 10; j++) 
                    {
                        // etwas warten bis der PIC die Daten verarbeitet hat und bei jedem Versuch länger!
                        if (Properties.Settings.Default.iReadSleep == 0)
                            Thread.Sleep(Properties.Settings.Default.writeSleep * j * j);
                        else
                            Thread.Sleep(Properties.Settings.Default.iReadSleep);

                        temp = sp.ReadLine();
                        Log.write(mainForm, temp, 1);
                        if (temp == "ERR\r") 
                        {
                            // zeile senden
                            sp.Write(zeile);

                            if (Properties.Settings.Default.logLevel > 0) 
                                Log.write(mainForm, ("Try: " + (j).ToString() + " " + zeile), 1);
                        } 
                        else 
                            break;
                    }
                    if (temp == "ERR\r") 
                    {
                        MessageBox.Show(errorLabels.GetString("flashError"), "Error!", 
                            MessageBoxButtons.OK, MessageBoxIcon.Error);
                        break;
                    }
                    // log schreiben wenn debuglevel

                    if (Properties.Settings.Default.logLevel > 0) 
                        Log.write(mainForm, temp, 1);

                }
                // wenn pic erfolgreich geschrieben wurde
                mainForm.infoleisteToolStripStatusLabel.Text = labels.GetString("BurnPICOk");
                Log.write(mainForm, labels.GetString("BurnPICOk"), 0);
                mainForm.picBootModus = false;
            } 
            catch (Exception e) 
            {
                MessageBox.Show(errorLabels.GetString("errorBurn"),"Error!",MessageBoxButtons.OK,MessageBoxIcon.Error);
                Log.write(mainForm, errorLabels.GetString("errorBurn") + "\r\n" + e.ToString(), 1);
                //verbindung trennen um für neuversuch mit status "B" geschlossen zu sein
                closeConnection(mainForm);
            }

            // Info um Board neu zu starten
            MessageBox.Show(labels.GetString("flashInfo"),"Info!",MessageBoxButtons.OK,MessageBoxIcon.Information);

            // Progressbar ausblenden und Cursor auf default setzen
            mainForm.toolStripProgressBar.Value = 0;
            mainForm.toolStripProgressBar.Visible = false;
            mainForm.toolStripProgressBar.Maximum = 100;
            mainForm.Cursor = Cursors.Default;
            mainForm.Enabled = true;
            
        }

        public ArrayList ContinueKey (FormMain mainForm)
        {
            string key = "x";
            bool con = true;
            // wenn noch keine Verbindung besteht automatisch öffnen
            if (!sp.IsOpen)
                con = connect(mainForm, false, true);
            else
                sp.DiscardInBuffer();

            // Puffer leeren wenn Boad neu gestartet wird kommt info sonst in den buffer

            ArrayList ret = new ArrayList();

            // wenn verbindung nicht ok
            if (con == false)
                return ret;

            string temp;

            try
            {
                // keine Zeiteinsparung mit write statt writeline - ende mit > leichter erfassbar.
                // deshalb wieder auf writeline umgestellt
                sp.Write(key);
                // log über sendekey
                if (Properties.Settings.Default.logLevel > 0)
                {
                    Log.write(mainForm, labels.GetString("writeParameter") + key, 1);
                }

                Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                temp = sp.ReadLine();
                // > kommt wenn nichts mehr kommt - aber nur bei writeLine!
                // für was war T: ???


                // auf ERR abfragen wenn der PIC sich im B status befindet
                if (temp == "ERR\r")
                {
                    Log.write(mainForm, "Board is in Bootloader Status!", 1);
                    ret.Add(temp);
                    mainForm.picBootModus = true;
                    MessageBox.Show(errorLabels.GetString("picBurnMode"), "Info!", MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                    return ret;
                }

                while (temp != ">") // T wird nur für Receiver benötigt wenn nur write und nicht writeline 
                // || temp.Substring(0,1) == "T")
                {
                    ret.Add(temp);

                    Thread.Sleep(Properties.Settings.Default.iReadSleep);  
                    temp = sp.ReadLine();

                    // log schreiben wenn debuglevel
                    if (Properties.Settings.Default.logLevel > 0)
                        Log.write(mainForm, temp, 1);
                }
            }
            catch (Exception e)
            {
                if (con == false)
                    MessageBox.Show(errorLabels.GetString("askPic"), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);

                Log.write(mainForm, e.ToString(), 1);
            }
            return ret;
        }

        public ArrayList AbortKey(FormMain mainForm)
        {
            string key = "z";
            bool con = true;
            // wenn noch keine Verbindung besteht automatisch öffnen
            if (!sp.IsOpen)
                con = connect(mainForm, false, true);
            else
                sp.DiscardInBuffer();

            // Puffer leeren wenn Boad neu gestartet wird kommt info sonst in den buffer

            ArrayList ret = new ArrayList();

            // wenn verbindung nicht ok
            if (con == false)
                return ret;

            string temp;

            try
            {
                sp.Write(key);
                if (Properties.Settings.Default.logLevel > 0)
                {
                    Log.write(mainForm, labels.GetString("writeParameter") + key, 1);
                }

                Thread.Sleep(Properties.Settings.Default.iReadSleep);
                temp = sp.ReadLine();

                if (temp == "ERR\r")
                {
                    Log.write(mainForm, "Board is in Bootloader Status!", 1);
                    ret.Add(temp);
                    mainForm.picBootModus = true;
                    MessageBox.Show(errorLabels.GetString("picBurnMode"), "Info!", MessageBoxButtons.OK,
                        MessageBoxIcon.Warning);
                    return ret;
                }

                while (temp != ">") // T wird nur für Receiver benötigt wenn nur write und nicht writeline 
                // || temp.Substring(0,1) == "T")
                {
                    ret.Add(temp);

                    Thread.Sleep(Properties.Settings.Default.iReadSleep);
                    temp = sp.ReadLine();

                    if (Properties.Settings.Default.logLevel > 0)
                        Log.write(mainForm, temp, 1);
                }
            }
            catch (Exception e)
            {
                if (con == false)
                    MessageBox.Show(errorLabels.GetString("askPic"), "Error!", MessageBoxButtons.OK, MessageBoxIcon.Error);

                Log.write(mainForm, e.ToString(), 1);
            }
            return ret;
        }

    }
}
