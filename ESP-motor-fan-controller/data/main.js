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
        <label for="fanTurnOnTemp">Turn on temp:</label>
        <input id="fanTurnOnTemp" type="number" min="20" max="100" step="1" value="${config.fanTurnOnTemp}"> °C
    </div>
    <div class="field">
        <label for="fanTurnOffTemp">Turn off temp:</label>
        <input id="fanTurnOffTemp" type="number" min="20" max="100" step="1" value="${config.fanTurnOffTemp}"> °C
    </div>
    <div class="field">
        <label for="maxFanSpeed">Max fan speed:</label>
        <input id="maxFanSpeed" type="number" min="0" max="1023" step="1" value="${config.maxFanSpeed}">
    </div>
    <div class="field">
        <label for="minFanSpeed">Min fan speed:</label>
        <input id="minFanSpeed" type="number" min="0" max="1023" step="1" value="${config.minFanSpeed}">
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
        fanTurnOnTemp: parseInt(document.getElementById('fanTurnOnTemp').value),
        fanTurnOffTemp: parseInt(document.getElementById('fanTurnOffTemp').value),
        maxFanSpeed: parseInt(document.getElementById('maxFanSpeed').value),
        minFanSpeed: parseInt(document.getElementById('minFanSpeed').value),
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
    renderCurrentTemperature(msg.data)
}

function renderCurrentTemperature(value) {
    document.getElementById('currentValue').innerHTML = value
}
