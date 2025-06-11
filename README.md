# datalogger

<a href="http://www.physycom.unibo.it">
<div class="image">
<img src="https://cdn.rawgit.com/physycom/templates/697b327d/logo_unibo.png" width="90" height="90" alt="© Physics of Complex Systems Laboratory - Physics and Astronomy Department - University of Bologna">
</div>
</a>

[![Physycom Continuous Integration](https://github.com/physycom/datalogger/actions/workflows/ccpp.yml/badge.svg)](https://github.com/physycom/datalogger/actions/workflows/ccpp.yml)

## Purpose

This document describes a tool used during tests performed at the Laboratory of Physics of the City on black boxes.  
It acquires data from different brands of devices through the serial port, using a wrapper around boost::asio, and dumps them into raw files on disk. It can also plot data in realtime using an FLTK widget. The biggest selling point is being able to deploy data analysis on all devices in a single passage, since data is internally represented in a single unified format.

## Installation

**CMake**, **Powershell** and a **C++11** compatible compiler are required. Dependencies are handled through **vcpkg** using the build script provided in the `cmake` submodule. Clone the repository and run

```pwsh
./cmake/build.ps1 -UseVCPKG
```
On Linux and macOS be sure to install `pwsh` (PowerShell) before executing the command.
