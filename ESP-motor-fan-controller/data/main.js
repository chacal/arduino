main()

function main() {
    connectWs()
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
