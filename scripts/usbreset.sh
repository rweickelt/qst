#!/bin/bash

if [[ $EUID != 0 ]] ; then
  echo This must be run as root!
  exit 1
fi

for xhci in /sys/bus/pci/drivers/?hci_hcd /sys/bus/pci/drivers/?hci-pci; do

  if ! cd $xhci ; then
    echo Failed to change directory to $xhci
    exit 1
  fi

  echo Trying $xhci driver...

  for dev in ????:??:??.? ; do
    if [ "${dev}" == "????:??:??.?" ] ; then
      # Folder is empty
      break
    fi
    echo Found $dev
    echo -n "$dev" > unbind
    echo -n "$dev" > bind
  done
done