const ESP_IP = "192.168.10.117";

const rangeChartctx = document.getElementById("rangeChart");

const rangeChartConfig = {
  type: 'line',
  data: {
    labels: [],
    datasets: [{
        label: 'Range (mm)',
        data: [],
    }]
  },
  options: {
    animation: false,
    scales: {
        y: {
            min: 0,
            max: 1200,
            ticks:
            {
                stepSize: 100,
            }
        }
    }
  }
};

const rangeChart = new Chart(rangeChartctx, rangeChartConfig);
const max_labels = 50;
const es = new EventSource(`http://${ESP_IP}/events`);
es.addEventListener("range", (e) => { 
    document.getElementById("range").textContent = e.data; 
    const d = rangeChartConfig.data.datasets[0].data;
    const l = rangeChartConfig.data.labels;
    d.push(Number(e.data));
    if (l.length < max_labels) {
        l.push(l.length);
    }
    if (d.length > max_labels) {
        d.shift();
    }
    rangeChart.update();
});


async function sendCommand(type, param1, param2) {
    let url = `http://${ESP_IP}/action?type=${type}`;
    if(type === "straight") {
        url += `&distance=${param1}`;
    } else if(type === "turn"){
        url += `&angle=${param1}&radius=${param2}`;
    } else if(type === "speed"){
        url += `&rpm=${param1}`;
    } else {
        console.error("Unknown command type:", type);
        return;  // Don't send request
    }

    try {
        const response = await fetch(url);
        const data = await response.json();
        console.log("Response:" , data);
    } catch(error) {
        console.error("Error:", error);
    }
}

document.getElementById("forwardBtn").addEventListener("click", () => {
    sendCommand("straight",100);
});
document.getElementById("forwardBtnFine").addEventListener("click", () => {
    sendCommand("straight",10);
});

document.getElementById("backBtn").addEventListener("click", () => {
    sendCommand("straight",-100);
});
document.getElementById("backBtnFine").addEventListener("click", () => {
    sendCommand("straight",-10);
});

document.getElementById("leftBtn").addEventListener("click", () => {
    sendCommand("turn",45,0);
});
document.getElementById("leftBtnFine").addEventListener("click", () => {
    sendCommand("turn",15,0);
});

document.getElementById("rightBtn").addEventListener("click", () => {
    sendCommand("turn",-45,0);
});
document.getElementById("rightBtnFine").addEventListener("click", () => {
    sendCommand("turn",-15,0);
});