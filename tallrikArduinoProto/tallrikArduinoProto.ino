#define SOLENOID1_PIN 3
#define CAPSENS1_PIN 8
#define CAPSENS2_PIN 9
#define TESTBUTTON_PIN 4
#define GREENLIGHT_PIN 6
#define REDLIGHT_PIN 7
#define SOLENOID_MOVETIME_MS 100
#define SUPER_TIMEOUT_MS 20000
#define SOLENOID_MOVELEVEL 255
#define SOLENOID_HOLDLEVEL 30
#define AFTERBLIPP_TIMEOUT 5000
#define COOLDOWN_TIME_MS 100

bool isSensor1Triggered = 0;
bool isSensor2Triggered = 0;

bool blippValue = 0;
unsigned long stateChangeTime = 0;
int currentState = 0;

void setup()
{
    pinMode(CAPSENS1_PIN, INPUT_PULLUP);
    pinMode(CAPSENS2_PIN, INPUT_PULLUP);
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
    isSensor1Triggered = !digitalRead(CAPSENS1_PIN);
    isSensor2Triggered = !digitalRead(CAPSENS2_PIN);
    blippValue = !digitalRead(TESTBUTTON_PIN);

    switch (currentState)
    {
    case 0:
        // Ready to blipp
        analogWrite(SOLENOID1_PIN, 0);
        digitalWrite(GREENLIGHT_PIN, LOW);
        digitalWrite(REDLIGHT_PIN, HIGH);

        if (blippValue == HIGH)
        {
            setState(1);
        }
        break;
    case 1:
        // Waiting for plate to travel, opens when sens1 triggers
        analogWrite(SOLENOID1_PIN, 0);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        if (isSensor1Triggered == HIGH)
        {
            setState(2);
        }
        if (millis() - stateChangeTime > SUPER_TIMEOUT_MS)
        {
            setState(0);
        }
        break;
    case 2:
        // Step 1 of opening sequence
        analogWrite(SOLENOID1_PIN, SOLENOID_MOVELEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        if (millis() - stateChangeTime > SOLENOID_MOVETIME_MS)
        {
            setState(3);
        }
        if (millis() - stateChangeTime > SUPER_TIMEOUT_MS)
        {
            setState(0);
        }
        break;
    case 3:
        // Step 2 of opening sequence, waiting for sens 2 to trigger
        analogWrite(SOLENOID1_PIN, SOLENOID_HOLDLEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);

        if (isSensor2Triggered == HIGH && isSensor1Triggered == HIGH)
        {
            setState(4);
        }
        if (millis() - stateChangeTime > SUPER_TIMEOUT_MS)
        {
            setState(0);
        }
        break;
    case 4:
        // Plate now mid machine
        analogWrite(SOLENOID1_PIN, SOLENOID_HOLDLEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);

        if (isSensor2Triggered == LOW)
        {
            // Plate moved backwards
            Serial.println("Backwards");
            setState(3);
        }

        if (isSensor1Triggered == LOW)
        {
            setState(5);
        }
        if (millis() - stateChangeTime > SUPER_TIMEOUT_MS)
        {
            setState(0);
        }
        break;
    case 5:
        // Plate past solenoid, waiting for plate to leave machine before locking
        analogWrite(SOLENOID1_PIN, SOLENOID_HOLDLEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        if (isSensor1Triggered == HIGH)
        {
            // plate moved backwards
            Serial.println("Backwards");
            setState(4);
        }
        if (isSensor2Triggered == LOW)
        {
            setState(6);
        }
        if (millis() - stateChangeTime > SUPER_TIMEOUT_MS)
        {
            setState(0);
        }
        break;
    case 6:
        // Plate left, cooldown
        analogWrite(SOLENOID1_PIN, SOLENOID_HOLDLEVEL);
        digitalWrite(GREENLIGHT_PIN, HIGH);
        digitalWrite(REDLIGHT_PIN, LOW);
        if (millis() - stateChangeTime > COOLDOWN_TIME_MS)
        {
            Serial.println("Cooldown");
            setState(0);
        }
        break;

    default:
        Serial.println("Should never happen!");
    }
}
