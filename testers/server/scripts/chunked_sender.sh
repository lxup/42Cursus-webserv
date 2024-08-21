#!/bin/sh

# Créer le fichier temporaire pour stocker le corps chunked
tmpfile=$(mktemp /tmp/chunked_body.XXXXXX)

# Corps de la requête en chunks
echo -e "4\r\nWiki\r\n7\r\npedia i\r\nB\r\n in chunks.\r\n0\r\n\r\n" > $tmpfile

# Envoyer la requête POST avec curl
curl -v -X POST http://localhost:1313 \
    -H "Transfer-Encoding: chunked" \
    --data-binary @$tmpfile

# Supprimer le fichier temporaire
rm $tmpfile