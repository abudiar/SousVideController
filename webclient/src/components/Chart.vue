<template>
  <div class="small">
    <line-chart class="chart" :chartData="datacollection" :options="chartOptions"></line-chart>
  </div>
</template>

<script>
  import LineChart from '@/assets/LineChart.js'

  export default {
    name: 'Chart',
    components: {
      LineChart
    },
    data () {
      return {
        datacollection: null,
        chartOptions: {
            curveType: "line",
            chartArea: {
                left: "10%",
                top: 20,
                right: "10%",
                bottom: 30
            },
            series: {
                2: { targetAxisIndex: 0 },
                0: { targetAxisIndex: 0 },
                1: { targetAxisIndex: 1 }
            },
            vAxes: {
                // Adds titles to each axis.
                0: { title: "Temps (Celsius)", minValue: 0, maxValue: 100 },
                1: {
                    title: "Power Output",
                    minValue: 0,
                    maxValue: 1,
                    format: "percent"
                }
            },
            vTicks: {
                1: {}
            },
            legend: { position: "bottom" },
            maintainAspectRatio: false
        }
      }
    },
    // mounted () {
    //   this.fillData()
    // },
    methods: {
        getCSV() {
            // this.$d3
            // .csv("/log.csv")
            // .then(this.makeChart);
            this.axios.get('/log.csv')
                .then(response => {
                    console.log('getCSV', response);
                    let arr = response.data.split('\n'); 
                    var jsonObj = [];
                    var headers = arr[0].split(',');
                    for(var i = 1; i < arr.length; i++) {
                    var data = arr[i].split(',');
                    var obj = {};
                    for(var j = 0; j < data.length; j++) {
                        obj[headers[j].trim()] = data[j].trim();
                    }
                    jsonObj.push(obj);
                    }
                    this.makeChart(jsonObj);
                })
                .catch(err => {
                    console.log('getCSV', err);
                })
        },
        makeChart(temps) {
            // players is an array of objects where each object is something like:
            // {
            //   "Time": "Steffi Graf",
            //   "Temperature": "377",
            //   "Output": "Female",
            //   "Target": 32
            // }
            console.log('Raw CSV', temps);

            let timeData = temps.map(function(d) {
                return d.Time;
            });
            let temperatureData = temps.map(function(d) {
                return d.Temperature;
            });
            let outputData = temps.map(function(d) {
                return d.Output;
            });
            let targetData = temps.map(function(d) {
                return d.Target;
            });

            this.datacollection = {
                labels: ['Time', 'Temperature', 'Output', 'Target'],
                datasets: [
                    {
                    label: 'Time',
                    data: timeData
                    }, 
                    {
                    label: 'Temperature',
                    data: temperatureData
                    }, 
                    {
                    label: 'Output',
                    data: outputData
                    }, 
                    {
                    label: 'Target',
                    data: targetData
                    }
                ]
            }
        }
    },
    created() {
        this.getCSV();
        setInterval(() => {
            console.log('Getting CSV!')
            this.getCSV();
        }, 15000)
    },
  }
</script>

<style>
  /* .small {
    max-height: 10em;
  } */
  .chart {
    max-height: 20em;
  } 
  /* canvas {
      height: 20em !important;
    width: 100% !important;
  } */
</style>