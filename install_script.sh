#!/bin/bash


mv ./coreconfig.xml  ~/coreconfig.xml

# Set the environment variable "APP_CONFIGURATION" to the file path
export GPCS_CONFIG_XML_PATH=~/coreconfig.xml

# Optional: Display a message to confirm the actions
echo "XML file moved to home directory and GPCS_CONFIG_XML_PATH environment variable set to: $GPCS_CONFIG_XML_PATH"

