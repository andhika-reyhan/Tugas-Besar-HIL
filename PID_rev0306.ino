#include <EEPROM.h>
#include <TimerOne.h>
#define pi 3.1415

int state = 0;
long int posisi = 0;
double inp_kp, inp_ki, inp_kd;  // menerima input Kp, Ki, Kd dari PC
float kp = 0, ki = 0, kd = 0;   // nilai Kp, Ki, Kd untuk pengendali PID
float setpoint = 0;             // menerima input set point dari potensiometer
float PID_output, sudut, kecepatan, prop, integral, diff, error, kecepatan, PIDTotal, pasterror, pastposisi;
float pastPID = 0;

void setup() {
  // Inisialisasi komunikasi serial pada 9600 bps:
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(A0, INPUT);         // port untuk potensiometer
  Timer1.initialize(1000);
  Timer1.attachInterrupt(PID);
}

// perhitungan parameter kendali dengan PID
void PID() {
  // perhitungan posisi dilakukan pada bagian looping
  error = setpoint - posisi;
  pastPID = PIDTotal; // menyimpan parameter kendali PID sebelumnya utk perhitungan filter digital
  
  // Perhitungan PID
  prop = kp * error;
  integral += (ki * error);
  diff = (error - pasterror) * kd / 0.001;

  // Penjumlahan PID dari ketiga komponen
  PIDTotal = prop + integral + diff;
  pasterror = error;
}

void loop() {
  // Membaca input setpoint dari potensiometer
  float pot = (float)analogRead(A0);
  time = millis()
  
  if(millis() - time < 2000){
    setpoint = 0;
  } else if(millis() - time >= 2000){
    setpoint = (pot / 1023) * 5;  // Vmax Arduino = 5 V, membaca setpoint dari potensio
  }
 
  pastposisi = posisi; // simpan posisi di waktu sebelumnya utk perhitungan filter digital
  
  // Update nilai Kp, Ki, Kd di EEPROM melalui port serial
  while(Serial.available() > 0) {
    inp_kp = double(Serial.readStringUntil('\n').toInt());
    kp = inp_kp;
    Serial.print("Isi nilai Kp : ");
    Serial.println(inp_kp);
    EEPROM.put(0x00, kp);

    inp_ki = double(Serial.readStringUntil('\n').toInt());
    ki = inp_ki;
    Serial.print("Isi nilai Ki : ");
    Serial.println(inp_ki);
    EEPROM.put(0x04, ki);

    inp_kd = double(Serial.readStringUntil('\n').toInt());
    kd = inp_kd;
    Serial.print("Isi nilai Kd : ");
    Serial.println(inp_kd);
    EEPROM.put(0x08, kd);
  }

  // Mengatur operasi pengendali PID terhadap Motor
  // menggunakan filter digital yang memodelkan fungsi transfer motor
  posisi = 0.993127*pastposisi + 0.003024*PIDTotal + 0.003024*pastPID;
  
  // Menampilkan hasil pada Serial Monitor
  Serial.println("Setpoint : ");
  Serial.print(setpoint);
  Serial.print(" Posisi: ");
  Serial.print(posisi);
}
