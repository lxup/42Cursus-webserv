#!/bin/sh

# Créer le fichier temporaire pour stocker le corps chunked
tmpfile=$(mktemp /tmp/chunked_body.XXXXXX)

# Corps de la requête en chunks
printf "Wiki\n" > $tmpfile
printf "pedia\n" >> $tmpfile
printf " in\n" >> $tmpfile
printf " \n" >> $tmpfile
printf "chunks.\n" >> $tmpfile

# Envoyer la requête POST avec curl
curl -v -X POST http://10.211.55.5:8080 \
    -H "Transfer-Encoding: chunked" \
    --data-binary @$tmpfile

# Supprimer le fichier temporaire
rm $tmpfile