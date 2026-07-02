#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <math.h>
#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "model_data.h"   

const char* ssid = "AndroidAP_4511";
const char* password = "PASSWORD";


const char* mqtt_server = "10.143.154.163";
const int mqtt_port = 1883;

const char* topic_temp = "motor/temp";
const char* topic_vib = "motor/vibration";
const char* topic_raw_mse = "motor/raw_mse";
const char* topic_avg_mse = "motor/avg_mse";
const char* topic_status = "motor/status";

WiFiClient espClient;
PubSubClient mqttClient(espClient);


#define NTC_PIN 34
#define ADC_MAX 4095.0f
#define R_FIXED 10000.0f
#define R0 10000.0f
#define BETA 3950.0f
#define T0 298.15f


#define MPU_ADDR 0x68
int16_t ax, ay, az, gx, gy, gz;
long ax_offset = 0, ay_offset = 0, az_offset = 0;


#define SEQ_LEN 30
#define INPUT_SIZE 60

float temp_buffer[SEQ_LEN];
float vib_buffer[SEQ_LEN];

int buffer_index = 0;
bool buffer_filled = false;


#define TEMP_MIN 26.89f
#define TEMP_MAX 51.02f
#define VIB_MIN  0.0f
#define VIB_MAX  1.227f

float normalize(float x, float min, float max) {
    return (x - min) / (max - min);
}

#define THRESHOLD_TRIGGER 0.075f
#define THRESHOLD_CLEAR   0.060f


#define MSE_WINDOW 10
float mse_buffer[MSE_WINDOW] = {0};
int mse_index = 0;
bool mse_filled = false;

float updateMSE(float new_mse) {
    mse_buffer[mse_index] = new_mse;
    mse_index = (mse_index + 1) % MSE_WINDOW;
    if (mse_index == 0) mse_filled = true;

    int count = mse_filled ? MSE_WINDOW : mse_index;
    float sum = 0;
    for (int i = 0; i < count; i++) sum += mse_buffer[i];
    return sum / count;
}


#define TENSOR_ARENA_SIZE (20 * 1024)

uint8_t* tensor_arena = nullptr;
const tflite::Model* tf_model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
tflite::MicroMutableOpResolver<4>* resolver = nullptr;
tflite::MicroErrorReporter* error_reporter = nullptr;
TfLiteTensor* input_tensor = nullptr;
TfLiteTensor* output_tensor = nullptr;


void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
}


void connectMQTT() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect("ESP32_AI")) {
            Serial.println("MQTT connected");
        } else {
            delay(2000);
        }
    }
}


float readTemperature() {
    int adc = analogRead(NTC_PIN);
    adc = constrain(adc, 1, 4094);

    float r_ntc = R_FIXED * ((float)adc / (ADC_MAX - adc));
    float tempK = 1.0f / ((1.0f / T0) + (1.0f / BETA) * logf(r_ntc / R0));
    return tempK - 273.15f;
}


void readMPU() {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 14, true);

    ax = Wire.read()<<8 | Wire.read();
    ay = Wire.read()<<8 | Wire.read();
    az = Wire.read()<<8 | Wire.read();

    Wire.read(); Wire.read();

    gx = Wire.read()<<8 | Wire.read();
    gy = Wire.read()<<8 | Wire.read();
    gz = Wire.read()<<8 | Wire.read();
}


float readVibration() {
    float sum = 0;
    for (int i = 0; i < 10; i++) {
        readMPU();
        float ax_g = (ax - ax_offset) / 16384.0f;
        float ay_g = (ay - ay_offset) / 16384.0f;
        float az_g = (az - az_offset) / 16384.0f;
        float mag  = sqrtf(ax_g*ax_g + ay_g*ay_g + az_g*az_g);
        sum += fabsf(mag - 1.0f);
        delay(5);
    }
    return sum / 10.0f;
}


void calibrateMPU() {
    long ax_sum=0, ay_sum=0, az_sum=0;

    for (int i = 0; i < 500; i++) {
        readMPU();
        ax_sum += ax;
        ay_sum += ay;
        az_sum += az;
        delay(2);
    }

    ax_offset = ax_sum / 500;
    ay_offset = ay_sum / 500;
    az_offset = (az_sum / 500) - 16384;
}


float runInference(float temp, float vib) {

    temp_buffer[buffer_index] = temp;
    vib_buffer[buffer_index]  = vib;

    buffer_index++;

    if (buffer_index >= SEQ_LEN) {
        buffer_index = 0;
        buffer_filled = true;
    }

    if (!buffer_filled) return -1;

    int idx = 0;

    for (int i = 0; i < SEQ_LEN; i++) {
        input_tensor->data.f[idx++] = normalize(temp_buffer[i], TEMP_MIN, TEMP_MAX);
        input_tensor->data.f[idx++] = normalize(vib_buffer[i],  VIB_MIN,  VIB_MAX);
    }

    if (interpreter->Invoke() != kTfLiteOk) return -1;

    float error = 0;

    for (int i = 0; i < INPUT_SIZE; i++) {
        float diff = input_tensor->data.f[i] - output_tensor->data.f[i];
        error += fabs(diff);
    }

    return error / INPUT_SIZE;
}


void setup() {
    Serial.begin(115200);
    Wire.begin(21, 22);

    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);

    calibrateMPU();

    connectWiFi();
    mqttClient.setServer(mqtt_server, mqtt_port);

    error_reporter = new tflite::MicroErrorReporter();

    tensor_arena = (uint8_t*)heap_caps_malloc(
        TENSOR_ARENA_SIZE,
        MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL
    );

    resolver = new tflite::MicroMutableOpResolver<4>();
    resolver->AddFullyConnected();
    resolver->AddRelu();

    tf_model = tflite::GetModel(model_data);

    interpreter = new tflite::MicroInterpreter(
        tf_model, *resolver, tensor_arena,
        TENSOR_ARENA_SIZE, error_reporter
    );

    interpreter->AllocateTensors();

    input_tensor  = interpreter->input(0);
    output_tensor = interpreter->output(0);

    Serial.println("SYSTEM READY");
}


void loop() {
    static bool is_anomaly = false;
    char msg[32];

    if (!mqttClient.connected()) connectMQTT();
    mqttClient.loop();

    float temp = readTemperature();
    float vib  = readVibration();

    float raw_mse = runInference(temp, vib);
    if (raw_mse < 0) return;

    float avg_mse = updateMSE(raw_mse);

    if (!is_anomaly && avg_mse > THRESHOLD_TRIGGER)
        is_anomaly = true;
    else if (is_anomaly && avg_mse < THRESHOLD_CLEAR)
        is_anomaly = false;

    dtostrf(temp, 6, 2, msg); mqttClient.publish(topic_temp, msg);
    dtostrf(vib, 7, 5, msg); mqttClient.publish(topic_vib, msg);
    dtostrf(raw_mse, 8, 6, msg); mqttClient.publish(topic_raw_mse, msg);
    dtostrf(avg_mse, 8, 6, msg); mqttClient.publish(topic_avg_mse, msg);

    sprintf(msg, "%d", is_anomaly ? 1 : 0);
    mqttClient.publish(topic_status, msg);

    Serial.print("Temp: "); Serial.print(temp);
    Serial.print(" Vib: "); Serial.print(vib);
    Serial.print(" MSE: "); Serial.print(avg_mse);
    Serial.print(" Status: ");
    Serial.println(is_anomaly ? "ANOMALY" : "NORMAL");

    delay(500);
}
