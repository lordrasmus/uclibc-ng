#!/usr/bin/python


import os
import sys
import hashlib
import requests
import secrets
import string


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

# HTTP-POST-Anfrage senden
response = requests.post(url, files=files)

# Ausgabe der Serverantwort
print(response.text)
