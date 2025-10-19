# 🥚 ESP32 Smart Egg Counter (Firebase Realtime Database)

Proyek ini menggunakan **ESP32** untuk menghitung jumlah telur yang terdeteksi oleh beberapa sensor, lalu mengirimkan hasilnya secara otomatis ke **Firebase Realtime Database**.  
Sistem ini dapat digunakan untuk memantau produktivitas kandang ayam secara **real-time dan otomatis**.

---

## 🧠 Fitur Utama

- Menghitung telur dari **4 kandang berbeda (A001, A002, B001, B002)**.
- Menyimpan data **harian** dan **bulanan** ke Firebase Realtime Database.
- Menggunakan **NTP (Network Time Protocol)** untuk sinkronisasi waktu otomatis (GMT+7).
- Tidak memerlukan jam RTC tambahan.
- Otomatis menambahkan data baru setiap kali telur terdeteksi oleh sensor.
- Data tersusun rapi per tanggal dan per bulan dalam format JSON di Firebase.

---

## ⚙️ Komponen yang Digunakan

| Komponen | Deskripsi |
|-----------|-----------|
| **ESP32 Devkit** | Mikrokontroler utama |
| **Sensor Telur (Switch / IR / Limit Sensor)** | Mendeteksi keberadaan telur |
| **Firebase Realtime Database** | Penyimpanan cloud |
| **WiFi Hotspot / Router** | Koneksi internet |
| **Library NTPClient** | Sinkronisasi waktu |
| **Library Firebase ESP Client** | Komunikasi dengan Firebase |

---

## 📡 Arsitektur Sistem

