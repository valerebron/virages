/*
   PTT Tester V5.0
   XIAO ESP32-S3

   1) Test / réglage cadran impulsion
   2) Conversion impulsions -> DTMF
*/

const byte PIN_DIAL = D10;

// Deux sorties DTMF (à sommer avec résistances, ex: 10k + 10k)
const byte PIN_DTMF_ROW = D0;
const byte PIN_DTMF_COL = D1;

//-------------------- Réglages --------------------

const uint32_t DEBOUNCE_US = 5000;
const uint32_t END_TIMEOUT = 500;
const uint32_t DTMF_ON_MS = 140;
const uint32_t DTMF_GAP_MS = 80;
const uint32_t DTMF_PRE_MS = 20;
const uint32_t NUMBER_TIMEOUT_MS = 4000;
const uint8_t MAX_DIGITS = 24;

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

char numberBuffer[MAX_DIGITS + 1] = {0};
uint8_t numberLen = 0;
uint32_t lastDigitMs = 0;
bool numberFinalized = true;

bool dtmfFrequenciesForDigit(uint8_t digit, uint16_t &fLow, uint16_t &fHigh)
{
    switch (digit)
    {
    case 1:
        fLow = 697;
        fHigh = 1209;
        return true;
    case 2:
        fLow = 697;
        fHigh = 1336;
        return true;
    case 3:
        fLow = 697;
        fHigh = 1477;
        return true;
    case 4:
        fLow = 770;
        fHigh = 1209;
        return true;
    case 5:
        fLow = 770;
        fHigh = 1336;
        return true;
    case 6:
        fLow = 770;
        fHigh = 1477;
        return true;
    case 7:
        fLow = 852;
        fHigh = 1209;
        return true;
    case 8:
        fLow = 852;
        fHigh = 1336;
        return true;
    case 9:
        fLow = 852;
        fHigh = 1477;
        return true;
    case 0:
        fLow = 941;
        fHigh = 1336;
        return true;
    default:
        return false;
    }
}

void playDTMFDigit(uint8_t digit)
{
    uint16_t fLow = 0;
    uint16_t fHigh = 0;

    if (!dtmfFrequenciesForDigit(digit, fLow, fHigh))
        return;

    // Petit delai de garde avant emission pour stabiliser la ligne audio.
    delay(DTMF_PRE_MS);

    ledcWriteTone(PIN_DTMF_ROW, fLow);
    ledcWriteTone(PIN_DTMF_COL, fHigh);

    delay(DTMF_ON_MS);

    ledcWriteTone(PIN_DTMF_ROW, 0);
    ledcWriteTone(PIN_DTMF_COL, 0);

    delay(DTMF_GAP_MS);

}

void clearNumberBuffer()
{
    numberLen = 0;
    numberBuffer[0] = '\0';
    numberFinalized = true;
}

void appendDigitToBuffer(uint8_t digit)
{
    // Nouveau numero: effacer l'ancien buffer des que le premier chiffre arrive.
    if (numberFinalized)
    {
        numberLen = 0;
        numberBuffer[0] = '\0';
    }

    if (numberLen >= MAX_DIGITS)
        return;

    numberBuffer[numberLen++] = char('0' + digit);
    numberBuffer[numberLen] = '\0';
    lastDigitMs = millis();
    numberFinalized = false;
}

void replayNumberDTMF()
{
    if (numberLen == 0)
    {
        Serial.println("Aucun numero en memoire.");
        return;
    }

    Serial.print("Replay DTMF: ");
    Serial.println(numberBuffer);

    for (uint8_t i = 0; i < numberLen; i++)
    {
        uint8_t digit = uint8_t(numberBuffer[i] - '0');
        playDTMFDigit(digit);
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_DIAL, INPUT_PULLUP);

    ledcAttach(PIN_DTMF_ROW, 1000, 10);
    ledcAttach(PIN_DTMF_COL, 1000, 10);

    ledcWriteTone(PIN_DTMF_ROW, 0);
    ledcWriteTone(PIN_DTMF_COL, 0);

    previousState = digitalRead(PIN_DIAL);

    lastTransitionUs = micros();

    Serial.println();
    Serial.println("================================");
    Serial.println("PTT Tester V5.0");
    Serial.println("================================");
    Serial.println("Mode 1: test cadran (D10)");
    Serial.println("Mode 2: conversion impulsions -> DTMF");
    Serial.println("Sorties DTMF:");
    Serial.println("- D0: frequence basse (row)");
    Serial.println("- D1: frequence haute (col)");
    Serial.println("Sommer D0 + D1 avec 2 resistances (ex: 10k) vers entree audio.");
    Serial.println("Commandes serie: 0..9=DTMF manuel, r=replay, c=clear");
}

void loop()
{
    while (Serial.available() > 0)
    {
        char cmd = (char)Serial.read();

        if (cmd == 'r' || cmd == 'R')
        {
            replayNumberDTMF();
        }
        else if (cmd == 'c' || cmd == 'C')
        {
            clearNumberBuffer();
            Serial.println("Memoire numero effacee.");
        }
        else if (cmd >= '0' && cmd <= '9')
        {
            uint8_t d = uint8_t(cmd - '0');
            Serial.print("DTMF manuel: ");
            Serial.println(d);
            playDTMFDigit(d);
        }
    }

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

        uint8_t digit = 0;

        if (pulses == 10)
        {
            Serial.println("10 (chiffre 0)");
            digit = 0;
        }
        else
        {
            Serial.println(pulses);
            digit = pulses;
        }

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

        Serial.print("DTMF emis : ");
        Serial.println(digit);

        appendDigitToBuffer(digit);

        // Emission immediate: la box attend les chiffres pendant la composition.
        playDTMFDigit(digit);

        Serial.println("==============================");

        pulses = 0;
        dialing = false;
        waitingClose = false;

        totalOpen = 0;
        totalClose = 0;
    }

    if (numberLen > 0 && !numberFinalized && (millis() - lastDigitMs > NUMBER_TIMEOUT_MS))
    {
        numberFinalized = true;

        Serial.println();
        Serial.println("----------- Numero compose -----------");
        Serial.println(numberBuffer);
        Serial.println("--------------------------------------");
    }
}
