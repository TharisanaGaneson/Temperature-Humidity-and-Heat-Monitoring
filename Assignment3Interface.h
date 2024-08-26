const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" rel="stylesheet">
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/chartjs-plugin-datalabels"></script> <!-- Plugin for data labels -->
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h1 { font-size: 2.0rem; }
      p { font-size: 2.0rem; }
      .chart-container {
        width: 90%;
        margin: auto;
      }
      .stats {
        margin-top: 20px;
        font-size: 1.5rem;
      }
      .stats p {
        margin: 5px 0;
      }
    </style>
  </head>
  <body>
    <h1>TEMPERATURE AND HUMIDITY MONITORING</h1>

    <div class="chart-container">
      <canvas id="sensorChart"></canvas>
    </div>

    <!-- Stats Section -->
    <div class="stats">
      <h2>Statistics</h2>
      <div id="temperatureStats">
        <p><b>Temperature:</b> Min: <span id="minTemp">N/A</span>°C, Max: <span id="maxTemp">N/A</span>°C, Avg: <span id="avgTemp">N/A</span>°C</p>
      </div>
      <div id="humidityStats">
        <p><b>Humidity:</b> Min: <span id="minHumidity">N/A</span>%, Max: <span id="maxHumidity">N/A</span>%, Avg: <span id="avgHumidity">N/A</span>%</p>
      </div>
      <div id="heatIndexStats">
        <p><b>Heat Index:</b> Min: <span id="minHeatIndex">N/A</span>°C, Max: <span id="maxHeatIndex">N/A</span>°C, Avg: <span id="avgHeatIndex">N/A</span>°C</p>
      </div>
    </div>

    <script>
      var previousValues = [null, null, null];  // Store previous values for Temperature, Humidity, and Heat Index
      var sensorChartCtx = document.getElementById('sensorChart').getContext('2d');

      var sensorChart = new Chart(sensorChartCtx, {
        type: 'line',
        data: {
          labels: [],
          datasets: [
            {
              label: 'Temperature (°C)',
              backgroundColor: 'rgba(255, 99, 132, 0.2)',
              borderColor: 'rgba(255, 99, 132, 1)',
              fill: false,
              data: []
            },
            {
              label: 'Humidity (%)',
              backgroundColor: 'rgba(54, 162, 235, 0.2)',
              borderColor: 'rgba(54, 162, 235, 1)',
              fill: false,
              data: []
            },
            {
              label: 'Heat Index (°C)',
              backgroundColor: 'rgba(255, 206, 86, 0.2)',
              borderColor: 'rgba(255, 206, 86, 1)',
              fill: false,
              data: []
            }
          ]
        },
        options: {
          scales: {
            x: {
              display: true,
              title: {
                display: true,
                text: 'Time'
              }
            },
            y: {
              display: true,
              title: {
                display: true,
                text: 'Values'
              }
            }
          },
          plugins: {
            datalabels: {
              anchor: 'end',
              align: 'top',
              formatter: function(value, context) {
                var datasetIndex = context.datasetIndex;
                var previousValue = previousValues[datasetIndex];

                // Show label only if value is different from the previous value
                if (value !== previousValue) {
                  previousValues[datasetIndex] = value;  // Update previous value
                  return value.toFixed(2);
                } else {
                  return '';  // Don't show the label if the value hasn't changed
                }
              },
              font: {
                weight: 'bold'
              }
            }
          }
        },
        plugins: [ChartDataLabels]  // Activate the plugin
      });

      // Update the chart with new data every 2 seconds
      setInterval(function() {
        updateData();
      }, 2000);

      // Function to fetch sensor data and update the chart
      function updateData() {
        getSensorData('readTemperature', 0);  // Update Temperature dataset
        getSensorData('readHumidity', 1);     // Update Humidity dataset
        getSensorData('readHeatIndex', 2);    // Update Heat Index dataset
      }

      // Function to fetch sensor data from the server
      function getSensorData(endpoint, datasetIndex) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState === 4 && this.status === 200) {
            var newValue = parseFloat(this.responseText);
            var labels = sensorChart.data.labels;
            var currentTime = new Date().toLocaleTimeString();

            // Add new label if needed
            if (labels.length === 0 || labels[labels.length - 1] !== currentTime) {
              labels.push(currentTime);
            }

            // Add new data to the dataset
            sensorChart.data.datasets[datasetIndex].data.push(newValue);

            // Limit the number of displayed data points (e.g., to 10 points)
            if (sensorChart.data.datasets[datasetIndex].data.length > 10) {
              sensorChart.data.datasets[datasetIndex].data.shift();  // Remove the oldest data point
              if (labels.length > 10) labels.shift();  // Remove the oldest label
            }

            // Update the chart
            sensorChart.update();
          }
        };
        xhttp.open('GET', endpoint, true);
        xhttp.send();
      }

      // Function to update stats every 1 minute
      setInterval(function() {
        updateStats();
      }, 60000);

      // Function to fetch stats from EEPROM and update the stats section
      function updateStats() {
        getStats('getTemperatureStats', 'minTemp', 'maxTemp', 'avgTemp');
        getStats('getHumidityStats', 'minHumidity', 'maxHumidity', 'avgHumidity');
        getStats('getHeatIndexStats', 'minHeatIndex', 'maxHeatIndex', 'avgHeatIndex');
      }

      // Function to fetch stats from the server and update the DOM elements
      function getStats(endpoint, minId, maxId, avgId) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState === 4 && this.status === 200) {
            var stats = JSON.parse(this.responseText);
            document.getElementById(minId).innerText = stats.min.toFixed(2);
            document.getElementById(maxId).innerText = stats.max.toFixed(2);
            document.getElementById(avgId).innerText = stats.avg.toFixed(2);
          }
        };
        xhttp.open('GET', endpoint, true);
        xhttp.send();
      }
    </script>
  </body>
</html>
)=====";
