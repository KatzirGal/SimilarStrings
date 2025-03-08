param (
    [string]$ContainerName
)

docker exec -i $ContainerName sh -c 'uname && if [ $(uname -o) = "Toybox" ]; then ps -A -o pid,comm,args; elif [ $(uname) = "Linux" ]; then ps axww -o pid,comm,args; elif [ $(uname) = "Darwin" ]; then ps axww -o pid,comm,args -c; fi'

exit 0