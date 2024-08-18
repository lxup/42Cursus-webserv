# Define the chunks
chunk1="4\r\nWiki\r\n"
chunk2="6\r\npedia\r\n"
chunk3="E\r\n in chunks.\r\n"
chunk4="0\r\n\r\n"

# Send chunks one at a time with curl
{
  echo -n "$chunk1"
  sleep 1
  echo -n "$chunk2"
  sleep 1
  echo -n "$chunk3"
  sleep 1
  echo -n "$chunk4"
} | curl -X POST http://localhost:8080 -H "Transfer-Encoding: chunked" --data-binary @-