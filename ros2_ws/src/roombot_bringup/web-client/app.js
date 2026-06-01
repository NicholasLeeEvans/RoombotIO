const ESP_IP = "192.168.10.117";

const rangeChartctx = document.getElementById("rangeChart");
const telemetryCanvas = /** @type {HTMLCanvasElement} */ (document.getElementById("telemetryCanvas"));
const teleCtx = telemetryCanvas.getContext('2d');

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

let lastPoint = null;
let points = [];
es.addEventListener("telemetry", (e) => {
    const p = JSON.parse(e.data);
    const angle_rad = p.angle * Math.PI / 180;
    const MAX_RANGE = 500;
    if(p.range <= MAX_RANGE){
        p.wx = p.x + p.range * Math.cos(angle_rad);
        p.wy = p.y + p.range * Math.sin(angle_rad);
    }
   if(!lastPoint || p.x != lastPoint.x || p.y != lastPoint.y || p.angle != lastPoint.angle){
        points.push(p);
        lastPoint = p;
        drawMap();
    }
});

function drawMap() {
    //draw a line between each of the robot points, for the lastPoint, draw the robot.
    const W = telemetryCanvas.width;
    const H = telemetryCanvas.height;
    const scale = 0.5;

    function toCanvas(x, y){
        return [W/2 + x * scale, H/2 - y * scale];
    }

    teleCtx.clearRect(0, 0, W, H);

    teleCtx.beginPath();
    teleCtx.strokeStyle = "red";
    for(let i = 0; i < points.length; i++){
        const [cx, cy] = toCanvas(points[i].x, points[i].y);
        i === 0 ? teleCtx.moveTo(cx, cy) : teleCtx.lineTo(cx, cy);
    }
    teleCtx.stroke();

    //draw a mark for each calculated wall hit
    for(const p of points){
        if(p.wx != undefined){
            const [wx, wy] = toCanvas(p.wx, p.wy);
            //draw small x
            const len = 8;
            teleCtx.beginPath();
            teleCtx.strokeStyle = "black";

            teleCtx.moveTo((wx - len), (wy - len));
            teleCtx.lineTo((wx + len), (wy + len));

            teleCtx.moveTo((wx - len), (wy + len));
            teleCtx.lineTo((wx + len), (wy - len));

            teleCtx.stroke();
        }
   }

    //draw robot
    const last = points[points.length - 1];
    const [rx, ry] = toCanvas(last.x, last.y);
    //draw arrow
    const arrow_len = 20;
    const angle_rad = last.angle * Math.PI / 180;
    const rx1 = rx + arrow_len * Math.cos(angle_rad);
    const ry1 = ry - arrow_len * Math.sin(angle_rad);
    teleCtx.beginPath();
    teleCtx.strokeStyle = "blue";
    teleCtx.moveTo(rx, ry);
    teleCtx.lineTo(rx1, ry1);
    teleCtx.stroke();

}

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

document.getElementById("leftBtnArcTight").addEventListener("click", () => {
    sendCommand("turn",45,60);
});
document.getElementById("leftBtnArcFineTight").addEventListener("click", () => {
    sendCommand("turn",15,60);
});
document.getElementById("leftBtnArc").addEventListener("click", () => {
    sendCommand("turn",45,120);
});
document.getElementById("leftBtnArcFine").addEventListener("click", () => {
    sendCommand("turn",15,120);
});

document.getElementById("rightBtnArcTight").addEventListener("click", () => {
    sendCommand("turn",-45,60);
});
document.getElementById("rightBtnArcFineTight").addEventListener("click", () => {
    sendCommand("turn",-15,60);
});
document.getElementById("rightBtnArc").addEventListener("click", () => {
    sendCommand("turn",-45,120);
});
document.getElementById("rightBtnArcFine").addEventListener("click", () => {
    sendCommand("turn",-15,120);
});

document.addEventListener("keydown", (e) => {
    // if (e.repeat) return;  // ignore held keys
    const shift = e.shiftKey;
    switch(e.key) {
        case "w": sendCommand("straight", 100); break;
        case "W": sendCommand("straight", 10); break;
        case "s": sendCommand("straight", -100); break;
        case "S": sendCommand("straight", -10); break;
        case "a": sendCommand("turn", 45, 0); break;
        case "A": sendCommand("turn", 15, 0); break;
        case "d": sendCommand("turn", -45, 0); break;
        case "D": sendCommand("turn", -15, 0); break;
    }
});