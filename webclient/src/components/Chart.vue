<template>
  <div class="small">
    <line-chart class="chart" :chartData="datacollection" :options="chartOptions"></line-chart>
  </div>
</template>

<script>
import LineChart from "@/assets/LineChart.js";

export default {
  name: "Chart",
  components: {
    LineChart
  },
  data() {
    return {
      datacollection: null,
      chartOptions: {
        // curveType: "line",
        // chartArea: {
        //   left: "10%",
        //   top: 20,
        //   right: "10%",
        //   bottom: 30
        // },
        // series: {
        //   2: { targetAxisIndex: 0 },
        //   0: { targetAxisIndex: 0 },
        //   1: { targetAxisIndex: 1 }
        // },
        // vAxes: {
        //   0: { title: "Temps (Celsius)", minValue: 0, maxValue: 100 },
        //   1: {
        //     title: "Power Output",
        //     minValue: 0,
        //     maxValue: 1,
        //     format: "percent"
        //   }
        // },
        // vTicks: {
        //   1: {}
        // },
        scales: {
          yAxes: [
            {
              display: true,
              position: "right",
              ticks: {
                beginAtZero: true,
                max: 100,
                min: 0
              },
              scaleLabel: {
                display: true,
                labelString: "Duty %"
              }
            },
            {
              display: true,
              position: "left",
              ticks: {
                beginAtZero: true,
                max: 100,
                min: 0
              },
              scaleLabel: {
                display: true,
                labelString: "Temp °C"
              }
            }
          ]
        },
        legend: { position: "bottom" },
        maintainAspectRatio: false,
        tooltips: {
          mode: "label"
        },
        elements: {
          point: {
            radius: 0,
            pointStyle: "cross",
            hitRadius: 2
          }
        },
        plugins: {
          zoom: {
            pan: {
              enabled: true,
              mode: "x"
            },
            zoom: {
              enabled: true,
              mode: "x"
            }
          }
        }
      },
      csvLog: ""
    };
  },
  methods: {
    getCSV() {
      this.axios
        .get("/log.csv")
        .then(response => {
          if (this.csvLog != response.data) {
            this.csvLog = response.data;
          }
        })
        .catch(err => {
          console.log("getCSV", err);
        });
    },
    makeChart(temps) {
      console.log("Raw CSV", temps);

      let timeData = temps.map(function(d) {
        return d.Time;
      });
      let temperatureData = temps.map(function(d) {
        return Number(d.Temperature);
      });
      let powerData = temps.map(function(d) {
        return (Number(d.Power) * 100).toFixed(2);
      });
      let targetData = temps.map(function(d) {
        return Number(d.Target);
      });

      this.datacollection = {
        labels: timeData,
        datasets: [
          {
            label: "Temperature",
            data: temperatureData,
            backgroundColor: "rgba(0,0,0,0)",
            borderColor: "red"
          },
          {
            label: "Duty",
            data: powerData,
            backgroundColor: "rgba(208,240,192,0.5)",
            borderColor: "green"
          },
          {
            label: "Target",
            data: targetData,
            backgroundColor: "rgba(0,0,0,0)",
            borderColor: "blue"
          }
        ]
      };
    }
  },
  watch: {
    csvLog() {
      console.log("Updating Chart");
      let arr = this.csvLog.split("\n");
      var jsonObj = [];
      var headers = arr[0].split(",");
      for (var i = 1; i < arr.length; i++) {
        var data = arr[i].split(",");
        var obj = {};
        for (var j = 0; j < data.length; j++) {
          obj[headers[j].trim()] = data[j].trim();
        }
        jsonObj.push(obj);
      }
      this.makeChart(jsonObj);
    }
  },
  created() {
    this.getCSV();
    setInterval(() => {
      console.log("Getting CSV!");
      this.getCSV();
    }, 15000);
  }
};
</script>

<style>
.chart {
  max-height: 20em;
}
</style>
