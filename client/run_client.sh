# move this file and specified .ini (client, server) files to apropriate build directories to test project

for i in {1..5}; do
  ./client &
done
wait
