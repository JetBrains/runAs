#!/bin/bash

# runAs (settings_file_name, command, bitness, password)
if [ $# -eq 4 ];
then
	# unquote args
	eval "argsFile="$1""
	args=$(cat "$argsFile")
	eval "command="$2""
	eval "password="$4""

	# if root
	if [[ "$EUID" -eq 0 ]];
	then
		# auth
		su -c "\"${0}\" auth \"$password\"" "$args"
		authCode=$?

		if [ "$authCode" = "0" ];
		then
			# su
			su -c "\"$command\"" "$args"
			exit $?
		else
			if [ "$authCode" = "254" ];
			then
				# if run as root for the agent under root, create temp user and run as from this user to check the root's password
				tmpuser="U$(date +%s)"
				adduser --system --no-create-home "$tmpuser" &>/dev/null

				tmpScriptFile=$(mktemp)
				cp -f "${0}" "$tmpScriptFile"
				chmod a+rwx "$tmpScriptFile"

				sudo -u "$tmpuser" "$tmpScriptFile" runAs "$args" "$command" "$password" arg5
				exitCode=$?

				rm "$tmpScriptFile" &>/dev/null
				userdel "$tmpuser" &>/dev/null
				if [ "$authCode" != "0" ];
					then
					exit $exitCode
				fi
			else
				echo "Authentication failure" >&2
				exit 1
			fi
		fi
	fi

	# if not root
	# Check an agent will be able to remove temporary files
	tmpScriptFile=$(mktemp)
	tmpFile=$tmpScriptFile.tmp

	cp -f "${0}" "$tmpScriptFile"
	chmod a+rwx "$tmpScriptFile"
	chmod a+rwx "$TMPDIR"
	echo "#!/bin/bash" > "$tmpScriptFile"
	echo "echo . > \"$tmpFile\"" >> "$tmpScriptFile"

	# create temp file
	"${0}" runAs "$args" "$tmpScriptFile" "$password" arg5

	# try remove
	rm "$tmpFile" &>/dev/null

	if [ -f "$tmpFile" ]; then
	  rm "$tmpScriptFile" &>/dev/null
	  echo ##teamcity[message text='Incorrect runAs configuration: agent won't be able to remove temporary files created by the build step, see teamcity-agent.log for details .' status='ERROR']
	  exit 1
	else
	  # forcible remove tmp file
	  echo "#!/bin/bash" > "$tmpScriptFile"
	  echo "rm \"$tmpFile\" &>/dev/null" >> "$tmpScriptFile"
	  "${0}" runAs "$args" "$tmpScriptFile" "$password" arg5
	  rm "$tmpScriptFile" &>/dev/null
	fi

	# Run as user
	"${0}" runAs "$args" "$command" "$password" arg5
	exit $?
fi

# runAs(auth, args, command, password)
# check user and password
if [ $# -eq 2 ];
then
	if [ "$1" = "auth" ];
	then
		password="$2"
		# if root
		if [[ "$EUID" -eq 0 ]];
		then
			# run as root for the agent under root, we should check the root's password
			# exit 254

			# do not check root password under root agent, see https://youtrack.jetbrains.com/issue/TW-50088
			exit 0
		fi

		sudo -k
		out1=$( sudo -lS 2>&1 << EOF
$password
EOF
)
		authCode=$?
		if [ $authCode -eq 0 ];
		then
			# user has sudo permission and the password is valid
			exit 0
		fi

		sudo -k
		out2=$( sudo -lS 2>&1 << EOF
salt_$password_salt
EOF
)

		if [ "$out1" == "$out2" ]
		then
			# incorrect user or password
			authCode=255
		else
			# user has no sudo pemission, but the password is valid
			authCode=0
		fi

		exit $authCode
	fi
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
			echo "##teamcity[message text='socat was not installed, see https://github.com/JetBrains/teamcity-runas-plugin/wiki/How-to-install#teamcity-agent-on-linux' status='ERROR']"
			exit 255
		fi

		args="$2"
		command="$3"
		password="$4"

		tmpFile=$(mktemp)
		chmod a+rw "$tmpFile"

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
				echo "##teamcity[message text='Error during sending password' status='ERROR']"
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

		# take exid code from file
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

		su -c "\"$command\"" "$args"
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

	echo socat is not installed, see https://github.com/JetBrains/teamcity-runas-plugin/wiki/How-to-install#on-linux >&2
fi

echo Usage: runAs.sh settings_file_name command_file_name bitness password >&2
exit 255