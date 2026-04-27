const express = require("express");
const cors = require("cors");
const path = require("path");

const app = express();
const PORT = 3000;

app.use(express.json());
app.use(cors());

// store latest sensor data
let sensorData = {
  roomTemp: 0,
  humidity: 0,
  bodyTemp: 0,
  bpm: 0
};

// ESP32 sends data here
app.post("/sensor", (req, res) => {

  sensorData = req.body;

  console.log("----- AXOMED SENSOR DATA -----");
  console.log(sensorData);

  res.send("OK");
});

// frontend reads data here
app.get("/sensor", (req, res) => {
  res.json(sensorData);
});

// serve HTML dashboard
app.get("/", (req, res) => {
res.sendFile(path.join(__dirname, "../web/index.html"));});

app.listen(PORT, "0.0.0.0", () => {
  console.log(`AXOMED Server running on http://localhost:${PORT}`);
});