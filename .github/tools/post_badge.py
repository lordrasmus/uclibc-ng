#!/usr/bin/python3

# pip3 install pybadges

import os
import sys
import requests
import json


import requests


from pprint import pprint


if not "GIST_TOKEN" in os.environ:
    print("GIST_TOKEN env not set")
    sys.exit(1)
    
github_token = os.environ["GIST_TOKEN"]
file_name = sys.argv[1]
gist_id = "None"



"""

# mit diesem code kann man die id zu einer datei finden

# Ersetzen Sie 'Benutzername' und 'Gist-ID' durch die entsprechenden Werte
gist_owner = 'lordrasmus'
gist_id = "None"

# GitHub API-URL für den Gist
headers = {
    "Authorization": "Bearer " + f"{github_token}",
    "Accept": "application/vnd.github+json",
    "X-GitHub-Api-Version":"2022-11-28"
}
gist_api_url = f'https://api.github.com/gists'

#pprint( headers )

# Führen Sie eine GET-Anfrage zur GitHub API durch
response = requests.get(gist_api_url, headers=headers )

if response.status_code == 200:
    gist_data = response.json()
    for f in gist_data:
        #print( f["id"] )
        #print( f["files"].keys() )
        
        if file_name in f["files"].keys():
            gist_id = f["id"]
            
            
    # Gist-ID aus der Antwort extrahieren
    #gist_id = gist_data['id']
    #print("Gist-ID:", gist_id)
else:
    print(f"Fehler beim Abrufen der Gist-Daten. Statuscode: {response.status_code}")
    
"""

"""

 eine gist id kann aus mehreren dateien bestehen
 darum hier die id für die testresults angeben in der alle testresult.svg's abgelegt werden
 
"""
gist_id = "867aa95ade60fa5b1ad098fa6c6a1968"

print( "ID: " + gist_id )



if gist_id == "None":

    #
    #   wenn es noch keine id gibt kann man so eine neue datei anlegen
    #   und bekommt dann die id zurück
    #

    # Ersetzen Sie die folgenden Werte durch Ihre eigenen Informationen

    gist_description = "Mein Gist"

    f = open("badge.svg","r")
    file_content = f.read()
    f.close()

    # Gist-Payload erstellen
    payload = {
        "description": gist_description,
        "public": True,
        "files": {
            file_name: {
                "content": file_content
            }
        }
    }

    # Gist erstellen
    url = "https://api.github.com/gists"
    headers = {
        "Authorization": f"token {github_token}"
    }
    response = requests.post(url, headers=headers, data=json.dumps(payload))

    # Prüfen, ob die Anfrage erfolgreich war
    if response.status_code == 201:
        gist_data = response.json()
        gist_id = gist_data["id"]
        gist_url = gist_data["html_url"]
        print(f"Erstellter Gist-ID: {gist_id}")
        print(f"Gist URL: {gist_url}")
    else:
        print(f"Fehler bei der Erstellung des Gists. Statuscode: {response.status_code}")
        print(response.text)

else:
    
    # Ersetzen Sie die folgenden Werte durch Ihre eigenen Informationen

    gist_description = "Mein Gist"

    f = open("badge.svg","r")
    file_content = f.read()
    f.close()

    # Gist-Payload erstellen
    payload = {
        "description": gist_description,
        "public": True,
        "files": {
            file_name: {
                "content": file_content
            }
        }
    }

    # Gist erstellen
    url = "https://api.github.com/gists/"+ gist_id
    headers = {
        "Authorization": f"Bearer {github_token}",
        "X-GitHub-Api-Version":"2022-11-28"
    }
    response = requests.post(url, headers=headers, data=json.dumps(payload))

    # Prüfen, ob die Anfrage erfolgreich war
    if response.status_code == 200:
        gist_data = response.json()
        gist_id = gist_data["id"]
        gist_url = gist_data["html_url"]
        print(f"Erstellter Gist-ID: {gist_id}")
        print(f"Gist URL: {gist_url}")
    else:
        print(f"Fehler bei der Erstellung des Gists. Statuscode: {response.status_code}")
        print(response.text)
