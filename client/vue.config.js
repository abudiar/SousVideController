module.exports = {
    chainWebpack: config => {
      config.externals({
        //   'vue': 'vue',
        //   'chartjs': 'chartjs',
        // 'vue-chartjs': 'vue-chartjs',
        // 'VueChartJs': 'vue-chartjs',
      })
    }
  }