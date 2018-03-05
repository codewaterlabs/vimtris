const path = require('path');
const UglifyJSPlugin = require('uglifyjs-webpack-plugin');

  module.exports = {
    entry: './lib/js/src/index.js',
    mode: 'production',
    output: {
      path: path.resolve(__dirname, 'dist'),
      filename: 'main.js'
    },
    plugins: [
      new UglifyJSPlugin()
    ]
  };