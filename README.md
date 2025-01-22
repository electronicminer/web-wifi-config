# esp32 web wifi_config

这个代码是一个基于ESP32的Wi-Fi配置和管理系统，主要用于让用户通过Web界面配置ESP32连接到指定的Wi-Fi网络。以下是代码的主要功能和中介绍：

### 1. **AP模式与Web配置界面**
   - 当ESP32启动时，它会尝试连接之前保存的Wi-Fi网络。如果连接失败，ESP32会进入AP模式（接入点模式），并创建一个名为`ESP32-AP`的Wi-Fi热点，密码为`12345678`。
   - 用户可以通过手机或电脑连接到这个热点，然后访问ESP32的Web服务器（默认IP地址为`192.168.4.1`）。
   - Web界面提供了一个简单的表单，用户可以输入Wi-Fi的SSID和密码，并提交以配置ESP32连接到指定的Wi-Fi网络。

### 2. **Wi-Fi扫描功能**
   - Web界面中集成了一个Wi-Fi扫描功能，用户可以点击页面上的按钮扫描周围的Wi-Fi网络。扫描结果会以列表形式显示在页面上，用户可以直接点击某个Wi-Fi名称，自动填充到SSID输入框中。

### 3. **NVS（非易失性存储）**
   - 使用`Preferences`库将用户配置的Wi-Fi信息（SSID和密码）保存到ESP32的非易失性存储（NVS）中。这样即使ESP32重启，它也能自动尝试连接之前保存的Wi-Fi网络，而无需重新配置。

### 4. **重置功能**
   - 代码中定义了一个重置按钮（连接到GPIO 0），当用户按下这个按钮时，ESP32会清除之前保存的Wi-Fi配置，并重启进入AP模式，等待用户重新配置。

### 5. **Web服务器**
   - 使用`WebServer`库创建了一个简单的HTTP服务器，处理用户的请求。服务器提供了三个路由：
     - `/`：显示Wi-Fi配置页面。
     - `/scan`：处理Wi-Fi扫描请求，返回扫描到的Wi-Fi列表（JSON格式）。
     - `/config`：处理用户提交的Wi-Fi配置表单，尝试连接指定的Wi-Fi网络，并将配置保存到NVS中。

### 6. **代码结构**
   - `wifi_init()`：初始化Wi-Fi连接，尝试连接保存的Wi-Fi网络，如果失败则进入AP模式并启动Web服务器。
   - `handleRoot()`：处理根路径请求，返回Wi-Fi配置页面的HTML代码。
   - `handleScan()`：处理Wi-Fi扫描请求，返回扫描到的Wi-Fi列表。
   - `handleConfig()`：处理用户提交的Wi-Fi配置表单，保存配置并尝试连接Wi-Fi。
   - `resetWiFiConfig()`：重置Wi-Fi配置，清除NVS中保存的Wi-Fi信息并重启ESP32。

### 7. **使用场景**
   - 适用于需要让用户通过简单的方式配置ESP32连接到Wi-Fi网络的场景，例如智能家居设备、物联网设备等。通过Web界面，用户可以轻松地配置设备连接到家庭或办公室的Wi-Fi网络。

可以轻松地配置ESP32连接到Wi-Fi网络，并且即使设备重启，也能自动连接之前配置的网络，非常适合物联网设备的应用场景。



# ESP32 Web Wi-Fi Configuration

This code is a Wi-Fi configuration and management system based on the ESP32, primarily designed to allow users to configure the ESP32 to connect to a specified Wi-Fi network through a web interface. Below are the main features and an introduction to the code:

### 1. **AP Mode and Web Configuration Interface**
   - When the ESP32 starts, it attempts to connect to a previously saved Wi-Fi network. If the connection fails, the ESP32 enters AP mode (Access Point mode) and creates a Wi-Fi hotspot named `ESP32-AP` with the password `12345678`.
   - Users can connect to this hotspot using a phone or computer and then access the ESP32's web server (default IP address is `192.168.4.1`).
   - The web interface provides a simple form where users can enter the SSID and password of the Wi-Fi network and submit it to configure the ESP32 to connect to the specified Wi-Fi network.

### 2. **Wi-Fi Scanning Function**
   - The web interface includes a Wi-Fi scanning feature. Users can click a button on the page to scan for nearby Wi-Fi networks. The scan results are displayed in a list on the page, and users can click on a Wi-Fi name to automatically fill it into the SSID input field.

### 3. **NVS (Non-Volatile Storage)**
   - The `Preferences` library is used to save the user-configured Wi-Fi information (SSID and password) to the ESP32's non-volatile storage (NVS). This allows the ESP32 to automatically attempt to connect to the previously saved Wi-Fi network even after a reboot, without requiring reconfiguration.

### 4. **Reset Function**
   - The code defines a reset button (connected to GPIO 0). When the user presses this button, the ESP32 clears the previously saved Wi-Fi configuration and reboots into AP mode, waiting for the user to reconfigure it.

### 5. **Web Server**
   - A simple HTTP server is created using the `WebServer` library to handle user requests. The server provides three routes:
     - `/`: Displays the Wi-Fi configuration page.
     - `/scan`: Handles Wi-Fi scan requests and returns a list of scanned Wi-Fi networks (in JSON format).
     - `/config`: Processes the Wi-Fi configuration form submitted by the user, attempts to connect to the specified Wi-Fi network, and saves the configuration to NVS.

### 6. **Code Structure**
   - `wifi_init()`: Initializes the Wi-Fi connection, attempts to connect to the saved Wi-Fi network, and if it fails, enters AP mode and starts the web server.
   - `handleRoot()`: Handles the root path request and returns the HTML code for the Wi-Fi configuration page.
   - `handleScan()`: Handles Wi-Fi scan requests and returns a list of scanned Wi-Fi networks.
   - `handleConfig()`: Processes the Wi-Fi configuration form submitted by the user, saves the configuration, and attempts to connect to the Wi-Fi network.
   - `resetWiFiConfig()`: Resets the Wi-Fi configuration, clears the saved Wi-Fi information in NVS, and restarts the ESP32.

### 7. **Use Cases**
   - This code is suitable for scenarios where users need a simple way to configure the ESP32 to connect to a Wi-Fi network, such as in smart home devices or IoT devices. Through the web interface, users can easily configure the device to connect to their home or office Wi-Fi network.

With this code, users can effortlessly configure the ESP32 to connect to a Wi-Fi network, and even after a reboot, the device will automatically reconnect to the previously configured network. This makes it highly suitable for IoT device applications.
