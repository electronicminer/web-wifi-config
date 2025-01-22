#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h> // ���ڷ���ʧ�Դ洢

// ����APģʽ��SSID������
const char* apSSID = "ESP32-AP";
const char* apPassword = "12345678";

// ���尴������
const int resetButtonPin = 1; // GPIO 0

// ����Web����������
WebServer server(80);

// ����Preferences��������NVS�洢
Preferences preferences;

// ����Wi-Fi����
String savedSSID = "";
String savedPassword = "";

// ��·��������
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

// ɨ��Wi-Fi������
// ɨ��Wi-Fi������
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

// ���ô�����
void handleConfig() {
  // ��ȡ������
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  // ���浽NVS
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);

  // ��������Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.println("Connecting to WiFi...");

  // �ȴ�����
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // �������״̬
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

// ����Wi-Fi����
void resetWiFiConfig() {
  preferences.remove("ssid");
  preferences.remove("password");
  Serial.println("Wi-Fi configuration reset.");
  ESP.restart(); // ����ESP32
}

void wifi_init() {
   // ��ʼ����������
  pinMode(resetButtonPin, INPUT);

  // ��ʼ��NVS
  preferences.begin("wifi-config", false);

  // ���Լ��ر����Wi-Fi����
  savedSSID = preferences.getString("ssid", "");
  savedPassword = preferences.getString("password", "");

  if (savedSSID != "") {
    // �������ӱ����Wi-Fi
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
      return; // ���ӳɹ��������������ģʽ
    }
  }

  // ����APģʽ
  WiFi.softAP(apSSID, apPassword);
  Serial.println("AP Mode Started");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  // ����Web������·��
  server.on("/", handleRoot);       // �����·��
  server.on("/scan", handleScan);   // ����Wi-Fiɨ������
  server.on("/config", handleConfig); // ������������

  // ����Web������
  server.begin();
  Serial.println("HTTP server started");
}
void setup() {
  Serial.begin(115200);

  wifi_init();
}

void loop() {
  // ����ͻ�������
  server.handleClient();

  // ��ⰴ���Ƿ���
  if (digitalRead(resetButtonPin) == HIGH) {
    delay(500); // �򵥷���
    if (digitalRead(resetButtonPin) == HIGH) {
      resetWiFiConfig();
    }
  }
}