import serial
import csv
import os
from datetime import datetime


# ============================================================
# COM-PORT ABFRAGEN
# ============================================================

print("========================================")
print("   MASTERPROPS TESTSTAND LOGGER")
print("========================================")
print()

SERIAL_PORT = "COM" + input(
    "COM-Port des Arduino eingeben (z.B. 7): "
).strip()

BAUD_RATE = 115200


# ============================================================
# HAUPTORDNER "MESSUNGEN"
# ============================================================

SAVE_FOLDER = "Messungen"

os.makedirs(SAVE_FOLDER, exist_ok=True)


# ============================================================
# NEUEN MESSORDNER ERSTELLEN
# ============================================================

timestamp = datetime.now().strftime(
    "%Y-%m-%d_%H-%M-%S"
)

measurement_folder = os.path.join(
    SAVE_FOLDER,
    f"Messung_{timestamp}"
)

os.makedirs(
    measurement_folder,
    exist_ok=True
)


# ============================================================
# CSV-DATEI
# ============================================================

filepath = os.path.join(
    measurement_folder,
    f"Messdaten_{timestamp}.csv"
)


# ============================================================
# ARDUINO VERBINDEN
# ============================================================

print()
print("Verbinde mit", SERIAL_PORT, "...")

try:

    arduino = serial.Serial(
        SERIAL_PORT,
        BAUD_RATE,
        timeout=1
    )

except serial.SerialException as e:

    print()
    print("FEHLER: Arduino konnte nicht verbunden werden.")
    print()
    print("Überprüfe den COM-Port.")
    print("Beispiel: COM7")
    print()
    print("Fehlermeldung:")
    print(e)
    print()

    input("ENTER zum Beenden...")
    exit()


print()
print("Arduino erfolgreich verbunden!")
print()
print("Messordner:")
print(os.path.abspath(measurement_folder))
print()
print("CSV-Datei:")
print(os.path.abspath(filepath))
print()
print("Warte auf Test...")
print("Drücke STRG+C zum Beenden.")
print()


# ============================================================
# CSV-DATEI ERSTELLEN
# ============================================================

with open(
    filepath,
    "w",
    newline="",
    encoding="utf-8"
) as csvfile:

    writer = csv.writer(
        csvfile,
        delimiter=";"
    )


    # --------------------------------------------------------
    # SPALTENÜBERSCHRIFTEN
    # --------------------------------------------------------

    writer.writerow([
        "Zeit_ms",
        "PWM",
        "RPM",
        "Spannung_V",
        "Strom_A",
        "Schub_N"
    ])

    csvfile.flush()


    # ========================================================
    # DATEN EMPFANGEN
    # ========================================================

    try:

        while True:

            line = arduino.readline()

            if not line:
                continue


            try:

                line = line.decode(
                    "utf-8"
                ).strip()

            except UnicodeDecodeError:

                continue


            # =================================================
            # TEST START
            # =================================================

            if line.startswith("START,"):

                mode = line.split(",", 1)[1]

                print()
                print("----------------------------------------")
                print("TEST GESTARTET")
                print("Modus:", mode)
                print("----------------------------------------")
                print()

                continue


            # =================================================
            # MESSDATEN
            # =================================================

            if line.startswith("DATA,"):

                data = line.split(",")


                # Erwartet:
                #
                # DATA,
                # Zeit,
                # PWM,
                # RPM,
                # Spannung,
                # Strom,
                # Schub
                #
                # = 7 Werte

                if len(data) != 7:
                    continue


                try:

                    time_ms = float(data[1])
                    pwm = int(data[2])


                    if data[3] == "NaN":
                        rpm = ""
                    else:
                        rpm = float(data[3])


                    if data[4] == "NaN":
                        voltage = ""
                    else:
                        voltage = float(data[4])


                    if data[5] == "NaN":
                        current = ""
                    else:
                        current = float(data[5])


                    force = float(data[6])


                except ValueError:

                    continue


                # ------------------------------------------------
                # DATEN IN CSV SCHREIBEN
                # ------------------------------------------------

                writer.writerow([
                    time_ms,
                    pwm,
                    rpm,
                    voltage,
                    current,
                    force
                ])

                csvfile.flush()


                # ------------------------------------------------
                # TERMINAL-AUSGABE
                # ------------------------------------------------

                print(
                    f"Zeit: {time_ms:8.0f} ms | "
                    f"PWM: {pwm:4d} | "
                    f"RPM: {rpm:7} | "
                    f"U: {voltage:6} V | "
                    f"I: {current:6} A | "
                    f"F: {force:6} N"
                )

                continue


            # =================================================
            # TEST ENDE
            # =================================================

            if line == "END":

                print()
                print("----------------------------------------")
                print("TEST BEENDET")
                print("----------------------------------------")
                print()
                print("Messung gespeichert unter:")
                print(os.path.abspath(measurement_folder))
                print()
                print("CSV-Datei:")
                print(os.path.abspath(filepath))
                print()


    except KeyboardInterrupt:

        print()
        print()
        print("----------------------------------------")
        print("LOGGER BEENDET")
        print("----------------------------------------")
        print()
        print("Messung gespeichert unter:")
        print(os.path.abspath(measurement_folder))
        print()


    finally:

        arduino.close()