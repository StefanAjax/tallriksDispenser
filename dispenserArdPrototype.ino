#define SOLENOID1_PIN 3
#define CAPSENS1_PIN 8
#define CAPSENS2_PIN 9
#define TESTBUTTON_PIN 4
#define GREENLIGHT_PIN 6
#define REDLIGHT_PIN 7
#define SOLENOID_MOVETIME_MS 100
#define SOLENOID_HOLDTIME_MS 5000
#define SOLENOID_MOVELEVEL 255
#define SOLENOID_HOLDLEVEL 30
#define AFTERBLIPP_TIMEOUT 5000

bool sensorValue = 0;
bool blippValue = 0;
unsigned long stateChangeTime = 0;
int currentState = 0;

void setup()
{
    pinMode(CAPSENS1_PIN, INPUT_PULLUP);
    pinMode(SOLENOID1_PIN, OUTPUT);
    pinMode(TESTBUTTON_PIN, INPUT_PULLUP);
    pinMode(GREENLIGHT_PIN, OUTPUT);
    pinMode(REDLIGHT_PIN, OUTPUT);
    Serial.begin(9600);
}

void setState(int nextState)
{
    stateChangeTime = millis();
    currentState = nextState;
    Serial.print("Moving to state: ");
    Serial.println(currentState);
}

void loop()
{
    switch (currentState)
    {
    case 0:
        analogWrite(SOLENOID1_PIN, 0);
        digitalWrite(GREENLIGHT_PIN, LOW);
        digitalWrite(REDLIGHT_PIN, HIGH);
        sensorValue = !digitalRead(CAPSENS1_PIN);
        blippValue = !digitalRead(TESTBUTTON_PIN);
        if (blippValue == HIGH)
        {
            setState(1);
        }
        break;
    case 1:
        analogWrite(SOLENOID1_PIN, 0);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        sensorValue = !digitalRead(CAPSENS1_PIN);
        if (sensorValue == HIGH)
        {
            setState(2);
        }
        if (millis() - stateChangeTime > AFTERBLIPP_TIMEOUT)
        {
            setState(0);
        }
        break;
    case 2:
        analogWrite(SOLENOID1_PIN, SOLENOID_MOVELEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        if (millis() - stateChangeTime > SOLENOID_MOVETIME_MS)
        {
            setState(3);
        }
        break;
    case 3:
        analogWrite(SOLENOID1_PIN, SOLENOID_HOLDLEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        if (millis() - stateChangeTime > SOLENOID_HOLDTIME_MS)
        {
            setState(0);
        }
        break;
    default:
        Serial.println("Should never happen!");
    }
}