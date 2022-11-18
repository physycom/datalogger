# datalogger

<a href="http://www.physycom.unibo.it">
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna">
</div>
</a>
<a href="https://travis-ci.com/physycom/datalogger">
<div class="image">
<img src="https://travis-ci.com/physycom/datalogger.svg?token=ujjUseBa9hYbKckXBkxJ&branch=master" width="90" height="20" alt="Build Status">
</div>
</a>

## Purpose

This document describes a tool used during tests performed at the Laboratory of Physics of the City on black boxes.  
It acquires data from different brands of devices through the serial port, using a wrapper around boost::asio, and dumps them into raw files on disk. It can also plot data in realtime using an FLTK widget. The biggest selling point is being able to deploy data analysis on all devices in a single passage, since data is internally represented in a single unified format.

## Installation

**CMake**, **Powershell** and a **C++11** compatible compiler are required. To build the executable, clone the repo and then type

```pwsh
./ci/build.ps1 -UseVCPKG
```
