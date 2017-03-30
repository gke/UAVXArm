using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;

namespace UAVXGUI
{


    public partial class PlotForm : Form
    {

        private void FillChart()
        {
            int blockSize = 100;

            // generates random data (i.e. 30 * blockSize random numbers)
            Random rand = new Random();
            var valuesArray1 = Enumerable.Range(0, blockSize * 30).Select(x => rand.Next(1, 10)).ToArray();
            var valuesArray2 = Enumerable.Range(0, blockSize * 30).Select(x => rand.Next(1, 10)).ToArray();

            // clear the chart
            AccKpChart.Series.Clear();

            // fill the chart
            var series1 = AccKpChart.Series.Add("Acc");
            series1.ChartType = SeriesChartType.Line;
            series1.XValueType = ChartValueType.Int32;

            var series2 = AccKpChart.Series.Add("Angle");
            series2.ChartType = SeriesChartType.Line;
            series2.XValueType = ChartValueType.Int32;


            for (int i = 0; i < valuesArray1.Length; i++) { 
                series1.Points.AddXY(i, valuesArray1[i]);
                series2.Points.AddXY(i, valuesArray2[i]);
            }

            var chartArea = AccKpChart.ChartAreas[series1.ChartArea];

            // set view range to [0,max]
            chartArea.AxisX.Minimum = 0;
            chartArea.AxisX.Maximum = valuesArray1.Length;

            // enable autoscroll
            chartArea.CursorX.AutoScroll = true;

            // let's zoom to [0,blockSize] (e.g. [0,100])
            chartArea.AxisX.ScaleView.Zoomable = true;
            chartArea.AxisX.ScaleView.SizeType = DateTimeIntervalType.Number;
            int position = 0;
            int size = blockSize;
            chartArea.AxisX.ScaleView.Zoom(position, size);

            // disable zoom-reset button (only scrollbar's arrows are available)
             chartArea.AxisX.ScrollBar.ButtonStyle = ScrollBarButtonStyles.SmallScroll;

            // set scrollbar small change to blockSize (e.g. 100)
            chartArea.AxisX.ScaleView.SmallScrollSize = blockSize;
        }

        public PlotForm()
        {
            InitializeComponent();

            FillChart();
        }

     
    }

}
