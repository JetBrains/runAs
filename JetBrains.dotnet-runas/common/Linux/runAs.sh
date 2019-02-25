#!/bin/bash

# Linux
# runAs (settings_file_name, command, bitness, password)
if [ $# -eq 4 ];
then
    # unquote args
    eval "argsFile="$1""
    args=$(cat "$argsFile")
    eval "command="$2""
    eval "password="$4""
    
    chmod a+rwx "$command"
    
    # if root
    if [[ "$EUID" -eq 0 ]];
    then
        # auth
        su -c "\"${0}\" auth \"$password\"" "$args"
        authCode=$?

        if [ "$authCode" = "0" ];
        then
            # su
            su "$args" -c "\"$command\""
            exit $?
        else
            echo "Can't run under ROOT user."
            exit 255
        fi
    fi

    # if not root	
    # Run as user
    "${0}" runAs "$args" "$command" "$password" arg5

    exit $?
fi

# runAs (runAs, args, command, password, arg5)
if [ $# -eq 5 ];
then
    # runAs without ROOT
    if [ "$1" = "runAs" ];
    then
        # check installed socat
        socat -h &>/dev/null
        socatExitCode=$?
        if [ "$socatExitCode" != "0" ];
        then
            echo "socat was not installed, see http://macappstore.org/socat/."
            exit 255
        fi

        args="$2"
        command="$3"
        password="$4"

        tmpFile=$(mktemp)
        touch "$tmpFile"
        chmod a+rwx "$tmpFile"
        
        cmd="'${0}' su '$tmpFile' '$command' '$args' arg5"
        eval "export -- SOCAT_CMD=\"$cmd\""

        # run command
        (
            # wait for password input
            attempts=50
            while [[ ! -s "$tmpFile" || attemps -gt 0 ]];
            do
                sleep .1
                attempts=$((attempts-1))
            done

            if [[ $attempts -eq 0 ]];
            then
                echo "Error during sending password."
                exit 255
            fi

            # send password to su stdIn
            echo "$password"

            # wait for process finish
            while ps axg | grep "$tmpFile" | grep -v "grep" > /dev/null;
            do
                sleep .1
            done
        ) | (
            # su
            socat - $'EXEC:"bash -exec +x \'eval $SOCAT_CMD\'",pty,ctty,setsid'
        ) 2> >(tee > "$tmpFile" >(grep -v "[Pp]assword:" >&2))

        # if exit file is empty
        if [ ! -s "$tmpFile" ];
        then
            exit 255
        fi

        # take exit code from file
        read -r exitCode<"$tmpFile"
        rm "$tmpFile" & > /dev/null

        exit $exitCode
    fi

    # change the user
    # su (su, tmp_file, command, args, arg5)
    if [ "$1" = "su" ];
    then 
        tmpFile="$2"
        command="$3"
        args="$4"
        
        echo
        #/bin/bash $command
        su "$args" -c "$command"
        exitCode=$?

        echo -e "$exitCode\n" > "$tmpFile"
        exit $exitCode
    fi
fi

if [[ "$EUID" -eq 0 ]];
then
    exit 0
else
    # check installed socat
    socat -h &>/dev/null
    if [ "$?" = "0" ];
    then
      exit 0
    fi

    echo socat is not installed, see http://macappstore.org/socat/ >&2
fi

echo Usage: runAs.sh settings_file_name command_file_name bitness password >&2
exit 255