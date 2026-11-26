/*
   PTT Dial Tester V4.1
   XIAO ESP32-S3

   Comptage fiable
   Mesure Break / Make
*/

const byte PIN_DIAL = D10;

//-------------------- Réglages --------------------

const uint32_t DEBOUNCE_US = 5000;
const uint32_t END_TIMEOUT = 500;

//--------------------------------------------------

bool previousState;

bool waitingClose = false;
bool dialing = false;

uint32_t lastEdgeUs = 0;
uint32_t lastActivityMs = 0;

uint32_t lastTransitionUs = 0;

uint8_t pulses = 0;

uint32_t totalOpen = 0;
uint32_t totalClose = 0;

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_DIAL, INPUT_PULLUP);

    previousState = digitalRead(PIN_DIAL);

    lastTransitionUs = micros();

    Serial.println();
    Serial.println("================================");
    Serial.println("PTT Dial Tester V4.1");
    Serial.println("================================");
}

void loop()
{
    bool state = digitalRead(PIN_DIAL);

    if (state != previousState)
    {
        uint32_t now = micros();

        if ((now - lastEdgeUs) > DEBOUNCE_US)
        {
            uint32_t duration = now - lastTransitionUs;

            // Etat précédent = fermé
            if (previousState == LOW)
                totalClose += duration;

            // Etat précédent = ouvert
            else
                totalOpen += duration;

            lastTransitionUs = now;
            lastEdgeUs = now;
            lastActivityMs = millis();

            // ouverture = impulsion
            if (state == HIGH)
            {
                if (!waitingClose)
                {
                    waitingClose = true;

                    pulses++;

                    if (!dialing)
                    {
                        dialing = true;
                        Serial.println();
                        Serial.println("Début numérotation");
                    }

                    Serial.print("Impulsion ");
                    Serial.println(pulses);
                }
            }
            else
            {
                waitingClose = false;
            }
        }

        previousState = state;
    }

    if (dialing && (millis() - lastActivityMs > END_TIMEOUT))
    {
        float total = totalOpen + totalClose;

        float breakPct = 0;
        float makePct = 0;
        float pps = 0;

        if (total > 0)
        {
            breakPct = (100.0 * totalOpen) / total;
            makePct = (100.0 * totalClose) / total;

            if (pulses > 1)
                pps = (1000000.0 * pulses) / total;
        }

        Serial.println();
        Serial.println("==============================");

        Serial.print("Impulsions : ");

        if (pulses == 10)
            Serial.println("10 (chiffre 0)");
        else
            Serial.println(pulses);

        Serial.print("Ouvert : ");
        Serial.print(totalOpen / 1000.0, 2);
        Serial.println(" ms");

        Serial.print("Ferme : ");
        Serial.print(totalClose / 1000.0, 2);
        Serial.println(" ms");

        Serial.print("Break : ");
        Serial.print(breakPct, 1);
        Serial.println(" %");

        Serial.print("Make : ");
        Serial.print(makePct, 1);
        Serial.println(" %");

        Serial.print("Cadence : ");
        Serial.print(pps, 2);
        Serial.println(" PPS");

        Serial.println("==============================");

        pulses = 0;
        dialing = false;
        waitingClose = false;

        totalOpen = 0;
        totalClose = 0;
    }
}
