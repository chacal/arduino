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