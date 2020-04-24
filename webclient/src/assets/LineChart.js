// import { Line, mixins } from '_VueChartJs'
// const { reactiveProp } = mixins

export default {
  extends: window._VueChartJs.Line,
  mixins: [window._VueChartJs.mixins.reactiveProp],
  props: ['options', 'chartData'],
  mounted () {
    // this.chartData is created in the mixin.
    // If you want to pass options please create a local options object
    this.renderChart(this.chartData, this.options)
  }
}