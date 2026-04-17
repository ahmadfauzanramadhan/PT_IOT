*JUDUL STUDI KASUS: BENDUNGAN PINTAR*

*NAMA KELOMPOK:* JARANG KUMPUL POSTTEST

*ANGGOTA:* Ahmad Fauzan Ramadhan 94
	 Aditya Wedakarna 100
	 Nashwan Faiz Nandana Astaman 125

*DESKRIPSI:*
1.Level ketinggian air:
- Level Aman (Sensor <= 800): Pintu air tertutup (Servo 0°), Buzzer mati.
- Level Waspada (801 - 1500): Pintu air terbuka setengah (Servo 90°), Buzzer mati.
- Level Bahaya (> 1500): Pintu air terbuka penuh (Servo 180°), Buzzer menyala berkedip
2. Pada Kodular menampilkan nilai level air, status level (aman/waspada/bahaya), status buzzer, dan nilai servo.
3. Pada Kodular terdapat 2 button mode otomatis dan manual, dengan ketentuan:
- Otomatis: aktuator (servo dan buzzer) bekerja sesuai dengan syarat pada ketentuan nomor 1
- Manual: aktuator (servo dan buzzer) dapat dikontrol secara manual pada kodular

*Komponen Yang Digunakan:*
- Servo
- Water Censor
- ESP32
- Buzzer
- Kabel Jumper
