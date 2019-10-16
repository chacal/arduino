main()

function main() {
  connectWs()
  getConfig()
    .then(config => {
      document.getElementById('config').outerHTML = renderConfigForm(config)
    })
}

function renderConfigForm(config) {
  return `
<div id="config">
    <h3>Configuration</h3>
    <div class="field">
        <label for="dstHost">Destination:</label>
        <input id="dstHost" type="text" value="${config.dstHost}">
    </div>
    <div class="field">
        <label for="pulseReportingPort">Pulse reporting port:</label>
        <input id="pulseReportingPort" type="number" min="0" max="65535" step="1" value="${config.pulseReportingPort}">
    </div>
    <div class="field">
        <label for="maxPulseLength">Max pulse length:</label>
        <input id="maxPulseLength" type="number" min="0" max="2000" step="5" value="${config.maxPulseLength}">
    </div>
    <div class="field">
        <label for="pulseStartCoef">Pulse start coef:</label>
        <input id="pulseStartCoef" type="number" min="0" max="5" step="0.1" value="${config.pulseStartCoef}">
    </div>
    <div class="field">
        <label for="pulseEndCoef">Pulse end coef:</label>
        <input id="pulseEndCoef" type="number" min="0" max="5" step="0.1" value="${config.pulseEndCoef}">
    </div>
    <div>
        <input type="button" id="saveConfig" onclick="saveConfig()" value="Save"/>
      </div>
</div>`
}

function getConfig() {
  return fetch('/config')
    .then(res => res.json())
}

function saveConfig() {
  const config = {
    dstHost: document.getElementById('dstHost').value,
    pulseReportingPort: parseInt(document.getElementById('pulseReportingPort').value),
    maxPulseLength: parseInt(document.getElementById('maxPulseLength').value),
    pulseStartCoef: parseFloat(document.getElementById('pulseStartCoef').value),
    pulseEndCoef: parseFloat(document.getElementById('pulseEndCoef').value)
  }
  fetch('/config', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(config)
  })
    .then(() => main())
}

function connectWs() {
  const connection = new WebSocket('ws://' + location.hostname + '/ws')
  connection.onerror = (error) => console.log('WebSocket Error ', error)
  connection.onmessage = onWsMessage
}

function onWsMessage(msg) {
  renderCurrentADCValue(msg.data)
}

function renderCurrentADCValue(value) {
  document.getElementById('currentValue').innerHTML = value
}

function loadSamples() {
  return fetch('/samples')
    .then(res => res.arrayBuffer())
    .then(buf => renderSamples(new Uint16Array(buf)))
}

let chart

function renderSamples(samples) {
  document.getElementById('samples').innerHTML = samples.join('\n')

  if (chart === undefined) {
    const labels = []
    for (let i = 0; i <= samples.length; i++) {
      labels.push(i)
    }

    const ctx = document.getElementById('sampleGraph')
    chart = new Chart(ctx, {
      type: 'line',
      data: {
        labels,
        datasets: [{
          data: samples,
          label: 'Samples',
          pointRadius: 0,
          borderWidth: 1
        }]
      },
      options: {
        animation: false,
        tooltips: {
          mode: 'index',
          intersect: false
        }
      }
    })
  } else {
    const labels = []
    for (let i = 0; i <= samples.length; i++) {
      labels.push(i)
    }
    chart.data.labels = labels
    chart.data.datasets[0].data = samples
    chart.update()
  }
}