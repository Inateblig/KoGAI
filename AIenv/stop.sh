#!/bin/sh

kill $(pgrep -af DDNet-AI | sed "s/ \..*$//g")

