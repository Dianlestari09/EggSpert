#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// -------------------- KONFIGURASI WIFI --------------------
#define WIFI_SSID "vivo V30"
#define WIFI_PASSWORD "dianlestari"

// -------------------- KONFIGURASI FIREBASE --------------------
#define API_KEY "AIzaSyDhzvMXLQ7f7hlZE4xtvunm9nc9EUa3WkY"
#define DATABASE_URL "https://lomba-1f6a1-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define DATABASE_SECRET "Zox6ghqOwzzgmDu6Of2a6pwsPVoHOR7u6PRE0YZq"

// -------------------- OBJEK FIREBASE --------------------
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// -------------------- PIN SENSOR --------------------
// Untuk ESP32 gunakan nomor GPIO langsung, bukan D1, D2
#define SENSOR_A001 18
#define SENSOR_A002 19
#define SENSOR_B001 21
#define SENSOR_B002 22

// -------------------- ID KANDANG --------------------
String kandangID[] = {"A001", "A002", "B001", "B002"};

// -------------------- WAKTU --------------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000); // GMT+7

// -------------------- VARIABEL --------------------
int jumlahTelur[4] = {0, 0, 0, 0};
String lastDate = "";
String lastMonth = "";

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_A001, INPUT_PULLUP);
  pinMode(SENSOR_A002, INPUT_PULLUP);
  pinMode(SENSOR_B001, INPUT_PULLUP);
  pinMode(SENSOR_B002, INPUT_PULLUP);

  // Koneksi ke WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi");

  // Konfigurasi Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Gunakan Legacy Database Secret
  config.signer.tokens.legacy_token = DATABASE_SECRET;

  Firebase.reconnectWiFi(true);
  Firebase.begin(&config, &auth);

  // NTP
  timeClient.begin();
}

// -------------------- LOOP --------------------
void loop() {
  timeClient.update();

  // Ambil waktu epoch
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);

  // Format tanggal dan bulan
  char dateBuffer[11];  // YYYY-MM-DD
  snprintf(dateBuffer, sizeof(dateBuffer), "%04d-%02d-%02d",
           ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
  String date = dateBuffer;

  char monthBuffer[8];  // YYYY-MM
  snprintf(monthBuffer, sizeof(monthBuffer), "%04d-%02d",
           ptm->tm_year + 1900, ptm->tm_mon + 1);
  String month = monthBuffer;

  lastDate = date;
  lastMonth = month;

  // Cek sensor
  checkSensor(SENSOR_A001, 0);
  checkSensor(SENSOR_A002, 1);
  checkSensor(SENSOR_B001, 2);
  checkSensor(SENSOR_B002, 3);

  delay(200);
}

// -------------------- CEK SENSOR --------------------
void checkSensor(int pin, int index) {
  if (digitalRead(pin) == LOW) {
    jumlahTelur[index]++;
    Serial.println("\nTelur terdeteksi di " + kandangID[index] +
                   " total sementara: " + String(jumlahTelur[index]));

    // Kirim langsung ke Firebase
    updateFirebase(kandangID[index], lastDate, lastMonth);

    delay(1000); // debounce
  }
}

// -------------------- UPDATE FIREBASE --------------------
void updateFirebase(String idKandang, String tanggal, String bulan) {
  String pathHarian = "idKandang/" + idKandang + "/Harian/" + tanggal + "/jumlahTelurPerhari";
  String pathBulanan = "idKandang/" + idKandang + "/Bulanan/" + bulan + "/jumlahTelurPerbulan";

  Serial.println("[DEBUG] Path Harian : " + pathHarian);
  Serial.println("[DEBUG] Path Bulanan: " + pathBulanan);

  // Update Harian (increment)
  int totalHarian = 0;
  if (Firebase.RTDB.getInt(&fbdo, pathHarian.c_str())) {
    totalHarian = fbdo.intData();
    Serial.println("[DEBUG] Nilai harian lama: " + String(totalHarian));
  } else {
    Serial.println("[DEBUG] Harian belum ada, mulai dari 0");
  }
  totalHarian++;
  if (Firebase.RTDB.setInt(&fbdo, pathHarian.c_str(), totalHarian)) {
    Serial.println("[DEBUG] Update harian sukses -> " + String(totalHarian));
  } else {
    Serial.println("[ERROR] Gagal update harian: " + fbdo.errorReason());
  }

  // Update Bulanan (increment)
  int totalBulan = 0;
  if (Firebase.RTDB.getInt(&fbdo, pathBulanan.c_str())) {
    totalBulan = fbdo.intData();
    Serial.println("[DEBUG] Nilai bulanan lama: " + String(totalBulan));
  } else {
    Serial.println("[DEBUG] Bulanan belum ada, mulai dari 0");
  }
  totalBulan++;
  if (Firebase.RTDB.setInt(&fbdo, pathBulanan.c_str(), totalBulan)) {
    Serial.println("[DEBUG] Update bulanan sukses -> " + String(totalBulan));
  } else {
    Serial.println("[ERROR] Gagal update bulanan: " + fbdo.errorReason());
  }
}
