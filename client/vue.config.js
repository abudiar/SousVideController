const HtmlWebpackPlugin = require('html-webpack-plugin')
const HtmlWebpackInlineSourcePlugin = require('html-webpack-inline-source-plugin');
module.exports = {
  css: {
    extract: false,
  },
  productionSourceMap: false,
  configureWebpack: {
    optimization: {
      splitChunks: false // makes there only be 1 js file - leftover from earlier attempts but doesn't hurt
    },
  }
}