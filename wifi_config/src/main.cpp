#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h> // 用于非易失性存储

// 定义AP模式的SSID和密码
const char* apSSID = "ESP32-AP";
const char* apPassword = "12345678";

// 定义按键引脚
const int resetButtonPin = 1; // GPIO 0

// 创建Web服务器对象
WebServer server(80);

// 创建Preferences对象用于NVS存储
Preferences preferences;

// 保存Wi-Fi配置
String savedSSID = "";
String savedPassword = "";

// 根路径处理函数
void handleRoot() {
  String html = R"=====(
<html>
<head>
  <title>ESP32 Wi-Fi Config</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f4f4f4;
      text-align: center;
      padding: 50px;
    }
    h1 {
      color: #333;
    }
    form {
      background: #fff;
      padding: 20px;
      border-radius: 8px;
      display: inline-block;
      box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
    }
    input[type="text"], input[type="password"] {
      width: 100%;
      padding: 10px;
      margin: 10px 0;
      border: 1px solid #ddd;
      border-radius: 4px;
    }
    input[type="submit"] {
      background: #28a745;
      color: #fff;
      border: none;
      padding: 10px 20px;
      border-radius: 4px;
      cursor: pointer;
    }
    input[type="submit"]:hover {
      background: #218838;
    }
    .wifi-list {
      margin-top: 20px;
      text-align: left;
    }
    .wifi-item {
      padding: 10px;
      border-bottom: 1px solid #ddd;
      cursor: pointer;
    }
    .wifi-item:hover {
      background: #f1f1f1;
    }
    .wifi-signal {
      float: right;
      color: #666;
    }
  </style>
  <script>
    function fillSSID(ssid) {
      document.getElementById("ssid").value = ssid;
    }
    function scanWiFi() {
      fetch("/scan")
        .then(response => response.json())
        .then(data => {
          let wifiList = document.getElementById("wifi-list");
          wifiList.innerHTML = "";
          data.forEach(wifi => {
            let item = document.createElement("div");
            item.className = "wifi-item";
            item.innerHTML = `
              ${wifi.ssid}
              <span class="wifi-signal">${wifi.rssi} dBm</span>
            `;
            item.onclick = () => fillSSID(wifi.ssid);
            wifiList.appendChild(item);
          });
        });
    }
    window.onload = scanWiFi;
  </script>
</head>
<body>
  <h1>ESP32 Wi-Fi Config</h1>
  <form action="/config" method="POST">
    SSID: <input type="text" id="ssid" name="ssid"><br>
    Password: <input type="password" name="password"><br>
    <input type="submit" value="Connect">
  </form>
  <div class="wifi-list" id="wifi-list"></div>
</body>
</html>
)=====";
  server.send(200, "text/html", html);
}

// 扫描Wi-Fi处理函数
// 扫描Wi-Fi处理函数
void handleScan() {
  String json = "[";
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    if (i) json += ",";
    json += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + WiFi.RSSI(i) + "}";
  }
  json += "]";
  server.send(200, "application/json", json);
}

// 配置处理函数
void handleConfig() {
  // 获取表单数据
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  // 保存到NVS
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);

  // 尝试连接Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("Connecting to WiFi...");

  // 等待连接
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // 检查连接状态
  if (WiFi.status() == WL_CONNECTED) {
    server.send(200, "text/plain", "WiFi connected! IP: " + WiFi.localIP().toString());
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    server.send(200, "text/plain", "Failed to connect to WiFi.");
    Serial.println("\nFailed to connect to WiFi.");
  }
}

// 重置Wi-Fi配置
void resetWiFiConfig() {
  preferences.remove("ssid");
  preferences.remove("password");
  Serial.println("Wi-Fi configuration reset.");
  ESP.restart(); // 重启ESP32
}

void wifi_init() {
   // 初始化按键引脚
  pinMode(resetButtonPin, INPUT);

  // 初始化NVS
  preferences.begin("wifi-config", false);

  // 尝试加载保存的Wi-Fi配置
  savedSSID = preferences.getString("ssid", "");
  savedPassword = preferences.getString("password", "");

  if (savedSSID != "") {
    // 尝试连接保存的Wi-Fi
    WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
    Serial.println("Connecting to saved WiFi...");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nConnected to saved WiFi!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      return; // 连接成功，无需进入配网模式
    }
  }

  // 启动AP模式
  WiFi.softAP(apSSID, apPassword);
  Serial.println("AP Mode Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // 设置Web服务器路由
  server.on("/", handleRoot);       // 处理根路径
  server.on("/scan", handleScan);   // 处理Wi-Fi扫描请求
  server.on("/config", handleConfig); // 处理配置请求

  // 启动Web服务器
  server.begin();
  Serial.println("HTTP server started");
}
void setup() {
  Serial.begin(115200);

  wifi_init();
}

void loop() {
  // 处理客户端请求
  server.handleClient();

  // 检测按键是否按下
  if (digitalRead(resetButtonPin) == HIGH) {
    delay(500); // 简单防抖
    if (digitalRead(resetButtonPin) == HIGH) {
      resetWiFiConfig();
    }
  }
}