const ESP_IP = "192.168.10.117";

async function sendCommand(type, param1, param2) {
    let url = `http://${ESP_IP}/action?type=${type}`;
    if(type === "straight") {
        url += `&distance=${param1}`;
    } else if(type === "turn"){
        url += `&angle=${param1}&radius=${param2}`;
    } else if(type === "speed"){
        url += `&rpm=${param1}`;
    } else if(type === "scan") {
        // scan needs no params
    } else {
        console.error("Unknown command type:", type);
        return;  // Don't send request
    }

    try {
        const response = await fetch(url);
        const data = await response.json();
        console.log("Response:" , data);
        document.getElementById("range").textContent = data.distance || "--"; 
    } catch(error) {
        console.error("Error:", error);
    }
}

document.getElementById("scanBtn").addEventListener("click", () => {
    sendCommand("scan");
});

document.getElementById("forwardBtn").addEventListener("click", () => {
    sendCommand("straight",50);
});
document.getElementById("backBtn").addEventListener("click", () => {
    sendCommand("straight",-50);
});

document.getElementById("leftBtn").addEventListener("click", () => {
    sendCommand("turn",30,0);
});

document.getElementById("rightBtn").addEventListener("click", () => {
    sendCommand("turn",-30,0);
});