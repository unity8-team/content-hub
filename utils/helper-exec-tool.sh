#!/bin/bash

initctl set-env "INSTANCE_ID=${INSTANCE_ID}"
exec /usr/lib/x86_64-linux-gnu/upstart-app-launch/click-exec

if [ $? -nq 0 ]; then
	echo "FAILED for ${APP_ID}"
	exit -1
fi
