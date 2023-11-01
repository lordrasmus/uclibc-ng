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
file_read = sys.argv[2]
gist_id = "None"



gist_id = "867aa95ade60fa5b1ad098fa6c6a1968"

print( "ID   : " + gist_id )
print("Read : " + file_read )
print("Name : " + file_name )


    
# Ersetzen Sie die folgenden Werte durch Ihre eigenen Informationen

gist_description = "Test Result"

f = open(file_read,"r")
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
    #pprint( gist_data )
    gist_id = gist_data["id"]
    gist_url = gist_data["html_url"]
    print(f"Erstellter Gist-ID: {gist_id}")
    print(f"Gist URL: {gist_url}")
else:
    print(f"Fehler bei der Erstellung des Gists. Statuscode: {response.status_code}")
    print(response.text)
    print(payload)
    print("github_token: " + github_token[0:10] + " ... " + github_token[-10:] )
