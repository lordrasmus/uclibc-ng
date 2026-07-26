#!/usr/bin/python


import os
import sys
import hashlib
import requests
import secrets
import string
import time
import random

from pprint import pprint

# Der Server sieht bis zu 70 parallele Uploads, wenn die ganze Matrix laeuft,
# und wirft dann TLS-Fehler. Mehrfach versuchen, mit Jitter damit die Jobs
# nicht gemeinsam in denselben Engpass zurueckkehren.
TRIES = 5
DELAY = 20

# Einstellungen
url = 'https://uclibc-ng.tangotanzen.de/'
file_path = sys.argv[1]
with open(file_path, 'rb') as f:
    data = f.read()


hash_object = hashlib.sha256()
hash_object.update( os.environ["UPLOAD_KEY"].encode('utf-8') )    
hash_object.update( data )    
api_key = hash_object.hexdigest()

#print( "Key : " + api_key )



# Datei und Schlüssel als Daten für das Formular vorbereiten
files = {'file': (file_path, data ), 'key': (None, api_key) }

if len( sys.argv ) > 2:
    files["path"] = ( None, sys.argv[2] )

if len( sys.argv ) > 3:
    files["file"] = ( sys.argv[3] , data )

#pprint( files )

# HTTP-POST-Anfrage senden
for attempt in range(1, TRIES + 1):
    try:
        response = requests.post(url, files=files, timeout=300)
        if response.status_code < 500:
            # Ausgabe der Serverantwort
            print(response.text)
            sys.exit(0)
        reason = "HTTP " + str(response.status_code)
    except requests.exceptions.RequestException as e:
        reason = str(e)

    if attempt == TRIES:
        print("upload failed after " + str(TRIES) + " tries: " + reason)
        sys.exit(1)

    wait = DELAY + random.uniform(0, DELAY / 2)
    print("upload attempt " + str(attempt) + "/" + str(TRIES) +
          " failed (" + reason + "), retry in " + str(round(wait)) + "s")
    sys.stdout.flush()
    time.sleep(wait)
